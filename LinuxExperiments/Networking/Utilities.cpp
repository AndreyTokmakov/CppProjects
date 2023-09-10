/**============================================================================
Name        : ExecutorAdapter.h
Created on  : 01.09.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : ExecutorAdapter
============================================================================**/

#include "Utilities.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <climits>

#include <iostream>
#include <thread>

namespace {

    constexpr uint32_t RECV_BUFFER_SIZE { 2048 };
    constexpr int INVALID_SOCKET { -1 };
    constexpr int SOCKET_ERROR { -1 };

    struct Socket {
        int socket { INVALID_SOCKET};

        Socket(int s): socket {s} {
        }

        Socket& operator=(int s) {
            if (socket != s)
                closeSocket(socket);

            socket = s;
            return *this;
        }

        ~Socket() {
            closeSocket(socket);
        }

        [[nodiscard]]
        inline bool isValid() const noexcept {
            return INVALID_SOCKET == socket;
        }

        [[nodiscard]]
        operator int() const { // No explicit
            return socket;
        }


        explicit operator bool() const noexcept {
            return (INVALID_SOCKET != socket);
        }

    private:

        static void closeSocket(int s)
        {
            std::cout << "Closing socket(" << s << ")\n";
            if (INVALID_SOCKET != s && SOCKET_ERROR == ::close(s)) {
                std::cout << "close() function failed with error: " << errno << std::endl;
            }
        }
    };

}


namespace Networking::Utilities
{
    [[nodiscard("Don't forget to use the return value somehow.")]]
    std::string HostToIp(std::string_view host) noexcept
    {
        const hostent* hostname { gethostbyname(host.data()) };
        if (hostname)
            return std::string { inet_ntoa(**(in_addr**)hostname->h_addr_list) };
        return std::string {};
    }

    void EstablishTCPConnection(std::string_view ipAddr,
                                uint16_t port = 80)
    {
        Socket socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (!socket) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        sockaddr_in server {PF_INET, htons(port), {.s_addr = inet_addr(ipAddr.data())}, {}};

        std::cout << "Connecting to server..." << std::endl;
        int error = ::connect(socket, (sockaddr*)&server, sizeof(server));
        if (SOCKET_ERROR == error) {
            std::cout << "Connect function failed with error: " << errno << std::endl;
            return;
        } else {
            std::cout << "Connected " << ipAddr << ":" << port << '\n';
        }
    }


    void ConnectToHostAndSleep(std::string_view hostName,
                               uint16_t port,
                               const std::chrono::duration<int64_t>& timeout)
    {
        const std::string ipAddr { HostToIp(hostName) };
        if (ipAddr.empty())
            return;

        Socket socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (!socket) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }


        sockaddr_in server {PF_INET, htons(port), {.s_addr = inet_addr(ipAddr.data())}, {}};

        std::cout << "Connecting to server..." << std::endl;
        int error = ::connect(socket, (sockaddr*)&server, sizeof(server));
        if (SOCKET_ERROR == error) {
            std::cout << "Connect function failed with error: " << errno << std::endl;
            return;
        } else {
            std::cout << "Connected " << ipAddr << ":" << port << '\n';
        }
        std::this_thread::sleep_for(timeout);
    }
};


void Networking::Utilities::TestAll()
{
    // EstablishTCPConnection("87.250.250.242", 80);
    // GetHostName();

    ConnectToHostAndSleep("ya.ru", 80, std::chrono::seconds(5));
};