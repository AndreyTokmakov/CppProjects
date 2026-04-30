/**============================================================================
Name        : TcpClientEpoll_Experimental.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "DateTimeUtilities.hpp"
#include "Clients.hpp"

#include <iostream>
#include <print>
#include <syncstream>

#include <coroutine>
#include <utility>
#include <unordered_map>
#include <vector>
#include <stdexcept>

#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#define LOG std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << ' '



void Clients::TcpAsyncClientEpoll::TestAll()
{
    LOG << "Test" << std::endl;

}
