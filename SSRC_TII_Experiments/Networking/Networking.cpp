/**============================================================================
Name        : Networking.cpp
Created on  : 10.12.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Networking
============================================================================**/

#include "Networking.h"

#include "Headers/EthernetHeader.h"
#include "Headers/IPv4Header.h"
#include "Headers/TCPHeader.h"
#include "Headers/UDPHeader.h"
#include "Headers/ICMPHeader.h"
#include "Utils/Utilities.h"

#include "iostream"


namespace
{
    constexpr uint32_t RECV_BUFFER_SIZE { 2048 };
}

namespace Networking::BuildPacket {

    EthernetHeader* initEthernetHeader(EthernetHeader* ethernetHeader,
                                       std::string_view src,
                                       std::string_view dst,
                                       uint16_t packetType = ETHERTYPE_IP) {
        ethernetHeader->SetDestinationMACAddress(dst.data());
        ethernetHeader->SetSourceMACAddress(src.data());
        ethernetHeader->SetType(packetType);
        return ethernetHeader;
    }

    EthernetHeader* initEthernetHeader(EthernetHeader* ethernetHeader,
                                       const sockaddr_ll& device,
                                       std::string_view dst,
                                       uint16_t packetType = ETHERTYPE_IP) {
        ethernetHeader->SetDestinationMACAddress(dst.data());
        ethernetHeader->SetSourceMACAddress(device.sll_addr);
        ethernetHeader->SetType(packetType);
        return ethernetHeader;
    }

    IPv4Header *initIPv4Header(IPv4Header *ipHeader,
                               uint16_t protocol,
                               uint16_t packetLength,
                               const std::string &sourceAddress,
                               const std::string &destAddress,
                               uint16_t packetId,
                               uint16_t serviceType = 0,
                               uint16_t ttl = 128,
                               uint16_t version = 4) {
        memset(ipHeader, 0, sizeof(TCPHeader));

        /** IP Header configuration: **/
        ipHeader->SetIPHeaderLength(20);   // Since in IPv4 its size is 20
        ipHeader->SetIPVersion(version);   // Trivial
        ipHeader->SetServiceType(serviceType);
        ipHeader->SetPacketLength(packetLength);
        ipHeader->SetPacketId(packetId);
        ipHeader->SetTimeToLive(ttl);
        ipHeader->SetProtocol(protocol);
        ipHeader->SetChecksum(0);
        ipHeader->SetDontFragment(1);
        ipHeader->SetSourceAddress(sourceAddress);
        ipHeader->SetDestAddress(destAddress);

        /** Calculate the IP Header Checksum: **/
        // TODO: static cast
        ipHeader->SetChecksum(Utilities::Checksum(reinterpret_cast<uint16_t*>(ipHeader),sizeof(IPv4Header)));
        return ipHeader;
    }

    void initTCPHeader(TCPHeader *tcpHeader,
                       const IPv4Header* const ipHeader,
                       uint16_t sourcePort, uint16_t destPort,
                       uint32_t sequence, uint32_t acknowledge,
                       uint16_t window,
                       bool SYN = true,
                       bool ACK = false,
                       bool FIN = false,
                       bool PSH = false,
                       bool RST = false,
                       const uint8_t *tcpOptions = nullptr,
                       uint16_t tcpOptsLength = 0,
                       const uint8_t *tcpData = nullptr,
                       uint16_t tcpDataLength = 0) {
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
        tcpHeader->SetFlag_ACK(ACK); // FIXME: ---> To enum???
        tcpHeader->SetFlag_PSH(PSH);
        tcpHeader->SetFlag_RST(RST);
        tcpHeader->SetFlag_SYN(SYN);
        tcpHeader->SetFlag_FYN(FIN);
        tcpHeader->SetChecksum(0);
        tcpHeader->SetUrgentPointer(0);
        tcpHeader->SetChecksum(TCPHeader::CalcChecksum(ipHeader, tcpHeader, tcpOptions, tcpData, tcpDataLength));
    }

