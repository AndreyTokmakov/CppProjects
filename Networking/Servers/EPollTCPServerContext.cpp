/**============================================================================
Name        : EPollTCPServerContext.h
Created on  : 19.01.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : EPollTCPServerContext
============================================================================**/

#include "EPollTCPServerContext.h"

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

namespace EPollTCPServerContext
{
    constexpr int32_t  INVALID_SOCKET { -1 };
    constexpr int32_t  SOCKET_ERROR { -1 };

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
        //else                              std::cout << "Unknown!!!\n";
        std::cout << "\n==========================================================================\n";
    }

    int32_t Error(std::string_view text)
    {
        std::cerr << text << ". Error = " << errno << "(" << errCodeToStr(errno) << ")\n";
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

namespace EPollTCPServerContext
{
    enum class State
    {
        Open,
        Closed
    };

    struct Session
    {
        std::string buffer;
        State state { State::Closed };

        Session(State state = State::Open): state {state} {
            std::cout << "Session created\n";
        }
    };

    class TCPServer
    {
        static inline constexpr uint32_t BACKLOG { 10 };
        static inline constexpr size_t   BUFFER_SIZE { 1024 * 4 };

        // TODO: Choose different value?
        static constexpr uint32_t kEpollWaitTime { 10 };  // epoll wait timeout 10 ms

        // TODO: Refactor ?
        static constexpr uint32_t kMaxEvents { 1024 };    // epoll wait return max size

        int32_t epollFd { INVALID_SOCKET };
        int32_t serverSocket { INVALID_SOCKET };

        std::string hostAddress;
        uint16_t listenPort {};

        std::unordered_map<int32_t, Session> sessions;

        static int32_t setNonBlock(int32_t handle)
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
        static int32_t setEpollEvents(int efd, int op, int handle, uint32_t events)
        {
            epoll_event event { events, {.fd = handle} };
            if (SOCKET_ERROR == epoll_ctl(efd, op, handle, &event)) {
                return Error("epoll_ctl() failed. (F_SETFL && O_NONBLOCK)");
            }
            return 0;
        }

        void closeClientSocket(int32_t socket, Session& session)
        {
            if (SOCKET_ERROR == ::close(socket)) {
                Error("close() failed");
            }
            session.state = State::Closed;
        }

        // TODO: Store session data --> HashTable
        [[noreturn]]
        void eventsPoller()
        {
            std::array<epoll_event, kMaxEvents>  epollEvents {};
            std::array<char, BUFFER_SIZE> buffer {};
            ssize_t bytes {0}, total {0};
            std::string reply = "PONG";
            auto [clientSock, events] = std::make_pair<int32_t, uint32_t>(0,0);

            while (true)
            {   // TODO: Check TimeOut for performance
                // TODO: Check num != -1
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
                        closeClientSocket(clientSock, session);
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
            }
        }

    public:

        TCPServer(std::string address, uint16_t port):
                hostAddress { std::move(address) }, listenPort {port} {
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
            int32_t clientSocket { INVALID_SOCKET };
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
        TCPServer server {"0.0.0.0", 52525};
        if (server.createSockets())
            server.runServer();
    }
}

void EPollTCPServerContext::TestAll()
{
    startSerer();
}