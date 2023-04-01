/**============================================================================
Name        : EPollTCPServerEx.cpp
Created on  : 01.04.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : EPollTCPServerEx
============================================================================**/

#include "EPollTCPServerEx.h"

#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>

#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <functional>
#include <utility>
#include <thread>


namespace EPollTCPServerEx
{
    constexpr int INVALID_SOCKET { -1 };
    constexpr int SOCKET_ERROR { -1 };
    constexpr uint32_t kEpollWaitTime { 10 }; // epoll wait timeout 10 ms
    constexpr uint32_t kMaxEvents { 100 };    // epoll wait return max size

    struct SocketGuard final
    {
        const int32_t handle { INVALID_SOCKET };

        explicit SocketGuard(int32_t s): handle {s} { /** **/ }
        explicit operator int32_t() { return handle; }

        ~SocketGuard() {
            ::close(handle);
        }
    };

    int32_t Error(std::string_view text)
    {
        std::cerr << text << ". Error = " << errno << std::endl;
        return SOCKET_ERROR;
    }


    int32_t setNonBlock(int32_t handle) {
        const int flags = ::fcntl(handle, F_GETFL, 0);
        if (flags < 0) {
            return Error("fcntl() failed. (F_GETFL)");
        }

        if (SOCKET_ERROR == ::fcntl(handle, F_SETFL, flags | O_NONBLOCK)) {
            return Error("fcntl() failed. (F_SETFL && O_NONBLOCK)");
        }
        return 0;
    }

    int32_t setEpollEvents(int efd, int op, int handle, uint32_t events)
    {
        epoll_event event { events, {.fd = handle} };
        if (SOCKET_ERROR == epoll_ctl(efd, op, handle, &event)) {
            return Error("epoll_ctl() failed. (F_SETFL && O_NONBLOCK)");
        }
        return 0;
    }

    void startSerer()
    {
        const int epollFd = epoll_create1(0);
        if (INVALID_SOCKET == epollFd) {
            std::cerr << "epoll_create1(0) failed. Error = " << errno << std::endl;
            return;
        }

        SocketGuard epollGuard {epollFd };

        const int serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == serverSocket) {
            Error("Failed to create socket");
        }

        SocketGuard guard {serverSocket};

        constexpr uint16_t port{52525};
        constexpr std::string_view host{"0.0.0.0"};
        sockaddr_in server {PF_INET, htons(port), {.s_addr = inet_addr(host.data())}, {}};

        if (SOCKET_ERROR == ::bind(serverSocket, reinterpret_cast<sockaddr*>(&server), sizeof(server))) {
            Error("Failed to bind socket");
        }

        constexpr uint16_t backlog { 10 };
        if (SOCKET_ERROR == ::listen(serverSocket, backlog)) {
            Error("Failed to Listen the socket.");
        }

        std::jthread poller = std::jthread([epollFd]{
            // TODO: Refactor
            epoll_event* epollEvents = new epoll_event[kMaxEvents];
            if (!epollEvents) {
                std::cerr << "ERROR: Failed to allocate epoll_event array" << std::endl;
                exit(1);
            }
            std::cout << "DEBUG: " <<  sizeof(epoll_event) * kMaxEvents << " allocated for epoll_event*\n" ;

            // TODO: Refactor --> std::array
            char buffer[4096] {};
            int64_t bytes {0};
            std::string message, reply;
            while (true)
            {   // TODO: Check TimeOut for performance
                const int num = epoll_wait(epollFd, epollEvents, kMaxEvents, kEpollWaitTime);
                if (num)
                    std::cout << "DEBUG: Polling events: epoll_wait() --> " << num << "\n" ;

                for (int i = 0; i < num; ++i)
                {
                    const int fd = epollEvents[i].data.fd;
                    const uint32_t events = epollEvents[i].events;

                    if (events & EPOLLIN)
                    {
                        //std::cout << "DEBUG: Read data" << std::endl;
                        message.clear();
                        while ((bytes = ::read(fd, buffer, sizeof(buffer))) > 0)
                            message.append(buffer, bytes);

                        std::cout << '[' << message << ']' << std::endl;

                        reply.assign("Reply:" + message);
                        // ::send(fd, reply.data(), reply.length(), 0);
                        int b = ::send(fd, reply.data(), reply.length(), 0);
                        std::cout << b << " bytes send\n";
                    }
                    else if (events & EPOLLOUT)  {
                        std::cout << "DEBUG: Socket(" << fd << ") is valid for writing \n";
                    }
                    else if (events & EPOLLRDHUP)
                    {
                        std::cout << "DEBUG: Closing connection (" << fd << ")\n";
                        ::close(fd);
                    }
                    else if ((events & EPOLLERR) || (events & EPOLLHUP))
                    {
                        std::cout << "DEBUG: Closing connection (" << fd << ")[epoll_wait error]\n";
                        ::close(fd);
                    }
                }
            }
        });

        sockaddr_in clientAddr{};
        socklen_t addLen { sizeof(clientAddr) };
        int clientSocket { INVALID_SOCKET };
        while (true)
        {
            std::cout << "DEBUG: Waiting for next connection ....\n";
            clientSocket = ::accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addLen);
            if (INVALID_SOCKET == clientSocket) {
                Error("Failed to create client socket");
            }

            std::cout << "DEBUG: Client connected " << inet_ntoa(clientAddr.sin_addr) << ':' << htons(clientAddr.sin_port) << std::endl;
            if (SOCKET_ERROR == setNonBlock(clientSocket))
                break;
            else {
                std::cout << "DEBUG: Client socket " << clientSocket << " is set to NonBlocking state\n";
            }

            if (SOCKET_ERROR == setEpollEvents(epollFd, EPOLL_CTL_ADD, clientSocket, EPOLLIN | EPOLLRDHUP | EPOLLET)) {
                // if something goes wrong, close this new socket
                Error("epoll_ctl() failed");
                break;
            }
        }
    }
};

void EPollTCPServerEx::Tests()
{
    startSerer();
};
