/**============================================================================
Name        : EPollTCPServerContextEx.cpp
Created on  : 28.01.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : EPollTCPServerContextEx.cpp
============================================================================**/

#include "EPollTCPServerContextEx.h"

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
#include <thread>
#include <memory>
#include <functional>
#include <utility>
#include <thread>

namespace
{
    using Socket = int32_t;

    constexpr Socket INVALID_SOCKET { -1 };
    constexpr Socket SOCKET_ERROR { -1 };

    static std::string errCodeToStr(int errCode)
    {
        switch (errCode)
        {
            case EPERM:   return "EPERM";
            case ENOENT:  return "ENOENT";
            case ESRCH:   return "ESRCH";
            case EINTR:   return "EINTR";
            case EIO:     return "EIO";
            case ENXIO:   return "ENXIO";
            case E2BIG:   return "E2BIG";
            case ENOEXEC: return "ENOEXEC";
            case EBADF:   return "EBADF";
            case ECHILD:  return "ECHILD";
            case EAGAIN:  return "EAGAIN";
            case ENOMEM:  return "ENOMEM";
            case EACCES:  return "EACCES";
            case EFAULT:  return "EFAULT";
            case ENOTBLK: return "ENOTBLK";
            case EBUSY:   return "EBUSY";
            case EEXIST:  return "EEXIST";
            case EXDEV:   return "EXDEV";
            case ENODEV:  return "ENODEV";
            case ENOTDIR: return "ENOTDIR";
            case EISDIR:  return "EISDIR";
            case EINVAL:  return "EINVAL";
            case ENFILE:  return "ENFILE";
            case EMFILE:  return "EMFILE";
            case ETXTBSY: return "ETXTBSY";
            case EFBIG:   return "EFBIG";
            case ENOSPC:  return "ENOSPC";
            case ESPIPE:  return "ESPIPE";
            case EROFS:   return "EROFS";
            case EMLINK:  return "EMLINK";
            case EPIPE:   return "EPIPE";
            case EDOM:    return "EDOM";
            case ERANGE:  return "ERANGE";
            default:  return "Unknown error";
        }
    }

    void printStateFlags(uint32_t events)
    {
        std::cout << "================================== State ==================================\n";
        if (events & EPOLLIN)        std::cout << "EPOLLIN ";
        if (events & EPOLLPRI)       std::cout << "EPOLLPRI ";
        if (events & EPOLLOUT)       std::cout << "EPOLLOUT ";
        if (events & EPOLLRDNORM)    std::cout << "EPOLLRDNORM ";
        if (events & EPOLLRDBAND)    std::cout << "EPOLLRDBAND ";
        if (events & EPOLLWRNORM)    std::cout << "EPOLLWRNORM ";
        if (events & EPOLLWRBAND)    std::cout << "EPOLLWRBAND ";
        if (events & EPOLLMSG)       std::cout << "EPOLLMSG ";
        if (events & EPOLLERR)       std::cout << "EPOLLERR ";
        if (events & EPOLLHUP)       std::cout << "EPOLLHUP ";
        if (events & EPOLLRDHUP)     std::cout << "EPOLLRDHUP ";
        if (events & EPOLLEXCLUSIVE) std::cout << "EPOLLEXCLUSIVE ";
        if (events & EPOLLWAKEUP)    std::cout << "EPOLLWAKEUP ";
        if (events & EPOLLONESHOT)   std::cout << "EPOLLONESHOT ";
        if (events & EPOLLET)        std::cout << "EPOLLET ";
        else                         std::cout << " **** Unknown * * * * * !!!\n";
        std::cout << "\n==========================================================================\n";
    }

