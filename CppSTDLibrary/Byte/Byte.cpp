//============================================================================
// Name        : Byte.cpp
// Created on  : 02.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Byte src
//============================================================================

#include "Byte.h"

#include <iostream>
#include <iomanip>
#include <spdlog/fmt/bundled/core.h>

namespace bytes
{
    template <typename T>
    auto only_return_int_type_bar(std::byte& b) {
        return T(b);
    }

    /*
    void BaseTests()
    {
        std::byte test {64};

        std::cout << only_return_int_type_bar<int>(test) << std::endl;
        std::cout << only_return_int_type_bar<float>(test) << std::endl;

        std::byte b{ 42 };

        std::cout << atoi(static_cast<char>(b)) << std::endl;
        b <<= 1;
        std::cout << std::to_integer<int>(b) << std::endl;
    }

    void ToInt()
    {
        std::byte b { 32 };
        std::cout << std::to_integer<int>(b) << std::endl;

        // b *= 2 compilation error
        b <<= 1;
        std::cout << std::to_integer<int>(b) << std::endl;
    }
    */
};

namespace bytes::Size
{

    class Class_3_Byte
    {
        std::byte b1;
        std::byte b2;
        std::byte b3;
    };

#pragma pack(push, 1)
    class Class1 {
        std::byte b1;
        std::byte b2;

        int var;
    };
#pragma pack(pop)


    void Test() {
        std::cout << "Class_3_Byte: " << sizeof(Class_3_Byte) << std::endl;
        std::cout << "Class1: " << sizeof(Class1) << std::endl;
    }
}



namespace bytes::Swap_Bytes
{
    template<std::integral T>
    void dump(T v, const char term = '\n')
    {
        std::cout << std::hex << std::uppercase << std::setfill('0')
                  << std::setw(sizeof(T) * 2) << v << " : ";
        for (std::size_t i{}; i != sizeof(T); ++i, v >>= 8)
            std::cout << std::setw(2) << static_cast<unsigned>(T(0xFF) & v) << ' ';
        std::cout << std::dec << term;
    }

    void swap_Bytes()
    {
        static_assert(std::byteswap('a') == 'a');

        std::cout << "byteswap for U16:\n";
        constexpr auto x = std::uint16_t(0xCAFE);
        dump(x);
        dump(std::byteswap(x));

        std::cout << "\nbyteswap for U32:\n";
        constexpr auto y = std::uint32_t(0xDEADBEEFu);
        dump(y);
        dump(std::byteswap(y));

        std::cout << "\nbyteswap for U64:\n";
        constexpr auto z = std::uint64_t{0x0123456789ABCDEFull};
        dump(z);
        dump(std::byteswap(z));

        /**
        byteswap for U16:
        CAFE : FE CA
        FECA : CA FE

        byteswap for U32:
        DEADBEEF : EF BE AD DE
        EFBEADDE : DE AD BE EF

        byteswap for U64:
        0123456789ABCDEF : EF CD AB 89 67 45 23 01
        EFCDAB8967452301 : 01 23 45 67 89 AB CD EF

        **/
    }
}

namespace bytes::hex
{
    void hex_string_to_int()
    {
        const int value = std::stoi("A7", nullptr, 16);
        std::cout << value << std::endl;
        // ---> 167
    }

    void print_bytes_as_hex()
    {
        unsigned char bytes[] = {0x1A, 0xB4, 0xC8, 0x5F, 0x02};

        // Print bytes as hexadecimal
        for (unsigned char byte : bytes) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }

        std::cout << std::endl;
        // --> 1a b4 c8 5f 02
    }
}


namespace bytes::handle_message
{
    void printMessage(const uint32_t* ptr, const size_t lenInBytes)
    {
        constexpr uint32_t typeSize = sizeof(uint32_t);
        for (uint32_t idx = 0, size = lenInBytes / typeSize; idx < size; ++idx) {
            std::cout << std::hex << std::uppercase << ptr[idx] << ' ';
        }
        std::cout << std::endl;
    }

    void printMessage_CastTo_Uint8T(const uint32_t* ptr, const size_t lenInBytes)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(ptr);
        constexpr uint32_t typeSize = sizeof(uint32_t);

        for (uint32_t idx = 0, size = lenInBytes; idx < size; idx += typeSize)
        {
            std::cout << std::hex << std::uppercase
                << static_cast<int>(bytes[idx + 3])
                << static_cast<int>(bytes[idx + 2])
                << static_cast<int>(bytes[idx + 1])
                << static_cast<int>(bytes[idx])  << ' ';
        }
        std::cout << std::endl;
    }

    void handeUIntBuffer()
    {
        constexpr std::array<uint32_t, 8> buffer {
            3000000031,
            3000000032,
            3000000033,
            3000000034,
            3000000035,
            3000000036,
            3000000037,
            3000000038
        };

        printMessage(buffer.data(), buffer.size() * sizeof(decltype(buffer)::value_type));
        printMessage_CastTo_Uint8T(buffer.data(), buffer.size() * sizeof(decltype(buffer)::value_type));

        // B2D05E1F B2D05E20 B2D05E21 B2D05E22 B2D05E23 B2D05E24 B2D05E25 B2D05E26
        // B2D05E1F B2D05E20 B2D05E21 B2D05E22 B2D05E23 B2D05E24 B2D05E25 B2D05E26
    }
}

namespace bytes::std_byte
{
    void byte_to_int()
    {
        constexpr std::byte b = std::byte { 0xA7 };
        constexpr int32_t x = std::to_integer<int32_t>(b);

        std::cout << x << std::endl; // --> 167
    }
}

void bytes::TestAll()
{
    // ToInt();
    // BaseTests();
    // Size::Test();

    // Swap_Bytes::swap_Bytes();

    // hex::print_bytes_as_hex();
    // hex::hex_string_to_int();

    // handle_message::handeUIntBuffer();

    std_byte::byte_to_int();
};