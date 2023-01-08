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
#include <iomanip>

#include <sys/socket.h>       // needed for socket()
#include <linux/if_ether.h>   // ETH_P_ARP = 0x0806
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <charconv>

#include "../Utilities/Utilities.h"
#include "../Headers/EthernetHeader.h"

#define FROM_OFFICE 1

#if FROM_OFFICE
constexpr std::string_view interface{"wlp0s20f3"};
#else
constexpr std::string_view interface { "wlp4s0" };
#endif


namespace UtilitiesTests
{
    using namespace Utilities::IP;

    void GetInterfaceMACAddress() {
        Utilities::checkRunningUnderRoot();

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


    void Test_IpToStr()
    {
        std::cout << Utilities::IpToStr(3232235776) << std::endl;
        std::cout << ipInt2Str(3232235776) << std::endl;
        std::cout << ipInt2StrOLD(3232235776) << std::endl;
    }

    void Test_OctetsToInt()
    {
        // 3232235776 --> 192.168.1.0
        std::cout << ip_octets_to_int(192,168,1,0) << std::endl;
        static_assert(3232235776 == ip_octets_to_int(192,168,1,0));

        std::cout << ip_octets_to_int2(192,168,1,0) << std::endl;
        static_assert(ip_octets_to_int(192,168,1,0) ==
                      ip_octets_to_int2(192,168,1,0));
    }

    void IP_String_To_Int()
    {
        // 3232235776 <--> 192.168.1.0
        std::cout << ipInt2Str("192.168.1.0") << std::endl;
        std::cout << ipInt2Str2("192.168.1.0") << std::endl;
        std::cout << std::boolalpha<< ( 3232235776 == ipInt2Str("192.168.1.0")) << std::endl;
        std::cout << std::boolalpha<< ( 3232235776 == ipInt2Str2("192.168.1.0")) << std::endl;
    }
}

void UtilitiesTests::TestAll()
{
    // GetInterfaceMACAddress();

    // Test_IpToStr();

    // Test_OctetsToInt();

    IP_String_To_Int();

}
