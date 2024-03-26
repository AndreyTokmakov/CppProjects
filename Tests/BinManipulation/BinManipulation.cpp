/**============================================================================
Name        : BinManipulation.cpp
Created on  : 25.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : BinManipulation.cpp
============================================================================**/

#include "BinManipulation.h"
#include <iostream>

namespace
{
    constexpr u_int8_t CHAR_BIT { 8 };
}


namespace BinManipulation
{
    bool detect_if_two_integers_have_opposite_signs(int x, int y)
    {
        return  ((x ^ y) < 0);
    }

    void Detect_If_Two_Integers_Have_Opposite_Signs()
    {
        std::cout << std::boolalpha << detect_if_two_integers_have_opposite_signs(1, -1) << std::endl;
        std::cout << std::boolalpha << detect_if_two_integers_have_opposite_signs(-1, -1) << std::endl;
        std::cout << std::boolalpha << detect_if_two_integers_have_opposite_signs(-1, 1) << std::endl;
    }
}

namespace BinManipulation
{
    u_int32_t compute_absolute_value(int32_t v)
    {
        const int32_t mask = v >> (sizeof(int32_t) * CHAR_BIT - 1);
        return (v + mask) ^ mask;
    }

    // Compute the integer absolute value (abs) without branching
    void ComputeAbsoluteValue()
    {
        std::cout << compute_absolute_value(123) << std::endl;
        std::cout << compute_absolute_value(-123) << std::endl;
        std::cout << compute_absolute_value(0) << std::endl;
    }
}

void BinManipulation::TestAll()
{
    // Detect_If_Two_Integers_Have_Opposite_Signs();
    // ComputeAbsoluteValue();

    /*
    const unsigned int n = 9;
    const unsigned int s = 7;
    const unsigned int d = 1U << s;     // So, `d` is one of 1, 2, 4, 8, 16, 32, …

    unsigned int m =  n & ((1U << s) - 1);

    std::cout << n << " % " <<  s << " = " << m << std::endl;
    */

    const unsigned int n = 9, s = 7;

    const unsigned int d = 1U << s;     // So, `d` is one of 1, 2, 4, 8, 16, 32, …

    unsigned int m  = n & (d - 1);

    printf("%d %% %d = %d", n, d, m);

}