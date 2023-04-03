/**============================================================================
Name        : EPollTCPServerDebug.cpp
Created on  : 02.04.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : EPollTCPServerDebug
============================================================================**/

#include "EPollTCPServerDebug.h"

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


namespace EPollTCPServerDebug::Utilities
{
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

    template<typename T>
    auto addSpace(const T& arg) -> decltype(auto) {
        std::cout << ' ';
        return arg;
    };

    template<typename ...Args>
    void debug(Args&&... args) {
        std::cout << "DEBUG ";
        (std::cout << ... << addSpace(std::forward<Args>(args))) << std::endl;
    }
}

namespace EPollTCPServerDebug::Old
{
    constexpr int32_t INVALID_SOCKET { -1 };
    constexpr int32_t SOCKET_ERROR { -1 };
    constexpr uint32_t kEpollWaitTime { 10 }; // epoll wait timeout 10 ms
    constexpr uint32_t kMaxEvents { 100 };    // epoll wait return max size

    using namespace Utilities;
    
    void signal_handler(int sigID)
    {
        if (SIGPIPE == sigID){
            // puts("hello SIGPIPE");
        }
    }

    struct SocketGuard final
    {
        const int32_t handle { INVALID_SOCKET };

        explicit SocketGuard(int32_t s): handle {s} { /** **/ }
        explicit operator int32_t() const { return handle; }

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

        // TODO: We have to ignore SIGPIPE
        signal(SIGPIPE , signal_handler);

        std::jthread poller = std::jthread([epollFd]{
            // TODO: Refactor
            epoll_event* epollEvents = new epoll_event[kMaxEvents];
            if (!epollEvents) {
                std::cerr << "ERROR: Failed to allocate epoll_event array" << std::endl;
                exit(1);
            }
            debug(sizeof(epoll_event) * kMaxEvents, "allocated for epoll_event*");

            // TODO: Refactor --> std::array
            char buffer[4096] {};
            int64_t bytes {0}, total {0};
            std::string message, reply;
            while (true)
            {   // TODO: Check TimeOut for performance
                const int num = epoll_wait(epollFd, epollEvents, kMaxEvents, kEpollWaitTime);
                // if (num) debug("Polling", num, "events with epoll_wait()");

                for (int i = 0; i < num; ++i)
                {   // TODO: Refactor
                    const int32_t clientSock = epollEvents[i].data.fd;
                    const uint32_t events = epollEvents[i].events;

                    printStateFlags(events);

                    if ((events & EPOLLERR) || (events & EPOLLHUP))
                    {   // TODO: handle EPOLL_CTL_DEL
                        debug("Closing connection. Socket = ", clientSock, "[epoll_wait error]");
                        ::close(clientSock);
                    }
                    else if (events & EPOLLRDHUP)
                    {  // TODO: handle EPOLL_CTL_DEL
                        debug("Closing connection. Socket = ", clientSock);
                        ::close(clientSock);
                    }
                    else if (events & EPOLLIN)
                    {
                        total = 0;
                        message.clear();

                        while ((bytes = ::read(clientSock, buffer, sizeof(buffer))) > 0) {
                            message.append(buffer, bytes);
                            total += bytes;
                        }

                        debug(total, "bytes received: ", message, "| events: ", events);
                        if (0 != bytes)
                        {
                            reply.assign("Reply:" + message);
                            bytes = ::send(clientSock, reply.data(), reply.length(), 0);
                            debug(bytes, "bytes send");
                        }
                    }
                    else if (events & EPOLLOUT)
                    {   // TODO: handle EPOLL_CTL_DEL
                        debug("Socket(", clientSock, ") is valid for writing");
                    }
                }
            }
        });

        sockaddr_in clientAddr{};
        socklen_t addLen { sizeof(clientAddr) };
        int clientSocket { INVALID_SOCKET };
        while (true)
        {
            debug("Waiting for next connection ....");
            clientSocket = ::accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addLen);
            if (INVALID_SOCKET == clientSocket) {
                Error("Failed to create client socket");
            }

            debug("Client connected", inet_ntoa(clientAddr.sin_addr), ':', htons(clientAddr.sin_port));
            if (SOCKET_ERROR == setNonBlock(clientSocket))
                break;

            // TODO: Need to use EPOLL_CTL_DEL on delete event
            if (SOCKET_ERROR == setEpollEvents(epollFd, EPOLL_CTL_ADD, clientSocket, EPOLLIN | EPOLLRDHUP | EPOLLET)) {
                // if something goes wrong, close this new socket
                Error("epoll_ctl() failed");
                break;
            }
        }
    }
}

namespace EPollTCPServerDebug::New
{
    using namespace Utilities;

    class TCPServer
    {
        static inline constexpr int32_t  INVALID_SOCKET { -1 };
        static inline constexpr int32_t  SOCKET_ERROR { -1 };
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

        static int32_t Error(std::string_view text)
        {
            std::cerr << text << ". Error = " << errno << std::endl;
            return SOCKET_ERROR;
        }

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

        void eventsPoller()
        {
            std::array<epoll_event, kMaxEvents>  epollEvents {};
            std::array<char, BUFFER_SIZE> buffer {};
            ssize_t bytes {0}, total {0};
            std::string message, reply;
            auto [clientSock, events] = std::make_pair<int32_t, uint32_t>(0,0);

            while (true)
            {   // TODO: Check TimeOut for performance
                // TODO: Check num != -1
                const int num = epoll_wait(epollFd, epollEvents.data(), kMaxEvents, kEpollWaitTime);

                for (int i = 0; i < num; ++i)
                {   // TODO: Refactor
                    clientSock = epollEvents[i].data.fd;
                    events = epollEvents[i].events;

                    if ((events & EPOLLERR) || (events & EPOLLHUP))
                    {   // TODO: handle EPOLL_CTL_DEL
                        debug("Closing connection. Socket = ", clientSock, "[epoll_wait error]");
                        ::close(clientSock);

                        if (SOCKET_ERROR == epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSock, nullptr)) {
                            Error("***** ERROR *****: epoll_ctl() failed. (EPOLL_CTL_DEL)");
                        }
                    }
                    else if (events & EPOLLRDHUP)
                    {  // TODO: handle EPOLL_CTL_DEL
                        debug("Closing connection. Socket = ", clientSock);
                        ::close(clientSock);

                        if (SOCKET_ERROR == epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSock, nullptr)) {
                            Error("***** ERROR *****: epoll_ctl() failed. (EPOLL_CTL_DEL)");
                        }
                    }
                    else if (events & EPOLLIN)
                    {
                        total = 0;
                        message.clear();

                        while ((bytes = ::read(clientSock, buffer.data(), buffer.size())) > 0) {
                            message.append(buffer.data(), bytes);
                            total += bytes;
                        }

                        debug(total, "bytes received: ", message, "| events: ", events);
                        if (0 != bytes)
                        {
                            reply.assign("Reply:" + message);
                            bytes = ::send(clientSock, reply.data(), reply.length(), 0);
                            debug(bytes, "bytes send");
                        }
                    }
                    else if (events & EPOLLOUT)
                    {   // TODO: handle EPOLL_CTL_DEL
                        debug("Socket(", clientSock, ") is valid for writing");
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
                if (SOCKET_ERROR == setEpollEvents(epollFd, EPOLL_CTL_ADD, clientSocket, EPOLLIN | EPOLLRDHUP | EPOLLET)) {
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

void EPollTCPServerDebug::Tests()
{
    // using namespace Old;
    using namespace New;

    startSerer();
};
