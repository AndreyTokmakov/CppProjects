/**============================================================================
Name        : HTTPS_AsyncServer.cpp
Created on  : 28.08.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : HTTPS_AsyncServer
============================================================================**/

#include "HTTPS_AsyncServer.h"

#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <format>

#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <functional>
#include <utility>
#include <deque>
#include <condition_variable>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/crypto.h>
#include <openssl/ssl.h>


namespace
{
    template<typename T>
    auto addSpace(const T& arg) -> decltype(auto) {
        std::cout << ' ';
        return arg;
    };

    template<typename ...Args>
    void debug([[maybe_unused]] Args&&... args) {
#if 1
        std::cout << "DEBUG ";
        (std::cout << ... << addSpace(std::forward<Args>(args))) << std::endl;
#endif
    }

    constexpr std::string_view certFile {
            R"(../../Networking/SSL/data/server.pem)"
    };

    constexpr std::string_view keyFile {
            R"(../../Networking/SSL/data/key.pem)"
    };

    constexpr int INVALID_SOCKET { -1 };
    constexpr int SOCKET_ERROR { -1 };

    struct SocketGuard final
    {
        int sock { INVALID_SOCKET };

        explicit SocketGuard(int s): sock {s} { }

        ~SocketGuard() {
            std::cout << "Socket " << sock << " closed\n";
            ::close(sock);
        }

        SocketGuard(const SocketGuard&) = delete;
        SocketGuard(SocketGuard&&) noexcept = delete;

        SocketGuard& operator=(const SocketGuard&) = delete;
        SocketGuard& operator=(SocketGuard&&) noexcept = delete;
    };

    struct CertificateDeleter {
        void operator()(BIO* bio) const {
            if (bio)
                ::BIO_free(bio);
        }

        void operator()(X509* x509) const {
            if (x509)
                ::X509_free(x509);
        }
    };

    [[maybe_unused]]
    int Error(std::string_view text)
    {
        std::cerr << text << ". Error = " << errno << std::endl;
        return errno;
    }

    [[maybe_unused]]
    int PutSSLError(std::string_view func_name)
    {
        std::cerr << func_name << "() failed. Error = " << errno << std::endl;
        ERR_print_errors_fp(stderr);
        return errno;
    }

    [[maybe_unused]]
    int PutSSLError(std::string_view func_name, int result)
    {
        std::cerr << func_name << "() failed" << "Result = " << result << ". Error = " << errno << std::endl;
        ERR_print_errors_fp(stderr);
        return errno;
    }

    [[maybe_unused]]
    void printPeerCertificateInfo(SSL* ssl)
    {
        const std::unique_ptr<X509, CertificateDeleter> certX509 {
                ::SSL_get_peer_certificate(ssl), CertificateDeleter {}
        };
        if (!certX509)
        {
            std::cout << "* * * * * Failed to get certificate * * * * * \n";
            /*
            std::unique_ptr<BIO, CertificateDeleter> bio {
                    ::BIO_new_mem_buf(content.data(), content.size()), CertificateDeleter{} };
            if (!bio) {
                std::cout << "BIO_new_mem_buf() failed" << std::endl;
                return;
            }

            certX509.reset(::PEM_read_bio_X509(bio.get(), nullptr, nullptr, nullptr));
            if (!certX509) {
                std::cout << "PEM_read_bio_X509() failed" << std::endl;
                return;
            }
            */
        }
        else
        {
            std::cout << "Version: " << X509_get_version(certX509.get()) +1 << std::endl;
        }
    }
}


namespace HTTPS_AsyncServer
{
    constexpr std::string_view pageHTML {
            "<html><head><title>AndTokmServer</title></head><body BGCOLOR='grey'>Welcome</body></html>"
    };

    const std::string response = std::string { "HTTP/1.1 200 OK\r\n"}
            .append("Date: Wed, 11 Feb 2009 11:20:59 GMT\r\n")
            .append("Server: AndTokmServer\r\n")
            .append("X-Powered-By: PHP/5.2.4-2ubuntu5wm1\r\n")
            .append("Last-Modified: Wed, 11 Feb 2009 11:20:59 GMT\r\n")
            .append("Content-Language: ru\r\n")
            .append("Content-Type: text/html; charset=utf-8\r\n")
            .append("Content-Length: " + std::to_string(pageHTML.length()) + "\r\n")
            .append("Connection: close\r\n\r\n")
            .append(pageHTML)
            .append("\r\n");


    class TCPServer
    {
        static inline constexpr int32_t  INVALID_SOCKET { -1 };
        static inline constexpr int32_t  SOCKET_ERROR { -1 };
        static inline constexpr uint32_t BACKLOG { 10 };
        static inline constexpr size_t   BUFFER_SIZE { 1024 * 4 };

        // TODO: Choose different value?
        static inline constexpr uint32_t EPOLL_WAIT_TIMEOUT { 10 };  // epoll wait timeout 10 ms
        // TODO: Refactor ?
        static inline constexpr uint32_t EVENTS_MAX { 1024 };    // epoll wait return max size

        /** Maximum number of request handler workers: **/
        static inline const size_t THREADS_COUNT { std::thread::hardware_concurrency() };

        static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> QUEUE_TIMEOUT {
                std::chrono::milliseconds(2000)
        };

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


        template<class Rep, class Period>
        bool getClientRequestHandle(handleType &task,
                                    const std::chrono::duration<Rep, Period> &timeout) noexcept {
            std::unique_lock<std::mutex> lock { mutex };
            if (!gotClientRequest.wait_for(lock, timeout, [this] { return !queue.empty(); }))
                return false;
            task = queue.front();
            queue.pop_front();
            lock.unlock();
            gotClientRequest.notify_all();
            return true;
        }

        [[noreturn]]
        void processRequest()
        {
            std::array<char, BUFFER_SIZE> buffer {};
            ssize_t bytes {0}, total {0};
            std::string message;
            handleType clientSock { INVALID_SOCKET };

            while (run)
            {
                if (bool result = getClientRequestHandle(clientSock, QUEUE_TIMEOUT); result)
                {
                    message.clear();
                    total = 0;

                    while ((bytes = ::read(clientSock, buffer.data(), buffer.size())) > 0) {
                        message.append(buffer.data(), bytes);
                        total += bytes;
                    }

                    debug(total, "bytes received:", message, "Tid:", std::this_thread::get_id());

                    if (0 != bytes) {
                        bytes = ::send(clientSock, response.data(), response.length(), 0);
                        debug(bytes, "bytes send");
                    }
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

        void eventsPoller()
        {
            std::array<epoll_event, EVENTS_MAX> epollEvents {};
            auto [clientSock, events] = std::make_pair<handleType, uint32_t>(0,0);

            while (true)
            {   // TODO: Check TimeOut for performance
                // TODO: Check num != -1
                const int num = epoll_wait(epollFd, epollEvents.data(), EVENTS_MAX, EPOLL_WAIT_TIMEOUT);

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
                    workers.emplace_back(&TCPServer::processRequest, this);
                }
            } catch (...) {
                run = false;
                throw;
            }
        }

        ~TCPServer() {
            run = false;
        }

        [[nodiscard("Do not forget to check result")]]
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

            std::cout << std::format("Running on http://{}:{}", hostAddress, listenPort) << std::endl;

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

void HTTPS_AsyncServer::TestAll()
{
    startSerer();
};
