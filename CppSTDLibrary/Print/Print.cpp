/**============================================================================
Name        : Print.cpp
Created on  : 19.08.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Print.cpp
============================================================================**/

#include "Print.h"

#include <print>


void Print::TestAll()
{
    std::print("{} {} {}\n", "One", 1, 1.0);
    std::println("{} {} {}", "One", 1, 1.0);
}