    void Test() {
        uint8_t buffer[sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(TCPHeader)];

        initEthernetHeader(reinterpret_cast<EthernetHeader *>(buffer),
                           "bc:6e:e2:03:74:ba",
                           "00:00:00:00:00:01");
        IPv4Header *ipHeader = initIPv4Header(reinterpret_cast<IPv4Header *>((buffer + sizeof(EthernetHeader))),
                                              IPPROTO_TCP,
                                              sizeof(IPv4Header) + sizeof(TCPHeader),
                                              "127.0.0.1",
                                              "127.0.0.2",
                                              12345);

        initTCPHeader(reinterpret_cast<TCPHeader *>((buffer + sizeof(EthernetHeader) + sizeof(IPv4Header))),
                      ipHeader,
                      52525, 443,
                      1000001, 1000002, 1024);


        Utilities::SocketScoped socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (-1 == socket) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        sockaddr_ll device = Utilities::ResolveInterfaceAddress("wlp0s20f3");
        long bytes = sendto(socket,
                            reinterpret_cast<uint8_t *>(&buffer),
                            sizeof(buffer),
                            0,
                            reinterpret_cast<sockaddr *>(&device),
                            sizeof(device));
        if (-1 == bytes) {
            std::cout << "Error sending packet: " << errno << std::endl;
            return;
        }
    }
}

namespace Networking::WiFi
{
    using namespace BuildPacket;
    constexpr std::string_view interfaceName { "wlp4s0" };

    void GetInterfaceInfo_WiFi()
    {
        constexpr std::string_view ifaceName { "wlp4s0" };

        // auto sockAddr = Utilities::ResolveInterfaceAddress(ifaceName);

        sockaddr_ll sockAddr {};
        Utilities::ResolveInterfaceAddress(ifaceName, sockAddr);

        Utilities::PrintMACAddress(sockAddr.sll_addr);
        std::cout << std::endl;
    }

    void TrySendPacket()
    {
        uint8_t buffer[sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(TCPHeader)];
        sockaddr_ll device = Utilities::ResolveInterfaceAddress(interfaceName);
        // TODO: Check device

        initEthernetHeader(reinterpret_cast<EthernetHeader *>(buffer), device,
                           "e4:5f:01:61:5b:fc");
        IPv4Header *ipHeader = initIPv4Header(reinterpret_cast<IPv4Header *>((buffer + sizeof(EthernetHeader))),
                                              IPPROTO_TCP,
                                              sizeof(IPv4Header) + sizeof(TCPHeader),
                                              "192.168.0.184",
                                              "192.168.1.5",
                                              12345);

        initTCPHeader(reinterpret_cast<TCPHeader *>((buffer + sizeof(EthernetHeader) + sizeof(IPv4Header))),
                      ipHeader,
                      12121, 52525,
                      1000001, 1000002, 1024);


        Utilities::SocketScoped socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (-1 == socket) {
            std::cerr << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        const int32_t value = 1;
        int32_t result = setsockopt(socket, SOL_PACKET, PACKET_QDISC_BYPASS,
                                    &value, sizeof(int32_t));
        if (-1 == result) {
            std::cerr << "setsockopt() failed. " << errno << std::endl;
            return;
        }


        for (size_t idx = 0; idx < 10000000; ++idx)
        {
            long bytes = sendto(socket,
                                reinterpret_cast<uint8_t *>(&buffer),
                                sizeof(buffer),
                                0,
                                reinterpret_cast<sockaddr *>(&device),
                                sizeof(device));
            if (-1 == bytes) {
                std::cerr << "Error sending packet: " << errno << std::endl;
                return;
            } else {
                // std::cout << bytes << " send\n";
            }
        }
    }

    // FIXME: BAD!!! Refactor 'uint8_t buffer[2048];'
    uint16_t CalcICMPChecksum(const ICMPHeader* icmpHeader,
                              const uint8_t* payload,
                              const uint16_t length)
    {
        uint8_t buffer[2048];
        memcpy(buffer, icmpHeader, sizeof(ICMPHeader));
        memcpy(buffer + sizeof(ICMPHeader), payload, length);
        return Utilities::Checksum((uint16_t*)buffer, sizeof(ICMPHeader) + length);
    }

    void SendICMP_PING()
    {
        sockaddr_ll device = Utilities::ResolveInterfaceAddress(interfaceName);
        uint8_t packet[sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(ICMPHeader)] {};

        initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device,
                           "e4:5f:01:61:5b:fc");
        IPv4Header* ipHeader = initIPv4Header(reinterpret_cast<IPv4Header*>((packet + sizeof(EthernetHeader))),
                                              IPPROTO_ICMP,
                                              sizeof(IPv4Header) + sizeof(ICMPHeader),
                                              "192.168.1.0",
                                              "192.168.1.5",
                                              12345, 0, 65);

        ICMPHeader* icmpHeader = reinterpret_cast<ICMPHeader*>(packet + sizeof(EthernetHeader) + sizeof(IPv4Header));
        memset(icmpHeader, 0, sizeof(ICMPHeader));

        constexpr uint16_t payloadLength = sizeof(IPv4Header) + 8;
        [[maybe_unused]]
        uint8_t payload[payloadLength] {};
        const uint16_t icmpType = 8, icmpCode = 0;

        icmpHeader->SetType(icmpType);
        icmpHeader->SetCode(icmpCode);
        icmpHeader->SetChecksum(0);
        icmpHeader->SetIdentifier(33245);
        icmpHeader->SetSequence(256);
        icmpHeader->SetChecksum(Utilities::Checksum(reinterpret_cast<uint16_t*>(icmpHeader), sizeof(ICMPHeader)));
        // icmpHeader->SetChecksum(CalcICMPChecksum(icmpHeader, payload, payloadLength)); // NOTE: With payload

        const Utilities::SocketScoped socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
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
}

namespace Sniffer
{
    constexpr std::string_view ifaceName { "wlp4s0" };

