/**============================================================================
Name        : ReqRepServer.cpp
Created on  : 11.10.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ReqRepServer.cpp
============================================================================**/

#include "Servers.hpp"
#include "../Common/Logger.hpp"

#include <iostream>
#include <string_view>
#include <vector>

#include <zmq.hpp>

namespace ReqRepServer
{
    void run(const std::vector<std::string_view> & )
    {
        logger::Logger logger { "zmq", "/tmp/Logs/server/trace.log" };

        zmq::context_t context(1);
        zmq::socket_t responder(context, zmq::socket_type::rep);
        responder.bind("tcp://*:5556");

        logger.info("Running server on {}", "tcp://*:5556");
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

            logger.info("Received request ({}): {}", bytesReceived, data);

            std::string reply_str = "World";
            zmq::message_t reply(reply_str.begin(), reply_str.end());
            responder.send(reply, zmq::send_flags::none);
        }
    }
}


void Servers::runReqRepServer(const std::vector<std::string_view> & args)
{
    ReqRepServer::run(args);
}

