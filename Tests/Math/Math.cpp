/**============================================================================
Name        : Math.cpp
Created on  : 16.07.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Math test
============================================================================**/

#include "Math.h"
#include <climits>
#include <limits>
#include <numeric>

namespace Math
{
    int calc(int val)
    {
        int count = 0;
        while (val > 1) {
            val /= 2;
            ++count;
        }
        return count;
    }

    void Log2Test()
    {
        calc(59218);
    }

    void Saturated_Operations()
    {
        constexpr int a = std::add_sat(3, 4); // no saturation occurs, T = int
        static_assert(a == 7);

        constexpr unsigned char b = std::add_sat<unsigned char>(UCHAR_MAX, 4); // saturated
        static_assert(b == UCHAR_MAX);

        constexpr unsigned char c = std::add_sat(UCHAR_MAX, 4); // not saturated, T = int
        // add_sat(int, int) returns int tmp == 259, then assignment truncates 259 % 256 == 3
        static_assert(c == 3);

        //  unsigned char d = std::add_sat(252, c); // Error: inconsistent deductions for T

        constexpr unsigned char e = std::add_sat<unsigned char>(251, a); // saturated
        static_assert(e == UCHAR_MAX);
        // 251 is of type T = unsigned char, `a` is converted to unsigned char value;
        // might yield an int -> unsigned char conversion warning for `a`

        constexpr signed char f = std::add_sat<signed char>(-123, -3); // not saturated
        static_assert(f == -126);

        constexpr signed char g = std::add_sat<signed char>(-123, -13); // saturated
        static_assert(g == std::numeric_limits<signed char>::min());          // g == -128
    }
}

void Math::TestAll()
{
    Log2Test();
}

