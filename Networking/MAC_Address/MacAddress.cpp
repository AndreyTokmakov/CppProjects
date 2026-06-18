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
        using size_type  = size_t;

        static constexpr size_type BytesNum { 6 };
        static constexpr size_type AddressStringSize { 17 };

        bool is_valid { false };

        explicit constexpr MAC_Address(const std::string_view str)
        {
            if (str.size() != AddressStringSize) {
                return;
            }
            for (size_type i = 0; i < BytesNum; ++i)
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

        constexpr uint8_t operator[](const size_type idx) const {
            return bytes[idx];
        }

    private:

        std::array<value_type, BytesNum> bytes{};
    };

}


void mac_address::TestAll()
{
    constexpr MAC_Address addr("00:11:22:33:44:55"); // -> [0, 17, 34, 51, 68, 85]
    static_assert(addr.is_valid);

    static_assert(addr[0] == 0);
    static_assert(addr[1] == 17);
    static_assert(addr[2] == 34);
    static_assert(addr[3] == 51);
    static_assert(addr[4] == 68);
    static_assert(addr[5] == 85);
}
