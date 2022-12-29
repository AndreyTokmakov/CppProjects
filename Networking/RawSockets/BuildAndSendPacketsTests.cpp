/**============================================================================
Name        : BuildAndSendPacketsTests.cpp
Created on  : 18.09.2022.
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : BuildAndSendPacketsTests
============================================================================**/

#include "BuildAndSendPacketsTests.h"

#include <iostream>
#include <string>
#include <cstring>

#include "../Headers/EthernetHeader.h"
#include "../Headers/IPv4Header.h"
#include "../Headers/TCPHeader.h"
#include "../Headers/UDPHeader.h"
#include "../Headers/ICMPHeader.h"

namespace LocalTools
{
    EthernetHeader* initEthernetHeader(EthernetHeader* ethernetHeader,
                                       std::string_view src,
                                       std::string_view dst) {
        ethernetHeader->SetDestinationMACAddress(dst.data());
        ethernetHeader->SetSourceMACAddress(src.data());
        ethernetHeader->SetType(ETHERTYPE_IP);
        return ethernetHeader;
    }

    EthernetHeader* initEthernetHeader(EthernetHeader* ethernetHeader,
                                       const sockaddr_ll& device,
                                       std::string_view dst) {
        ethernetHeader->SetDestinationMACAddress(dst.data());
        ethernetHeader->SetSourceMACAddress(device.sll_addr);
        ethernetHeader->SetType(ETHERTYPE_IP);
        return ethernetHeader;
    }

    IPv4Header* initIPv4Header(IPv4Header* ipHeader,
                               uint16_t protocol,
                               uint16_t packetLength,
                               const std::string& sourceAddress,
                               const std::string& destAddress,
                               uint16_t packetId,
                               uint16_t serviceType = 0,
                               uint16_t ttl = 128,
                               uint16_t version = 4)
    {
        /** IP Header configuration: **/
        ipHeader->SetIPHeaderLength(20);   // Since in IPv4 its size is 20
        ipHeader->SetIPVersion(version);       // Trivial
        ipHeader->SetServiceType(serviceType);
        ipHeader->SetPacketLength(packetLength);
        ipHeader->SetPacketId(packetId);
        ipHeader->SetTimeToLive(ttl);
        ipHeader->SetProtocol(protocol);
        ipHeader->SetChecksum(0);
        ipHeader->SetDontFragment(1);
        ipHeader->SetSourceAddress(sourceAddress);
        ipHeader->SetDestAddress(destAddress);

        ipHeader->frag_offset = 0;
        ipHeader->more_fragment = 0;
        ipHeader->reserved_zero = 0;
        ipHeader->frag_offset1 = 0;

        /** Calculate the IP Header Checksum: **/
        // TODO: static cast
        ipHeader->SetChecksum(Utilities::Checksum(reinterpret_cast<uint16_t*>(ipHeader), sizeof(IPv4Header)));
        return ipHeader;
    }

    void initTCPHeader(TCPHeader* tcpHeader,
                       const IPv4Header* ipHeader,
                       uint16_t sourcePort, uint16_t destPort,
                       uint32_t sequence, uint32_t acknowledge,
                       uint16_t window,
                       bool SYN = true,
                       bool ACK = false,
                       bool FIN = false,
                       bool PSH = false,
                       bool RST = false,
                       const uint8_t* tcpOptions = nullptr,
                       uint16_t tcpOptsLength = 0,
                       const uint8_t* tcpData = nullptr,
                       uint16_t tcpDataLength = 0)
    {
        memset(tcpHeader, 0, sizeof(TCPHeader));

        // memset(tcpOptsPayload, 0, sizeof(tcpOptsPayload));

        /** TCP Header configuration: **/
        tcpHeader->SetSourcePort(sourcePort);
        tcpHeader->SetDestinationPort(destPort);
        tcpHeader->SetWindow(window);
        tcpHeader->SetAcknowledgeSeqNum(acknowledge);
        tcpHeader->SetSequenceNum(sequence);
        tcpHeader->SetDataOffset(sizeof(TCPHeader) + tcpOptsLength);
        tcpHeader->SetFlag_URG(false);
        tcpHeader->SetFlag_ACK(ACK);
        tcpHeader->SetFlag_PSH(PSH);
        tcpHeader->SetFlag_RST(RST);
        tcpHeader->SetFlag_SYN(SYN);
        tcpHeader->SetFlag_FYN(FIN);
        tcpHeader->SetChecksum(0);
        tcpHeader->SetUrgentPointer(0);
        tcpHeader->SetChecksum(TCPHeader::CalcChecksum(ipHeader, tcpHeader, tcpOptions, tcpData, tcpDataLength));
    }
}

