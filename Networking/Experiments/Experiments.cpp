/**============================================================================
Name        : Experiments.cpp
Created on  : 18.03.2022.
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Experiments
============================================================================**/

#include <iostream>
#include <string>
#include <cstring>
#include <array>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>           // close()
#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_RAW, INET_ADDRSTRLEN
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/if.h>           // struct ifreq
#include <linux/if_ether.h>   // ETH_P_ARP = 0x0806
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>

#include "Experiments.h"
#include "../Headers/EthernetHeader.h"

namespace {
    constexpr uint32_t RECV_BUFFER_SIZE { 2048 };
    constexpr int INVALID_SOCKET { -1 };
    constexpr int SOCKET_ERROR { -1 };
}

namespace Experiments
{
    std::array<unsigned char, 6> GetMacAddress(std::string_view interfaceName) {
        std::array<unsigned char, 6> addr {};
        if (const int sock = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_IP); -1 != sock) {
            ifreq s {};
            memcpy(s.ifr_name, interfaceName.data(), interfaceName.length());
            if (-1 != ioctl(sock, SIOCGIFHWADDR, &s))
                std::copy_n(s.ifr_addr.sa_data, 6, addr.data());
            ::close(sock);
        }
        return addr;
    }

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

};


void BindTwoSocketsOnTheSamePort()
{
    const int udpSocket = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (INVALID_SOCKET == udpSocket) {
        std::cerr << "Failed to create UDP socket. Error = " << errno << std::endl;
    } else {
        std::cout << "UDP Socket created" << std::endl;
    }

    const int tcpSocket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
    if (INVALID_SOCKET == tcpSocket) {
        std::cerr << "Failed to create TCP socket. Error = " << errno << std::endl;
    } else {
        std::cout << "TCP Socket created" << std::endl;
    }

    constexpr uint16_t port { 52525 };
    constexpr std::string_view hostAddress { "0.0.0.0" };

    const sockaddr_in server { PF_INET, htons(port) ,
                               {.s_addr = inet_addr(hostAddress.data())}};

    if (SOCKET_ERROR == ::bind(udpSocket, reinterpret_cast<const sockaddr*>(&server), sizeof(server))) {
        std::cerr << "Failed to bind UDP socket. Error = " << errno << std::endl;
        ::close(udpSocket);
    }

    if (SOCKET_ERROR == ::bind(tcpSocket, reinterpret_cast<const sockaddr*>(&server), sizeof(server))) {
        std::cerr << "Failed to bind UDP socket. Error = " << errno << std::endl;
        ::close(tcpSocket);
    }

    ::close(udpSocket);
    ::close(tcpSocket);
}


//	memset(&device, 0, sizeof(device));
//	memcpy(device.sll_addr, ifr.ifr_hwaddr.sa_data, 6 * sizeof (uint8_t)); // Copy source MAC address.

void Experiments::Tests()
{

    /*
    constexpr std::string_view interfaceName {"enp6s0"};
    unsigned char buffer[1024] {};

    sockaddr_ll device {AF_PACKET, 0, 0, 0, 0, 6, {0}};
    EthernetHeader* ethernetHeader {reinterpret_cast<EthernetHeader*>(&buffer[0]) };
    if (const int sock = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_IP); -1 != sock) {
        ifreq ifr {};
        std::copy_n(interfaceName.data(), interfaceName.length(), ifr.ifr_name);
        if (-1 != ioctl(sock, SIOCGIFHWADDR, &ifr)) {
            std::copy_n(ifr.ifr_addr.sa_data, 6, ethernetHeader->sourceMac);
            std::copy_n(ifr.ifr_addr.sa_data, 6, device.sll_addr);
        }

        // Find interface index from interface name and store index in struct sockaddr_ll device,
        // which will be used as an argument of sendto():
        if ((device.sll_ifindex = if_nametoindex(ifr.ifr_name)) == 0) {
            std::cout << "Failed to obtain interface index" << std::endl;
        }
        ::close(sock);
    }


    int socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (-1 == socket)
        std::cout << "Failed to create socket. Error = " << errno << std::endl;

    int bytes = sendto(socket, buffer, 34, 0, (sockaddr*)&device, sizeof (device));
    if (-1 == bytes)
        std::cout << "Error: " << errno << std::endl;



    ethernetHeader->PrintSourceMACAddres();
    std::cout << std::endl;

    */


    BindTwoSocketsOnTheSamePort();
};