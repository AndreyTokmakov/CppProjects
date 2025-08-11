/**============================================================================
Name        : EPollTCPServerContextEx.cpp
Created on  : 28.01.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : EPollTCPServerContextEx.cpp
============================================================================**/

#include "EPollTCPServerContextEx.h"
#include "../Utilities/Utilities.h"

#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <netinet/in.h>
#include <cstdlib>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <csignal>

#include <iostream>
#include <string>
#include <deque>
#include <vector>
#include <chrono>
#include <format>
#include <memory>
#include <memory>
#include <functional>
#include <utility>
#include <thread>
#include <condition_variable>

namespace
{
    using Socket = int32_t;
    constexpr Socket INVALID_SOCKET { -1 };
    constexpr Socket SOCKET_ERROR { -1 };


    template<typename Ty = Socket>
    Ty Error(const std::string_view text, const Ty exitError = SOCKET_ERROR)
    {
        std::cerr << text << ". Error = " << errno << "(" << Utilities::errCodeToStr(errno) << ")\n";
        return exitError;
    }

    template<typename T>
    auto addSpace(const T& arg) -> decltype(auto)
    {
        std::cout << ' ';
        return arg;
    };

    template<typename ...Args>
    void DEBUG(Args&&... args)
    {
        const std::chrono::time_point now = std::chrono::system_clock::now();
        std::cout << std::format("{:%d-%m-%Y %H:%M:%OS}", now) << " DEBUG ";
        (std::cout << ... << addSpace(std::forward<Args>(args))) << std::endl;
    }
}

namespace EPollTCPServerContextEx
{
    enum class State
    {
        Idle,
        Open,
        Closed,
        ClosedWithError
    };

    enum class EventType
    {
        None,
        Read,
        ReadClose,
        Write,
        WriteClose,
        ReadWrite,
        ReadWriteClose,
        Error,
        Close
    };

    struct Session
    {
        Socket clientSocket {INVALID_SOCKET};
        State state{State::Idle};
        // TODO: Replace with std::vector<std::byte> ????
        std::string buffer;

        Session(Socket socket, State state = State::Open) :
            clientSocket {socket}, state {state} {
        }
    };

    // TODO: Check alignment
    struct Event
    {
        EventType type { EventType::None };
        // TODO: ref ??  ptr ??
        Session* session { nullptr };
    };

    class TCPServer
    {
        using PortType = uint16_t;
        using SizeType = uint32_t;

        static inline constexpr SizeType backLog { 10 };

        // TODO: BUFFER_SIZE --> MTU ???
        static inline constexpr SizeType readBufferSize { 1024 * 2 };

        // TODO: Choose different value?
        static inline constexpr SizeType kEpollWaitTime { 10 };  /** epoll wait timeout 10 ms **/
        static inline constexpr SizeType kMaxEvents { 1024 };    /** epoll wait return max size **/

        /** Maximum number of request handler workers: **/
        // static inline const size_t threadsCount { std::thread::hardware_concurrency() };
        static inline constexpr SizeType threadsCount { 8 };

        static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> QUEUE_TIMEOUT {
                std::chrono::milliseconds(2000)
        };

        Socket epollFd { INVALID_SOCKET };
        Socket serverSocket { INVALID_SOCKET };

        std::string hostAddress { "0.0.0.0" };
        PortType listenPort { 52525 };

        // TODO: char --> uint8_t ?? uint8_t --> std::byte ??
        inline static thread_local std::array<char , readBufferSize> buffer {};

        std::unordered_map<Socket, Session> sessions;
        std::deque<Event> eventQueue;

        mutable std::mutex eventQueueMutex;

        std::vector<std::jthread> workers {};
        std::atomic_bool run { true };
        std::condition_variable eventCV;

    public:

        static Socket setNonBlock(Socket handle)
        {
            const int flags = ::fcntl(handle, F_GETFL, 0);
            if (flags < 0) {
                return Error("fcntl() failed. (F_GETFL)");
            }

            if (SOCKET_ERROR == ::fcntl(handle, F_SETFL, flags | O_NONBLOCK)) {
                return Error("fcntl() failed. (F_SETFL && O_NONBLOCK)");
            }
            return 0;
        }

