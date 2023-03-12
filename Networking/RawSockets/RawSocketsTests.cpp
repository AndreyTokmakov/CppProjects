//============================================================================
// Name        : RawSocketsTests.h
// Created on  : 14.03.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : RawSocketsTests
//============================================================================

#include "RawSocketsTests.h"

#include <iostream>
#include <string>
#include <cstring>

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

#include "../Headers/EthernetHeader.h"
#include "../Headers/IPv4Header.h"
#include "../Headers/TCPHeader.h"

namespace {
    constexpr uint32_t RECV_BUFFER_SIZE { 2048 };
    constexpr int INVALID_SOCKET { -1 };
    constexpr int SOCKET_ERROR { -1 };
}


namespace RawSocketsTests
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

    // TODO: Refactor: HEX ??
    bool SetMACAddress(const char* s, unsigned char* d)
    {
        unsigned u[6], i;
        if (sscanf(s, "%02x:%02x:%02x:%02x:%02x:%02x", u+0, u+1, u+2, u+3, u+4, u+5) != 6)
            return false;
        for (i = 0; i < 6; i++) {
            if (u[i] > 255)
                return false;;
            d[i] = u[i];
        }
        return true;
    }

    // TODO: Refactor
    void PrintMACAddres(unsigned char* mac)
    {
        for (int i = 0; i < 5; i++)
            printf ("%02x:", mac[i]);
        printf ("%02x", mac[5]);
    }


    sockaddr_ll yResolveInterfaceAddress(std::string_view interfaceName) {
        sockaddr_ll device {};

        /** Submit request for a socket descriptor to look up interface. **/
        int socketHandle = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
        if (socketHandle < 0) {
            std::cout << "Failed to create socket" << std::endl;
            throw std::runtime_error("Ohh shit!");
        }
        /** Interface request structure used for socket ioctl's: **/
        ifreq ifr {};
        memset (&device, 0, sizeof (device));
        memset (&ifr, 0, sizeof (ifr));
        snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interfaceName.data());
        if (ioctl (socketHandle, SIOCGIFHWADDR, &ifr) < 0) {
            std::cout << "Failed to get source MAC address" << std::endl;
            throw std::runtime_error("Ohh shit!");
        }
        if (-1 == close(socketHandle)) {
            std::cout << "Failed to close socket" << std::endl;
            throw std::runtime_error("Ohh shit!");
        }
        /** **/
        memset(&device, 0, sizeof(device));
        memcpy(device.sll_addr, ifr.ifr_hwaddr.sa_data, 6 * sizeof (uint8_t)); // Copy source MAC address.

        /** Find interface index from interface name and store index in struct sockaddr_ll device, which will be used as an argument of sendto(): **/
        if ((device.sll_ifindex = if_nametoindex(ifr.ifr_name)) == 0) {
            std::cout << "Failed to obtain interface index" << std::endl;
            throw std::runtime_error("Ohh shit!");
        }
        /** Fill out sockaddr_ll: **/
        device.sll_family = AF_PACKET;
        device.sll_halen = 6;

        return device;
    }

};

void RawSocketsTests::TestAll()
{
    Socket socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (-1 == socket)
        std::cout << "Failed to create socket. Error = " << errno << std::endl;

    [[maybe_unused]]
    sockaddr_ll device = yResolveInterfaceAddress("enp6s0");
};
