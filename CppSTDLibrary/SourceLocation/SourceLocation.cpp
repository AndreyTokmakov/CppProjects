//============================================================================
// Name        : SourceLocation.cpp
// Created on  : 09.07.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Source location  src
//============================================================================

#include <iostream>
#include <source_location>
#include "SourceLocation.h"

namespace SourceLocation
{

    void log(const std::string_view message,
        const std::source_location location = std::source_location::current())
    {
        std::cout << "file: "
            << location.file_name() << "("
            << location.line() << ":"
            << location.column() << ") `"
            << location.function_name() << "`: "
            << message << '\n';
    }

    template <typename T>
    void fun(T x)
    {
        log(x);
    }


    void Test1() {
        log("Hello world!");
        fun("Hello C++20!");
    }

    //--------------------------------------------------------------------

    void Test2()
    {
        constexpr std::source_location location = std::source_location::current();

        std::cout << "file_name     : " << location.file_name() << std::endl;
        std::cout << "function_name : " << location.function_name() << std::endl;
        std::cout << "line          : " << location.line() << std::endl;
        std::cout << "column        : " << location.column() << std::endl;

    }
};

void SourceLocation::TestAll()
{
    // Test1();
    Test2();
};