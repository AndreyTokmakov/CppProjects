/**============================================================================
Name        : Utilities.cpp
Created on  : 09.12.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities
============================================================================**/

#include "Utilities.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <array>
#include <charconv>

#include <netinet/in.h>
#include <sys/ioctl.h>
#include <bits/ioctls.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>

void Utilities::PrintMACAddress(const uint8_t* mac)
{
    for (int i = 0; i < 5; i++)
        printf ("%02x:", mac[i]);
    printf ("%02x", mac[5]);
}

void Utilities::checkRunningUnderRoot()
{
    const uint32_t userID { getuid() };
    if (0 != userID)
    {
        // throw std::runtime_error("Application require the ROOT user access"  );
        std::cerr << "Application require the ROOT user access" << std::endl;
        std::exit(0);
    }
}


[[nodiscard("nodiscard")]]
std::string Utilities::IpToStr(unsigned long address) {
    return inet_ntoa({ static_cast<in_addr_t>(htonl(address)) });
}


[[nodiscard("Don't forget to use the return value somehow.")]]
std::string HostToIp(std::string_view host) noexcept
{
    const hostent* hostname { gethostbyname(host.data()) };
    if (hostname)
        return std::string { inet_ntoa(**(in_addr**)hostname->h_addr_list) };
    return std::string {};
}


[[nodiscard("Call may be expensive")]]
uint16_t Utilities::Checksum(uint16_t *ptr, uint16_t bytes) noexcept
{
    long sum = 0; // FIXME
    uint16_t oddByte {0};

    while (bytes > 1) {
        sum += *ptr++;
        bytes -= 2;
    }
    if (bytes == 1) {
        oddByte = 0;
        *((u_char *) & oddByte) = *(u_char *) ptr;
        sum += oddByte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    // FIXME
    uint16_t answer = ~sum;

    return answer;
}


[[nodiscard]]
sockaddr_ll Utilities::ResolveInterfaceAddress(std::string_view interfaceName)
{
    sockaddr_ll device {};

    /** Submit request for a socket descriptor to look up interface. **/
    Utilities::SocketScoped socketHandle = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (socketHandle < 0) {
        std::cout << "Failed to create socket" << std::endl;
        throw std::runtime_error("Ohh shit!");
    }

    /** Interface request structure used for socket ioctl: **/
    ifreq ifr { {""},{} };
    snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interfaceName.data()); // FIXME

    if (ioctl (socketHandle, SIOCGIFHWADDR, &ifr) < 0) {
        std::cout << "Failed to get source MAC address" << std::endl;
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

bool Utilities::ResolveInterfaceAddress(std::string_view interfaceName,
                                        sockaddr_ll& device) {
    /** Submit request for a socket descriptor to look up interface. **/
    Utilities::SocketScoped socketHandle = ::socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (socketHandle < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    /** Interface request structure used for socket ioctl: **/
    ifreq ifr { {""},{} };
    snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interfaceName.data()); // FIXME

    if (ioctl (socketHandle, SIOCGIFHWADDR, &ifr) < 0) {
        std::cerr << "Failed to get source MAC address" << std::endl;
        //::close(socketHandle);
        return false;
    }

    /** ZeroMemory + copy source MAC address. **/
    memset(&device, 0, sizeof(device));
    memcpy(device.sll_addr, ifr.ifr_hwaddr.sa_data, 6 * sizeof (uint8_t));

    /** Find interface index from interface name and store index in struct
     * sockaddr_ll device, which will be used as an argument of sendto(): **/
    if ((device.sll_ifindex = if_nametoindex(ifr.ifr_name)) == 0) {
        std::cerr << "Failed to obtain interface index" << std::endl;
        return false;
    }

    /** Fill out sockaddr_ll: **/
    device.sll_family = AF_PACKET;
    device.sll_halen = 6;

    return true;
}

void Utilities::SocketScoped::closeSocket(int s)
{
    if (INVALID_HANDLE != s && SOCKET_ERROR == ::close(s)) {
        std::cerr << "close() function failed with error: " << errno << std::endl;
    }
}
