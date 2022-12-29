/**============================================================================
Name        : ARPTester.cpp
Created on  : 28.12.2022.
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ARPTester
============================================================================**/

#include "ARPTester.h"

#include "../Headers/EthernetHeader.h"
#include "../Headers/IPv4Header.h"
#include "../Headers/TCPHeader.h"
#include "../Headers/UDPHeader.h"
#include "../Headers/ICMPHeader.h"

#include <net/if_arp.h>   // ARPOP_REQUEST
#include <iostream>

namespace ARPTester
{
    struct ARPHeader final
    {
        uint16_t htype {0};
        uint16_t ptype {0};
        uint8_t  hlen {};
        uint8_t  plen {};
        uint16_t opcode {0};
        uint8_t  sender_mac[6]{};
        uint32_t sender_ip {};
        uint8_t  target_mac[6]{};
        uint32_t target_ip {};

    public:
        bool SetSenderMACAddress(std::string_view mac) {
            return EthernetHeader::SetMACAddress(mac.data(), sender_mac);
        }

        bool SetTargetMACAddress(std::string_view mac) {
            return EthernetHeader::SetMACAddress(mac.data(), target_mac);
        }

        inline void SetSenderMACAddress(const uint8_t* const mac) {
            std::copy_n(mac, std::size(sender_mac), sender_mac);
        }

        inline void SetTargetMACAddress(const uint8_t* const mac) {
            std::copy_n(mac, std::size(target_mac), target_mac);
        }

        // TODO: Remove?
        void SetSenderAddress(std::string_view address) {
            sender_ip = inet_addr(address.data());
        }

        // TODO: Remove?
        void SetTargetAddress(std::string_view address) {
            target_ip = inet_addr(address.data());
        }
    } __attribute__((packed, aligned(1))) ;;

    EthernetHeader* initEthernetHeader(EthernetHeader* ethernetHeader,
                                       const sockaddr_ll& device,
                                       std::string_view dst,
                                       const uint16_t type = ETH_P_ARP) {
        ethernetHeader->SetDestinationMACAddress(dst.data());
        ethernetHeader->SetSourceMACAddress(device.sll_addr);
        ethernetHeader->SetType(type);
        return ethernetHeader;
    }


    ARPHeader* initARPHeader(ARPHeader* arpHeader,
                             std::string_view sourceMac)
    {
        memset(arpHeader, 0, sizeof(ARPHeader));
        arpHeader->htype = htons(1);               // Hardware type (16 bits): 1 for ethernet
        arpHeader->ptype = htons(ETH_P_IP);        // Protocol type (16 bits): 2048 for IP
        arpHeader->hlen = 6;                       // Hardware address length (8 bits): 6 bytes for MAC address
        arpHeader->plen = 4;                       // Protocol address length (8 bits): 4 bytes for IPv4 address
        arpHeader->opcode = htons(ARPOP_REQUEST);  // OpCode: 1 for ARP request

        /** Sender hardware address (48 bits): MAC address **/
        // memcpy(&arpHeader.sender_mac, device.sll_addr, 6 * sizeof (uint8_t));
        arpHeader->SetSenderMACAddress(sourceMac);

        /** Target hardware address (48 bits): zero, since we don't know it yet. **/
        memset(&arpHeader->target_mac, 0,       6 * sizeof (uint8_t));

        return arpHeader;
    }

    ARPHeader* initARPHeader(ARPHeader* arpHeader,
                             const sockaddr_ll& device,
                             std::string_view targetMac)
    {
        memset(arpHeader, 0, sizeof(ARPHeader));
        arpHeader->htype = htons(1);               // Hardware type (16 bits): 1 for ethernet
        arpHeader->ptype = htons(ETH_P_IP);        // Protocol type (16 bits): 2048 for IP
        arpHeader->hlen = 6;                       // Hardware address length (8 bits): 6 bytes for MAC address
        arpHeader->plen = 4;                       // Protocol address length (8 bits): 4 bytes for IPv4 address
        arpHeader->opcode = htons(ARPOP_REQUEST);  // OpCode: 1 for ARP request

        /** Sender hardware address (48 bits): MAC address **/
        // memcpy(&arpHeader.sender_mac, device.sll_addr, 6 * sizeof (uint8_t));
        arpHeader->SetSenderMACAddress(device.sll_addr);

        /** Target hardware address (48 bits): zero, since we don't know it yet. **/
        // memset(&arpHeader->target_mac, 0,       6 * sizeof (uint8_t));
        arpHeader->SetTargetMACAddress(targetMac);

        return arpHeader;
    }

    void checkRunningUnderRoot()
    {
        const uint32_t userID { getuid() };
        if (0 != userID)
        {
            // throw std::runtime_error("Application require the ROOT user access"  );

            std::cerr << "Application require the ROOT user access" << std::endl;
            std::exit(0);
        }
    }
}

void ARPTester::TestAll()
{
    constexpr std::string_view srcMac { "a8:93:4a:4e:00:6b" };
    constexpr std::string_view dstMac { "f4:8c:eb:b8:b8:61" };
    constexpr std::string_view interface { "wlp4s0" };

    checkRunningUnderRoot();

    sockaddr_ll device = Utilities::ResolveInterfaceAddress(interface);
    uint8_t packet[sizeof(EthernetHeader) + sizeof(ARPHeader)] {};

    initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device,
                       "01:01:00:00:00:01");

    ARPHeader* arpHeader = initARPHeader(reinterpret_cast<ARPHeader*>((packet + sizeof(EthernetHeader))),
                                         device, dstMac);

    arpHeader->SetSenderAddress("111.111.111.111");
    arpHeader->SetTargetAddress("222.222.222.222");

    Utilities::SocketScoped socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (-1 == socket) {
        std::cout << "Failed to create socket. Error = " << errno << std::endl;
        return;
    }

    for (int i = 0; i < 20; ++i) {

        long bytes = sendto(socket,
                            reinterpret_cast<uint8_t *>(&packet),
                            sizeof(packet),
                            0,
                            reinterpret_cast<sockaddr *>(&device),
                            sizeof(device)
        );
        if (-1 == bytes) {
            std::cerr << "Error sending packet: " << errno << std::endl;
        } else {
            std::cout << bytes << " send\n";
        }
    }
}