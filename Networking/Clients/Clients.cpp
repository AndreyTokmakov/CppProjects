//============================================================================
// Name        : Clients.cpp
// Created on  : 06.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : main
//============================================================================


#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cerrno>
#include <netdb.h>

#include "Clients.hpp"

namespace
{
    constexpr uint32_t RECV_BUFFER_SIZE { 2048 };
    constexpr int INVALID_SOCKET { -1 };
    constexpr int SOCKET_ERROR { -1 };
}

namespace ClientsTests
{
    void ConnectAndSendRequest()
    {
        constexpr uint16_t port { 52525 };
        const std::string_view host {"127.0.0.1"};

        int socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == socket) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        const sockaddr_in server {PF_INET, htons(port), {.s_addr = inet_addr(host.data())}, {}};
        std::cout << "Connecting to server..." << std::endl;
        int error = ::connect(socket, (sockaddr*)&server, sizeof(server));
        if (error == SOCKET_ERROR) {
            std::cout << "Connect function failed with error: " << errno << std::endl;
            ::close(socket);
            return;
        }
        else {
            std::cout << "Connected.\n";
        }

        const std::string httpRequest = "GET / HTTP/1.1\r\nHost: test\r\n\r\n";
        int bytes = ::send(socket, httpRequest.c_str(), httpRequest.length(), 0);
        std::cout << bytes << " bytes send\n";

        std::string respose;
        char buffer[RECV_BUFFER_SIZE];
        while (bytes > 0) {
            bytes = ::recv(socket, buffer, RECV_BUFFER_SIZE, 0);
            if (SOCKET_ERROR == bytes) {
                break;
            }
            std::cout << bytes << " bytest received" << std::endl;
            // std::cout << ::recv(socket, buffer, RECV_BUFFER_SIZE, MSG_PEEK) << " bytest left to read" << std::endl;
            respose.append(buffer, bytes);
            std::cout << respose << std::endl;
        }


        std::cout << respose << std::endl;

        if (SOCKET_ERROR == ::close(socket)) {
            std::cout << "Closesocket function failed with error: " << errno << std::endl;
        }
        else {
            std::cout << "Connection closed" << std::endl;
        }
    }
}


namespace HTTP {

    struct HTTPHeadrs {
        constexpr static char CONTENT_LENGTH[] = "Content-length";
    };

    uint32_t Extract_Content_Length(const std::string& headers)
    {
        auto start = headers.find(HTTPHeadrs::CONTENT_LENGTH);
        if (std::string::npos == start)
            return 0;

        auto end = headers.find("\r\n", start + std::size(HTTPHeadrs::CONTENT_LENGTH));
        if (std::string::npos == end)
            return 0;

        const std::string_view len = std::string_view(headers).
                substr(start + std::size(HTTPHeadrs::CONTENT_LENGTH) + 1,
                       end - start - std::size(HTTPHeadrs::CONTENT_LENGTH) - 1);

        return atoi(len.data());
    }


    // TODO:
    void Send_GET_Request()
    {
        constexpr uint16_t port { 52525 };
        const std::string_view host {"127.0.0.1"};

        int socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == socket) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        const sockaddr_in server {PF_INET, htons(port), {.s_addr = inet_addr(host.data())}, {}};
        int error = ::connect(socket, (sockaddr*)&server, sizeof(server));
        if (error == SOCKET_ERROR) {
            std::cout << "Failed. Error =  " << errno << std::endl;
            ::close(socket);
            return;
        }
        else {
            std::cout << "Done.\n";
        }

        const std::string httpRequest = "GET / HTTP/1.1\r\nHost: test\r\n\r\n";
        int bytes = ::send(socket, httpRequest.c_str(), httpRequest.length(), 0);

        std::string response;
        char buffer[RECV_BUFFER_SIZE];


        // HACK: 1. Read only HEADERs part
        //       2. Read remaing part according 'Content-length' size

        uint32_t payloadPos = 0;
        while (0 < (bytes = ::recv(socket, buffer, RECV_BUFFER_SIZE, 0))) {
            response.append(buffer, bytes);
            if (payloadPos = response.find("\r\n\r\n"); std::string::npos != payloadPos) {
                payloadPos += 4;
                break;
            }
        }

        uint32_t payloadBytes = Extract_Content_Length(response);
        uint32_t leftToRead{ payloadBytes + static_cast<uint32_t>(response.length()) - payloadPos };
        if (leftToRead) {
            response.reserve(response.length() + leftToRead);
        }

        while (leftToRead > 0 && bytes > 0) {
            bytes = ::recv(socket, buffer, leftToRead, 0);
            response.append(buffer, bytes);
            leftToRead -= bytes;
        }

        std::cout << response << std::endl;;
        if (SOCKET_ERROR == ::close(socket)) {
            std::cout << "Closesocket function failed with error: " << errno << std::endl;
        }
    }
}

namespace Clients::ClickHouse
{
    struct Socket {
        int handle { INVALID_SOCKET };

        Socket(int s = INVALID_SOCKET) : handle {s} {
        }

        Socket& operator=(int s) {
            if (handle != s)
                closeSocket(handle);

            handle = s;
            return *this;
        }

        operator int() const { // No explicit
            return handle;
        }

        ~Socket() {
            closeSocket(handle);
        }

    private:

        static void closeSocket(int s) {
            // std::cout << "Closing socket(" << s << ")\n";
            if (INVALID_SOCKET != s && SOCKET_ERROR == ::close(s)) {
                std::cout << "close() function failed with error: " << errno << std::endl;
            }
        }
    };

