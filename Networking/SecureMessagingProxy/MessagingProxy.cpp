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
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <string_view>
#include <format>
#include <print>

#include <array>
#include <vector>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include "../Utilities/Utilities.h"

namespace
{
    constexpr int INVALID_SOCKET { -1 };
    constexpr int SOCKET_ERROR { -1 };

    using Socket = int32_t;
    using UserId = uint32_t;
    using HashType = size_t;
    using IdType   = size_t;
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
        MessageUpdate, /// Rename ???
        Call,
        Media,
    };

    // TODO: Shall we have a Delivery Status ?
    enum class MessageStatus: uint8_t
    {
        Unknown = 0,
        Success,
        Failed,
        Send,
        Received,
        Read,
        NotDelivered
    };

    struct Message
    {
        MessageType type { MessageType::Ping };
        MessageStatus status { MessageStatus::Send };
        IdType userId { 0 };       /// No Need for: Register
        HashType originator { 0 }; /// User Hash ?? - non need when UserID is given
        IdType messageID { 0 };    /// If failed ---> messageID is the OF the failed message
        IdType destUserID { 0 };   /// non need when Register, Delete,
        // Data
        // Timestamp
    };

    Message parseMessage(const std::string& strMessage)
    {
        const nlohmann::json jsonMessage = nlohmann::json::parse(strMessage);

        Message message;
        message.type = jsonMessage["t"].get<decltype(message.type)>();
        message.status = jsonMessage["s"].get<decltype(message.status)>();
        message.userId = jsonMessage["ui"].get<decltype(message.userId)>();
        message.originator = jsonMessage["o"].get<decltype(message.originator)>();
        message.messageID = jsonMessage["mi"].get<decltype(message.messageID)>();
        message.destUserID = jsonMessage["di"].get<decltype(message.destUserID)>();

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
            case MessageType::MessageUpdate : return "MessageUpdate"s;
            case MessageType::Call : return "Call"s;
            case MessageType::Media : return "Media"s;
            default: return "Unknown"s;
        }
    }

    std::string toString(const MessageStatus msgStatus)
    {
        switch (msgStatus)
        {
            case MessageStatus::Success : return "Success"s;
            case MessageStatus::Failed : return "Failed"s;
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
            R"(", "Status": ")" +  toString(message.status) +
            R"(", "User ID": )" +  std::to_string(message.userId) +
            R"(", "Originator": )" +  std::to_string(message.originator) +
            R"(", "Message ID": )" +  std::to_string(message.messageID) +
            R"(", "DestUserID": )" +  std::to_string(message.destUserID) +
            R"( })";
    }
}


namespace MessagingProxy
{
    using namespace Common;

    struct UserInfo
    {
        UserId userId { 0 };
        HashType userName { 0 };
        HashType password { 0 };

        UserInfo(const UserId id, const HashType uname, const HashType passwd):
            userId { id }, userName { uname }, password { passwd }
        {
            std::print("Mismatch count: {}\n", userId);
        }
    };

    // TODO:
    //  - Field: Last Online
    //  - Field: Status
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

        Socket serverSocket { -1 };
        uint16_t serverPort { 52525 };

        // TODO: Create struct Session (to store list of active sockaddr_in and etc)
        std::unordered_map<HashType, Session> sessions {};
        std::unordered_map<UserId, UserInfo> users {};

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

        ~Proxy()
        {
            ::close(serverSocket);
        }

        // TODO: User std::expected as return value
        void handleReceivedData(const std::string& data,
                                const sockaddr_in& senderAddress)
        {
            const Message message = parseMessage(data);
            std::cout << toString(message) << std::endl;

            if (MessageType::Register == message.type)
            {
                const auto& [iter, created] = users.try_emplace(1, message.userId, message.originator, 0);
                if (created) {
                    std::cout << "New User created(Id:" << iter->second.userId << ")"<< std::endl;
                }
                else {
                    std::cout << "Handling existing user: ID = " << message.userId<< std::endl;
                }
            }
            else if (MessageType::Message == message.type)
            {
                // TODO: Add session
                if (const auto& [itSession, nonExisting] = sessions.try_emplace(1, senderAddress); nonExisting)
                {
                    std::cout << "New Session created" << std::endl;
                }
            }
        }

        /// TODO: Use select/poll/epoll instead od ::recvfrom(....) ???
        [[noreturn]]
        void runServer()
        {
            sockaddr_in senderAddress {};
            socklen_t len = sizeof(senderAddress);
            std::array<uint8_t, receiveBufferSize> buffer {};
            std::string payload {};
            ssize_t bytesReceived { -1 };

            while (true)
            {
                bytesReceived = ::recvfrom(serverSocket,
                                           buffer.data() ,receiveBufferSize, 0,
                                           reinterpret_cast<sockaddr*>(&senderAddress), &len);
                if (SOCKET_ERROR != bytesReceived)
                {
                    payload.assign(reinterpret_cast<const char*>(buffer.data()), bytesReceived);
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

    void parseTest_BytesArray()
    {
        const std::array<uint8_t, 1024> bytes {'{','"','i','d','"',':','1','2','3','4','5','}'};
        std::string strMessage;

        {
            std::string_view payload(reinterpret_cast<const char*>(bytes.data()), 12);
            const nlohmann::json jsonMessage = nlohmann::json::parse(payload);
            std::cout << jsonMessage << std::endl;
        }

        {
            strMessage.assign(reinterpret_cast<const char*>(bytes.data()), 12);
            const nlohmann::json jsonMessage = nlohmann::json::parse(strMessage);
            std::cout << jsonMessage << std::endl;
        }
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

    void HashingTest()
    {
        std::string username = "@andtokm";
        const HashType hashValue = std::hash<decltype(username)>{}(username);
        std::cout << hashValue << std::endl;
    }

    void String_View_from_Bytes()
    {
        const std::array<uint8_t, 1024> bytes { 'h', 'e', 'l', 'l', 'o'};
        std::string_view payload(reinterpret_cast<const char*>(bytes.data()), 5);


        std::cout << payload << std::endl;
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

// TODO:
//  - How to handle message Update

// TODO: **** Registration | Restore password | Authorize from the different device ****
//  - Store Hashed [userID, password]
//  - ????
//  ----- Client -----
//  - No UserNameHashCode (new device) ---> Send REGISTER messages with ClientGeneratedUserName
//  - Have UserNameHashCode ---> No need to send REGISTER

void MessagingProxy::TestAll()
{
    Tests::runServer();

    // Tests::parseTest();
    // Tests::parseTest_BytesArray();
    // Tests::emplaceSessionTest();
    // Tests::HashingTest();
    // Tests::String_View_from_Bytes();
}
