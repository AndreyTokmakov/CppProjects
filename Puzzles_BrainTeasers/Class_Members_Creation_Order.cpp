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
        double degrees;

        Degrees() : degrees(0)
        {
            std::cout << "Default constructed\n" ;
        }

        Degrees( double degrees) : degrees(degrees)
        {
            std::cout << "Constructed with " << degrees << "\n" ;
        }
    };

    struct Position
    {
        Degrees latitude;
        Degrees longitude;

        Position() : latitude{1} {
            longitude = Degrees{2};
        }
    };
}

namespace
{
    void demo()
    {
        Position position;
    }
}

void Puzzles::Class_Members_Creation_Order()
{
    demo();
}

/**
Constructed with 1
Default constructed
Constructed with 2
**/