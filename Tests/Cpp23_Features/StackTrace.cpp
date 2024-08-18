/**============================================================================
Name        : StackTrace.cpp
Created on  : 17.08.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StackTrace.cpp
============================================================================**/

#include "StackTrace.h"


#include <iostream>
#include <vector>
#include <stacktrace>
#include <format>
#include <print>


namespace StackTrace
{
#if 0
    void testSimple()
    {
        auto stacktrace = std::stacktrace::current();
        /*
        for (const auto& entry: stacktrace) {
            std::cout << "Description: " << entry.description() << '\n';
            std::cout << "file: " << entry.source_file() << " and line: " << entry.source_line() <<'\n';
            std::cout << '\n';
        }
        */


        auto currentStacktrace = std::stacktrace(); // Won't work as one might expect!
        for (const auto& entry : currentStacktrace) {
            std::cout << entry.description() << std::endl;
            std::cout << entry.source_file() << std::endl;
            std::cout << entry.source_line() << std::endl;
        }
    }


    void foo()
    {
        auto trace = std::stacktrace::current();
        for (const auto& entry: trace) {
            std::cout << "Description: " << entry.description() << std::endl;
            std::cout << "file: " << entry.source_file() << std::endl;
            std::cout << "line: " << entry.source_line() << std::endl;
            std::cout << "------------------------------------" << std::endl;
        }
    }

    void Test2()
    {
        foo();
    }

#endif
}

void StackTrace::TestAll()
{
    // testSimple();
    // Test2();
}