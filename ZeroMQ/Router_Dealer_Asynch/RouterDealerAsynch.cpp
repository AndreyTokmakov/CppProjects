/**============================================================================
Name        : RouterDealerAsynch.cpp
Created on  : 05.10.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RouterDealerAsynch.cpp
============================================================================**/

#include "RouterDealerAsynch.hpp"

#include <iostream>
#include <syncstream>
#include <print>
#include <format>
#include <thread>
#include <zmq.hpp>
#include <zmq_addon.hpp>

#include "DateTimeUtilities.hpp"

#define LOG std::cout << DateTimeUtilities::getCurrentTime() << ' '

namespace
{
    void server()
    {
        zmq::message_t emptyMessage {};

        zmq::context_t context{1};
        zmq::socket_t router{context, zmq::socket_type::router};
        router.bind("tcp://*:5557");

        zmq::pollitem_t items[] = {
            { static_cast<void*>(router), 0, ZMQ_POLLIN, 0 }
        };

        LOG << "[SERVER] Polling ROUTER server at tcp://*:5557\n";

        std::vector<zmq::message_t> parts;
        zmq::recv_result_t result;
        while (true)
        {
            // Периодическое опрос сокета с таймаутом 1 сек
            zmq::poll(items, 1, std::chrono::milliseconds(1000UL));

            if (items[0].revents & ZMQ_POLLIN)
            {
                // Получаем multipart: [identity][payload]
                parts.clear();
                do {
                    result = router.recv(parts.emplace_back(), zmq::recv_flags::none);
                    // LOG << "[SERVER] result: " << result.value() << "\n";
                } while (result && router.get(zmq::sockopt::rcvmore));

                if (parts.size() >= 2)
                {
                    zmq::message_t& identity = parts[0];
                    zmq::message_t& payload  = parts[1];

                    std::string msg(static_cast<char*>(payload.data()), payload.size());
                    LOG << "[SERVER] Got from client: " << msg << "\n";

                    // Ответ клиенту
                    std::string reply = "Processed: " + msg;
                    router.send(identity, zmq::send_flags::sndmore);
                    router.send(emptyMessage, zmq::send_flags::sndmore);
                    router.send(zmq::message_t(reply), zmq::send_flags::none);
                }
                else
                {
                    LOG << "[SERVER] Invalid message. Size = " << parts.size() << "\n";
                }
            }
            else
            {
                // Ничего не пришло за 1 секунду → фоновая задача
                LOG << "[SERVER] No messages — background task...\n";
            }
        }
    }

