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

#define FROM_OFFICE 1

#if FROM_OFFICE
constexpr std::string_view interface{"wlp0s20f3"};
#else
constexpr std::string_view interface { "wlp4s0" };
#endif


namespace UtilitiesTests
{

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

    constexpr uint32_t ip_octets_to_int(uint8_t first,uint8_t second, uint8_t third, uint8_t fourth)
    {
        return (first << 24) | (second << 16) | (third << 8) | (fourth);
    }

    constexpr void ip_to_octets(uint32_t ip,
                                uint32_t& first, uint32_t& second, uint32_t& third, uint32_t& fourth) {
        first = (ip >> 24) & 0xFF;
        second = (ip >> 16) & 0xFF;
        third = (ip >> 8) & 0xFF;
        fourth = ip & 0xFF;
    }

    [[nodiscard]]
    std::string ipInt2Str(uint32_t ip)
    {
        std::string ipStr { "000.000.000.000"};
        const uint32_t len = snprintf(ipStr.data(), ipStr.capacity(), "%d.%d.%d.%d",
                                      ip / 16777216 % 256, ip / 65536 % 256, ip / 256 % 256, ip % 256);
        ipStr.resize(len);
        ipStr.shrink_to_fit();
        return ipStr;
    }



    void Test_IpToStr()
    {
        std::cout << Utilities::IpToStr(3232235776) << std::endl;
        std::cout << ipInt2Str(3232235776) << std::endl;
    }

    void Test_OctetsToInt()
    {
        std::cout << ip_octets_to_int(192,168,1,0) << std::endl;
        static_assert(3232235776 == ip_octets_to_int(192,168,1,0));
    }

    void Test_IntToOctets()
    {
        constexpr uint32_t ip {3232235776}; // 192.168.1.0

        std::array<uint32_t, 4> octets {};
        ip_to_octets(192,octets[0], octets[1],octets[2], octets[3]);
        for (auto o: octets)
            std::cout << o << '.';
    }
}

void UtilitiesTests::TestAll()
{
    // GetInterfaceMACAddress();

    // Test_IpToStr();
    // Test_OctetsToInt();
    Test_IntToOctets();

}

/*
    @staticmethod
    def IP2Int(ip_address: str) -> int:
        octets: List = [int(v) for v in ip_address.split('.')]
        res: int = (16777216 * octets[0]) + (65536 * octets[1]) + (256 * octets[2]) + octets[3]
        return res

    @staticmethod
    def IP2Int_2(ip_address: str) -> int:
        octets: List = [int(v) for v in ip_address.split('.')]
        return octets[3] | octets[2] << 8 | octets[1] << 16 | octets[0] << 24

    @staticmethod
    def IP2Int_3(ip_address: str) -> int:
        return reduce(lambda x, y: x * 256 + y, [int(v) for v in ip_address.split('.')])
 *
 * */