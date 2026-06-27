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

    struct Producer
    {
        void run()
        {
            serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
            const Utilities::SocketScoped socketGuard { serverSocket };
            if (InvalidHandle == serverSocket) {
                ERR << "Failed to created socket. Error = " << errno << "\n";
                return;
            }

            if (!allowMulticastTtl()) {
                return;
            }

            sockaddr_in multicastAddr = createMulticastAddress(multicastGroup, serverPort);
            constexpr std::string_view message = "Hello, Multicast World!";
            for ( uint32_t messageId = 0; true; ++messageId)
            {
                const std::string full_message = std::format("{} #{}", message, messageId);
                const int64_t bytes_sent = ::sendto(serverSocket, full_message.c_str(), full_message.length(), 0,
                                       reinterpret_cast<struct sockaddr*>(&multicastAddr), sizeof(multicastAddr));
                if (bytes_sent < 0) {
                    ERR << "Failed to send packet. Error = " << errno << "\n";
                } else {
                    LOG << full_message.size() << " bytes send\n";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }

        ~Producer()
        {
            if (InvalidHandle != serverSocket) {
                ::close(InvalidHandle);
            }
        }

        static sockaddr_in createMulticastAddress(const std::string_view group, const Port port)
        {
            return  {
                .sin_family=AF_INET,
                .sin_port=htons(port),
                .sin_addr={.s_addr = inet_addr(group.data()) },
                .sin_zero={}
            };
        }

        [[nodiscard]]
        bool allowMulticastTtl(const MulticastTTL ttl =  MulticastTTL::Subnet) const
        {
            if ( InvalidHandle == ::setsockopt(serverSocket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(MulticastTTL)))  {
                ERR << "Ошибка установки TTL" << std::endl;
                return false;
            }
            return true;
        }

    private:

        Socket serverSocket { InvalidHandle };
    };
}

void udp_multicast::Producers::TestAll()
{
    Producer{}.run();
}

