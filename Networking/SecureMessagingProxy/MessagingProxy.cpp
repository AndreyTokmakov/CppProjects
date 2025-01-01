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
        size_t userId { 0 };
        // MessageID
        // Originator | ID - Identifier
        // Destination | ID
        // Data
    };

    Message parseMessage(const std::string& strMessage)
    {
        const nlohmann::json jsonMessage = nlohmann::json::parse(strMessage);

        Message message;
        message.type = jsonMessage["t"].get<decltype(message.type)>();
        message.status = jsonMessage["s"].get<decltype(message.status)>();
        message.userId = jsonMessage["id"].get<decltype(message.userId)>();

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
            R"(", "status": ")" +  toString(message.status) +
            R"(", "id": )" +  std::to_string(message.userId) +
            R"( })";
    }
}


namespace MessagingProxy
{
    using namespace Common;

    struct Session
    {
        sockaddr_in address { };

        explicit Session(const sockaddr_in& addr) : address { addr } {
        }

        Session(const Session& other)  = default;
        Session& operator=(const Session& other) = default;
        Session(Session&& other) noexcept = default;
        Session& operator=(Session&& other) noexcept = default;
        ~Session() = default;
    };

    struct Proxy
    {
        constexpr static uint32_t receiveBufferSize { 1024 };
        constexpr static std::string_view host {"0.0.0.0"};

        int serverSocket { -1 };
        uint16_t serverPort { 52525 };

        // TODO: Create struct Session (to store list of active sockaddr_in and etc)
        std::unordered_map<size_t, Session> sessions {};

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

        void handleReceivedData(const std::string& data,
                                const sockaddr_in& senderAddress)
        {
            // TODO: Store session [IP, Port] - 6 bytes?
            const Message message = parseMessage(data);

            const auto& [itSession, nonExisting] = sessions.emplace(1, senderAddress);
            if (nonExisting)
            {
                std::cout << "New Session created" << std::endl;
                std::cout << toString(message) << std::endl;
            }
        }

        void runServer()
        {
            sockaddr_in senderAddress {};
            socklen_t len = sizeof(senderAddress);
            std::string payload(receiveBufferSize, '0');
            ssize_t bytesReceived { -1 };

            while (true)
            {
                bytesReceived = ::recvfrom(serverSocket,payload.data() ,receiveBufferSize, 0,
                                           reinterpret_cast<sockaddr*>(&senderAddress), &len);
                if (SOCKET_ERROR != bytesReceived) {
                    payload.resize(bytesReceived);
                    handleReceivedData(payload, senderAddress);
                } else  {
                    std::cerr << "Failed to receive data from server" << std::endl;
                }
            }
        }
    };
}



namespace Tests
{
    using namespace MessagingProxy;
    using namespace Common;

    struct TestSession
    {
        int socket { INVALID_SOCKET };

        explicit TestSession(const int socket) : socket { socket } {
            std::cout << "TestSession(" << socket << ")" << std::endl;
        }

        TestSession(const TestSession& other) : socket { other.socket } {
            std::cout << "TestSession(const TestSession& other)(" << socket << ")" << std::endl;
        }

        TestSession& operator=(const TestSession& other)
        {
            socket = other.socket;
            std::cout << "TestSession& operator=(const TestSession& other)(" << socket << ")" << std::endl;
            return *this;
        }

        TestSession(TestSession&& other) noexcept: socket { std::exchange(other.socket, INVALID_SOCKET) } {
            std::cout << "TestSession(const TestSession& other)(" << socket << ")" << std::endl;
        }

        TestSession& operator=(TestSession&& other) noexcept
        {
            socket = std::exchange(other.socket, INVALID_SOCKET);
            std::cout << "TestSession& operator=(TestSession&& other) noexcept (" << socket << ")" << std::endl;
            return *this;
        }

        ~TestSession() {
            std::cout << "~TestSession(" << socket << ")" << std::endl;
            socket = INVALID_SOCKET;
        }
    };

    struct TestSession2
    {
        sockaddr_in address { };

        explicit TestSession2(const sockaddr_in& addr) : address { addr } {
            std::cout << "TestSession2(" << "address" << ")" << std::endl;
        }

        TestSession2(const TestSession2& other) : address { other.address } {
            std::cout << "TestSession2(const TestSession2& other)(" << "address" << ")" << std::endl;
        }

        TestSession2& operator=(const TestSession2& other)
        {
            address = other.address;
            std::cout << "TestSession2& operator=(const TestSession2& other)(" << "address" << ")" << std::endl;
            return *this;
        }

        TestSession2(TestSession2&& other) noexcept: address { other.address } {
            std::cout << "TestSession2(const TestSession2& other)(" << "address" << ")" << std::endl;
        }

        TestSession2& operator=(TestSession2&& other) noexcept
        {
            //address = std::exchange(other.address, INVALID_SOCKET);
            std::cout << "TestSession2& operator=(TestSession2&& other) noexcept (" << "address" << ")" << std::endl;
            return *this;
        }

        ~TestSession2() {
            std::cout << "~TestSession2(" << "address" << ")" << std::endl;
            //address = INVALID_SOCKET;
        }
    };

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

    void emplaceSessionTest()
    {
        std::unordered_map<size_t, TestSession2> sessions {};
        sockaddr_in senderAddress {};

        {
            const auto& [iter, ok] = sessions.emplace(1, senderAddress);
            std::cout << std::boolalpha << ok << std::endl;
        }

        {
            const auto& [iter, ok] = sessions.try_emplace(1, senderAddress);
            std::cout << std::boolalpha << ok << std::endl;
        }

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
    // Tests::emplaceSessionTest();
}
