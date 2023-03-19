/**============================================================================
Name        : SocketTCPServer.cpp
Created on  : 18.03.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SocketTCPServer
============================================================================**/

#include "SocketTCPServer.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)

namespace SocketTCPServer
{

    struct SocketGuard {
        int sock { INVALID_SOCKET };

        explicit SocketGuard(int s): sock {s} {
        }

        ~SocketGuard() {
            std::cout << "Closing socket (" << sock  << ")\n";
            ::close(sock);
        }
    };

    int Error(std::string_view text)
    {
        std::cout << text << ". Error = " << errno << std::endl;
        return errno;
    }


    void runServer1() {
        const int socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == socket) {
            Error("Failed to create socket");
        }

        SocketGuard guard {socket};

        constexpr uint16_t port{52525};
        constexpr std::string_view host{"0.0.0.0"};
        sockaddr_in server {PF_INET, htons(port), {.s_addr = inet_addr(host.data())}, {}};

        if (SOCKET_ERROR == ::bind(socket, reinterpret_cast<sockaddr*>(&server), sizeof(server))) {
            Error("Failed to bind socket");
        }

        constexpr uint16_t backlog{10};
        if (SOCKET_ERROR == ::listen(socket, backlog)) {
            Error("Failed to Listen the socket.");
        }

        sockaddr_in clientAddr{};
        socklen_t addLen { sizeof(clientAddr) };
        int clientSocket { INVALID_SOCKET };
        while (true)
        {
            std::cout << "Waiting for next connection ....\n";
            clientSocket = ::accept(socket, reinterpret_cast<sockaddr*>(&clientAddr), &addLen);
            if (INVALID_SOCKET == clientSocket) {
                Error("Failed to create client socket");
            }

            std::cout << "Client connected " << inet_ntoa(clientAddr.sin_addr) << ':' << htons(clientAddr.sin_port) << std::endl;

        }
    }
};

void SocketTCPServer::Tests()
{
    runServer1();

};
