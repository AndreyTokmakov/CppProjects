/**============================================================================
Name        : UnixSocketServer.cpp
Created on  : 28.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : UnixSocketServer.cpp
============================================================================**/

#include "UnixSocketServer.h"

#include <unistd.h>
#include <cerrno>
#include <sys/socket.h>
#include <sys/un.h>

#include <iostream>
#include <string_view>
#include <thread>
#include <chrono>

#define RESULT_SUCCESS   (0)
#define INVALID_HANDLE  (-1)
#define SERVER_SOCK_PATH "/tmp/unix_socket"

namespace
{
    using Socket = int32_t;

    struct SocketGuard
    {
        using HandleType = int32_t;
        HandleType socketHandle { INVALID_HANDLE };

        explicit SocketGuard(HandleType sock): socketHandle { sock } {
        }

        ~SocketGuard()
        {
            if (INVALID_HANDLE != socketHandle)
            {
                // TODO: Check result
                [[maybe_unused]] const int32_t result = ::close(socketHandle);
                // std::cout << "Socket " << socketHandle << " is closed\n";
            }
        }

        SocketGuard(const SocketGuard&) = delete;
        SocketGuard(SocketGuard&&) noexcept = delete;

        SocketGuard& operator=(const SocketGuard&) = delete;
        SocketGuard& operator=(SocketGuard&&) noexcept = delete;
    };
}


namespace UnixSocketServer
{
    struct UDSAsynchServer
    {
        Socket serverSocket { INVALID_HANDLE };
        std::string filePath { SERVER_SOCK_PATH };

        UDSAsynchServer()
        {
            serverSocket = ::socket(AF_UNIX, SOCK_STREAM, 0);
            if (INVALID_HANDLE == serverSocket) {
                throw std::runtime_error("CLIENT: Create socket failed.");
            }

            if (const int32_t result = ::unlink(filePath.data()); INVALID_HANDLE == result)
            {
                if( const int error = errno;  ENOENT != error && RESULT_SUCCESS != error) {
                    throw std::runtime_error("Failed to unlink " + filePath + " file");
                }
            }
        }

        ~UDSAsynchServer()
        {
            if (const int32_t result = ::remove(filePath.data());
                    INVALID_HANDLE == result)
            {
                std::cerr << "remove() failed. Result = " << result << std::endl;
            }

            if (const int32_t result = ::close(serverSocket); INVALID_HANDLE == result)
            {
                std::cerr << "close() failed. Result = " << result << std::endl;
            }
        }

        bool bind()
        {
            const sockaddr_un serverAddr { .sun_family = AF_UNIX, .sun_path = SERVER_SOCK_PATH };
            uint32_t len = sizeof(serverAddr);
            if (RESULT_SUCCESS != ::bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddr), len)) {
                std::cerr << "SERVER: Server bind() error"<< std::endl;
                return false;
            }
            if (RESULT_SUCCESS != ::listen(serverSocket, 10)) {
                std::cerr << "SERVER: Server listen() failed" << std::endl;
                return false;
            }
            return true;
        }

        void start()
        {
            std::cout << "SERVER: Socket listening..." << std::endl;

            sockaddr clientAddr {};
            uint32_t len = sizeof(clientAddr);
            const int clientSocket = ::accept(serverSocket, &clientAddr, (socklen_t*)&len);
            if (INVALID_HANDLE == clientSocket) {
                std::cerr << "SERVER: accept() failed" << std::endl;

            } else {
                std::cout << "SERVER: Connection request" << std::endl;
            }
            SocketGuard clientSocketGuard { clientSocket };

            std::string buffer(1024, '\0');
            const int64_t bytesReceived = ::recv(clientSocket, buffer.data(), buffer.size(), 0);
            if (INVALID_HANDLE == bytesReceived) {
                std::cerr << "SERVER: Error when receiving message() failed" << std::endl;
            } else {
                buffer.resize(bytesReceived);
                std::cout << "SERVER: <== Data received: " << buffer << std::endl;
            }

            std::cout << "SERVER: ==> Sending the response..." << std::endl;
            constexpr std::string_view message { "HELLO FROM SERVER" };
            // const int64_t bytesSend = ::send(clientSocket, message.data(), message.size(), 0);
            const int64_t bytesSend = ::write(clientSocket, message.data(), message.size());
            if (INVALID_HANDLE == bytesSend) {
                // error("SERVER: Error when sending message to client");
            }
        }
    };
}

void UnixSocketServer::TestAll()
{
    UDSAsynchServer server;
    if (!server.bind()) {
        std::cerr << "Bind failed" << std::endl;
    }

    server.start();
}