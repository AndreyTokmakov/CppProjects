/**============================================================================
Name        : ZeroMqProxy.cpp
Created on  : 03.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ZeroMqProxy.cpp
============================================================================**/

#include "ZeroMqProxy.hpp"

#include <iostream>
#include <syncstream>
#include <print>
#include <format>
#include <thread>

#include <zmq_addon.hpp>

#include "DateTimeUtilities.hpp"

#define LOG std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << ' '

namespace Proxy_SimpleTest
{
    constexpr uint16_t routerPort { 5555 };
    constexpr uint16_t dealerPort { 5556 };

    void proxy()
    {
        zmq::context_t ctx { 1 };

        zmq::socket_t frontend(ctx, zmq::socket_type::router);
        frontend.bind(std::format("tcp://*:{}", routerPort));

        zmq::socket_t backend(ctx, zmq::socket_type::dealer);
        backend.bind(std::format("tcp://*:{}", dealerPort));

        LOG << "[Broker] Starting proxy..." << std::endl;
        zmq::proxy(frontend, backend);
    }

    void worker()
    {
        zmq::context_t ctx { 1 };
        zmq::socket_t worker(ctx, zmq::socket_type::rep);
        worker.connect(std::format("tcp://0.0.0.0:{}", dealerPort));

        std::optional<size_t> result;
        zmq::message_t request;
        while (true)
        {
            result = worker.recv(request, zmq::recv_flags::none);
            LOG << "[Worker] Received (" << result.value_or(0) << "): " << request.to_string_view() << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(100u));
            std::string reply = "Processed: " + request.to_string();
            worker.send(zmq::buffer(reply), zmq::send_flags::none);
        }
    }

    void client()
    {
        zmq::context_t ctx { 1 };
        zmq::socket_t client(ctx, zmq::socket_type::req);
        client.connect(std::format("tcp://0.0.0.0:{}", routerPort));

        std::optional<size_t> result;
        zmq::message_t reply;
        for (int i = 0; i < 10; ++i)
        {
            std::string msg = "Hello " + std::to_string(i);
            client.send(zmq::buffer(msg), zmq::send_flags::none);

            result = client.recv(reply, zmq::recv_flags::none);
            LOG << "[Client] Received (" << result.value_or(0) << "): " << reply.to_string_view() << std::endl;
        }
    }

    void run()
    {
        std::vector<std::jthread> tasks;
        tasks.emplace_back(proxy);
        tasks.emplace_back(worker);
        tasks.emplace_back(client);
    }
}


void ZeroMqProxy::TestAll()
{
    Proxy_SimpleTest::run();
}