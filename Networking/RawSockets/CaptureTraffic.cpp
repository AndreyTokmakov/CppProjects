/**============================================================================
Name        : CaptureTraffic.cpp
Created on  : 11.03.2023.
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CaptureTraffic
============================================================================**/

#include "CaptureTraffic.h"

#include "../Headers/ARPHeader.h"
#include "../Headers/IPv4Header.h"
#include "../Headers/TCPHeader.h"
#include "../Headers/UDPHeader.h"

#include <net/if_arp.h>   // ARPOP_REQUEST
#include <iostream>


namespace CaptureTraffic
{
    struct WiFiMACHeader final
    {
        uint8_t ctrl1 { 0 };
        uint8_t ctrl2 { 0 };
        // uint16_t control { 0 };

        uint16_t duration { 0 };
        uint8_t da [6] {};
        uint8_t sa [6] {};
        uint8_t bss [6] {};
        uint16_t sec { 0 };
    } __attribute__((packed, aligned(1)));

    // TODO: SocketScoped ----> Socket ???
    // FIXME: We need to use SocketScoped in the right way. or follow the rule of 5 ??
    Utilities::SocketScoped createSocket()
    {
        Utilities::SocketScoped socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (-1 == socket) {
            std::cerr << "Failed to create socket. Error = " << errno << std::endl;
            std::exit(0);
        }
        return socket;
    }

    [[nodiscard]]
    sockaddr_ll yResolveInterfaceAddress(std::string_view interfaceName)
    {
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

    void snifferTest1()
    {
        Utilities::SocketScoped socket = createSocket();

        unsigned char buffer[2048] {};
        ssize_t bytes = 0;
        while(true)
        {
            bytes = ::recvfrom(socket, buffer, sizeof(buffer), 0, nullptr, nullptr);
            std::cout << bytes << " bytes receives\n";
        }
    }

    [[noreturn]]
    void snifferTest2()
    {
        constexpr std::string_view interfaceName { "wlx00c0cab21ffc" };

        ifreq ifr {};
        snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), interfaceName.data());

        Utilities::SocketScoped socket = createSocket();

        int result = ::ioctl(socket, SIOCGIFINDEX, &ifr);
        if (-1 == result) {
            std::cerr << "ioctl() call error. Error = " << errno << std::endl;
            return;
        } else {
            const int idx = ::if_nametoindex(ifr.ifr_name);
            std::cout << "Interface '" << interfaceName << "' id = " << idx << std::endl;
        }

        result = ::setsockopt(socket, SOL_SOCKET, SO_BINDTODEVICE,  (void*)&ifr, sizeof(ifr));
        if (-1 == result) {
            std::cerr << "setsockopt() call error. Error = " << errno << std::endl;
            return;
        }

        unsigned char frame[2048] {};
        ssize_t bytes = 0;
        while(true)
        {
            bytes = ::recvfrom(socket, frame, sizeof(frame), 0, nullptr, nullptr);
            // std::cout << "----------------------------------------  " << bytes << " --------------------------------------------\n";

            WiFiMACHeader* macHeader = reinterpret_cast<WiFiMACHeader*>(frame);
            // std:: cout << macHeader->control << " | " << macHeader->duration  << " | " << macHeader->sec << std::endl;

            const uint16_t type = (macHeader->ctrl1 & 0xC) >> 2;
            const uint16_t subtype = (macHeader->ctrl1 & 0xF0) >> 4;

            std::cout << "type: " << type << ", subtype: " << subtype << "  |  ";

            // Utilities::PrintMACAddress(macHeader->sa); std::cout << "   ";
            // Utilities::PrintMACAddress(macHeader->da); std::cout << "   ";
            // Utilities::PrintMACAddress(macHeader->bss);std::cout << "   " << std::endl;

            if (type == 2)   // type:2 = data frame
            {
                const uint16_t toDs = macHeader->ctrl2 & 0x1;           // to distributed system flag
                const uint16_t fromDs = (macHeader->ctrl2& 0x2) >> 1;  // from distributed system flag

                std::cout << "toDs: " << toDs << ", fromDs: " << fromDs << std::endl;
            }

            std::cout << std::endl;
        }
    }
};

void CaptureTraffic::TestAll()
{
    Utilities::checkRunningUnderRoot();
    // snifferTest1();
    snifferTest2();

};

