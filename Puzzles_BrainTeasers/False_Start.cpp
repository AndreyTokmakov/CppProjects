/**============================================================================
Name        : False_Start.cpp
Created on  : 10.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : False_Start.h
============================================================================**/

#include "Puzzles.h"
#include <iostream>
#include <exception>

namespace
{
    struct Engine
    {
        ~Engine() { std::cout << "Engine stopped\n" ; }
    };

    struct Machine
    {
        Machine()
        {
            throw std::runtime_error{ "Failed tostart machine" };
        }

        ~Machine()
        {
            std::cout << "Machine stopped\n" ;
        }

        Engine engine_;
    };
}

namespace
{
    void demo()
    {
        try {
            Machine machine;
        }
        catch (...) {
        }
    }
}

void Puzzles::False_Start()
{
    demo();
}

/**
* Output:
* Engine stopped
*/