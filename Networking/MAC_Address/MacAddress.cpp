/**============================================================================
Name        : MacAddress.cpp
Created on  : 14.06.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MacAddress.cpp
============================================================================**/

#include "MacAddress.hpp"

#include <iostream>
#include <print>
#include <format>
#include <string_view>
#include <array>
#include <cstdint>
#include <charconv>

namespace mac_address
{

    struct MAC_Address
    {
        using value_type = uint8_t;

        static constexpr std::size_t BytesNum { 6 };
        static constexpr std::size_t AddressStringSize { 17 };

        std::array<value_type, BytesNum> bytes{};
        bool is_valid { false };

        explicit constexpr MAC_Address(const std::string_view str)
        {
            if (str.size() != AddressStringSize) {
                return;
            }
            for (size_t i = 0; i < BytesNum; ++i)
            {
                const std::string_view byte_str = str.substr(i * 3, 2);
                value_type value { 0 };
                auto [ptr, ec] = std::from_chars(byte_str.data(), byte_str.data() + byte_str.size(), value, 16);
                if (ec != std::errc()) {
                    return;
                }
                bytes[i] = value;
            }
            is_valid = true;
        }
    };

}


void mac_address::TestAll()
{
    constexpr MAC_Address addr("00:11:22:33:44:55");
    static_assert(addr.is_valid);
    std::cout << static_cast<unsigned int>(addr.bytes.at(5)) << std::endl;

}
