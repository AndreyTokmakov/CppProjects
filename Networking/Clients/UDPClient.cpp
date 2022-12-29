/**============================================================================
Name        : UDPClient.cpp
Created on  : 26.12.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : UDPClient
============================================================================**/

#include "UDPClient.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>           // close()
#include <cstring>            // strcpy, memset(), and memcpy()
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
#include <cerrno>            // errno, perror()


#include <iostream>
#include "../Utilities/Utilities.h"

namespace UDPClient
{
    void SendUDP()
    {
        Utilities::SocketScoped socketHandle = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socketHandle < 0) {
            std::cout << "Failed to create socket" << std::endl;
            throw std::runtime_error("Ohh shit!");
        }

        constexpr uint16_t port { 52 };
        constexpr std::string_view host {"8.8.8.8"};

        const sockaddr_in server { AF_INET, htons(port) ,
                                   {.s_addr = inet_addr(host.data())}, {}};


        constexpr std::string_view data {"SOME_TEST_DATA"};

        for (int i = 0; i < 1000; ++i)
        {
            const long bytesSend = ::sendto(socketHandle, data.data(), data.size(), MSG_CONFIRM,
                                            reinterpret_cast<const sockaddr *>(&server), sizeof(sockaddr_in));

            // std::cout << bytesSend << " bytes send" << std::endl;
        }
    }

}


void UDPClient::TestAll()
{

    SendUDP();

}