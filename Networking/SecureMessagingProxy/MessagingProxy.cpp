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

#include <nlohmann/json.hpp>
#include "../Utilities/Utilities.h"

namespace
{
    constexpr int INVALID_SOCKET { -1 };
    constexpr int SOCKET_ERROR { -1 };
}

namespace Common
{
    using namespace std::string_literals;

    enum class MessageType: uint8_t
    {
        Unknown = 0,
        Register,
        Delete,
        Ping,
        Message,
        Call,
        Media,
    };

    enum class MessageStatus: uint8_t
    {
        Unknown = 0,
        Send,
        Received,
        Read,
        NotDelivered
    };

    struct Message
    {
        MessageType type { MessageType::Ping };
        MessageStatus status { MessageStatus::Send };
        // Originator | ID
        // Destination | ID
        // Data
    };

    Message parseMessage(const std::string& strMessage)
    {
        const nlohmann::json jsonMessage = nlohmann::json::parse(strMessage);

        Message message;
        message.type = jsonMessage["t"].get<MessageType>();
        message.status = jsonMessage["s"].get<MessageStatus>();

        return message;
    }

    std::string toString(const MessageType msgType)
    {
        switch (msgType)
        {
            case MessageType::Register : return "Register"s;
            case MessageType::Delete : return "Delete"s;
            case MessageType::Ping : return "Ping"s;
            case MessageType::Message : return "Message"s;
            case MessageType::Call : return "Call"s;
            case MessageType::Media : return "Media"s;
            default: return "Unknown"s;
        }
    }

    std::string toString(const MessageStatus msgStatus)
    {
        switch (msgStatus)
        {
            case MessageStatus::Send : return "Send"s;
            case MessageStatus::Received : return "Received"s;
            case MessageStatus::Read : return "Read"s;
            case MessageStatus::NotDelivered : return "NotDelivered"s;
            default: return "Unknown"s;
        }
    }

    std::string toString(const Message& message)
    {
        return std::string(R"({ "type":")") + toString(message.type) +
            R"(", "status": ")" +  toString(message.status) + R"(" })";
    }
}


namespace MessagingProxy
{
    using namespace Common;

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
                                        reinterpret_cast<sockaddr*>(&senderAddress),
                                        &len);
                payload.resize(bytesReceived);

                std::cout << len << std::endl;

                const Message message = parseMessage(payload);
                std::cout << toString(message) << std::endl;
            }
        }
    };
}



namespace Tests
{
    using namespace MessagingProxy;
    using namespace Common;

    void runServer()
    {
        Proxy proxy;
        proxy.runServer();
    }

    void parseTest()
    {
        const std::string strMessage = R"({"t": 1, "s": 1})";
        const Message message = parseMessage(strMessage);

        std::cout << toString(message) << std::endl;
    }
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

void MessagingProxy::TestAll()
{
    Tests::runServer();
    // Tests::parseTest();
}
