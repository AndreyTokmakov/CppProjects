/**============================================================================
Name        : Router_Dealer_Simple.cpp
Created on  : 02.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Router_Dealer_Simple.cpp
============================================================================**/

#include "Router_Dealer_Simple.hpp"

#include <iostream>
#include <format>
#include <print>
#include <unordered_map>
#include <thread>


#include <zmq.hpp>


namespace Router_Dealer_Simple
{
    // constexpr uint16_t clientPort { 5555 };

    void router()
    {
        zmq::context_t ctx;
        zmq::socket_t router(ctx, zmq::socket_type::router);
        router.bind("tcp://*:5555");

        std::unordered_map<std::string, std::string> routing_table; // from client to worker
        zmq::message_t msgIdentity, msgEmpty, msgPayload;

        std::println("[Router] Starting ....");

        int counter = 0;
        while (true)
        {
            std::optional<size_t> result = router.recv(msgIdentity);

            const std::string_view identity = msgIdentity.to_string_view();
            std::println("[Router] Received: 'identity' message size: {}, data: {} ",
                         result.value_or(0), identity);

            /*
            result = router.recv(msgEmpty);
            const std::string_view empty = msgEmpty.to_string_view();
            std::println("[Router] Received: 'empty' message size: {}, data: {} ",
                         result.value_or(0), empty);*/

            result = router.recv(msgPayload);
            const std::string_view payload = msgPayload.to_string_view();
            std::println("[Router] Received: 'payload' message size: {}, data: {} ",
                         result.value_or(0), payload);

            std::string target = (payload.find("worker1") != std::string::npos) ? "worker1" : "worker2";
            if (identity.rfind("client", 0) == 0)
                routing_table[target] = identity;

            zmq::message_t dst_id(target.data(), target.size());
            router.send(dst_id, zmq::send_flags::sndmore);
            router.send(zmq::message_t(), zmq::send_flags::sndmore);
            router.send(zmq::buffer(payload), zmq::send_flags::none);

            if (counter++ > 10)break;
        }
    }

    void client()
    {
        zmq::context_t ctx;
        zmq::socket_t dealer(ctx, zmq::socket_type::dealer);
        dealer.set(zmq::sockopt::routing_id, "client1");
        dealer.connect("tcp://localhost:5555");

        std::println("[Client] before send");
        dealer.send(zmq::buffer("request to worker1"), zmq::send_flags::none);

        zmq::message_t reply;
        const std::optional<size_t> result = dealer.recv(reply);

        std::println("[Client] Got reply ({}): {}", result.value_or(0), reply.to_string_view());
    }

    void worker()
    {
        zmq::context_t ctx;
        zmq::socket_t dealer(ctx, zmq::socket_type::dealer);
        dealer.set(zmq::sockopt::routing_id, "worker1");
        dealer.connect("tcp://localhost:5555");

        zmq::message_t msg;
        while (true)
        {
            msg.rebuild();

            std::println("[Worker] before receive");
            const std::optional<size_t> result = dealer.recv(msg);

            std::string text = msg.to_string();

            std::println("[Worker] Received ({}): {}", result.value_or(0), text);

            std::string reply = "processed: " + text;
            std::println("[Worker] Send response : {}", reply);
            dealer.send(zmq::buffer(reply), zmq::send_flags::none);
        }
    }


    void run()
    {
        std::vector<std::jthread> tasks;

        tasks.emplace_back(router);

        std::this_thread::sleep_for(std::chrono::milliseconds (100u));
        tasks.emplace_back(worker);

        std::this_thread::sleep_for(std::chrono::milliseconds (100u));
        tasks.emplace_back(client);
    }
}

void Router_Dealer_Simple::TestAll()
{
    run();
}