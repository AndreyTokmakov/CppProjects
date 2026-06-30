/**============================================================================
Name        : Example_1.cpp
Created on  : 25.06.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : UDPMulticast.cpp
============================================================================**/

#include "UDPMulticast.hpp"
#include "DateTimeUtilities.hpp"
#include "../Utilities/Utilities.h"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <format>
#include <thread>
#include <chrono>
#include <expected>
#include <syncstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LOG  std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << " "
#define ERR  std::osyncstream { std::cerr } << DateTimeUtilities::getCurrentTime() << " "


namespace
{
    using Socket = int32_t;
    using Port =  uint16_t;

    constexpr Socket InvalidHandle = Socket {-1};
    constexpr Port serverPort { 8888 };

    constexpr std::string_view multicastGroup { "239.255.0.1" };

    [[nodiscard]]
    sockaddr_in createMulticastAddress(const Port port)
    {
        return {
            .sin_family=AF_INET,
            .sin_port=htons(port),
            .sin_addr={.s_addr = INADDR_ANY},
            .sin_zero={}
        };
    }

    [[nodiscard]]
    sockaddr_in createMulticastAddress(const std::string_view group, const Port port)
    {
        return {
            .sin_family=AF_INET,
            .sin_port=htons(port),
            .sin_addr={.s_addr = inet_addr(group.data()) },
            .sin_zero={}
        };
    }

    [[nodiscard]]
    std::expected<ip_mreq, int> joinMulticastGroup(const Socket socket,
                                                   const std::string_view multicastGroup)
    {
        const ip_mreq multicastRequest {
            .imr_multiaddr = in_addr  { .s_addr = inet_addr(multicastGroup.data()) },
            .imr_interface = in_addr {  .s_addr = INADDR_ANY },
        };
        if (InvalidHandle == ::setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                &multicastRequest, sizeof(multicastRequest)))
        {
            ERR << "Failed to join multicat group. Error = " << errno << std::endl;
            return std::unexpected(errno);
        }
        return multicastRequest;
    }

    void server()
    {
        const Socket sock = socket(AF_INET, SOCK_DGRAM, 0);
        const Utilities::SocketScoped socketGuard { sock };
        if (InvalidHandle == sock) {
            ERR << "Ошибка создания сокета" << std::endl;
            return;
        }

        sockaddr_in multicastAddr { createMulticastAddress(multicastGroup, serverPort) };

        // Разрешаем отправку на мультикаст адрес
        constexpr int multicastTtl = 1;
        if (::setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &multicastTtl, sizeof(multicastTtl)) < 0) {
            ERR << "Ошибка установки TTL" << std::endl;
            return;
        }

        constexpr std::string_view message = "Hello, Multicast World!";
        uint32_t messageId = 0;
        while (true)
        {
            const std::string full_message = std::format("{} #{}", message, ++messageId);
            const int64_t bytes_sent = ::sendto(sock, full_message.c_str(), full_message.length(), 0,
                                   reinterpret_cast<struct sockaddr*>(&multicastAddr), sizeof(multicastAddr));
            if (bytes_sent < 0) {
                ERR << "Ошибка отправки" << std::endl;
            } else {
                LOG  << "Отправлено: " << full_message << " (" << bytes_sent << " байт)" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    void client(const uint32_t receiverId)
    {
        const Socket sock = ::socket(AF_INET, SOCK_DGRAM, 0);
        const Utilities::SocketScoped socketGuard { sock };
        if (InvalidHandle == sock) {
            ERR << "Ошибка создания сокета" << std::endl;
            return;
        }

        constexpr int reuse = 1;
        if (InvalidHandle == ::setsockopt(sock,SOL_SOCKET, SO_REUSEADDR, &reuse,sizeof(reuse))) {
            ERR << "Failed to set SO_REUSEADDR. Error = " << errno << std::endl;
            return;
        }
        if (InvalidHandle == ::setsockopt(sock,SOL_SOCKET, SO_REUSEPORT, &reuse,sizeof(reuse))) {
            ERR << "Failed to set SO_REUSEPORT. Error = " << errno << std::endl;
            return;
        }

        sockaddr_in address { createMulticastAddress(serverPort) };
        if (InvalidHandle == ::bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address))) {
            ERR << "Ошибка привязки сокета" << std::endl;
            return;
        }

        // Настраиваем присоединение к мультикаст группе
        const std::expected<ip_mreq, int> multicastRequest = joinMulticastGroup(sock, multicastGroup);
        if (!multicastRequest.has_value()) {
            return;
        }

        sockaddr_in senderAddr {};
        socklen_t sender_len = sizeof(senderAddr);
        for (std::array<char, 1024> buffer{} ;; ) {
            const int64_t bytesReceived = ::recvfrom(sock, buffer.data(), sizeof(buffer) - 1, 0,
                reinterpret_cast<struct sockaddr*>(&senderAddr), &sender_len);
            if (bytesReceived > 0)
            {
                const std::string sender_ip = inet_ntoa(senderAddr.sin_addr);
                const int senderPort = ntohs(senderAddr.sin_port);

                LOG << "[" << receiverId << "] Получено от " << sender_ip << ":" << senderPort << " -> "
                    << std::string_view { buffer.data(), buffer.data() + bytesReceived } << std::endl;
            }
        }

        // Выход из группы (обычно не выполняется, т.к. программа завершается)
        :: setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                      &multicastRequest.value(), sizeof(ip_mreq));
    }

    void run()
    {
        std::vector<std::jthread> threads;
        threads.emplace_back(server);
        threads.emplace_back(client, 1);
        threads.emplace_back(client, 2);
        threads.emplace_back(client, 3);
    }
}

void udp_multicast::Example_1::TestAll()
{
    run();
}

