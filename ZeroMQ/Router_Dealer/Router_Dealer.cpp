/**============================================================================
Name        : Router_Dealer.h
Created on  : 29.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Router_Dealer.h
============================================================================**/

#include "Router_Dealer.hpp"
#include "Logger.hpp"

#include <iostream>
#include <thread>

#include <zmq.hpp>
#include <zmq_addon.hpp>



namespace Router_Dealer
{
    using namespace std::string_view_literals;

    constexpr std::string_view connString { "tcp://0.0.0.0:5556"sv };

    void server(logger::Logger& logger)
    {
        zmq::context_t context(1);
        zmq::socket_t socket (context, zmq::socket_type::router);
        socket.setsockopt( ZMQ_IDENTITY, "A", 1);
        socket.bind(connString.data());

        while (true)
        {
            zmq::message_t request;
            const std::optional<size_t> result = socket.recv(request, zmq::recv_flags::none);
            if (!result) {
                logger.info("[Server] Receive error");
                continue;
            }

            const std::string_view data = request.to_string_view();
            logger.info("[Server] Received request ({} bytes) : {}", request.size(), data);

            std::string reply_str = "World";
            zmq::message_t reply(reply_str.begin(), reply_str.end());
            socket.send(reply, zmq::send_flags::none);
        }
    }

    void client(logger::Logger& logger)
    {
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::dealer);
        socket.setsockopt( ZMQ_IDENTITY, "B", 1);
        socket.connect(connString.data());

        //zmq::message_t reqIdentity("A");

        for (int i = 0; i < 3; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100U));

            std::string msg = "Hello";
            zmq::message_t request(msg.begin(), msg.end());

            socket.send(zmq::str_buffer("A"), zmq::send_flags::sndmore);
            socket.send(zmq::message_t(), zmq::send_flags::sndmore);
            socket.send(request, zmq::send_flags::none);

            std::this_thread::sleep_for(std::chrono::milliseconds(100U));

            zmq::message_t reply;
            const std::optional<size_t> result = socket.recv(reply, zmq::recv_flags::none);
            if (!result) {
                logger.info("[Client] Receive error");
                continue;
            }

            const std::string_view data = reply.to_string_view();
            logger.info("[Client] Received reply ({} bytes) : {}", request.size(), data);
        }
    }

    void run()
    {
        logger::Logger logger { "zmq", "/tmp/Logs/server/trace.log" };

        std::jthread s(server, std::ref(logger)),  c(client, std::ref(logger));
    }
}

void Router_Dealer::TestAll()
{
    run();
}
