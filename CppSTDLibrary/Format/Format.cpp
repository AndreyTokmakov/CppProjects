//============================================================================
// Name        : Format.cpp
// Created on  : 09.07.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Format 
//============================================================================

#include <iostream>
#include <string>
#include <string_view>

#include <algorithm>
#include <array>
#include <vector>

#include <chrono>
// #include <format>
#include <source_location>


#include "Format.h"

/*
namespace Format 
{
    void SimpleTest() {
        auto s = std::format("{} {}!", "Hello", "world", "something"); // OK, produces "Hello world!"
        std::cout << s << std::endl;

        s = std::format("{} {} {}", "Hello", "world", "something"); // OK, produces "Hello world!"
        std::cout << s << std::endl;
    }

    void Test_2() {

        using namespace std::literals::chrono_literals;

        std::string s1 = std::format("The answer is {}.\n", 42);
        std::cout << s1 << std::endl;

        std::string s2 = std::format("I'd rather be {1} than {0}.\n", "right", "happy");
        std::cout << s2 << std::endl;

        std::string buffer;

        std::format_to(std::back_inserter(buffer), "Hello, C++{}!\n", "20");
        std::cout << buffer;
        buffer.clear();

        std::format_to(std::back_inserter(buffer), "Hello, {0}::{1}!{2}\N", "std", "format_to()", "\n", "extra param(s)...");
        std::cout << buffer;
        buffer.clear();
    }

    void Test_Numbers() {
        std::string str = std::format("{0:{1}.{2}}", 4.2f, 4, 5);
        std::cout << str << std::endl;

        std::string str2 = std::format("{:{}.{}}", 4.2f, 4, 5);
        std::cout << str2 << std::endl; 

        std::string str3 = std::format("AAAA_{}_BBBB_{}", 123, 456);
    }

    void Format_To() {
        std::string buffer;

        std::format_to(std::back_inserter(buffer), //< OutputIt
                       "Hello, C++{}!\n",          //< fmt 
                       "20");                      //< arg
        std::cout << buffer;
        buffer.clear();
    }

    void Format_To_2() {
        std::string buffer;
        std::format_to(
            std::back_inserter(buffer), //< OutputIt
            "Hello, {0}::{1}!{2}",      //< fmt 
            "std",                      //< arg {0}
            "format_to()",              //< arg {1}
            "\n",                       //< arg {2}
            "extra param(s)...");       //< unused
        std::cout << buffer;
    }


    void TESTS() {
        const std::string out { R"(C:\Projects\cpp\VTK_Tests\DATA_AND_EXAMPLES\Out\{0}.stl)"};

        const auto destFile { std::format(out, 1)};
        std::cout << destFile << std::endl;
    }
};
 */

void Format::TestAll()
{
    // SimpleTest();
    
    // Format_To();
    // Format_To_2();
    
    // Test_2();

    // Test_Numbers();

    // TESTS();
}