namespace Tests
{
    using namespace LocalTools;
    constexpr std::string_view interface { "wlp4s0" };

    /*
    void SendPacket(const int socket,
                    sockaddr_ll device,
                    const uint8_t* buffer,
                    size_t size)
    {
        long bytes = sendto(socket,
                            reinterpret_cast<uint8_t*>(&buffer),
                            size,
                            0,
                            reinterpret_cast<sockaddr*>(&device),
                            sizeof(device));
        if (-1 == bytes) {
            std::cout << "Error sending packet: " << errno << std::endl;
            return;
        }

        std::cout << bytes << " send\n";
    }*/

    void SendICMP()
    {
        sockaddr_ll device = Utilities::ResolveInterfaceAddress(interface);
        uint8_t packet[sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(ICMPHeader)] {};

        initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device,
                           "01:01:00:00:00:01");
        IPv4Header* ipHeader = initIPv4Header(reinterpret_cast<IPv4Header*>((packet + sizeof(EthernetHeader))),
                                              IPPROTO_ICMP,
                                              sizeof(IPv4Header) + sizeof(ICMPHeader),
                                              "192.168.0.184",
                                              "192.168.0.1",
                                              12345);

        const uint16_t icmpType = 3, icmpCode = 4;
        ICMPHeader* icmpHeader = reinterpret_cast<ICMPHeader*>(packet + sizeof(EthernetHeader) + sizeof(IPv4Header));
        memset(icmpHeader, 0, sizeof(ICMPHeader));

        icmpHeader->SetType(icmpType);
        icmpHeader->SetCode(icmpCode);
        icmpHeader->SetChecksum(0);
        icmpHeader->SetIdentifier(33245);
        icmpHeader->SetSequence(256);
        icmpHeader->SetChecksum(Utilities::Checksum(reinterpret_cast<uint16_t*>(icmpHeader), sizeof(ICMPHeader)));

        Utilities::SocketScoped socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (-1 == socket) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        long bytes = sendto(socket,
                            reinterpret_cast<uint8_t*>(&packet),
                            sizeof(packet),
                            0,
                            reinterpret_cast<sockaddr*>(&device),
                            sizeof(device)
                            );
        if (-1 == bytes) {
            std::cerr << "Error sending packet: " << errno << std::endl;
        } else {
            std::cout << bytes << " send\n";
        }
    }

    void SendTCP()
    {
        sockaddr_ll device = Utilities::ResolveInterfaceAddress(interface);
        uint8_t packet[sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(TCPHeader)] {};

        initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device,
                           "01:01:00:00:00:01");
        IPv4Header* ipHeader = initIPv4Header(reinterpret_cast<IPv4Header*>((packet + sizeof(EthernetHeader))),
                                              IPPROTO_TCP,
                                              sizeof(IPv4Header) + sizeof(TCPHeader),
                                              "192.168.0.184",
                                              "127.0.0.2",
                                              12345);

        initTCPHeader(reinterpret_cast<TCPHeader*>((packet + sizeof(EthernetHeader) + sizeof(IPv4Header))),
                      ipHeader,
                      52525, 443,
                      1000001, 1000002, 1024);

        Utilities::SocketScoped socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (-1 == socket)
            std::cout << "Failed to create socket. Error = " << errno << std::endl;

        long bytes = sendto(socket,
                            reinterpret_cast<uint8_t*>(&packet),
                            sizeof(packet),
                            0,
                            reinterpret_cast<sockaddr*>(&device),
                            sizeof(device));
        if (-1 == bytes) {
            std::cerr << "Error sending packet: " << errno << std::endl;
        } else {
            std::cout << bytes << " send\n";
        }
    }
}

void BuildAndSendPacketsTests::TestAll()
{
    Tests::SendICMP();
    // Tests::SendTCP();

}










