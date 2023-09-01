/**============================================================================
Name        : HTTPS_Server_ThreadPool.cpp
Created on  : 01.09.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : HTTPS_Server_ThreadPool
============================================================================**/

#include "HTTPS_Server_ThreadPool.h"

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/crypto.h>
#include <openssl/ssl.h>

#include <iostream>
#include <string_view>
#include <memory>
#include <array>
#include <format>
#include <thread>
#include <deque>
#include <vector>
#include <condition_variable>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

namespace
{
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

    int Error(std::string_view text)
    {
        std::cerr << text << ". Error = " << errno << std::endl;
        return errno;
    }

    int PutSSLError(std::string_view func_name)
    {
        std::cerr << func_name << "() failed. Error = " << errno << std::endl;
        ERR_print_errors_fp(stderr);
        return errno;
    }

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

namespace HTTPS_Server_ThreadPool
{
    struct HTTPS_Server
    {
        static inline constexpr std::string_view pageHTML {
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

        /** SSL Context **/
        std::unique_ptr<SSL_CTX, decltype(&::SSL_CTX_free)> sslContext {
                std::unique_ptr<SSL_CTX, decltype(&::SSL_CTX_free)> {nullptr, SSL_CTX_free }
        };

        mutable std::mutex mutex;
        std::deque<int> sessions;
        std::condition_variable gotClientRequest;
        std::atomic_bool run { true };
        std::vector<std::jthread> workers {};

        template<class Rep, class Period>
        bool getClientRequestHandle(int& clientHandle,
                                    const std::chrono::duration<Rep, Period> &timeout) noexcept
        {
            {
                std::unique_lock<std::mutex> lock {mutex};
                if (!gotClientRequest.wait_for(lock, timeout, [this] { return !sessions.empty(); }))
                    return false;
                clientHandle = sessions.front();
                sessions.pop_front();
            }

            gotClientRequest.notify_all();
            return true;
        }

        [[noreturn]]
        void handleClientSession()
        {
            int clientSocket {INVALID_SOCKET};
            constexpr size_t bytesToRead {44}; // FIXME
            std::array<char, 1024> buffer {};

            while (run)
            {
                if (bool result = getClientRequestHandle(clientSocket, QUEUE_TIMEOUT); result) {

                    const SocketGuard clientSockGuard{clientSocket};
                    std::unique_ptr<SSL, decltype(&::SSL_free)> ssl{SSL_new(sslContext.get()), SSL_free};
                    if (!ssl) {
                        PutSSLError("SSL_new");
                        return;
                    }
                }
            }
        }

        int processClientConnection(int clientSocket)
        {
            const SocketGuard clientSockGuard {clientSocket };
            std::unique_ptr<SSL, decltype(&::SSL_free)> ssl {SSL_new(sslContext.get()), SSL_free };
            if (!ssl) {
                return PutSSLError("SSL_new");
            }

            // const X509* cert = SSL_get_peer_certificate(ssl.get());
            // printPeerCertificateInfo(ssl.get());

            if (const int result = SSL_set_fd(ssl.get(), clientSocket); 1 != result) {
                return PutSSLError("SSL_set_fd", result);
            }
            if (const int result = SSL_accept(ssl.get()); 1 != result) {
                return PutSSLError("SSL_accept", result);
            }

            constexpr size_t bytesToRead {44}; // FIXME
            std::array<char, 1024> buffer {};
            if (const int bytesRead = SSL_read(ssl.get(), buffer.data(), bytesToRead); bytesToRead != bytesRead)
            {
                std::cout << std::string_view {buffer.data(), bytesToRead} << std::endl;
            }
            else {
                PutSSLError("SSL_read", bytesRead);
            }


            if (const int bytesWritten = SSL_write(ssl.get(), response.data(), response.length()); bytesWritten) {
                std::cout << bytesWritten << " bytes send\n";
            }
            else {
                PutSSLError("SSL_read", bytesWritten);
            }

            return SSL_shutdown(ssl.get());
        }

        bool initSSL()
        {
            SSL_library_init();
            SSL_load_error_strings();
            OpenSSL_add_ssl_algorithms();

            const SSL_METHOD *method = SSLv23_server_method();
            sslContext = std::unique_ptr<SSL_CTX, decltype(&::SSL_CTX_free)>{SSL_CTX_new(method), SSL_CTX_free };
            if (!sslContext) {
                PutSSLError("SSL_CTX_new");
                return false;
            }

            if (1 == SSL_CTX_use_certificate_file(sslContext.get(), certFile.data(), SSL_FILETYPE_PEM)) {
                std::cout << "CERTIFICATE INITIALISED" << std::endl;
            } else {
                return false;
            }

            if (1 == SSL_CTX_use_PrivateKey_file(sslContext.get(), keyFile.data(), SSL_FILETYPE_PEM)) {
                std::cout << "KEY INITIALISED" << std::endl;
            } else {
                return false;
            }

            if (1 == SSL_CTX_check_private_key(sslContext.get())) {
                std::cout << "KEY VALIDATED" << std::endl;
            } else {
                return false;
            }

            return true;
        }

        int start()
        {
            if (!initSSL())
                return -1;

            const int serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (INVALID_SOCKET == serverSocket) {
                Error("Failed to create socket");
            }

            const SocketGuard guard { serverSocket };

            constexpr uint16_t port { 52525 };
            constexpr std::string_view host {"0.0.0.0"};
            sockaddr_in server {PF_INET, htons(port), {.s_addr = inet_addr(host.data())}, {}};

            if (SOCKET_ERROR == ::bind(serverSocket, reinterpret_cast<sockaddr*>(&server), sizeof(server))) {
                return Error("Failed to bind socket");
            }

            constexpr uint16_t backlog { 10 };
            if (SOCKET_ERROR == ::listen(serverSocket, backlog)) {
                return Error("Failed to Listen the socket.");
            }

            std::cout << std::format("Running on https://{}:{}", host, port) << std::endl;

            sockaddr_in clientAddr {};
            socklen_t addLen { sizeof(clientAddr) };

            while (true)
            {
                std::cout << "\nWaiting for next connection ....\n";
                const int clientSocket = ::accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addLen);
                if (INVALID_SOCKET == clientSocket) {
                    Error("Failed to create client socket");
                    continue;
                }

                std::cout << "Client connected " << inet_ntoa(clientAddr.sin_addr)
                          << ':' << htons(clientAddr.sin_port) << std::endl;

                std::thread T { &HTTPS_Server::processClientConnection, this, clientSocket};
                T.detach();
            }
        }
    };
};

void HTTPS_Server_ThreadPool::TestAll()
{

};