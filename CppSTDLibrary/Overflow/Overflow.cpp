/**============================================================================
Name        : Overflow.cpp
Created on  : 02.10.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Overflow.cpp
============================================================================**/

#include "Overflow.h"
#include <iostream>

namespace Overflow
{

};

void Overflow::TestAll()
{
    // bool __builtin_add_overflow(type1 a, type2 b, type3 *res);
    // bool __builtin_sub_overflow(type1 a, type2 b, type3* res);
    // bool __builtin_mul_overflow(type1 a, type2 b, type3* res);


    int32_t result { 0 };
    int32_t a { std::numeric_limits<int32_t>::max() / 2 + 1 };
    int32_t b { std::numeric_limits<int32_t>::max() / 2  + 1};

    std::cout << std::boolalpha << __builtin_add_overflow(a, b, &result) << std::endl;
}