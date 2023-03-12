//============================================================================
// Name        : Networking.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Tests C++ project
//============================================================================

#include <iostream>
#include <string>
#include <vector>
#include <string_view>

#include "Clients/Clients.h"
#include "Clients/UDPClient.h"
#include "DNS/DNS_ResponseParser.h"
#include "RawSockets/RawSocketsTests.h"
#include "RawSockets/BuildAndSendPacketsTests.h"
#include "RawSockets/ARPTester.h"
#include "Experiments/Experiments.h"
#include "Experiments/UtilitiesTests.h"
#include "NetLink/NetLink.h"
#include "Utilities/Utilities.h"
#include "Servers//TestWebServer.h"


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Clients::Tests();
    // UDPClient::TestAll();

    // DNS_ResponseParser::TestAll();

    // RawSocketsTests::TestAll();
    // ARPTester::TestAll();
    // BuildAndSendPacketsTests::TestAll();

    // Experiments::Tests();
    // UtilitiesTests::TestAll();

    // NetLink::TestAll();

    // Networking::Utilities::TestAll();

    TestWebServer::Tests();

    return EXIT_SUCCESS;
}