    template<typename _Ty = Socket>
    _Ty Error(std::string_view text, const _Ty exitError = SOCKET_ERROR)
    {
        std::cerr << text << ". Error = " << errno << "(" << errCodeToStr(errno) << ")\n";
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
        std::cout << "DEBUG ";
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
        Session& session;
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
        static inline const SizeType threadsCount { 1 };

        static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> QUEUE_TIMEOUT {
                std::chrono::milliseconds(2000)
        };

        Socket epollFd { INVALID_SOCKET };
        Socket serverSocket { INVALID_SOCKET };

        std::string hostAddress { "0.0.0.0" };
        PortType listenPort { 52525 };

        // TODO: uint8_t --> std::byte ??
        inline static thread_local std::array<uint8_t , readBufferSize> buffer {};

        std::unordered_map<Socket, Session> sessions;

        std::vector<std::jthread> workers {};
        std::atomic_bool run { true };
        std::deque<Event> eventQueue;
        mutable std::mutex eventQueueMutex;

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
            while (run.load(std::memory_order_acquire))
            {
                std::cout << "sessionHandler()\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

        [[noreturn]]
        void eventsPoller()
        {
            std::array<epoll_event, kMaxEvents> epollEvents {};
            auto [clientSock, events] = std::make_pair<int32_t, uint32_t>(0,0);

            while (run.load(std::memory_order_acquire))
            {
                std::cout << "eventsPoller()\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));

                // TODO: Check TimeOut for performance
                // TODO: Check num != -1
                /*
                const int num = epoll_wait(epollFd, epollEvents.data(), kMaxEvents, kEpollWaitTime);
                for (int i = 0; i < num; ++i)
                {   // TODO: Refactor
                    clientSock = epollEvents[i].data.fd;
                    events = epollEvents[i].events;
                    printStateFlags(events);

                    const auto [iter, ok] = sessions.try_emplace(clientSock, State::Closed);
                    Session& session = iter->second;

                    if (events & EPOLLERR)
                    {
                        if (SOCKET_ERROR == epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSock, nullptr))
                            Error("epoll_ctl() failed. (EPOLL_CTL_DEL)");
                        closeClientSocket(clientSock, session, State::ClosedWithError);
                        continue;
                    }

                    if (events & EPOLLIN)
                    {
                        total = 0;
                        while ((bytes = ::read(clientSock, buffer.data(), buffer.size())) > 0) {
                            session.buffer.append(buffer.data(), bytes);
                            total += bytes;
                        }

                        if (total)
                            session.state = State::Open;
                        else if (events & EPOLLHUP || events & EPOLLRDHUP) {
                            closeClientSocket(clientSock, session);
                            continue;
                        }
                    }

                    if (events & EPOLLOUT)
                    {
                        if (State::Open == session.state)
                        {
                            if (SOCKET_ERROR == ::send(clientSock, reply.data(), reply.length(), 0))
                                Error("send() failed");
                            session.buffer.clear();
                        }


                    }

                    if (events & EPOLLHUP || events & EPOLLRDHUP)
                    {
                        if (SOCKET_ERROR == epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSock, nullptr))
                            Error("epoll_ctl() failed. (EPOLL_CTL_DEL)");
                        closeClientSocket(clientSock, session);
                        continue;
                    }
                }
                 */
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
            return true;

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

            while (run.load(std::memory_order_acquire))
            {
                std::cout << "runServer() accepting connections\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));

                /*
                debug("Waiting for next connection ....");
                clientSocket = ::accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addLen);
                if (INVALID_SOCKET == clientSocket) {
                    Error("Failed to create client socket");
                    break;
                }

                debug("Client connected", inet_ntoa(clientAddr.sin_addr), ':', htons(clientAddr.sin_port));
                if (SOCKET_ERROR == setNonBlock(clientSocket))
                    break;

                // TODO: Need to use EPOLL_CTL_DEL on delete event
                if (SOCKET_ERROR == setEpollEvents(epollFd, EPOLL_CTL_ADD, clientSocket,
                                                   EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET)) {
                    // if something goes wrong, close this new socket
                    Error("epoll_ctl() failed");
                    break;
                }
                */
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