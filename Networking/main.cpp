/**============================================================================
Name        : Networking.cpp
Created on  : 15.09.2021
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Tests C++ project
============================================================================**/

#include <iostream>
#include <string>
#include <vector>
#include <string_view>

#include "CurpCpp/CurlCpp.hpp"

#include "Clients/Clients.hpp"
#include "Clients/UDPClient.h"
#include "DNS/DNS_ResponseParser.h"

#include "RawSockets/RawSocketsTests.h"
#include "RawSockets/BuildAndSendPacketsTests.h"
#include "RawSockets/ARPTester.h"
#include "RawSockets/CaptureTraffic.h"

#include "PcapAnalyzer/PcapAnalyzer.h"

#include "Experiments/Experiments.h"
#include "Experiments/UtilitiesTests.h"
#include "NetLink/NetLink.h"
#include "Utilities/Utilities.h"

#include "Servers/TestWebServer.h"
#include "Servers/SimpleServerSelect.h"
#include "Servers/SocketTCPServer.h"
#include "Servers/EPollTCPServer.h"
#include "Servers/EPollTCPServerTwo.h"
#include "Servers/EPollTCPServerEx.h"
#include "Servers/EPollTCPServerDebug.h"
#include "Servers/EPollTCPServerContext.h"
#include "Servers/EPollTCPServerContextEx.h"
#include "Servers/EPollTCPServerMultithreaded.h"
#include "ServersTCP/AsynchTcpServerWithSessions.hpp"
#include "ServerTcpAsynch/AsynchTcpService.hpp"
#include "ServerTcpAsynchEPoll/ServerTcpAsynchEPoll.hpp"
#include "ServerTcpAsynchURing/ServerTcpAsynchURing.hpp"

#include "ssh/SshExperiments.h"
#include "SSL/SSL_Experiments.h"
#include "SSL/TCP_SSL_Server.h"
#include "SSL/HTTPS_Server.h"
#include "SSL/HTTPS_AsyncServer.h"
#include "SSL/HTTPS_ServerMultithreaded.h"
#include "SSL/HTTPS_Server_ThreadPool.h"

#include "APIServer/APIServer.h"
#include "SecureMessagingProxy/MessagingProxy.h"

#include "MAC_Address/MacAddress.hpp"

#include "UnixDomainSockets/UDSAsynchServer.h"


#include "JRPC/JRPC.h"

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Clients::Tests();
    // UDPClient::TestAll();

    // DNS_ResponseParser::TestAll();
    mac_address::TestAll();

    // RawSocketsTests::TestAll();
    // CaptureTraffic::TestAll();
    // ARPTester::TestAll();
    // BuildAndSendPacketsTests::TestAll();

    // Experiments::Tests();

    // UtilitiesTests::TestAll();

    // NetLink::TestAll();

    // Networking::Utilities::TestAll();

    // TestWebServer::Tests();
    // SimpleServerSelect::Tests();
    // SocketTCPServer::Tests();
    // EPollTCPServer::Tests();
    // EPollTCPServerTwo::Tests();
    // EPollTCPServerEx::Tests();
    // EPollTCPServerDebug::Tests();
    // EPollTCPServerContext::TestAll();
    // EPollTCPServerContextEx::TestAll();
    // EPollTCPServerMultithreaded::Tests();

    // tcp_server::TestAll();
    // AsynchTcpService::TestAll();
    // server_tcp_asynch_epoll::TestAll();
    // server_tcp_asynch_uring::TestAll();

    // PcapAnalyzer::TestAll();

    // SshExperiments::TestAll(argc, argv);

    // SSL_Experiments::TestAll();
    // TCP_SSL_Server::TestAll();
    // HTTPS_Server::TestAll();
    // HTTPS_AsyncServer::TestAll();
    // HTTPS_ServerMultithreaded::TestAll();
    // HTTPS_Server_ThreadPool::TestAll();

    // APIServer::TestAll();

    // JRPC::TestAll();

    // MessagingProxy::TestAll();

    // UDSAsynchServer::TestAll();

    // CurpCpp::TestAll();

    return EXIT_SUCCESS;
}

