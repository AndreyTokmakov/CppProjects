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

namespace Strings
{
    constexpr size_t getLength(const char* str)
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

void Strings::TestAll()
{
    // GetStringLengthAsCompileTime();

    std::cout << sizeof(std::string) << std::endl;

};
