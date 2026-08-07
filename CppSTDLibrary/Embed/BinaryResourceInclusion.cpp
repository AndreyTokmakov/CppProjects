/**============================================================================
Name        : BinaryResourceInclusion.cpp
Created on  : 23.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : BinaryResourceInclusion.cpp
============================================================================**/

#include "BinaryResourceInclusion.hpp"

#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>
#include <print>

#define TEST_DIR "\"../../CppSTDLibrary/data/"
#define TEST_DIR_FILE(file) TEST_DIR file "\""

namespace
{
    constexpr unsigned char data[] {
        #embed "../../CppSTDLibrary/data/test_file.txt"
            ,'\0' // null terminator
    };

    constexpr unsigned char message[] {
        #embed "../../CppSTDLibrary/data/message.txt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
            ,'\0' // null terminator
    };

    constexpr unsigned char largeMessage[] {
        #embed "../../CppSTDLibrary/data/large_message.txt" limit(16)
    };

    constexpr unsigned char default_config[] = {
        #embed "../../CppSTDLibrary/data/defaults.json"
    };

    std::vector<uint8_t> loadConfig(const std::filesystem::path & path)
    {
        if (std::ifstream file(path, std::ios::binary); file.is_open()) {
            return {std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
        }
        return {std::begin(default_config), std::end(default_config)};
    }

    /*
    static constexpr unsigned char data2[] = {
        #embed "../../CppSTDLibrary/data/test_file.txt" suffix("\0")
    };

    constexpr auto config = {
        #embed "../../CppSTDLibrary/data/large_message.txt" as(text)
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

    // limit(64)

    void test()
    {
        std::cout << data << std::endl;
        std::cout << message << std::endl;
        std::cout << largeMessage << std::endl;
    }

    void loadConfigTest()
    {
        // std::println("{}", loadConfig(""));
        for (char c: loadConfig("")) {
            std::cout << c;
        }
    }
}


void Embed::TestAll()
{
    // test();
    loadConfigTest();
}
