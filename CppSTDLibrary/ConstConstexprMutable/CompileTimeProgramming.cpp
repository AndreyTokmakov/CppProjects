/**============================================================================
Name        : CompileTimeProgramming.cpp
Created on  : 17.11.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CompileTimeProgramming.cpp
============================================================================**/

#include "CompileTimeProgramming.h"

namespace CompileTimeProgramming
{
    template <int N>
    struct factorial
    {
        static const int value = N * factorial<N - 1>::value;
    };

    template <>
    struct factorial<0> {
        static const int value = 1;
    };


    constexpr int factorialFunc(const int value)
    {
        int result = 1;
        for (int i = 2; i <= value; i++)
            result *= i;
        return result;
    };


    void Factorial()
    {
        static_assert(1 == factorial<0>::value);
        static_assert(1 == factorial<1>::value);
        static_assert(2 == factorial<2>::value);
        static_assert(6 == factorial<3>::value);
        static_assert(24 == factorial<4>::value);

        static_assert(1 == factorialFunc(0));
        static_assert(1 == factorialFunc(1));
        static_assert(2 == factorialFunc(2));
        static_assert(6 == factorialFunc(3));
        static_assert(24 == factorialFunc(4));
    }
}


void CompileTimeProgramming::TestAll()
{
    Factorial();
}