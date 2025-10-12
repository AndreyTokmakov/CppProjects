/**============================================================================
Name        : Request_Response.cpp
Created on  : 29.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Request_Response.h
============================================================================**/

#include "Request_Response.hpp"
#include "Logger.hpp"

#include <thread>

#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace Request_Response
{
    void server(logger::Logger& logger)
    {
        zmq::context_t context(1);
        zmq::socket_t responder(context, zmq::socket_type::rep);
        responder.bind("tcp://*:5556");

        while (true)
        {
            zmq::message_t request;
            const std::optional<size_t> result = responder.recv(request, zmq::recv_flags::none);
            if (!result) {
                logger.info("Receive error");
                continue;
            }

            const size_t bytesReceived = result.value();
            const std::string_view data(static_cast<char*>(request.data()), request.size());

            logger.info("Received request ({} bytes): {}", bytesReceived, data);

            std::string reply_str = "World";
            zmq::message_t reply(reply_str.begin(), reply_str.end());
            responder.send(reply, zmq::send_flags::none);
        }
    }

    void client(logger::Logger& logger)
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
                logger.info("Receive error");
                continue;
            }

            const size_t bytesReceived = result.value();
            const std::string_view data(static_cast<char*>(reply.data()), reply.size());

            logger.info("Received reply ({} bytes): {}", bytesReceived, data);
        }
    }

    void run()
    {
        logger::Logger logger { "zmq", "/tmp/Logs/server/trace.log" };

        std::jthread s(server, std::ref(logger));
        std::jthread c(client, std::ref(logger));
    }

    /**
    [2025-10-12 08:25:08.448] [zmq] [info] Received request (5 bytes): Hello
    [2025-10-12 08:25:08.449] [zmq] [info] Received reply (5 bytes): World
    [2025-10-12 08:25:08.449] [zmq] [info] Received request (5 bytes): Hello
    [2025-10-12 08:25:08.449] [zmq] [info] Received reply (5 bytes): World
    [2025-10-12 08:25:08.449] [zmq] [info] Received request (5 bytes): Hello
    [2025-10-12 08:25:08.449] [zmq] [info] Received reply (5 bytes): World
    [2025-10-12 08:25:08.449] [zmq] [info] Received request (5 bytes): Hello
    [2025-10-12 08:25:08.449] [zmq] [info] Received reply (5 bytes): World
    [2025-10-12 08:25:08.449] [zmq] [info] Received request (5 bytes): Hello
    [2025-10-12 08:25:08.449] [zmq] [info] Received reply (5 bytes): World
    [2025-10-12 08:25:08.449] [zmq] [info] Received request (5 bytes): Hello
    [2025-10-12 08:25:08.450] [zmq] [info] Received reply (5 bytes): World
    [2025-10-12 08:25:08.450] [zmq] [info] Received request (5 bytes): Hello
    [2025-10-12 08:25:08.450] [zmq] [info] Received reply (5 bytes): World
    [2025-10-12 08:25:08.450] [zmq] [info] Received request (5 bytes): Hello
    [2025-10-12 08:25:08.450] [zmq] [info] Received reply (5 bytes): World
    [2025-10-12 08:25:08.450] [zmq] [info] Received request (5 bytes): Hello
    [2025-10-12 08:25:08.450] [zmq] [info] Received reply (5 bytes): World
    [2025-10-12 08:25:08.450] [zmq] [info] Received request (5 bytes): Hello
    [2025-10-12 08:25:08.450] [zmq] [info] Received reply (5 bytes): World
    **/
}

void Request_Response::TestAll()
{
    run();
}