    void SendRequest() {
        constexpr uint16_t port { 8123 };
        constexpr std::string_view host {"127.0.0.1"};

        const Socket socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == socket) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        const sockaddr_in server { PF_INET, htons(port) , {.s_addr = inet_addr(host.data())}, {}};

        std::cout << "Connecting to server..." << std::endl;
        int error = ::connect(socket, (sockaddr*)&server, sizeof(server));
        if (error == SOCKET_ERROR) {
            std::cout << "Connect function failed with error: " << errno << std::endl;
            return;
        } else {
            std::cout << "Connected.\n";
        }

        const std::string httpRequest = "GET / HTTP/1.1\r\nHost: localhost:8123\r\n\r\n";
        long bytes = ::send(socket, httpRequest.c_str(), httpRequest.length(), 0);

        std::string response;
        char buffer[RECV_BUFFER_SIZE];

        // HACK: 1. Read only HEADERs part
        //       2. Read remaing part according 'Content-length' size

        while (bytes > 0) {
            bytes = ::recv(socket, buffer, RECV_BUFFER_SIZE, 0);
            response.assign(buffer, bytes);
            std::cout << "==================================== " << bytes << " ==========================" << std::endl;
            std::cout << response << std::endl;
        }
    }

    // INFO: CREATE DATABASE src
    // INFO: CREATE TABLE strings (short String, long String) ENGINE Memory;
    // INFO: insert into strings values('aaaa', 'aaaaa');

    void ExecuteQuery() {
        constexpr uint16_t port { 8123 };
        constexpr std::string_view host {"127.0.0.1"};

        const Socket socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == socket) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        const sockaddr_in server {PF_INET, htons(port), {.s_addr = inet_addr(host.data())}, {}};

        std::cout << "Connecting to server..." << std::endl;
        int error = ::connect(socket, (sockaddr*)&server, sizeof(server));
        if (error == SOCKET_ERROR) {
            std::cout << "Connect function failed with error: " << errno << std::endl;
            return;
        } else {
            std::cout << "Connected.\n";
        }

        std::string sBinary;
        for (uint8_t i = 0; i < 10; i++)
            sBinary.append(reinterpret_cast<const char*>(&i), sizeof(i));
        // const std::string insert = "INSERT INTO tests.test_table (data) FORMAT RowBinary " + sBinary;
        const std::string query = "SELECT * FROM tests.strings;";


        const std::string request =
                "POST /? HTTP/1.1\r\n"
                "Host: localhost:8123\r\n"
                "Content-Length: " + std::to_string(query.size()) + "\r\n"
                "Content-Type: application/x-www-form-urlencoded\r\n\r\n"
                + query;
        long bytes = ::send(socket, request.c_str(), request.length(), 0);

        std::string response;
        char buffer[RECV_BUFFER_SIZE];

        // HACK: 1. Read only HEADERs part
        //       2. Read remaing part according 'Content-length' size

        while (bytes > 0) {
            bytes = ::recv(socket, buffer, RECV_BUFFER_SIZE, 0);
            response.assign(buffer, bytes);
            std::cout << "==================================== " << bytes << " ==========================" << std::endl;
            std::cout << response << std::endl;
        }
    }

    void ExecuteQuery_BadSize() {
        constexpr uint16_t port { 8123 };
        constexpr std::string_view host {"127.0.0.1"};

        const Socket socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == socket) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        const sockaddr_in server { PF_INET, htons(port),{.s_addr = inet_addr(host.data())}, {}};

        std::cout << "Connecting to server..." << std::endl;
        int error = ::connect(socket, (sockaddr*)&server, sizeof(server));
        if (error == SOCKET_ERROR) {
            std::cout << "Connect function failed with error: " << errno << std::endl;
            return;
        } else {
            std::cout << "Connected.\n";
        }

        std::string sBinary;
        for (uint8_t i = 1; i <= 30; i++)
            sBinary.append(reinterpret_cast<const char*>(&i), sizeof(i));
        const std::string insert = "INSERT INTO tests.test_table (data) FORMAT RowBinary " + sBinary;
        const std::string query = "SELECT * FROM src.strings;";


        const std::string request =
                "POST /? HTTP/1.1\r\n"
                "Host: localhost:8123\r\n"
                "Content-Length: " + std::to_string(insert.size()) + "\r\n"
                "Content-Type: application/x-www-form-urlencoded\r\n\r\n"
                + insert;

        std::cout << "LEN = " << insert.size() << std::endl;

        long bytes = ::send(socket, request.data(), request.length() - 20, 0);
        bytes = ::send(socket, request.data() + request.length() - 20, 10, 0);
        bytes = ::send(socket, request.data() + request.length() - 10, 10, 0);


        std::string response;
        char buffer[RECV_BUFFER_SIZE];
        while (bytes > 0) {
            bytes = ::recv(socket, buffer, RECV_BUFFER_SIZE, 0);
            response.assign(buffer, bytes);
            std::cout << "==================================== " << bytes << " ==========================" << std::endl;
            std::cout << response << std::endl;
        }
    }
}

void Clients::Tests() {

    // ClickHouse::SendRequest();
    // ClickHouse::ExecuteQuery();
    // ClickHouse::ExecuteQuery_BadSize();

    TcpAsyncClientEpoll::TestAll();
}
