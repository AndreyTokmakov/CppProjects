/**============================================================================
Name        : UnixSockets.cpp
Created on  : 01.09.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : UnixSockets.cpp
============================================================================**/

#include "UnixSockets.h"

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

#define SERVER_SOCK_PATH "unix_socket"

namespace UnixSockets
{
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

    struct SocketFileGuard
    {
        std::string fileName;

        explicit SocketFileGuard(std::string file): fileName { std::move(file) } {
            // TODO: Check result
            [[maybe_unused]] const int32_t result = ::unlink(fileName.data());
            // std::cout << "File " << fileName << " is unlinked\n";
        }

        ~SocketFileGuard()
        {
            if (!fileName.empty())
            {
                // TODO: Check result
                [[maybe_unused]] const int32_t result = ::remove(fileName.data());
                // std::cout << "File " << fileName << " is remove\n";
            }
        }

        SocketFileGuard(const SocketFileGuard&) = delete;
        SocketFileGuard(SocketFileGuard&&) noexcept = delete;

        SocketFileGuard& operator=(const SocketFileGuard&) = delete;
        SocketFileGuard& operator=(SocketFileGuard&&) noexcept = delete;
    };

    int error(std::string_view description)
    {
        const auto errorCode = errno;
        std::cout << description << ". Error code = " << errorCode << ", Error text: " <<  strerror(errorCode) << '\n';
        return errno;
    }

    int clientProcess()
    {
        const int clientSocket = ::socket(AF_UNIX, SOCK_STREAM, 0);
        if (INVALID_HANDLE == clientSocket) {
            return error("CLIENT: Create socket failed");
        }

        SocketGuard clientSocketGuard { clientSocket };
        // SocketFileGuard clientFileGuard { SERVER_SOCK_PATH };

        const sockaddr_un serverAddr { .sun_family = AF_UNIX, .sun_path = SERVER_SOCK_PATH };
        uint32_t len = sizeof(serverAddr);
        if (RESULT_SUCCESS != ::connect(clientSocket, reinterpret_cast<const sockaddr*>(&serverAddr), len)) {
            return error("CLIENT: Connect error");
        }

        std::cout << "CLIENT: ==> Sending message to server.." << std::endl;
        constexpr std::string_view message { "HELLO FROM CLIENT" };
        // const int64_t bytesSend = ::send(clientSocket, message.data(), message.size(), 0);
        const int64_t bytesSend = ::write(clientSocket, message.data(), message.size());
        if (INVALID_HANDLE == bytesSend) {
            return error("CLIENT: Error when sending message to server");
        }

        std::string buffer(1024, '\0');
        const int64_t bytesReceived = ::recv(clientSocket, buffer.data(), buffer.size(), 0);
        if (INVALID_HANDLE == bytesReceived) {
            return error("CLIENT: Error when receiving message() failed");
        } else {
            buffer.resize(bytesReceived);
            std::cout << "CLIENT: <== Data received: " << buffer << std::endl;
        }

        return 0;
    }

    int serverProcess()
    {
        const int serverSocket = ::socket(AF_UNIX, SOCK_STREAM, 0);
        if (INVALID_HANDLE == serverSocket) {
            return error("SERVER: Create socket failed");
        }

        SocketGuard serverSocketGuard { serverSocket };
        SocketFileGuard serverFileGuard { SERVER_SOCK_PATH };

        const sockaddr_un serverAddr { .sun_family = AF_UNIX, .sun_path = SERVER_SOCK_PATH };
        uint32_t len = sizeof(serverAddr);
        if (RESULT_SUCCESS != ::bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddr), len)) {
            return error("SERVER: Server bind() error");
        }
        if (RESULT_SUCCESS != ::listen(serverSocket, 10)) {
            return error("SERVER: listen() failed");
        }

        std::cout << "SERVER: Socket listening..." << std::endl;
        sockaddr clientAddr {};
        const int clientSocket = ::accept(serverSocket, &clientAddr, (socklen_t*)&len);
        if (INVALID_HANDLE == clientSocket) {
            return error("SERVER: accept() failed");
        } else {
            std::cout << "SERVER: Connection request" << std::endl;
        }
        SocketGuard clientSocketGuard { clientSocket };

        std::string buffer(1024, '\0');
        const int64_t bytesReceived = ::recv(clientSocket, buffer.data(), buffer.size(), 0);
        if (INVALID_HANDLE == bytesReceived) {
            return error("SERVER: Error when receiving message() failed");
        } else {
            buffer.resize(bytesReceived);
            std::cout << "SERVER: <== Data received: " << buffer << std::endl;
        }

        std::cout << "SERVER: ==> Sending the response..." << std::endl;
        constexpr std::string_view message { "HELLO FROM SERVER" };
        // const int64_t bytesSend = ::send(clientSocket, message.data(), message.size(), 0);
        const int64_t bytesSend = ::write(clientSocket, message.data(), message.size());
        if (INVALID_HANDLE == bytesSend) {
            error("SERVER: Error when sending message to client");
        }

        return 0;
    }

    void MultiProcessTest()
    {
        if (const pid_t pid = fork(); pid == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds (10));
            clientProcess();
        } else if (pid > 0) {
            serverProcess();
        }
    }
}

void UnixSockets::TestAll()
{
    MultiProcessTest();

    // serverProcess();
}