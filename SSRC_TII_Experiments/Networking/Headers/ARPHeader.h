/**============================================================================
Name        : ARPHeader.cpp
Created on  : 29.12.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : ARPHeader
============================================================================**/

#ifndef CPPPROJECTS_ARPHEADER_H
#define CPPPROJECTS_ARPHEADER_H

#include "EthernetHeader.h"


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

    void SetTargetAddress(uint32_t address) {
        target_ip = address;
    }
} __attribute__((packed, aligned(1))) ;;


#endif //CPPPROJECTS_ARPHEADER_H
