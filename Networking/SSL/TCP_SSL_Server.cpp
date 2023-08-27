/**============================================================================
Name        : TCP_SSL_Server.cpp
Created on  : 27.08.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TCP_SSL_Server.c
============================================================================**/

#include "TCP_SSL_Server.h"

#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/buffer.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

#include <openssl/ssl.h>

#include <iostream>
#include <string_view>
#include <memory>
#include <vector>
#include <array>
#include <fstream>

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
}

namespace TCP_SSL_Server_OK
{
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

    int Error(std::string_view text)
    {
        std::cerr << text << ". Error = " << errno << std::endl;
        return errno;
    }

    void PutSSLError(std::string_view func_name)
    {
        std::cerr << func_name << "() failed. Error = " << errno << std::endl;
        ERR_print_errors_fp(stderr);
    }

    void PutSSLError(std::string_view func_name, int result)
    {
        std::cerr << func_name << "() failed" << "Result = " << result << ". Error = " << errno << std::endl;
        ERR_print_errors_fp(stderr);
    }


    void runServer()
    {
        const int serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == serverSocket) {
            Error("Failed to create socket");
        }

        const SocketGuard guard { serverSocket };

        {
            SSL_library_init();
            SSL_load_error_strings();
            OpenSSL_add_ssl_algorithms();
        }

        const SSL_METHOD *method = SSLv23_server_method();
        std::unique_ptr<SSL_CTX, decltype(&::SSL_CTX_free)> ctx {SSL_CTX_new(method), SSL_CTX_free };
        if (!ctx) {
            PutSSLError("SSL_CTX_new");
            return;
        }

        {
            if (1 == SSL_CTX_use_certificate_file(ctx.get(), certFile.data(), SSL_FILETYPE_PEM)) {
                std::cout << "CERTIFICATE INITIALISED" << std::endl;
            }
            if (1 == SSL_CTX_use_PrivateKey_file(ctx.get(), keyFile.data(), SSL_FILETYPE_PEM)) {
                std::cout << "KEY INITIALISED" << std::endl;
            }
            if (1 == SSL_CTX_check_private_key(ctx.get())) {
                std::cout << "KEY VALIDATED" << std::endl;
            }
        }

        constexpr uint16_t port {52525};
        constexpr std::string_view host {"0.0.0.0"};
        sockaddr_in server {PF_INET, htons(port), {.s_addr = inet_addr(host.data())}, {}};

        if (SOCKET_ERROR == ::bind(serverSocket, reinterpret_cast<sockaddr*>(&server), sizeof(server))) {
            Error("Failed to bind socket");
        }

        constexpr uint16_t backlog { 10 };
        if (SOCKET_ERROR == ::listen(serverSocket, backlog)) {
            Error("Failed to Listen the socket.");
        }

        constexpr size_t bytesToRead {44}; // FIXME
        sockaddr_in clientAddr {};
        socklen_t addLen { sizeof(clientAddr) };
        int clientSocket { INVALID_SOCKET };
        std::array<char, 1024> buffer {};

        while (true)
        {
            std::cout << "\nWaiting for next connection ....\n";
            clientSocket = ::accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addLen);
            if (INVALID_SOCKET == clientSocket) {
                Error("Failed to create client socket");
                continue;
            }

            const SocketGuard clientSockGuard {clientSocket };
            std::cout << "Client connected " << inet_ntoa(clientAddr.sin_addr) << ':' << htons(clientAddr.sin_port) << std::endl;

            std::unique_ptr<SSL, decltype(&::SSL_free)> ssl {SSL_new(ctx.get()), SSL_free };
            if (!ssl) {
                PutSSLError("SSL_new");
                return;
            }

            if (const int result = SSL_set_fd(ssl.get(), clientSocket); 1 != result)
            {
                PutSSLError("SSL_set_fd", result);
                continue;
            }
            else {
                std::cerr << "OK. SSL_set_fd() = " << result << std::endl;
            }

            if (const int result = SSL_accept(ssl.get()); 1 != result)
            {
                PutSSLError("SSL_accept", result);
                continue;
            }
            else {
                std::cerr << "OK. SSL_accept() = " << result << std::endl;
            }

