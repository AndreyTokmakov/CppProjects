/**============================================================================
Name        : FunctionCall_Resolution_Templates.cpp
Created on  : 03.04.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FunctionCall_Resolution_Templates.h
============================================================================**/

#include "Puzzles.h"
#include <iostream>
#include <exception>

namespace
{
    template<typename T>
    void serialize(T&)
    {
        std::cout << "template\n";
    }

    template<>
    void serialize<>(const std::string&)
    {
        std::cout << "specialization\n";
    }

    void serialize(const std::string&)
    {
        std::cout << "normal function\n";
    }
}

namespace
{
    void demo()
    {
        std::string hello_world{"Hello, world!"};
        serialize(hello_world);
        serialize(std::string{"Good bye, world!"});
    }
}

void Puzzles::FunctionCall_Resolution_Templates()
{
    demo();
}

/**
template
normal function
*/