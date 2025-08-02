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

#include "DateTimeUtilities.h"
#include "PubSub/Publisher_Subscriber.hpp"
#include "Request_Response/Request_Response.hpp"
#include "Secure_Communication/Secure_Communication.hpp"
#include "Router_Dealer_One/Router_Dealer_One.hpp"
#include "Router_Dealer/Router_Dealer.hpp"
#include "Server/DebugServer.hpp"
#include "Experiments/Experiments.hpp"


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    Experiments::TestAll();

    // Publisher_Subscriber::TestAll();
    // Request_Response::TestAll();
    // Secure_Communication::TestAll();

    // Router_Dealer::TestAll();
    // Router_Dealer_One::TestAll();

    // DebugServer::TestAll(args);
    return EXIT_SUCCESS;
}


