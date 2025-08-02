/**============================================================================
Name        : Router_Dealer.h
Created on  : 29.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Router_Dealer.h
============================================================================**/

#include "Router_Dealer.h"
#include "DateTimeUtilities.h"

#include <iostream>
#include <thread>

#include <zmq.hpp>
#include <zmq_addon.hpp>

#define LOG std::cout << getCurrentTime() << ' '

namespace Router_Dealer
{
    using namespace DateTimeUtilities;
    using namespace std::string_view_literals;

    constexpr std::string_view connString { "tcp://0.0.0.0:5556"sv };

    void server()
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
                LOG << "[Server] Receive error\n";
                continue;
            }

            const std::string_view data = request.to_string_view();
            LOG << "[Server] Received request (" << request.size() << " bytes) : " << data << "\n";

            std::string reply_str = "World";
            zmq::message_t reply(reply_str.begin(), reply_str.end());
            socket.send(reply, zmq::send_flags::none);
        }
    }

    void client()
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
                LOG << "[Client] Receive error\n";
                continue;
            }

            const std::string_view data = reply.to_string_view();
            LOG << "[Client] Received reply (" << data.size() << " bytes) : " << data << "\n";
        }
    }

    void run()
    {
        std::jthread s(server),  c(client);
    }
}

void Router_Dealer::TestAll()
{
    run();
}
