/**============================================================================
Name        : UDPSockets.cpp
Created on  : 28.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : UDPSockets.cpp
============================================================================**/

#include "IPC_Performance_Experiments.h"

#include <cstdint>
#include <string_view>
#include <array>
#include <chrono>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace
{
    constexpr uint32_t receiveBufferSize { 10 * 1024 };
    constexpr std::string_view host {"0.0.0.0" };
    constexpr uint16_t serverPort { 52525 };

    constexpr int INVALID_SOCKET { -1 };
    constexpr int SOCKET_ERROR { -1 };

    using Socket = int32_t;

    struct SocketGuard
    {
        Socket socket { INVALID_SOCKET };

        ~SocketGuard() {
            if (INVALID_SOCKET != socket) {
                ::close(socket);
            }
        }
    };
}

namespace UDPSockets
{
    void startServer()
    {
        Socket serverSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (INVALID_SOCKET == serverSocket)
            throw std::runtime_error("Failed to create UDP socket");

        SocketGuard sGuard { serverSocket };

        sockaddr_in server {PF_INET, htons(serverPort), {.s_addr = inet_addr(host.data())}, {}};
        if (SOCKET_ERROR == ::bind(serverSocket, reinterpret_cast<sockaddr*>(&server), sizeof(server))) {
            throw std::runtime_error("Failed to bind socket on port " + std::to_string(serverPort));
        }

        std::array<uint8_t, receiveBufferSize> buffer {};
        std::string message {};
        ssize_t bytesReceived { -1 };
        uint32_t counter = 0;
        uint64_t bytesTotal {0};

        std::chrono::high_resolution_clock::time_point start;
        while (1'000'000 > counter)
        {
            if (0 == counter) {
                start = std::chrono::high_resolution_clock::now();
            }

            bytesReceived = ::recvfrom(serverSocket,
                                       buffer.data() ,receiveBufferSize, 0, nullptr, nullptr);
            if (SOCKET_ERROR != bytesReceived)
            {
                message.assign(reinterpret_cast<const char *>(buffer.data()), bytesReceived);
                ++counter;
                bytesTotal += bytesReceived;
            }
        }

        auto end = std::chrono::high_resolution_clock::now(); \
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \

        std::cout << "Messages received: " << counter << ", Bytes: " << bytesTotal << std::endl;
        std::cout << "Result: " << duration << " microseconds" << std::endl;
    }
}

void UDPSockets::TestAll()
{
    startServer();

    // Result:
    //      Messages received: 1'000'000, Bytes: 1024'000'000
    //         Result: 6 675 522 microseconds
    //         Result: 5 570 706 microseconds
    //  + Some message losses
}