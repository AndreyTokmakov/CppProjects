/**============================================================================
Name        : Strings.h
Created on  : 20.11.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Strings texts and experiments
============================================================================**/

#include "Strings.h"

#include <iostream>
#include <string>
#include <algorithm>

namespace Strings
{
    constexpr size_t getLength(const char*)
    {
        return 2;
    }

    constexpr std::size_t constexprLength1(std::string_view str)
    {
        return std::char_traits<char>::length(str.data());
        // return std::string::traits_type::length(s);
    }

    constexpr std::size_t constexprLength2(std::string_view str)
    {
        return std::string::traits_type::length(str.data());
    }

    void GetStringLengthAsCompileTime()
    {
        constexpr std::string_view text { "123456789123456789123456789"};

        // static_assert(text.size() == constexprLength(text));
    }

    constexpr bool checkSSO()
    {
        // constexpr std::string sd;
        return true;
    }
};

namespace Strings::Literals
{

    class Literal {
        const char* const mData{};
        const size_t      mLen{};

        friend constexpr Literal operator""_l(const char* data, size_t len);
        friend constexpr bool operator==(const Literal& l1, const Literal& l2);

        constexpr Literal(const char* data, size_t len)
                : mData { data }, mLen { len } {
        }

    public:
        // imagine the access functions you need
    };

    constexpr Literal operator""_l(const char* data, size_t len){
        return {data, len};
    }

    constexpr bool operator==(const Literal &l1, const Literal &l2) {
        // TODO: Add str content check
        return l1.mLen == l2.mLen;
    }

    void Insert([[maybe_unused]] Literal str,
                [[maybe_unused]] size_t len) {}

    void Test()
    {
        Insert("Hello"_l, 20);
        static_assert("Hello"_l == "Hello"_l);
    }
}


void Strings::TestAll()
{
    // GetStringLengthAsCompileTime();

    // std::cout << sizeof(std::string) << std::endl;

    std::string  buffer {};
    // buffer.reserve(32);
    buffer.append(128, '\0');

    // strcpy(buffer.data(), "qwerty", 6);
    std::copy_n( "qwerty", 6, buffer.data());

    std::cout << buffer << std::endl;
};
