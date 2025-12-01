/**============================================================================
Name        : BinaryResourceInclusion.cpp
Created on  : 23.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : BinaryResourceInclusion.cpp
============================================================================**/

#include "BinaryResourceInclusion.hpp"

#include <iostream>

namespace Embed
{
    static constexpr unsigned char data[] = {
        #embed "../../CppSTDLibrary/data/test_file.txt"
            ,'\0' // null terminator
    };

    static constexpr unsigned char message[] =
    {
        #embed "../../CppSTDLibrary/data/message.txt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
            ,'\0' // null terminator
    };

    /*
    static constexpr unsigned char data2[] = {
        #embed "../../CppSTDLibrary/data/test_file.txt" suffix("\0")
    };

    constexpr auto config = {
        #embed "config.toml" as(text)
    };

    constexpr auto blob = {
        #embed "data.bin" as(binary)
    };

    constexpr std::array<std::uint8_t, sizeof(
        #embed "script.sql"
    )> data = {
        #embed "script.sql"
    };

    constexpr auto handlers = std::to_array<std::string_view>({
        #embed "routes.txt"
    });

    */

    void test()
    {
        std::cout << data << std::endl;
        std::cout << message << std::endl;
    }
}


void Embed::TestAll()
{
    Embed::test();
}
