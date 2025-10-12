/**============================================================================
Name        : Router_Dealer_One.cpp
Created on  : 30.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Router_Dealer_One.cpp
============================================================================**/

#include "Router_Dealer_One.hpp"
#include "Logger.hpp"

#include <thread>
#include <format>

#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace Router_Dealer_One
{
    constexpr uint16_t clientPort { 5555 };
    constexpr uint16_t serverPort { 5556 };

    void broker(logger::Logger&)
    {
        zmq::context_t ctx(1);
        zmq::socket_t clientSocket(ctx, zmq::socket_type::router);
        zmq::socket_t serverSocket(ctx, zmq::socket_type::dealer);

        clientSocket.bind(std::format("tcp://*:{}", clientPort));
        serverSocket.bind(std::format("tcp://*:{}", serverPort));

        std::vector<zmq::pollitem_t> items {
            zmq::pollitem_t { static_cast<void*>(clientSocket), 0, ZMQ_POLLIN, 0 },
            zmq::pollitem_t { static_cast<void*>(serverSocket),  0, ZMQ_POLLIN, 0 }
        };
        std::array<zmq::socket_t*, 2> receiveFrom {  &clientSocket, &serverSocket };
        std::array<zmq::socket_t*, 2> replyTo { &serverSocket, &clientSocket };

        zmq::message_t msg;
        std::vector<zmq::message_t> parts;

        bool hasMore = true;
        while (true)
        {
            [[maybe_unused]]
            int n = zmq::poll(items);
            // std::cout << n << std::endl;

            for (uint32_t itemId = 0; itemId < items.size(); ++itemId, hasMore = true)
            {
                if (items[itemId].revents & ZMQ_POLLIN)
                {
                    while (hasMore)
                    {
                        auto _ = receiveFrom[itemId]->recv(msg, zmq::recv_flags::none);
                        hasMore = msg.more();
                        parts.push_back(std::move(msg));
                    }

                    for (size_t i = 0; i < parts.size(); ++i) {
                        replyTo[itemId]->send(parts[i], i + 1 < parts.size() ? zmq::send_flags::sndmore : zmq::send_flags::none);
                    }
                }
                parts.clear();
            }
        }
    }

    void worker(const std::string& name, logger::Logger& logger)
    {
        zmq::context_t ctx(1);
        zmq::socket_t sock(ctx, zmq::socket_type::dealer);
        sock.connect(std::format("tcp://0.0.0.0:{}", serverPort));

        zmq::message_t client_id;
        zmq::message_t empty;
        zmq::message_t request;

        while (true)
        {
            zmq::recv_result_t result = sock.recv(client_id, zmq::recv_flags::none);
            result = sock.recv(empty, zmq::recv_flags::none);
            result = sock.recv(request, zmq::recv_flags::none);

            logger.info("[{}] Received", name, request.to_string_view());

            std::this_thread::sleep_for(std::chrono::milliseconds(100u));

            std::string reply = "Reply to: " + request.to_string();;

            sock.send(client_id, zmq::send_flags::sndmore);
            sock.send(zmq::message_t(), zmq::send_flags::sndmore);
            sock.send(zmq::buffer(reply), zmq::send_flags::none);
        }
    }

    void client(const std::string& name, logger::Logger& logger)
    {
        zmq::context_t ctx(1);
        zmq::socket_t sock(ctx, zmq::socket_type::req);
        sock.connect(std::format("tcp://0.0.0.0:{}", clientPort));

        zmq::message_t reply;
        for (int i = 0; i < 5; ++i)
        {
            std::string msg = std::format("Request {} from {}", i, name);
            sock.send(zmq::buffer(msg), zmq::send_flags::none);

            [[maybe_unused]]
            zmq::recv_result_t result = sock.recv(reply, zmq::recv_flags::none);

            logger.info("[Client] Got reply", reply.to_string_view());

            std::this_thread::sleep_for(std::chrono::milliseconds(150u));
        }
    }

    void run()
    {
        logger::Logger logger { "zmq", "/tmp/Logs/server/trace.log" };
        std::vector<std::jthread> tasks;

        tasks.emplace_back(broker, std::ref(logger));

        tasks.emplace_back(worker, "Worker-1", std::ref(logger));
        tasks.emplace_back(worker, "Worker-2", std::ref(logger));
        tasks.emplace_back(worker, "Worker-3", std::ref(logger));

        tasks.emplace_back(client, "Client-1", std::ref(logger));
        tasks.emplace_back(client, "Client-2", std::ref(logger));
        tasks.emplace_back(client, "Client-3", std::ref(logger));
        tasks.emplace_back(client, "Client-4", std::ref(logger));
        tasks.emplace_back(client, "Client-5", std::ref(logger));
        tasks.emplace_back(client, "Client-6", std::ref(logger));
    }

}

void Router_Dealer_One::TestAll()
{
    run();
}