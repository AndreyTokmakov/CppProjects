/**============================================================================
Name        : Lifetime_Global_and_Static_Variables.cpp
Created on  : 02.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Lifetime_Global_and_Static_Variables.h
============================================================================**/

#include "Puzzles.h"
#include <iostream>
#include <exception>

#define ENABLED 0

namespace
{
#if ENABLED
    struct Connection
    {
        std::string name;

        explicit Connection(const std::string &name) : name(name) {
            std::cout << "Created " << name << "\n";
        }

        ~Connection() {
            std::cout << "Destroyed " << name << "\n";
        }
    };

    Connection global {"global" };

    Connection &get()
    {
        static Connection localStatic{"local static"};
        return localStatic;
    }
#endif

}

namespace
{

    void demo()
    {
#if ENABLED
        Connection local{"local"};
        Connection &tmp1 = get();
        Connection &tmp2 = get();
#endif
    }
}

void Puzzles::Lifetime_Global_and_Static_Variables()
{
    demo();
}

/**
Created global
Created local
Created local static

Destroyed local
Destroyed local static
Destroyed global
*/
