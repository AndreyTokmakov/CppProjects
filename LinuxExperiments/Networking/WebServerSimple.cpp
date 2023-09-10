//============================================================================
// Name        : WebServerSimple.h
// Created on  : 05.06.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : WebServerSimple
//============================================================================

#include "WebServerSimple.h"

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cerrno>
#include <netdb.h>

#define INVALID_SOCKET (-1)

namespace WebServerSimple
{
    void closeSocket(int s) {
        std::cout << "Closing socket (" << s << ")\n";
    }

    struct Ctrl final {

    };

    struct Socket final {
        int handle { INVALID_SOCKET };
        uint32_t useCount { 1 };

        Socket(int s): handle {s} {
            std::cout << "Socket(" << s << ") created\n";
        }

        Socket(const Socket &sock): handle {sock.handle} {

        }

        /*
        Socket(Socket && obj) : value(std::exchange(obj.value, 0)) {
            std::cout << __FUNCTION__ << " [Move contructor]. (" << obj.value << ")" << std::endl;
        }*/


        ~Socket() {
            std::cout << "~Socket(" << handle << ")[" << useCount << " -> " << useCount - 1<< "]\n";
            --useCount;
            if (INVALID_SOCKET != handle && 0 == useCount) {
                closeSocket(handle);
            }
        }

        /*
        Integer& operator=(const Integer& right) {
            std::cout << "[Copy assignment operator from Integer -> Integer]" << std::endl;
            if (this == &right) {
                return *this;
            }
            value = right.value;
            return *this;
        }

        Integer& operator=(int val) {
            std::cout << "[Copy assignment operator from int -> Integer]" << std::endl;
            this->value = val;
            return *this;
        }

        Integer& operator=(Integer&& integer) noexcept {
            std::cout << "[Move assignment operator]" << std::endl;
            this->value = std::exchange(integer.value, 0);
            return *this;
        }
        */
    };

    void foo([[maybe_unused]] Socket sock) {

    }


    //--------------------------------------------------------------------------------

    void runServer()
    {
        constexpr size_t bufferSize {128};
        constexpr int port = 52525;

        sockaddr_in socket_address {};
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons(port);
        socket_address.sin_addr.s_addr = htonl(INADDR_ANY);


        /* Create a socket for internet communication */
        int hSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (-1 == hSocket) {
            std::cout << "socket() failed. Error = " << errno << std::endl;
            return;
        }

        int result = ::bind(hSocket, reinterpret_cast<sockaddr*>(&socket_address), sizeof(socket_address));
        if (-1 == result) {
            std::cout << "Failed to bind socket. Error = " << errno << std::endl;
            // TODO: Exit! Close socket!
        } else {
            std::cout << "Socket is bind to 127.0.0.1:" << port << std::endl;
        }

        result = ::listen(hSocket, 128);  // TODO: 128 to const
        if (-1 == result) {
            std::cout << "Failed to listen socket. Error = " << errno << std::endl;
            // TODO: Exit! Close socket!
        } else {
            std::cout << "Listening socket" << std::endl;
        }

        const std::string html = "<html><head><title>AndTokmServer</title></head><body BGCOLOR='grey'>Welcome</body></html>\r\n";
        std::string resp = "HTTP/1.1 200 OK\r\n";
        resp += "Date: Wed, 11 Feb 2022 11:20:59 GMT\r\n";
        resp += "Server: AndTokmServer\r\n";
        resp += "X-Powered-By: PHP/5.2.4-2ubuntu5wm1\r\n";
        resp += "Last-Modified: Wed, 11 Feb 2009 11:20:59 GMT\r\n";
        resp += "Content-Language: ru\r\n";
        resp += "Content-Type: text/html; charset=utf-8\r\n";
        resp += "Content-Length: " + std::to_string(html.size()) + "\r\n";
        resp += "Connection: close\r\n\r\n";
        resp += html;

        int clientSocket = -1;
        size_t bytes = 0;
        std::string request;
        char buffer[bufferSize];
        while (true) {
            if (INVALID_SOCKET == (clientSocket = ::accept(hSocket, nullptr, nullptr))) {
                std::cout << "Failed to create client socket, Error = " << errno << std::endl;
            }
            request.clear();
            while ((bytes = ::recv(clientSocket, buffer, bufferSize, 0)) > 0) {
                request.append(buffer, bytes);
                if (bufferSize > bytes)
                    break;
            }
            std::cout << request;


            if (std::string::npos != request.find("HTTP/") && std::string::npos != request.find("\r\n\r\n")) {
                send(clientSocket, const_cast<char*>(resp.c_str()), resp.length(), 0);
            }

#if 0
            if (0 != shutdown(clientSocket, SHUT_RDWR))
                std::cout << "Failed to close socket" << std::endl;
            if (0 != close(clientSocket))
                std::cout << "Failed to close socket" << std::endl;


            if ((bytes = recv(clientSocket, buffer, 1524, 0)) > 0) {
                request.assign(buffer, bytes);
                if (std::string::npos != request.find("HTTP/") && std::string::npos != request.find("\r\n\r\n")) {
                    send(clientSocket, const_cast<char*>(resp.c_str()), resp.length(), 0);
                }
                if (0 != shutdown(clientSocket, SHUT_RDWR))
                    std::cout << "Failed to close socket" << std::endl;
                if (0 != close(clientSocket))
                    std::cout << "Failed to close socket" << std::endl;
            }
#endif
        }
    }
};




void WebServerSimple::TestAll()
{
    // runServer();

    // std::cout << sizeof(Socket) << std::endl;

    Socket s = 123;
    foo(s);
};
