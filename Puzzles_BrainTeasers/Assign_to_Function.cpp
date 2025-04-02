/**============================================================================
Name        : Assign_to_Function.cpp
Created on  : 02.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Assign_to_Function.h
============================================================================**/

#include "Puzzles.h"
#include <iostream>
#include <exception>

namespace
{
    struct String
    {
        std::string value;

        String(std::string&& str) : value(std::move(str)) {
            std::cout << "String::String(" << value << ")\n";
        }

        String(const String& str) : value { str.value } {
            std::cout << "String::String(const String& [" << value << "])\n";
        }

        String(String&& str) noexcept : value { std::move(str.value) } {
            std::cout << "String::String(String&& [" << value << "])\n";
        }

        String& operator=(const String& str) {
            value = str.value;
            std::cout << "String& operator=(const String& [" << value << "])\n";
            return *this;
        }

        String& operator=(String&& str) noexcept {
            value = std::move(str.value);
            std::cout << "String& operator=(String&& [" << value << "]) noexcept \n";
            return *this;
        }

        ~String()  {
            std::cout << "String::~String( " << value << ")\n";
        }
    };
}

namespace
{
    String getName()
    {
        return String{"Alice"};
    }

}

namespace
{
    void demo1()
    {
        String name{"<--- Bob"};
        getName() = name;
        std::cout << "Assigned to a function!\n";
    }

    void demo2()
    {
        String name{"<--- Bob"};
        getName() = std::move(name);
        std::cout << "Assigned to a function!\n";
    }
}

void Puzzles::Assign_to_Function()
{
    demo1();
    // demo2();
}

/**
String::String(<--- Bob)
String::String(Alice)
String& operator=(const String& [<--- Bob])
String::~String( <--- Bob)
Assigned to a function!
String::~String( <--- Bob)
*/