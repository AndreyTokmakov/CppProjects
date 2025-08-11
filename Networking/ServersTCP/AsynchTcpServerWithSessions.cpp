/**============================================================================
Name        : AsynchTcpServerWithSessions.cpp
Created on  : 19.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Asynch (EPoll) TCP Server with Persistent sessions
============================================================================**/

#include "AsynchTcpServerWithSessions.hpp"
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
#include <thread>
#include <memory>
#include <functional>
#include <utility>
#include <thread>


namespace
{
    using Socket   = int32_t;
    using PortType = uint16_t;
    using SizeType = uint32_t;

    constexpr Socket  INVALID_SOCKET { -1 };
    constexpr Socket  SOCKET_ERROR { -1 };

    int32_t Error(const std::string_view text)
    {
        std::cerr << text << ". Error = " << errno << "(" << Utilities::errCodeToStr(errno) << ")\n";
        return SOCKET_ERROR;
    }

    template<typename T>
    auto addSpace(const T& arg) -> decltype(auto)
    {
        std::cout << ' ';
        return arg;
    };

    template<typename ...Args>
    void debug(Args&&... args)
    {
        std::cout << "DEBUG ";
        (std::cout << ... << addSpace(std::forward<Args>(args))) << std::endl;
    }
}

namespace tcp_server
{
    enum class State
    {
        Idle,
        Open,
        // Reading,
        // Writing,
        Closed,
        ClosedWithError
    };

    struct Session
    {
        static constexpr size_t initialBufferSize { 1024 * 4 };

        std::string buffer {};
        // std::vector<char> buffer {};

        State state { State::Closed };

        explicit Session(const State state = State::Open):
        // buffer(initialBufferSize),
                state {state} {
            buffer.resize(128);
        }
    };

    struct TCPServer
    {
        static constexpr SizeType BACKLOG { 10 };

        // TODO: Choose different value -  epoll wait timeout 10 ms
        static constexpr SizeType kEpollWaitTime { 10 };

        // TODO: Refactor - epoll wait return max size
        static constexpr SizeType kMaxEvents { 32 };

        // TODO: Char --> std::byte ??
        inline static thread_local std::array<char, Session::initialBufferSize> buffer {};

        Socket epollFd { INVALID_SOCKET };
        Socket serverSocket { INVALID_SOCKET };

        std::vector<Session*> sessions {};
        std::string hostAddress;
        PortType listenPort {};

        // FIXME: Remove -- its temporary
        const std::string reply = "PONG";

        static int32_t setNonBlock(const Socket handle)
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
        static int32_t setEpollEvents(const int efd, const int op, const int handle, const SizeType events)
        {
            epoll_event event { events, {.fd = handle} };
            if (SOCKET_ERROR == epoll_ctl(efd, op, handle, &event)) {
                return Error("epoll_ctl() failed. (F_SETFL && O_NONBLOCK)");
            }
            return 0;
        }

        static void closeClientSocket(const Socket socket,
                                      Session* session,
                                      const State finalState = State::Closed)
        {
            if (SOCKET_ERROR == ::close(socket)) {
                Error("close() failed");
            }
            session->state = finalState;
        }

        // TODO: Store session data --> HashTable
        [[noreturn]]
        void eventsPoller()
        {
            std::array<epoll_event, kMaxEvents> epollEvents {};
            for (SizeType idx = 0; idx < kMaxEvents; ++idx)
            {
                epollEvents[idx].data.ptr = sessions[idx];

                Session* session = reinterpret_cast<Session*>(epollEvents[idx].data.ptr);
                std::cout << idx << " [" << session << "] = " <<  session->buffer.size() << std::endl;
            }

            ssize_t bytes {0}, total {0};
            auto [clientSock, events] = std::make_pair<Socket, uint32_t>(0,0);

            Session* session {nullptr };

            for (int i = 0; i < 32; ++i)
            {
                session = reinterpret_cast<Session*>(epollEvents[i].data.ptr);
                std::cout << i << " [" << session << "] = " << std::endl;
            }

            std::cout << "==============================\n";

#if 0
            while (true)
            {
                // TODO: Check TimeOut for performance
                // TODO: Check num != -1
                const int num = epoll_wait(epollFd, epollEvents.data(), kMaxEvents, kEpollWaitTime);
                for (int i = 0; i < num; ++i)
                {
                    // TODO: Refactor
                    clientSock = epollEvents[i].data.fd;
                    events = epollEvents[i].events;

                    // printStateFlags(events);
                    // const auto [iter, ok] = sessions.try_emplace(clientSock, State::Closed);
                    // session = &(iter->second);

                    session = reinterpret_cast<Session*>(epollEvents[i].data.ptr);

                    std::cout << i << std::endl;
                    std::cout << i << " [" << session << "] = " << std::endl;
                    std::cout << session->buffer.size() << std::endl;
                    std::cout << i << std::endl;

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
                        // TODO: Read ---> to the Session Buffer
                        while ((bytes = ::read(clientSock, buffer.data(), buffer.size())) > 0) {
                            session->buffer.append(buffer.data(), bytes);
                            total += bytes;
                        }

                        if (total)
                            session->state = State::Open;
                        else if (events & EPOLLHUP || events & EPOLLRDHUP) {
                            closeClientSocket(clientSock, session);
                            continue;
                        }
                    }

                    if (events & EPOLLOUT)
                    {
                        if (State::Open == session->state)
                        {
                            if (SOCKET_ERROR == ::send(clientSock, reply.data(), reply.length(), 0))
                                Error("send() failed");
                            session->buffer.clear();
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
            }
#endif
        }

    public:

        TCPServer(std::string address, const PortType port):
                // sessions { kMaxEvents },
                hostAddress { std::move(address) },
                listenPort { port }
        {
            sessions.reserve(kMaxEvents);
            for (int i = 0; i < kMaxEvents; ++i) {
                sessions.push_back(new Session);
            }

            std::cout << "Server created: \n"
                      << "\t Max Sessions: " << sessions.size() << ". Memory allocated: "
                      << sessions.size() * sizeof(Session)
                      << std::endl;
        }

        bool createSockets()
        {
            epollFd = epoll_create1(0);
            if (INVALID_SOCKET == epollFd) {
                std::cerr << "epoll_create1(0) failed. Error = " << errno << std::endl;
                return false;
            }

            serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (INVALID_SOCKET == serverSocket) {
                Error("Failed to create socket");
                return false;
            }

            sockaddr_in server {PF_INET, htons(listenPort), {.s_addr = inet_addr(hostAddress.data())}, {}};
            if (SOCKET_ERROR == ::bind(serverSocket, reinterpret_cast<sockaddr*>(&server), sizeof(server))) {
                Error("Failed to bind socket");
                return false;
            }

            if (SOCKET_ERROR == ::listen(serverSocket, BACKLOG)) {
                Error("Failed to Listen the socket.");
                return false;
            }

            return true;
        }

        void runServer()
        {

            // TODO: To class member ???
            std::jthread thread(&TCPServer::eventsPoller, this);
            /*
             sockaddr_in clientAddr{};
             socklen_t addLen { sizeof(clientAddr) };
             Socket clientSocket { INVALID_SOCKET };
             while (true)
             {
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
             }*/
        }
    };

    void startSerer()
    {
        if (TCPServer server {"0.0.0.0", 52525}; server.createSockets())
            server.runServer();
    }
}

void tcp_server::TestAll()
{
    startSerer();
}



