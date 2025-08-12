/**============================================================================
Name        : AsynchTcpServerWithSessions.cpp
Created on  : 19.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Asynch (EPoll) TCP Server with Persistent sessions
============================================================================**/

#include "AsynchTcpServerWithSessions.hpp"
#include "Buffer.hpp"
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
        Socket socket { INVALID_SOCKET };
        State state { State::Closed };
        common::Buffer buffer {};

        explicit Session(const Socket socket, const State state = State::Open):
            socket { socket }, state { state } {
        }
    };

    struct TCPServer
    {
        static constexpr SizeType BACKLOG { 10 };
        static constexpr SizeType maxReadBlockSize { 1024 };

        // TODO: Choose different value -  epoll wait timeout 10 ms
        static constexpr SizeType kEpollWaitTime { 10 };

        // TODO: Refactor - epoll wait return max size
        static constexpr SizeType kMaxEvents { 1024 };

        Socket epollFd { INVALID_SOCKET };
        Socket serverSocket { INVALID_SOCKET };

        std::unordered_map<Socket, Session> sessions;
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

        static void closeClientSocket(Session& session,
                                      const State finalState = State::Closed)
        {
            if (SOCKET_ERROR == ::close(session.socket)) {
                Error("close() failed");
            }
            session.state = finalState;
        }

        // TODO: Store session data --> HashTable
        void eventsPoller()
        {
            std::array<epoll_event, kMaxEvents> epollEvents {};

            ssize_t bytes {0}, total {0};
            auto [clientSock, events] = std::make_pair<Socket, uint32_t>(0,0);

            while (true)
            {
                // TODO: Check TimeOut for performance
                // TODO: Check num != -1
                const int32_t num = epoll_wait(epollFd, epollEvents.data(), kMaxEvents, kEpollWaitTime * 1000);
                for (int i = 0; i < num; ++i)
                {
                    // TODO: Refactor
                    clientSock = epollEvents[i].data.fd;
                    events = epollEvents[i].events;

                    const auto [iter, ok] = sessions.try_emplace(clientSock, clientSock, State::Closed);
                    Session& session = iter->second;

                    if (events & EPOLLERR)
                    {
                        if (SOCKET_ERROR == epoll_ctl(epollFd, EPOLL_CTL_DEL, session.socket, nullptr)) {
                            Error("epoll_ctl() failed. (EPOLL_CTL_DEL)");
                        }
                        closeClientSocket(session, State::ClosedWithError);
                        continue;
                    }

                    if (events & EPOLLIN)
                    {
                        total = 0;
                        do {
                            session.buffer.validateCapacity(maxReadBlockSize);
                            bytes = ::read(session.socket, session.buffer.head(), maxReadBlockSize);
                            session.buffer.incrementLength(bytes);
                            total += bytes;
                        } while (bytes > 0);

                        if (total) {
                            session.state = State::Open;
                        }
                        else if (events & EPOLLHUP || events & EPOLLRDHUP) {
                            closeClientSocket(session);
                            continue;
                        }
                    }

                    if (events & EPOLLOUT)
                    {
                        if (State::Open == session.state)
                        {
                            if (SOCKET_ERROR == ::send(session.socket, reply.data(), reply.length(), 0)) {
                                Error("send() failed");
                            }
                            session.buffer.clear();
                        }

                    }

                    if (events & EPOLLHUP || events & EPOLLRDHUP)
                    {
                        if (SOCKET_ERROR == epoll_ctl(epollFd, EPOLL_CTL_DEL, session.socket, nullptr)) {
                            Error("epoll_ctl() failed. (EPOLL_CTL_DEL)");
                        }
                        closeClientSocket(session);
                        continue;
                    }
                }
            }
        }

    public:

        TCPServer(std::string address, const PortType port):
                hostAddress { std::move(address) }, listenPort { port }
        {
            sessions.reserve(kMaxEvents);
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
            }
        }
    };

    void startSerer()
    {
        if (TCPServer server {"0.0.0.0", 52525}; server.createSockets())
            server.runServer();
    }
}


namespace BufferTests
{
    void testBuffer()
    {
        auto print = [](const common::Buffer& buffer) {
            std::cout << buffer.size() << " | " <<
                std::string_view(buffer.data<const char>(), buffer.size()) << std::endl;
        };

        auto write = [](common::Buffer& buffer, const std::string& str) {
            buffer.validateCapacity(str.size());
            std::copy_n(str.data(), str.size(), buffer.head());
            buffer.incrementLength(str.size());
        };


        common::Buffer buffer;

        {
            std::string text(32, 'a') ;
            write(buffer, text);
            print(buffer);
        }

        {
            std::string text(32, 'b') ;
            write(buffer, text);
            print(buffer);
        }

        {
            std::string text(1100, 'c') ;
            write(buffer, text);
            print(buffer);
        }

        {
            std::string text(11100, 'd') ;
            write(buffer, text);
            print(buffer);
        }
    }
}

void tcp_server::TestAll()
{
    startSerer();
    // BufferTests::testBuffer();
}



