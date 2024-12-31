/**============================================================================
Name        : MessagingProxy.cpp
Created on  : 31.12.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "MessagingProxy.h"

#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <string_view>

#include <array>
#include <vector>
#include <unordered_map>



#include "../Utilities/Utilities.h"

namespace
{
    constexpr int INVALID_SOCKET { -1 };
    constexpr int SOCKET_ERROR { -1 };
}

namespace MessagingProxy
{
    enum class MessagingProxy: uint8_t
    {
        Register,
        Delete,
        Ping,
        Message,
        Call,
        Media,
    };

    enum class MessagingStatus: uint8_t
    {
        Send,
        Received,
        Read,
        NotDelivered
    };

    struct Proxy
    {
        constexpr static uint32_t receiveBufferSize { 1024 };
        constexpr static std::string_view host {"0.0.0.0"};

        int serverSocket { -1 };
        uint16_t serverPort { 52525 };

        // TODO: Messages Queue
        // TODO: Thread to process messages?

        explicit Proxy(const uint16_t port = 52525) : serverPort { port }
        {
            serverSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (INVALID_SOCKET == serverSocket)
                throw std::runtime_error("Failed to create UDP socket");

            sockaddr_in server {PF_INET, htons(serverPort), {.s_addr = inet_addr(host.data())}, {}};
            if (SOCKET_ERROR == ::bind(serverSocket, reinterpret_cast<sockaddr*>(&server), sizeof(server))) {
                throw std::runtime_error("Failed to bind socket on port " + std::to_string(serverPort));
            }
        }

        void runServer()
        {
            sockaddr_in senderAddress {};
            socklen_t len = sizeof(senderAddress);
            std::string payload(receiveBufferSize, '0');

            while (true)
            {
                const ssize_t bytesReceived = ::recvfrom (serverSocket,
                                        payload.data() ,
                                        receiveBufferSize,
                                        0 ,
                                        reinterpret_cast<sockaddr*>(&senderAddress) , &len);
                payload.resize(bytesReceived);
                std::cout << payload << std::endl;
            }
        }
    };

}

// TODO:
//  - Store connected client | Statuses
//  - Forward message from one client to another
//  - How to handle when both A and B offline
//  - Store history only on the clients
//  - MySQL as storage for Clients
//  - TLS ???
//  - e2e messages Encryption

// TODO:
//  - UDP Server
//  - Send Heartbeats from client
//  - Send status request from Server --> Client | if Messages is send B from A
//  - Server PORT 50000
//  - Client PORT 50001
//  - When A search for B (using name and etc) server will ask each clients using UDP messages ?

// TODO: **** SERVER ****
//  - Online Clients list? | Sessions  [hash, {ip, port}]
//  - Register Client list: [ID, HASH] + ptr to the session ???
//  -

// TODO: Message types:
//  -  Use Protobuff ???

/**
Message:
{
    'type:': 1     <--- Binary ... not 'type'
    'message id':  <--- Unique Per Client
    'sender:':     <--- 'ID of originator USER ????'
    'dest:':       <--- 'ID of dest USER ????'
    'payload'      <--- if needed
}

**/


void MessagingProxy::TestAll()
{
    Proxy proxy;
    proxy.runServer();
}