    void client(const std::string& clientId)
    {
        zmq::context_t context{1};
        zmq::socket_t dealer{context, zmq::socket_type::dealer};
        dealer.set(zmq::sockopt::routing_id, clientId);
        dealer.connect("tcp://localhost:5557");
        LOG << "[" << clientId << "] Connected to server\n";

        zmq::pollitem_t items[] = {
            { static_cast<void*>(dealer), 0, ZMQ_POLLIN, 0 }
        };

        std::vector<zmq::message_t> messages;
        for (int counter = 0; ;/** **/)
        {
            // Отправляем сообщение каждые 2 секунды
            if (counter % 4 == 0) {
                std::string msg = "Ping " + std::to_string(counter / 4) + " (" + clientId + ")";
                dealer.send(zmq::message_t(msg), zmq::send_flags::none);
                LOG << "[" << clientId << "] Sent: " << msg << "\n";
            }

            zmq::poll(items, 1, std::chrono::milliseconds(100U)); /** Ожидаем ответ максимум 500 мс **/
            if (items[0].revents & ZMQ_POLLIN)
            {
                messages.clear();
                const std::optional<uint64_t> result = zmq::recv_multipart(dealer, std::back_inserter(messages));
                if (result) //  same as !messages.empty()
                {
                    const zmq::message_t& message = messages.back();
                    LOG << "[" << clientId << "] Got reply: (size: " << message.size() << ") Data: " << message.to_string_view() << "\n";
                }
            }
            else /** Если нет ответа — можно делать другие вещи **/
            {
                LOG << "[" << clientId << "] No reply, idle...\n";
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
            counter++;
        }
    }

    void run()
    {
        std::vector<std::jthread> tasks;
        tasks.emplace_back(server);

        std::this_thread::sleep_for(std::chrono::milliseconds(100U));
        tasks.emplace_back(client, "Worker-1");

        std::this_thread::sleep_for(std::chrono::milliseconds(100U));
        tasks.emplace_back(client, "Worker-2");

        std::this_thread::sleep_for(std::chrono::milliseconds(100U));
        tasks.emplace_back(client, "Worker-3");
    }
}

void RouterDealerAsynch::TestAll()
{
    run();
}

/**
2025-10-05 09:41:48.996369 [SERVER] Polling ROUTER server at tcp://*:5557
2025-10-05 09:41:49.096574 [Worker] Connected to server
2025-10-05 09:41:49.096597 [Worker] Sent: Ping 0
2025-10-05 09:41:49.098085 [SERVER] Got from client: Ping 0
2025-10-05 09:41:49.098433 [Worker] Got reply: (size: 17) Data: Processed: Ping 0
2025-10-05 09:41:49.698715 [Worker] No reply, idle...
2025-10-05 09:41:50.099277 [SERVER] No messages — background task...
2025-10-05 09:41:50.298990 [Worker] No reply, idle...
2025-10-05 09:41:50.899735 [Worker] No reply, idle...
2025-10-05 09:41:51.100390 [SERVER] No messages — background task...
2025-10-05 09:41:51.399869 [Worker] Sent: Ping 1
2025-10-05 09:41:51.400582 [SERVER] Got from client: Ping 1
2025-10-05 09:41:51.401026 [Worker] Got reply: (size: 17) Data: Processed: Ping 1
2025-10-05 09:41:52.001305 [Worker] No reply, idle...
2025-10-05 09:41:52.401006 [SERVER] No messages — background task...
2025-10-05 09:41:52.601859 [Worker] No reply, idle...
2025-10-05 09:41:53.202494 [Worker] No reply, idle...
2025-10-05 09:41:53.402453 [SERVER] No messages — background task...
2025-10-05 09:41:53.702735 [Worker] Sent: Ping 2
2025-10-05 09:41:53.703151 [SERVER] Got from client: Ping 2
2025-10-05 09:41:53.703550 [Worker] Got reply: (size: 17) Data: Processed: Ping 2
2025-10-05 09:41:54.304399 [Worker] No reply, idle...
2025-10-05 09:41:54.704249 [SERVER] No messages — background task...
2025-10-05 09:41:54.904955 [Worker] No reply, idle...
2025-10-05 09:41:55.505324 [Worker] No reply, idle...
2025-10-05 09:41:55.705345 [SERVER] No messages — background task...
2025-10-05 09:41:56.005554 [Worker] Sent: Ping 3
2025-10-05 09:41:56.005824 [SERVER] Got from client: Ping 3
2025-10-05 09:41:56.006235 [Worker] Got reply: (size: 17) Data: Processed: Ping 3
2025-10-05 09:41:56.606909 [Worker] No reply, idle...
2025-10-05 09:41:57.007211 [SERVER] No messages — background task...
2025-10-05 09:41:57.207533 [Worker] No reply, idle...
2025-10-05 09:41:57.808370 [Worker] No reply, idle...
2025-10-05 09:41:58.008339 [SERVER] No messages — background task...
2025-10-05 09:41:58.308834 [Worker] Sent: Ping 4
2025-10-05 09:41:58.309190 [SERVER] Got from client: Ping 4
2025-10-05 09:41:58.309517 [Worker] Got reply: (size: 17) Data: Processed: Ping 4
2025-10-05 09:41:58.909848 [Worker] No reply, idle...
2025-10-05 09:41:59.310345 [SERVER] No messages — background task...
**/