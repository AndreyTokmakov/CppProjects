/**============================================================================
Name        : Utilities.cpp
Created on  : 29.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Utilities
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>
#include <thread>
#include <chrono>

#include <zmq.hpp>
#include <zmq_addon.hpp>

#include "DateTimeUtilities.h"
#include "PubSub/Publisher_Subscriber.h"
#include "Request_Response/Request_Response.h"
#include "Secure_Communication/Secure_Communication.h"
#include "Router_Dealer_One/Router_Dealer_One.h"
#include "Router_Dealer/Router_Dealer.h"
#include "Server/DebugServer.h"

namespace TelemetrixClient
{
    using namespace DateTimeUtilities;

    void test()
    {
        zmq::context_t context(1);
        zmq::socket_t requester(context, zmq::socket_type::req);
        requester.connect("tcp://localhost:10003");

        for (int i = 0; i < 1000; ++i)
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
}

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Publisher_Subscriber::TestAll();
    // Request_Response::TestAll();
    // Secure_Communication::TestAll();

    // Router_Dealer::TestAll();
    // Router_Dealer_One::TestAll();

    DebugServer::TestAll(args);

    // TelemetrixClient::test();

    return EXIT_SUCCESS;
}


