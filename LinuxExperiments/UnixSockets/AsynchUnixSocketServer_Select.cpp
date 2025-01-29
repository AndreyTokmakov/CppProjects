/**============================================================================
Name        : AsynchUnixSocketServer_Select.cpp
Created on  : 28.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AsynchUnixSocketServer_Select.cpp
============================================================================**/

#include "AsynchUnixSocketServer_Select.h"
#include <unistd.h>
#include <cerrno>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#include <iostream>
#include <string_view>
#include <thread>
#include <chrono>

#define RESULT_SUCCESS   (0)
#define INVALID_HANDLE   (-1)
#define SOCKET_ERROR     (-1)
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


namespace AsynchUnixSocketServer_Select
{
    struct UDSAsynchServer
    {
        Socket serverSocket { INVALID_HANDLE };
        std::string filePath { SERVER_SOCK_PATH };

        std::vector<Socket> clients;

        // TODO: --> std::chrono
        timeval timeout {};

        std::array<char, 1024> buffer {};
        std::string request;

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
                INVALID_HANDLE == result) {
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


        // TODO: Rename
        bool setListenerOptions() const
        {
            if (SOCKET_ERROR == ::fcntl(serverSocket, F_SETFL, O_NONBLOCK)) {
                std::cerr << "FCNTL failed. Error = " << errno << std::endl;
                return false;
            }

            int32_t yes { 1 };
            if (SOCKET_ERROR == ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
                std::cout << "Setsockopt failed. Error = " << errno << std::endl;
                return false;
            }
            return true;
        }

        bool waitEvent(fd_set *fdSet)
        {
            const Socket maxClient = clients.empty() ? 0 : *max_element(clients.cbegin(), clients.cend());
            const Socket mx = std::max(serverSocket, maxClient);

            timeout.tv_sec = 3;

            if (SOCKET_ERROR == ::select(mx + 1, fdSet, nullptr, nullptr, &timeout)) {
                std::cout << "Select failed. Error = " << errno << std::endl;
                return false;
            }
            return true;
        }

        bool handleConnection(const fd_set *readset)
        {
            if (FD_ISSET(this->serverSocket, readset))
            {
                std::cout << __FUNCTION__ << std::endl;

                Socket clientSocket = ::accept(serverSocket, nullptr, nullptr);
                if (INVALID_HANDLE == clientSocket) {
                    std::cerr << "Failed to accept client connection. Error = " << errno << std::endl;
                    return false;
                }
                fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                clients.push_back(clientSocket);
                std::cout << "Client connection accepted (Clients in table " << clients.size() << ")" << std::endl;
            }
            return true;
        }

        bool handleRead(const fd_set *readset)
        {
            std::cout << __FUNCTION__  << std::endl;
            for (Socket clientSocket: clients)
            {
                if (clients.empty()) {
                    std::cout << "END! Break" << std::endl;
                    break;
                }
                if (FD_ISSET(clientSocket, readset))
                {
                    std::cout << __FUNCTION__  << std::endl;
                    const int64_t bytesRead = ::recv(clientSocket, buffer.data(), buffer.size(), 0);
                    if (bytesRead <= 0)
                    {
                        std::erase_if(clients, [&](const auto sock) {
                            return clientSocket == sock;
                        });
                        ::close(clientSocket);
                        std::cout << "Connection closed (Clients in table " << clients.size() << ")" << std::endl;
                        continue;
                    }
                    /*** Handle client request: ***/
                    request.assign(buffer.data(), bytesRead);
                    std::cout << request << std::endl;

                    /*** Send response: ***/
                    // ::send(clientSocket, const_cast<char *>(response.c_str()), response.length(), 0);
                }
            }
            return true;
        }

        void start()
        {
            std::cout << "SERVER: Socket listening..." << std::endl;
            while (true) {
                fd_set readset{};
                FD_ZERO(&readset);
                FD_SET (serverSocket, &readset);

                for (const Socket client: clients)
                    FD_SET(client, &readset);
                if (!waitEvent(&readset))
                    break;
                handleConnection(&readset);
                handleRead(&readset);
            }
        }
    };
}

void AsynchUnixSocketServer_Select::TestAll()
{
    UDSAsynchServer server;
    if (!server.setListenerOptions()) {
        std::cerr << "setListenerOptions failed" << std::endl;
    }
    if (!server.bind()) {
        std::cerr << "Bind failed" << std::endl;
    }

    server.start();
}