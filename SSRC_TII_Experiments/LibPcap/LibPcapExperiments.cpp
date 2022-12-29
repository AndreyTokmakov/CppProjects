/**============================================================================
Name        : LibPcap.cpp
Created on  : 12.12.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : LibPcap
============================================================================**/

#include "LibPcapExperiments.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#include <pcap.h>


#include <iostream>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#include "../Networking/Utils/Utilities.h"
#include "../Networking/Headers/EthernetHeader.h"
#include "../Networking/Headers/IPv4Header.h"
#include "../Networking/Headers/TCPHeader.h"
#include "../Networking/Headers/UDPHeader.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

namespace AirCrack
{

}


namespace LibPcapExperiments
{
    void ListDevices()
    {
        pcap_if_t *devices = nullptr;
        char errBuffer[PCAP_ERRBUF_SIZE] {};

        if (-1 == pcap_findalldevs(&devices, errBuffer)) {
            std::cerr << "pcap_findalldevs() failed\n";
            return;
        }


        if (devices)
        {
            pcap_if* device = devices;
            while (nullptr != device) {
                std::cout << device->name << std::endl;
                device = device->next;
            }
        }
    }

    void GetDeviceInfo()
    {
        pcap_if_t* devices = nullptr;
        char errBuffer[PCAP_ERRBUF_SIZE] {};

        if (-1 == pcap_findalldevs(&devices, errBuffer)) {
            std::cerr << "pcap_findalldevs() failed\n";
            return;
        }

        if (devices)
        {
            bpf_u_int32 netmask, srcIp;
            pcap_if *device = devices;

            while (nullptr != device)
            {
                std::cout << "\nName: " << device->name << std::endl;
                if (PCAP_ERROR == pcap_lookupnet(device->name, &srcIp, &netmask, errBuffer)) {
                    std::cerr << "pcap_lookupnet() failed:" << errBuffer << std::endl;
                    break;
                }

                std::cout << "srcip   : " << srcIp << std::endl;
                std::cout << "netmask : " << netmask << std::endl;

                device = device->next;
            }
        }
    }

    // TODO: Use my owh headers library
    void print_packet_info(const u_char *packet,
                           [[maybe_unused]] const pcap_pkthdr* header)
    {
        // printf("Packet capture length: %d\n", header->caplen);
        // std::cout << "Len: " << header->len << "| ";

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

    void my_packet_handler([[maybe_unused]] uint8_t *args,
                           const pcap_pkthdr *header,
                           const uint8_t *body)
    {
        print_packet_info(body, header);
    }

    void CapturePackets()
    {
        constexpr std::string_view interface { "wlp4s0" };
        [[maybe_unused]] bpf_u_int32 netmask;
        char errBuffer[PCAP_ERRBUF_SIZE] {};

        // char *device = nullptr;
        int timeout_limit = 10000; /* In milliseconds */

        /* Open device for live capture */
        pcap_t *handle = pcap_open_live(interface.data(), BUFSIZ, 0, timeout_limit, errBuffer);
        if (nullptr == handle) {
            std::cerr << "Could not open device '" << interface << "' : " <<  errBuffer << std::endl;
            return;
        }

        pcap_loop(handle, 0, my_packet_handler, nullptr);
    }

    void  pcap_lookupdev_TEST() {
        char ip[64] {}, subnet_mask[64] {};
        bpf_u_int32 ip_raw;          /* IP address as integer */
        bpf_u_int32 subnet_mask_raw; /* Subnet mask as integer */
        char error_buffer[PCAP_ERRBUF_SIZE]; /* Size defined in pcap.h */

        /* Find a device */
        const char *device = pcap_lookupdev(error_buffer);
        if (!device) {
            printf("1: %s\n", error_buffer);
            return;
        }

        /* Get device info */
        int lookup_return_code = pcap_lookupnet(device, &ip_raw, &subnet_mask_raw, error_buffer);
        if (lookup_return_code == -1) {
            printf("2: %s\n", error_buffer);
            return;
        }

        /*
        If you call inet_ntoa() more than once you will overwrite the buffer. If we only stored the pointer
        to the string returned by inet_ntoa(), and then we call it again later for the subnet mask, our first
        pointer (ip address) will actually have the contents of the subnet mask. That is why we are using a
        string copy to grab the contents while it is fresh. The pointer returned by inet_ntoa() is always the same.

        This is from the man:
        The inet_ntoa() function converts the Internet host address in, given in network byte order, to a string in
        IPv4 dotted-decimal notation. The string is returned in a statically allocated buffer, which subsequent
        calls will overwrite.
        */

        /* Get ip in human readable form */
        in_addr address { ip_raw };
        strcpy(ip, inet_ntoa(address));
        if (nullptr == ip) {
            perror("inet_ntoa"); /* print error */
            return;
        }

        /* Get subnet mask in human readable form */
        address.s_addr = subnet_mask_raw;
        strcpy(subnet_mask, inet_ntoa(address));
        if (nullptr == subnet_mask) {
            perror("inet_ntoa");
            return;
        }

        printf("Device: %s\n", device);
        printf("IP address: %s\n", ip);
        printf("Subnet mask: %s\n", subnet_mask);
    }
};

void LibPcapExperiments::TestAll()
{
    LibPcapExperiments::ListDevices();
    // LibPcapExperiments::GetDeviceInfo();

    // LibPcapExperiments::CapturePackets();

    // LibPcapExperiments::pcap_lookupdev_TEST(); // FIXME: OLD_C_STYPE;
}