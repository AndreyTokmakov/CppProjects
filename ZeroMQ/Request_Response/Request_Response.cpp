/**============================================================================
Name        : Request_Response.cpp
Created on  : 29.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Request_Response.h
============================================================================**/

#include "Request_Response.h"
#include "DateTimeUtilities.h"

#include <iostream>
#include <thread>

#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace Request_Response
{
    using namespace DateTimeUtilities;

    void server()
    {
        zmq::context_t context(1);
        zmq::socket_t responder(context, zmq::socket_type::rep);
        responder.bind("tcp://*:5556");

        while (true)
        {
            zmq::message_t request;
            const std::optional<size_t> result = responder.recv(request, zmq::recv_flags::none);
            if (!result) {
                std::cerr << getCurrentTime() << " Receive error\n";
                continue;
            }

            const size_t bytesReceived = result.value();
            const std::string_view data(static_cast<char*>(request.data()), request.size());

            std::cout << getCurrentTime() << " Received request (" << bytesReceived << " bytes) : " << data << "\n";

            std::string reply_str = "World";
            zmq::message_t reply(reply_str.begin(), reply_str.end());
            responder.send(reply, zmq::send_flags::none);
        }
    }

    void client()
    {
        zmq::context_t context(1);
        zmq::socket_t requester(context, zmq::socket_type::req);
        requester.connect("tcp://localhost:5556");

        for (int i = 0; i < 10; ++i)
        {
            std::string msg = "Hello";
            zmq::message_t request(msg.begin(), msg.end());
            requester.send(request, zmq::send_flags::none);

            zmq::message_t reply;
            const std::optional<size_t> result = requester.recv(reply, zmq::recv_flags::none);
            if (!result) {
                std::cerr << getCurrentTime() << " Receive error\n";
                continue;
            }

            const size_t bytesReceived = result.value();
            const std::string_view data(static_cast<char*>(reply.data()), reply.size());

            std::cout << getCurrentTime() << " Received reply (" << bytesReceived << " bytes) : " << data << "\n";
        }
    }

    void run()
    {
        std::jthread s(server),  c(client);
    }
}

void Request_Response::TestAll()
{
    run();
}