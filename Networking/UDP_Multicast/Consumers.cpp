/**============================================================================
Name        : Producers.cpp
Created on  : 26.06.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Producers.cpp
============================================================================**/

#include "UDPMulticast.hpp"
#include "DateTimeUtilities.hpp"
#include "../Utilities/Utilities.h"

#include <iostream>
#include <syncstream>
#include <format>
#include <print>
#include <string>
#include <string_view>

#include <vector>
#include <thread>
#include <chrono>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <cstdint>
#include <unistd.h>

#define LOG  std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << " "
#define ERR  std::osyncstream { std::cerr } << DateTimeUtilities::getCurrentTime() << " "

namespace
{
    using Socket = int32_t;
    using Port =  uint16_t;

    constexpr Socket InvalidHandle = Socket {-1};
    constexpr Port serverPort { 8888 };

    constexpr std::string_view multicastGroup { "239.255.0.1" };

    enum class MulticastTTL : std::uint8_t
    {
        Host      = 0,
        Subnet    = 1,
        Site      = 32,
        Region    = 64,
        Continent = 128,
        Global    = 255
    };

    sockaddr_in createMulticastAddress(const Port port)
    {
        return  {
            .sin_family=AF_INET,
            .sin_port=htons(port),
            .sin_addr={.s_addr = INADDR_ANY },
            .sin_zero={}
        };
    }

    void run()
    {
        const Socket sock = ::socket(AF_INET, SOCK_DGRAM, 0);
        const Utilities::SocketScoped socketGuard { sock };
        if (InvalidHandle == sock) {
            ERR << "Failed to created socket. Error = " << errno << "\n";
            return;
        }

        constexpr int reuse = 1;
        if (InvalidHandle == ::setsockopt(sock,SOL_SOCKET,
                    SO_REUSEADDR, &reuse,sizeof(reuse))) {
            ERR << "Failed to set SO_REUSEADDR. Error = " << errno << std::endl;
            return;
        }
        if (InvalidHandle == ::setsockopt(sock,SOL_SOCKET,
                    SO_REUSEPORT, &reuse,sizeof(reuse))) {
            ERR << "Failed to set SO_REUSEPORT. Error = " << errno << std::endl;
            return;
        }

        sockaddr_in address = createMulticastAddress(serverPort);
        if (::bind(sock, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0) {
            ERR << "Failed to bind socket. Error = " << errno << std::endl;
            return;
        }

        ip_mreq multicastRequest {};
        multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.data());
        multicastRequest.imr_interface.s_addr = INADDR_ANY; // Интерфейс

        if (::setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicastRequest, sizeof(multicastRequest)) < 0) {
            ERR << "Failed to join multicat group. Error = " << errno << std::endl;
            return;
        }

        std::array<char, 1024> buffer{};
        sockaddr_in senderAddr {};
        socklen_t sender_len = sizeof(senderAddr);
        while (true)
        {
            const int64_t bytesReceived = ::recvfrom(sock, buffer.data(), sizeof(buffer) - 1, 0,
                reinterpret_cast<struct sockaddr*>(&senderAddr), &sender_len);
            if (bytesReceived > 0)
            {
                const std::string sender_ip = inet_ntoa(senderAddr.sin_addr);
                const int senderPort = ntohs(senderAddr.sin_port);

                LOG << "Received from " << sender_ip << ":" << senderPort << " -> "
                    << std::string_view { buffer.data(), buffer.data() + bytesReceived } << std::endl;
            }
        }

        // Выход из группы (обычно не выполняется, т.к. программа завершается)
        :: setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                      &multicastRequest, sizeof(multicastRequest));
    }
}

void udp_multicast::Consumers::TestAll()
{
    run();
}

