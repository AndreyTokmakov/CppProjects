//============================================================================
// Name        : Networking.h
// Created on  : 25.04.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Networking
//============================================================================

#include "Networking.h"

#include "Poco/Net/DNS.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/StreamCopier.h"

#include <iostream>

using Poco::Net::DNS;
using Poco::Net::IPAddress;
using Poco::Net::HostEntry;

namespace Networking
{
    void ResolveDnsName()
    {
        const Poco::Net::HostEntry& entry = Poco::Net::DNS::hostByName("www.appinf.com");
        std::cout << "Canonical Name: " << entry.name() << std::endl;
        const Poco::Net::HostEntry::AliasList& aliases = entry.aliases();

        for (const auto &aliase : aliases)
            std::cout << "Alias: " << aliase << std::endl;

        const Poco::Net::HostEntry::AddressList& addrs = entry.addresses();
        for (const auto & addr : addrs)
            std::cout << "Address: " << addr.toString() << std::endl;
    }

    void Test() {
        Poco::Net::SocketAddress sa("www.appinf.com", 80);
        Poco::Net::StreamSocket socket(sa);
        Poco::Net::SocketStream str(socket);
        str << "GET / HTTP/1.1\r\n"
               "Host: www.appinf.com\r\n"
               "\r\n";
        str.flush();
        Poco::StreamCopier::copyStream(str, std::cout);
    }
}

namespace Networking::APITests
{
    void SendRequest()
    {
        Poco::Net::SocketAddress address {"0.0.0.0", 52525 };
        Poco::Net::StreamSocket socket(address);
        Poco::Net::SocketStream str(socket);
        str << "GET /debug HTTP/1.1\r\n"
               "Host: 0.0.0.0:52525\r\n"
               "Connection: close\r\n"
               "\r\n";
        str.flush();
        Poco::StreamCopier::copyStream(str, std::cout);
    }

    void SendRequestPost()
    {
        Poco::Net::SocketAddress address {"0.0.0.0", 52525 };
        Poco::Net::StreamSocket socket(address);
        Poco::Net::SocketStream str(socket);
        str << "POST /api/entities?param1=value1&param2=value2 HTTP/1.1\r\n"
               "Host: 0.0.0.0:52525\r\n"
               "\r\n";
        str.flush();
        Poco::StreamCopier::copyStream(str, std::cout);
    }
}


namespace Networking::Server
{
    void runServer()
    {
        Poco::Net::ServerSocket srv(8080); // does bind + listen
        while (true)
        {
            std::cout << "Got some\n";
            Poco::Net::StreamSocket ss = srv.acceptConnection();
            Poco::Net::SocketStream str(ss);
            str << "HTTP/1.0 200 OK\r\n"
                   "Content-Type: text/html\r\n"
                   "\r\n"
                   "<html><head><title>My 1st Web Server</title></head>"
                   "<body><h1>Hello, world!!!</h1></body></html>"
                << std::flush;
        }
    }
}

void Networking::TestAll()
{
    // ResolveDnsName();
    // Test();

    // APITests::SendRequest();
    // APITests::SendRequestPost();

    Server::runServer();
}