        // TODO: Rename to subscribe ?
        static Socket setEpollEvents(int efd, int op, int handle, uint32_t events)
        {
            epoll_event event { events, {.fd = handle} };
            if (SOCKET_ERROR == epoll_ctl(efd, op, handle, &event)) {
                return Error("epoll_ctl() failed. (F_SETFL && O_NONBLOCK)");
            }
            return 0;
        }

        template<class Rep, class Period>
        bool getNextEvent(Event &event,
                          const std::chrono::duration<Rep, Period> &timeout) noexcept {
            std::unique_lock<std::mutex> lock { eventQueueMutex };
            if (!eventCV.wait_for(lock, timeout, [this] { return !eventQueue.empty(); }))
                return false;
            event = eventQueue.front();
            eventQueue.pop_front();
            eventCV.notify_all();
            return true;
        }

        void enqueueSessionEvent(EventType eventType, Session* session) noexcept
        {
            std::lock_guard<std::mutex> lock(eventQueueMutex);
            eventQueue.emplace_back(eventType, session);
            eventCV.notify_one();
        }

        void closeClientSocket(Socket socket,
                               Session& session,
                               State finalState = State::Closed)
        {
            if (SOCKET_ERROR == ::close(socket)) {
                Error("close() failed");
            }
            session.state = finalState;
        }

        [[noreturn]]
        void sessionHandler()
        {
            ssize_t bytes {0}, total {0};
            Event event;
            std::string reply;

            // TODO: Check for performance
            // while (run.load(std::memory_order_acquire))
            while (true)
            {

                if (bool result = getNextEvent(event, QUEUE_TIMEOUT); result)
                {
                    // DEBUG("Handling new event");

                    Session& session = *event.session;
                    if (EventType::ReadWrite == event.type)
                    {
                        //DEBUG("Handling new event: Read");
                        while ((bytes = ::read(session.clientSocket, buffer.data(), buffer.size())) > 0) {
                            session.buffer.append(buffer.data(), bytes);
                            total += bytes;
                        }

                        reply.append("[").append(session.buffer).append("]");
                        if (SOCKET_ERROR == ::send(session.clientSocket, reply.data(), reply.length(), 0))
                            Error("send() failed");

                        session.buffer.clear();
                        reply.clear();
                    }
                    else if (EventType::Read == event.type)
                    {
                        //DEBUG("Handling new event: Read");
                        while ((bytes = ::read(session.clientSocket, buffer.data(), buffer.size())) > 0) {
                            session.buffer.append(buffer.data(), bytes);
                            total += bytes;
                        }
                    }
                    else if (EventType::Write == event.type)
                    {
                        //DEBUG("Handling new event: Write");
                        reply.append("[").append(session.buffer).append("]");
                        if (SOCKET_ERROR == ::send(session.clientSocket, reply.data(), reply.length(), 0))
                            Error("send() failed");

                        session.buffer.clear();
                        reply.clear();
                    }
                    else if (EventType::Close == event.type)
                    {
                        //DEBUG("Handling new event: Close");
                        closeClientSocket(session.clientSocket, session);
                    }
                    else if (EventType::Error == event.type)
                    {
                        //DEBUG("Handling new event: Error");
                        closeClientSocket(session.clientSocket, session, State::ClosedWithError);
                    } else
                    {
                        DEBUG("Handling new event: ELSE");
                    }
                }
            }
        }

