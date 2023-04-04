/**============================================================================
Name        : EPollTCPServerDebug.cpp
Created on  : 03.04.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : EPollTCPServerDebug
============================================================================**/

#include "EPollTCPServerMultithreaded.h"


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
#include <deque>
#include <condition_variable>


namespace EPollTCPServerMultithreaded::Utilities
{
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


namespace EPollTCPServerMultithreaded
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

        using handleType = int32_t;

        handleType epollFd { INVALID_SOCKET };
        handleType serverSocket { INVALID_SOCKET };

        std::string hostAddress;
        uint16_t listenPort {};

        /**------------------------------------------------------------------------------------------**/

        mutable std::mutex mutex;
        std::deque<handleType> queue;
        std::condition_variable gotClientRequest;
        std::atomic_bool run { true };
        std::vector<std::jthread> workers {};

        /** Maximum number of request handler workers: **/
        static inline const size_t THREADS_COUNT { std::thread::hardware_concurrency() };

        static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> TIMEOUT {
                std::chrono::milliseconds(2000)
        };

        template<class Rep, class Period>
        bool wait_for_and_pop(handleType &task,
                              const std::chrono::duration<Rep, Period> &timeout) noexcept {
            {
                std::unique_lock<std::mutex> lock(mutex);
                if (!gotClientRequest.wait_for(lock, timeout, [this] { return !queue.empty(); }))
                    return false;
                task = queue.front();
                queue.pop_front();
            }
            gotClientRequest.notify_all();
            return true;
        }

        // TODO: Rename
        void worker_thread() {
            // TODO: Move 'handleClientRequest' here ?
            handleType obj { -1 };
            while (run) {
                if (auto result = wait_for_and_pop(obj, TIMEOUT); result) {
                    handleClientRequest(obj);
                }
            }
        }

        // TODO: Rename
        void submit(handleType new_value) noexcept {
            {
                std::lock_guard<std::mutex> lock(mutex);
                queue.push_back(new_value);
            }
            gotClientRequest.notify_one();
        }

        /**------------------------------------------------------------------------------------------**/


        static handleType Error(std::string_view text)
        {
            std::cerr << text << ". Error = " << errno << std::endl;
            return SOCKET_ERROR;
        }

        static handleType setNonBlock(handleType handle)
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

        // TODO: Rename to subscribe | return True/False??
        [[nodiscard]]
        handleType addEpollEvent(handleType handle, uint32_t events) const
        {
            epoll_event event { events, {.fd = handle} };
            if (SOCKET_ERROR == ::epoll_ctl(epollFd, EPOLL_CTL_ADD, handle, &event)) {
                return Error("epoll_ctl(EPOLL_CTL_ADD) failed");
            }
            return 0;
        }

        // TODO: return True/False?
        [[nodiscard]]
        handleType closeClientConnection(handleType handle) const
        {
            if (SOCKET_ERROR == ::epoll_ctl(epollFd, EPOLL_CTL_DEL, handle, nullptr)) {
                return Error("epoll_ctl(EPOLL_CTL_DEL) failed");
            }
            if (SOCKET_ERROR == ::close(handle)) {
                return Error("close failed");
            }
            return 0;
        }


        // TODO: return True/False?
        [[nodiscard]]
        handleType handleClientRequest(handleType clientSock) const
        {
            std::array<char, BUFFER_SIZE> buffer {};
            ssize_t bytes {0}, total {0};
            std::string message, reply;

            while ((bytes = ::read(clientSock, buffer.data(), buffer.size())) > 0) {
                message.append(buffer.data(), bytes);
                total += bytes;
            }

            debug(total, "bytes received:", message);

            if (0 != bytes)
            {
                reply.assign("Reply:" + message);
                bytes = ::send(clientSock, reply.data(), reply.length(), 0);
                debug(bytes, "bytes send");
            }

            return 0;
        }

        void eventsPoller()
        {
            std::array<epoll_event, kMaxEvents> epollEvents {};
            auto [clientSock, events] = std::make_pair<int32_t, uint32_t>(0,0);

            while (true)
            {   // TODO: Check TimeOut for performance
                // TODO: Check num != -1
                const int num = epoll_wait(epollFd, epollEvents.data(), kMaxEvents, kEpollWaitTime);

                for (int i = 0; i < num; ++i) // TODO: Refactor
                {
                    clientSock = epollEvents[i].data.fd;
                    events = epollEvents[i].events;

                    if ((events & EPOLLERR) || (events & EPOLLHUP)) {
                        debug("Closing connection. Socket = ", clientSock, "[epoll_wait error]");
                        closeClientConnection(clientSock);
                    }
                    else if (events & EPOLLRDHUP) {
                        debug("Closing connection. Socket = ", clientSock);
                        closeClientConnection(clientSock);
                    }
                    else if (events & EPOLLIN) {
                        // handleClientRequest(clientSock);
                        submit(clientSock);
                    }
                    else if (events & EPOLLOUT) {
                        debug("Socket(", clientSock, ") is valid for writing");
                    }
                }
            }
        }

    public:

        TCPServer(std::string address, uint16_t port):
                hostAddress { std::move(address) }, listenPort {port} {
            try {
                for (size_t i = 0; i < THREADS_COUNT; ++i) {
                    workers.emplace_back(&TCPServer::worker_thread, this);
                }
            } catch (...) {
                run = false;
                throw;
            }
        }

        ~TCPServer() {
            run = false;
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
            handleType clientSocket { INVALID_SOCKET };
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
                if (SOCKET_ERROR == addEpollEvent(clientSocket, EPOLLIN | EPOLLRDHUP | EPOLLET)) {
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
};

void EPollTCPServerMultithreaded::Tests()
{
    startSerer();
};
