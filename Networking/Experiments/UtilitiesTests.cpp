/**============================================================================
Name        : UtilitiesTests.cpp
Created on  : 13.12.2022.
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : UtilitiesTests
============================================================================**/

#include "UtilitiesTests.h"

#include <iostream>
#include <string_view>
#include <numeric>

#include <sys/socket.h>       // needed for socket()
#include <linux/if_ether.h>   // ETH_P_ARP = 0x0806
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)

#include "../Utilities/Utilities.h"
#include "../Headers/EthernetHeader.h"

namespace UtilitiesTests
{


    void GetInterfaceMACAddress()
    {
        constexpr std::string_view interface { "wlp4s0" };
        sockaddr_ll device = Utilities::ResolveInterfaceAddress(interface);
        Utilities::SocketScoped socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (-1 == socket) {
            std::cerr << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        Utilities::PrintMACAddress(device.sll_addr);
        std::cout << std::endl;

        EthernetHeader ethernetHeader;
        ethernetHeader.SetSourceMACAddress(device.sll_addr);

        Utilities::PrintMACAddress(ethernetHeader.sourceMac);
        std::cout << std::endl;

    }
}

void UtilitiesTests::TestAll()
{
    GetInterfaceMACAddress();

}