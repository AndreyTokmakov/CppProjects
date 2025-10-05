/**============================================================================
Name        : NullPtr_T.cpp
Created on  : 01.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : NullPtr_T
============================================================================**/

#include <iostream>
#include "Utilities.h"

namespace
{
    void foo(int n)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void foo(char* s)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void foo(std::nullopt_t ptr)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

}

void Utilities::NullPtr_T::TestAll()
{

    foo(1);
    foo("sdsd");
    foo(std::nullopt);

    // void {anonymous}::foo(int)
    // void {anonymous}::foo(char*)
    // void {anonymous}::foo(std::nullopt_t)
}