    struct MACHeader {
        uint16_t ctrl {0};
        uint16_t duration {0};
        uint8_t srcMac[6] {};
        uint8_t destMac[6] {};
        uint8_t addr2[6] {};
        uint16_t seq {0};
        uint8_t addr3[6] {};
    };

    void PrintPacketInfo(const uint8_t* packet)
    {
        //std::cout << bytes << "   " << len <<  std::endl;

        /*
        auto* macHeader = (MACHeader*)buffer;

        Utilities::PrintMACAddress(macHeader->srcMac);std::cout << "    ";
        Utilities::PrintMACAddress(macHeader->destMac);std::cout << "    ";
        Utilities::PrintMACAddress(macHeader->addr2);std::cout << "    ";
        Utilities::PrintMACAddress(macHeader->addr3);std::cout << "\n";
        */

        const EthernetHeader *ethHeader = (EthernetHeader*)packet;
        const uint16_t type = ethHeader->GetType();

        if (ETHERTYPE_IP == type)
        {
            const iphdr* ipHeader = (iphdr*)(packet + sizeof(EthernetHeader));
            const uint16_t ipHdrLen = ipHeader->ihl * 4;
            const u_char protocol = *(reinterpret_cast<const unsigned char*>(ipHeader) + 9);

            Utilities::PrintMACAddress(ethHeader->sourceMac); std::cout << " --> ";
            Utilities::PrintMACAddress(ethHeader->destMac);

            if (IPPROTO_TCP == protocol)
            {
                const TCPHeader* tcpHeader = (TCPHeader*)(packet + sizeof(EthernetHeader) + ipHdrLen);

                std::cout << " TCP | ";
                std::cout << Utilities::IpToStr(ipHeader->saddr) << ":" << tcpHeader->GetSourcePort()
                          << " --> " << Utilities::IpToStr(ipHeader->daddr) << ":" << tcpHeader->GetDestinationPort();
            }
            else if (IPPROTO_UDP == protocol)
            {
                const UDPHeader* udpHeader = (UDPHeader*)(packet + sizeof(EthernetHeader) + ipHdrLen);

                std::cout << " UDP | ";
                std::cout << Utilities::IpToStr(ipHeader->saddr) << ":" << udpHeader->GetSourcePort()
                          << " --> " << Utilities::IpToStr(ipHeader->daddr) << ":" << udpHeader->GetDestinationPort();
            }

            std::cout << std::endl;
        }
        else if (ETHERTYPE_ARP == type) {
            // TODO
        }
        else if (ETHERTYPE_REVARP == type) {
            // TODO
        }
    }

