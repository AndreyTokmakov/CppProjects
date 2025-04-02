/**============================================================================
Name        : No_Memory_CleanUp.cpp
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
    int getId () {
        return 1337;
    }

    void restrictedTask1 ()
    {
        int id = getId();
        if (id == 1337) {
            std::cout << "did task 1\n" ;
        }
    }

    void restrictedTask2 ()
    {
        int id;
        if (id == 1337) {
            std::cout << "did task 2\n" ;
        }
    }
}

namespace
{
    void demo()
    {
        restrictedTask1();
        restrictedTask2();
    }
}

void Puzzles::No_Memory_CleanUp()
{
    demo();
}

/**
* Output:  ------------- could be ------------
*   did task 1
*   did task 2
*/