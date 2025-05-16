/**============================================================================
Name        : Stacktrace.cpp
Created on  : 16.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++Stacktrace
============================================================================**/

#include "Stacktrace.h"

#include <iostream>
#include <stacktrace>

namespace Stacktrace
{
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

}


void Stacktrace::TestAll()
{
    // testSimple();
    Test2();
}