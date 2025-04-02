/**============================================================================
Name        : Class_Members_Creation_Order.cpp
Created on  : 10.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Class_Members_Creation_Order.h
============================================================================**/

#include "Puzzles.h"
#include <iostream>
#include <exception>

namespace
{
    struct Degrees
    {
        Degrees() : degrees(0)
        {
            std::cout << "Default constructed\n" ;
        }

        Degrees( double degrees) : degrees(degrees)
        {
            std::cout << "Constructed with " << degrees << "\n" ;
        }
        double degrees;
    };

    struct Position
    {
        Position() : latitude_{1} { longitude_ =
                                            Degrees{2}; }
        Degrees latitude_;Degrees longitude_;
    };
}

namespace
{
    void demo()
    {

    }
}

void Puzzles::Class_Members_Creation_Order()
{
    demo();
}

/**

**/