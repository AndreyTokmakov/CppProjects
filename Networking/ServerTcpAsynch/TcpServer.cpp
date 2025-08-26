/**============================================================================
Name        : TcpServer.cpp
Created on  : 23.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TcpServer.cpp
============================================================================**/

#include "TcpServer.hpp"


namespace tcp_server_asynch
{
    int32_t Error(const std::string_view text)
    {
        std::cerr << text << ". Error = " << errno << "(" << Utilities::errCodeToStr(errno) << ")\n";
        return SOCKET_ERROR;
    }
}