            if (const int bytesRead = SSL_read(ssl.get(), buffer.data(), bytesToRead); bytesToRead != bytesRead)
            {
                std::cout << std::string_view {buffer.data(), bytesToRead} << std::endl;
            }
            else {
                PutSSLError("SSL_read", bytesRead);
            }

            // SSL_shutdown(ssl.get());
        }
    }
};


namespace TCP_SSL_DebugServer
{
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

    int runServer()
    {
        const int serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == serverSocket) {
            Error("Failed to create socket");
        }

        const SocketGuard guard { serverSocket };

        {
            SSL_library_init();
            SSL_load_error_strings();
            OpenSSL_add_ssl_algorithms();
        }

        const SSL_METHOD *method = SSLv23_server_method();
        std::unique_ptr<SSL_CTX, decltype(&::SSL_CTX_free)> ctx {SSL_CTX_new(method), SSL_CTX_free };
        if (!ctx) {
            return PutSSLError("SSL_CTX_new");
        }

        {
            if (1 == SSL_CTX_use_certificate_file(ctx.get(), certFile.data(), SSL_FILETYPE_PEM)) {
                std::cout << "CERTIFICATE INITIALISED" << std::endl;
            }
            if (1 == SSL_CTX_use_PrivateKey_file(ctx.get(), keyFile.data(), SSL_FILETYPE_PEM)) {
                std::cout << "KEY INITIALISED" << std::endl;
            }
            if (1 == SSL_CTX_check_private_key(ctx.get())) {
                std::cout << "KEY VALIDATED" << std::endl;
            }
        }

        constexpr uint16_t port {52525};
        constexpr std::string_view host {"0.0.0.0"};
        sockaddr_in server {PF_INET, htons(port), {.s_addr = inet_addr(host.data())}, {}};

        if (SOCKET_ERROR == ::bind(serverSocket, reinterpret_cast<sockaddr*>(&server), sizeof(server))) {
            return Error("Failed to bind socket");
        }

        constexpr uint16_t backlog { 10 };
        if (SOCKET_ERROR == ::listen(serverSocket, backlog)) {
            return Error("Failed to Listen the socket.");
        }

        constexpr size_t bytesToRead {44}; // FIXME
        sockaddr_in clientAddr {};
        socklen_t addLen { sizeof(clientAddr) };
        std::array<char, 1024> buffer {};

        while (true)
        {
            std::cout << "\nWaiting for next connection ....\n";
            const int clientSocket = ::accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addLen);
            if (INVALID_SOCKET == clientSocket) {
                Error("Failed to create client socket");
                continue;
            }

            const SocketGuard clientSockGuard {clientSocket };
            std::cout << "Client connected " << inet_ntoa(clientAddr.sin_addr) << ':' << htons(clientAddr.sin_port) << std::endl;

            std::unique_ptr<SSL, decltype(&::SSL_free)> ssl {SSL_new(ctx.get()), SSL_free };
            if (!ssl) {
                PutSSLError("SSL_new");
                continue;
            }

            if (const int result = SSL_set_fd(ssl.get(), clientSocket); 1 != result)
            {
                PutSSLError("SSL_set_fd", result);
                continue;
            }
            else {
                std::cerr << "OK. SSL_set_fd() = " << result << std::endl;
            }

            if (const int result = SSL_accept(ssl.get()); 1 != result)
            {
                PutSSLError("SSL_accept", result);
                continue;
            }
            else {
                std::cerr << "OK. SSL_accept() = " << result << std::endl;
            }

            if (const int bytesRead = SSL_read(ssl.get(), buffer.data(), bytesToRead); bytesToRead != bytesRead)
            {
                std::cout << std::string_view {buffer.data(), bytesToRead} << std::endl;
            }
            else {
                PutSSLError("SSL_read", bytesRead);
            }

            SSL_shutdown(ssl.get());
        }
    }
};

void TCP_SSL_Server::TestAll()
{
    // TCP_SSL_Server_OK::runServer();
    TCP_SSL_DebugServer::runServer();
};