    void DumpWiFiTraffic()
    {
        sockaddr_ll device {};
        if (!Utilities::ResolveInterfaceAddress(ifaceName, device)) {
            std::cerr << "Failed to get '" << ifaceName << "' MAC address\n";
            return;
        }

        Utilities::SocketScoped socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        uint8_t buffer[2048];
        long bytes = 0;
        socklen_t len;
        while (bytes >= 0) {
            bytes = recvfrom(socket, buffer, std::size(buffer), 0, (sockaddr*)&device, &len);
            PrintPacketInfo(buffer);
        }
    }
}

namespace Networking::Batman
{
    using namespace BuildPacket;
    constexpr std::string_view interfaceName { "wlp4s0" };

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


    struct BatmanOGMHeader final
    {
        uint8_t  packetType { 0 };
        uint8_t  version { 0 };
        uint8_t  ttl { 0 };
        uint8_t  flags { 0 };
        uint32_t seqNumber { 0 };
        uint8_t  originatorMAC[ETH_ALEN]{};
        uint8_t  prevSenderMAC[ETH_ALEN]{};
        uint8_t  reserved { 0 };
        uint8_t  tq { 0 };
        uint16_t tvlvLength  { 0 };
    };

    void InspectPacket(const uint8_t* packet,
                       [[maybe_unused]] ssize_t size)
    {
        constexpr uint16_t batmanProtocolId = 1347;

        const EthernetHeader *ethHeader = (EthernetHeader*)packet;
        const uint16_t type = ethHeader->GetType();

        if (batmanProtocolId == type)
        {
            std::cout << "Got B.A.T.M.A.N packet" << std::endl;
        }
        else
        {
            std::cout << type << std::endl;
        }
    }

    void SniffNetworkPackets()
    {
        Utilities::checkRunningUnderRoot();

        sockaddr_ll device {};
        if (!Utilities::ResolveInterfaceAddress(interfaceName, device)) {
            std::cerr << "Failed to get '" << interfaceName << "' MAC address\n";
            return;
        }

        Utilities::SocketScoped socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        uint8_t buffer[2048];
        ssize_t bytes = 0;
        socklen_t len;
        while (bytes >= 0) {
            bytes = recvfrom(socket, buffer, std::size(buffer), 0, (sockaddr*)&device, &len);
            InspectPacket(buffer, bytes);
        }
    }

    void SendOGMMessage()
    {
        sockaddr_ll device = Utilities::ResolveInterfaceAddress(interfaceName);
        uint8_t packet[sizeof(EthernetHeader) + sizeof(BatmanOGMHeader)] {};

        initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet), device,
                           "00:30:1a:4f:8d:c4", ETH_P_BATMAN);
        BatmanOGMHeader* ogmHeader = reinterpret_cast<BatmanOGMHeader*>(packet + sizeof(EthernetHeader));
        ogmHeader->packetType = 0;
        ogmHeader->version = 15;
        ogmHeader->ttl = 50;
        ogmHeader->flags = 0;
        ogmHeader->seqNumber = htonl(100000001);
        EthernetHeader::SetMACAddress("a8:93:4a:4e:00:6b", ogmHeader->originatorMAC);
        EthernetHeader::SetMACAddress("a8:93:4a:4e:00:6b", ogmHeader->prevSenderMAC);
        ogmHeader->reserved = 0;
        ogmHeader->tq = 191;
        ogmHeader->tvlvLength = htons(36);

        Utilities::SocketScoped socket = createSocket();
        enableBroadcast(socket);


        const long bytes = sendto(socket,reinterpret_cast<uint8_t*>(&packet),std::ssize(packet),
                                  0,reinterpret_cast<sockaddr*>(&device),sizeof(device));
        if (-1 == bytes) {
            std::cerr << "Error sending packet: " << errno << std::endl;
        } else {
            std::cout << bytes << " send\n";
        }
    }
}


void Networking::TestAll()
{
    // BuildPacket::Test();

    // WiFi::GetInterfaceInfo_WiFi();
    // WiFi::TrySendPacket();
    // WiFi::SendICMP_PING();

    // Sniffer::DumpWiFiTraffic();

    // Batman::SniffNetworkPackets();
    Batman::SendOGMMessage();
}