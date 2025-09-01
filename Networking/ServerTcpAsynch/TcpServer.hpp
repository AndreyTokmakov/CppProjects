/**============================================================================
Name        : TcpServer.hpp
Created on  : 23.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TcpServer.hpp
============================================================================**/

#ifndef CPPWORKPROJECTS_TCPSERVER_HPP
#define CPPWORKPROJECTS_TCPSERVER_HPP


#include "Session.hpp"
#include "../Utilities/Utilities.h"
#include "DateTimeUtilities.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <iostream>
#include <string>
#include <thread>
#include <ostream>
#include <unordered_map>
#include <memory>
#include <functional>
#include <utility>


namespace tcp_server_asynch
{
    using namespace common;

    // FIXME --> Remove
    int32_t Error(std::string_view text);

    // FIXME --> Remove
    template<typename T>
    auto addSpace(const T& arg) -> decltype(auto)
    {
        std::cout << ' ';
        return arg;
    };

    // FIXME --> Remove
    template<typename ...Args>
    void debug(Args&&... args)
    {
        std::cout << DateTimeUtilities::getCurrentTime() << " [DEBUG]";
        (std::cout << ... << addSpace(std::forward<Args>(args))) << std::endl;
    }

    template<common::RequestProcessor Processor>
    struct TCPServer
    {
        static constexpr SizeType backLog { 10 };
        static constexpr SizeType maxReadBlockSize { 1024 };

        // TODO: Choose different value -  epoll wait timeout 10 ms
        static constexpr SizeType kEpollWaitTime { 10 };

        // TODO: Refactor - epoll wait return max size
        static constexpr SizeType kMaxEvents { 1024 };

        Socket epollFd { INVALID_SOCKET };
        Socket serverSocket { INVALID_SOCKET };

        std::unordered_map<Socket, Session> sessions;
        std::string hostAddress;
        PortType listenPort{};

        TCPServer(std::string address, const PortType port, Processor &processor) :
                hostAddress { std::move(address) }, listenPort { port }, processor { processor }
        {
            sessions.reserve(kMaxEvents);
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
                // debug ("Waiting for next connection ....");
                clientSocket = ::accept(serverSocket, reinterpret_cast<sockaddr *>(&clientAddr), &addLen);
                if (INVALID_SOCKET == clientSocket) {
                    Error("Failed to create client socket");
                    break;
                }

                // debug("Client connected", inet_ntoa(clientAddr.sin_addr), ':', htons(clientAddr.sin_port));
                if (SOCKET_ERROR == setNonBlock(clientSocket))
                    break;

                if (SOCKET_ERROR == addEpollEvents(clientSocket, EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET)) {
                    // if something goes wrong, close this new socket
                    Error("epoll_ctl() failed");
                    break;
                }

                // TODO:
                //  1. Create new session and add it to 'sessions'
            }
        }

        bool initialize()
        {
            epollFd = ::epoll_create1(0);
            if (INVALID_SOCKET == epollFd) {
                std::cerr << "epoll_create1(0) failed. Error = " << errno << std::endl;
                return false;
            }

            serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (INVALID_SOCKET == serverSocket) {
                Error("Failed to create socket");
                return false;
            }

            sockaddr_in server{PF_INET, htons(listenPort), {.s_addr = inet_addr(hostAddress.data())}, {}};
            if (SOCKET_ERROR == ::bind(serverSocket, reinterpret_cast<sockaddr *>(&server), sizeof(server))) {
                Error("Failed to bind socket");
                return false;
            }

            if (SOCKET_ERROR == ::listen(serverSocket, backLog)) {
                Error("Failed to Listen the socket.");
                return false;
            }

            return true;
        }

        static int32_t setNonBlock(const Socket handle)
        {
            const int32_t flags = ::fcntl(handle, F_GETFL, 0);
            if (flags < 0) {
                return Error("fcntl() failed. (F_GETFL)");
            }

            if (SOCKET_ERROR == ::fcntl(handle, F_SETFL, flags | O_NONBLOCK)) {
                return Error("fcntl() failed. (F_SETFL && O_NONBLOCK)");
            }
            return 0;
        }

        int32_t epollCtrl(const int32_t op, const int32_t handle, epoll_event* event)
        {
            if (SOCKET_ERROR == ::epoll_ctl(epollFd, op, handle, event)) {
                return Error("epoll_ctl() failed");
            }
            return 0;
        }

        int32_t removeEpollEvents(const Session& session)
        {
            return epollCtrl(EPOLL_CTL_DEL, session.socket, nullptr);
        }

        int32_t addEpollEvents(const int32_t handle, const uint32_t events)
        {
            epoll_event event { events, {.fd = handle} };
            return epollCtrl( EPOLL_CTL_ADD, handle, &event);
        }

        [[noreturn]]
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
                for (int32_t i = 0; i < num; ++i)
                {
                    // TODO: Refactor
                    clientSock = epollEvents[i].data.fd;
                    events = epollEvents[i].events;

                    const auto [iter, ok] = sessions.try_emplace(clientSock, clientSock, State::Closed);
                    Session& session = iter->second;

                    if (events & EPOLLERR)
                    {
                        removeEpollEvents(session);
                        session.Close(State::ClosedWithError);
                        sessions.erase(iter);
                        continue;
                    }

                    if (events & EPOLLIN)
                    {
                        total = 0;
                        do {
                            session.request.validateCapacity(maxReadBlockSize);
                            bytes = ::read(session.socket, session.request.head(), maxReadBlockSize);
                            session.request.incrementLength(bytes);
                            total += bytes;
                        } while (bytes > 0);

                        if (total) {
                            session.state = State::Open;
                            processor.process(session);
                        }
                    }

                    if (events & EPOLLOUT)
                    {
                        if (State::ReadyToWrite == session.state)
                        {
                            bytes = ::send(session.socket, session.response.data(), session.response.length(), 0);
                            if (SOCKET_ERROR == bytes) {
                                Error("send() failed");
                            } else {
                                // debug(bytes, "bytes send");
                            }

                            session.response.clear();
                            session.state = State::Open;
                        }
                    }

                    if (events & EPOLLHUP || events & EPOLLRDHUP)
                    {
                        removeEpollEvents(session);
                        session.Close();
                        sessions.erase(iter);
                    }
                }
            }
        }

    private:

        Processor& processor;
    };
}

#endif //CPPWORKPROJECTS_TCPSERVER_HPP
