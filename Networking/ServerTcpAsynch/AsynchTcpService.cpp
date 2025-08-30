/**============================================================================
Name        : AsynchTcpService.cpp
Created on  : 26.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AsynchTcpService.cpp
============================================================================**/

#include "AsynchTcpService.hpp"
#include "TcpServer.hpp"

#include <iostream>
#include <string_view>
#include <vector>

namespace
{
    struct Processor
    {
        bool process(common::Session& session)
        {
            using enum common::State;
            using namespace std::string_literals;

            // std::cout << "Processing Session { socket: " << session.socket << "}\n";
            session.response = "Response ["s.append(session.request.asString()).append(1, ']') ;

            session.request.clear();
            session.state = ReadyToWrite;

            return true;
        }
    };
}


// TODO:
//  1. Logger ?
//  2. Keep some alive session in Table (avoid re-creation)

void AsynchTcpService::TestAll()
{
    Processor processor;
    tcp_server_asynch::TCPServer<Processor> server {"0.0.0.0", 52525, processor};
    server.initialize();
    server.runServer();
}