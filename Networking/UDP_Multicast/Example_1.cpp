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
#include <vector>
#include <mutex>
#include <thread>
#include <string>
#include <chrono>
#include <syncstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LOG  std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << " "
#define ERR  std::osyncstream { std::cerr } << DateTimeUtilities::getCurrentTime() << " "

namespace
{
    using Socket = int32_t;

    void server()
    {
        const Socket sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            std::cerr << "Ошибка создания сокета" << std::endl;
            return;
        }

        const Utilities::SocketScoped socketGuard { sock };

        // Настройка адреса назначения (мультикаст группа)
        sockaddr_in multicastAddr {};
        multicastAddr.sin_family = AF_INET;
        multicastAddr.sin_addr.s_addr = inet_addr("239.255.0.1"); // Мультикаст группа
        multicastAddr.sin_port = htons(8888);

        // Разрешаем отправку на мультикаст адрес
        constexpr int multicastTtl = 1;
        if (::setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &multicastTtl, sizeof(multicastTtl)) < 0) {
            std::cerr << "Ошибка установки TTL" << std::endl;
            return;
        }

        const std::string message = "Hello, Multicast World!";
        int message_num = 0;

        std::cout << "Мультикаст сервер запущен. Отправка на 239.255.0.1:8888" << std::endl;
        std::cout << "Нажмите Ctrl+C для остановки" << std::endl;

        while (true)
        {
            const std::string full_message = message + " #" + std::to_string(++message_num);
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

    void client()
    {
        const Socket sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            std::cerr << "Ошибка создания сокета" << std::endl;
            return;
        }

        sockaddr_in localAddr = {};
        localAddr.sin_family = AF_INET;
        localAddr.sin_addr.s_addr = INADDR_ANY; // Принимаем с любого интерфейса
        localAddr.sin_port = htons(8888); // Тот же порт, что и у сервера

        if (::bind(sock, reinterpret_cast<struct sockaddr*>(&localAddr), sizeof(localAddr)) < 0) {
            std::cerr << "Ошибка привязки сокета" << std::endl;
            return;
        }

        // Настраиваем присоединение к мультикаст группе
        ip_mreq multicastRequest {};
        multicastRequest.imr_multiaddr.s_addr = inet_addr("239.255.0.1"); // Группа
        multicastRequest.imr_interface.s_addr = INADDR_ANY; // Интерфейс

        if (::setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicastRequest, sizeof(multicastRequest)) < 0) {
            std::cerr << "Ошибка присоединения к мультикаст группе" << std::endl;
            return;
        }

        std::cout << "Мультикаст клиент запущен. Ожидание данных..." << std::endl;
        std::cout << "Нажмите Ctrl+C для остановки" << std::endl;

        std::array<char, 1024> buffer{};
        sockaddr_in senderAddr {};
        socklen_t sender_len = sizeof(senderAddr);
        while (true)
        {
            const int64_t bytesReceived = ::recvfrom(sock, buffer.data(), sizeof(buffer) - 1, 0,
                reinterpret_cast<struct sockaddr*>(&senderAddr), &sender_len);
            if (bytesReceived > 0)
            {
                const std::string  sender_ip = inet_ntoa(senderAddr.sin_addr);
                const int senderPort = ntohs(senderAddr.sin_port);

                LOG  << "Получено от " << sender_ip << ":" << senderPort << " -> "
                    << std::string_view { buffer.data(), buffer.data() + bytesReceived } << std::endl;
            }
        }

        // Выход из группы (обычно не выполняется, т.к. программа завершается)
        :: setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                      &multicastRequest, sizeof(multicastRequest));
    }

    void run()
    {
        std::vector<std::jthread> threads;
        threads.emplace_back(server);
        threads.emplace_back(client);
    }
}

void udp_multicast::Example_1::TestAll()
{
    run();
}

