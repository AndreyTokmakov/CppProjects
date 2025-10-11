/**============================================================================
Name        : DebugServer.cpp
Created on  : 02.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DebugServer.cpp
============================================================================**/

#include "Servers.hpp"
#include "../Common/Logger.hpp"

#include <iostream>
#include <string_view>
#include <vector>

#include <zmq.hpp>


namespace DebugServer
{
    using namespace std::string_view_literals;

    struct Service
    {
        std::string name;
        uint32_t port { 0 };
    };

    constexpr std::array<Service, 3> services {{
        {"Srv1", 10001 },
        {"Srv2", 10002 },
        {"Srv3", 10003 },
    }};

    struct ZeroMQServer
    {
        zmq::context_t context;
        logger::Logger logger { "zmq", "/tmp/Logs/server/trace.log" };

        explicit ZeroMQServer(int32_t workersCount = 4): context { workersCount } {

        }

        void process(const Service& service)
        {
            zmq::socket_t responder(context, zmq::socket_type::rep);
            responder.bind("tcp://*:" + std::to_string(service.port));

            logger.info("[{}] Running on tcp://*:{}"sv, service.name, service.port);
            while (true)
            {
                zmq::message_t request;
                const std::optional<size_t> result = responder.recv(request, zmq::recv_flags::none);
                if (!result) {
                    logger.error("[{}] Receive error", service.name);
                    continue;
                }

                const size_t bytesReceived = result.value();
                const std::string_view data(static_cast<char*>(request.data()), request.size());

                logger.info("[{}] Received request ({} bytes): {}", service.name, bytesReceived, data);
                std::string reply_str = "Echo: " + std::string {data};
                zmq::message_t reply(reply_str.begin(), reply_str.end());
                responder.send(reply, zmq::send_flags::none);
            }
        }

        void run()
        {
            std::vector<std::jthread> workers;

            for (const Service& service : services) {
                workers.emplace_back(&ZeroMQServer::process, this, service);
            }

            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100u));
            }
        }
    };
}

void Servers::runDebugServer(const std::vector<std::string_view>&)
{
    DebugServer::ZeroMQServer server;
    server.run();
}