        [[noreturn]]
        void eventsPoller()
        {
            std::array<epoll_event, kMaxEvents> epollEvents {};
            auto [clientSock, events] = std::make_pair<int32_t, uint32_t>(0,0);

            // TODO: Check for performance
            // while (run.load(std::memory_order_acquire))
            while (true)
            {
                // TODO: Check TimeOut for performance
                // TODO: Check num != -1
                const int num = epoll_wait(epollFd, epollEvents.data(), kMaxEvents, kEpollWaitTime);
                for (int i = 0; i < num; ++i)
                {   // TODO: Refactor
                    clientSock = epollEvents[i].data.fd;
                    events = epollEvents[i].events;
                    // printStateFlags(events);

                    const auto [iter, ok] = sessions.try_emplace(clientSock, clientSock);
                    Session& session = iter->second;

                    if (events & EPOLLERR)
                    {
                        if (SOCKET_ERROR == epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSock, nullptr))
                            Error("epoll_ctl() failed. (EPOLL_CTL_DEL)");
                        enqueueSessionEvent(EventType::Close, &session);
                    }

                    // session.state = State::Open;
                    if (events & EPOLLIN && events & EPOLLOUT && (events & EPOLLHUP || events & EPOLLRDHUP)) {
                        enqueueSessionEvent(EventType::ReadWriteClose, &session);
                    }
                    else if (events & EPOLLIN && (events & EPOLLHUP || events & EPOLLRDHUP))
                    {
                        enqueueSessionEvent(EventType::ReadClose, &session);
                    }
                    else if (events & EPOLLOUT && (events & EPOLLHUP || events & EPOLLRDHUP))
                    {
                        enqueueSessionEvent(EventType::WriteClose, &session);
                    }
                    else if (events & EPOLLIN && events & EPOLLOUT)
                    {
                        enqueueSessionEvent(EventType::ReadWrite, &session);
                    }
                    else if (events & EPOLLIN)
                    {
                        enqueueSessionEvent(EventType::Read, &session);
                    }
                    else if (events & EPOLLOUT)
                    {
                        enqueueSessionEvent(EventType::Write, &session);
                    }

                    if (events & EPOLLHUP || events & EPOLLRDHUP)
                    {
                        if (SOCKET_ERROR == epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSock, nullptr))
                            Error("epoll_ctl() failed. (EPOLL_CTL_DEL)");
                        enqueueSessionEvent(EventType::Close, &session);
                    }
                }
            }
        }

    public:

        TCPServer(std::string address, uint16_t port):
                hostAddress { std::move(address) }, listenPort {port}
        {
            for (size_t i = 0; i < threadsCount; ++i) {
                workers.emplace_back(&TCPServer::sessionHandler, this);
            }
        }

        [[nodiscard]]
        bool createSockets()
        {
            epollFd = epoll_create1(0);
            if (INVALID_SOCKET == epollFd)
                return Error("epoll_create1(0) failed.", false);

            serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (INVALID_SOCKET == serverSocket)
                return Error("Failed to create socket", false);

            sockaddr_in server {PF_INET, htons(listenPort), {.s_addr = inet_addr(hostAddress.data())}, {}};
            if (SOCKET_ERROR == ::bind(serverSocket, reinterpret_cast<sockaddr*>(&server), sizeof(server)))
                return Error("Failed to bind socket", false);

            if (SOCKET_ERROR == ::listen(serverSocket, backLog))
                return Error("Failed to Listen the socket.", false);

            return true;
        }

        void runServer()
        {
            std::jthread thread(&TCPServer::eventsPoller, this);

            sockaddr_in clientAddr {};
            socklen_t addLen { sizeof(clientAddr) };
            int32_t clientSocket { INVALID_SOCKET };

            // TODO: Check for performance
            while (run.load(std::memory_order_acquire))
            {
                DEBUG("Waiting for next connection ....");
                clientSocket = ::accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addLen);
                if (INVALID_SOCKET == clientSocket) {
                    Error("Failed to create client socket");
                    break;
                }

                DEBUG("Client connected", inet_ntoa(clientAddr.sin_addr), ':', htons(clientAddr.sin_port));
                if (SOCKET_ERROR == setNonBlock(clientSocket))
                    break;

                // TODO: Need to use EPOLL_CTL_DEL on delete event
                if (SOCKET_ERROR == setEpollEvents(epollFd, EPOLL_CTL_ADD, clientSocket,
                                                   EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET)) {
                    // if something goes wrong, close this new socket
                    Error("epoll_ctl() failed");
                    break;
                }
            }
        }
    };

    void startSerer()
    {
        TCPServer server {"0.0.0.0", 52525};
        if (server.createSockets())
            server.runServer();
    }
}


void EPollTCPServerContextEx::TestAll()
{
    startSerer();
}