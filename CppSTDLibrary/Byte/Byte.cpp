/**============================================================================
Name        : Byte.cpp
Created on  : 30.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : bytes
============================================================================**/


#include "Byte.h"

#include <iostream>
#include <iomanip>
#include <concepts>
#include <bit>
#include <print>
#include <iostream>
#include <utility>
#include <cstddef>

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

namespace bytes::bit_cast_number_to_bytes
{
    template<std::integral Ty>
    constexpr std::array<std::byte, sizeof(Ty)> toBytesArray(const Ty value)
    {
        return std::bit_cast<std::array<std::byte, sizeof(Ty)>>(value);
    }

    template<std::integral Ty>
    constexpr Ty fromBytesArray(const std::array<std::byte, sizeof(Ty)>& array)
    {
        return std::bit_cast<Ty>(array);
    }

    void numToBytes_AndBack()
    {
        {
            constexpr uint16_t value = std::numeric_limits<uint16_t>::max() - 1;
            const std::array arrayOfBytes = toBytesArray(value);
            const auto result = fromBytesArray<decltype(value)>(arrayOfBytes);

            std::println("Original: {}, Result: {}", value, result);
        }
        {
            constexpr int32_t value = std::numeric_limits<int32_t>::max() - 1;
            const std::array arrayOfBytes = toBytesArray(value);
            const auto result = fromBytesArray<decltype(value)>(arrayOfBytes);

            std::println("Original: {}, Result: {}", value, result);
        }
        {
            constexpr uint64_t value = std::numeric_limits<uint64_t>::max() - 1;
            const std::array arrayOfBytes = toBytesArray(value);
            const auto result = fromBytesArray<decltype(value)>(arrayOfBytes);

            std::println("Original: {}, Result: {}", value, result);
        }

        // Original: 65534, Result: 65534
        // Original: 2147483646, Result: 2147483646
        // Original: 18446744073709551614, Result: 18446744073709551614
    }
}

namespace bytes::big_endian
{
    template<std::integral Ty, size_t N = sizeof(Ty)>
    constexpr std::array<uint8_t, N> toBytesArray(const Ty value)
    {
        std::array<uint8_t, N> bytes {};
        for (size_t idx = 0, mask = (N - 1) * 8; idx < N; ++idx, mask -= 8) {
            bytes[idx] =  static_cast<uint8_t>(value >> mask);
        }
        return bytes;
    }

    template<std::integral Ty, size_t N = sizeof(Ty)>
    Ty fromBytesArray(const std::array<uint8_t, N> bytes)
    {
        Ty result = {};
        for (size_t idx = 0, mask = (N - 1) * 8; idx < N; ++idx, mask -= 8) {
            result |= static_cast<Ty>(bytes[idx] << mask);
        }
        return result;
    }

    template<std::integral Ty, size_t N = sizeof(Ty)>
    constexpr std::array<uint8_t, N> toBytesArrayFold(Ty value)
    {
        return [&]<size_t... Idx> (std::index_sequence<Idx...>) { return std::array<uint8_t, N>{
            (static_cast<uint8_t>( value >> ((N - 1 - Idx) * 8 ) & 0xff))...
        };} (std::make_index_sequence<N>{});
    }

    template<std::integral Ty, size_t N = sizeof(Ty)>
    constexpr Ty fromBytesArrayFold(const std::array<uint8_t, N> bytes)
    {
        return [&]<size_t... Idx> (std::index_sequence<Idx...>) {
            return (( static_cast<Ty>(bytes[Idx]) << ((N - 1 - Idx) * 8)) + ... );
        } (std::make_index_sequence<N>{});
    }

    void testOne()
    {
        constexpr uint32_t value = 0x12345678;
        std::cout << value << " => " << std::hex << value << std::endl;

        const auto bytes = toBytesArray(value);
        for (const auto b : bytes) {
            std::cout << std::hex << static_cast<int>(b) << ' ';
        }

        const uint32_t result = fromBytesArray<uint32_t>(bytes);
        std::cout << '\n' << std::dec  << result << " => " << std::hex << result << std::endl;
    }

    void testFold()
    {
        constexpr uint32_t value = 0x12345678;
        std::cout << value << " => " << std::hex << value << std::endl;

        constexpr auto bytes = toBytesArrayFold(value);
        for (const auto b : bytes) {
            std::cout << std::hex << static_cast<int>(b) << ' ';
        }

        constexpr auto result = fromBytesArrayFold<uint32_t>(bytes);
        std::cout << '\n' << std::dec  << result << " => " << std::hex << result << std::endl;

        // 305419896 => 12345678
        // 12 34 56 78
        // 305419896 => 12345678
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

    // std_byte::byte_to_int();

    // bit_cast_number_to_bytes::numToBytes_AndBack();

    // big_endian::testOne();
    big_endian::testFold();
};