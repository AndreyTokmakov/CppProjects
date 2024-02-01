/**============================================================================
Name        : UnikieInterviews.cpp
Created on  : 01.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : UnikieInterviews.h
============================================================================**/

#include "UnikieInterviews.h"
#include <iostream>
#include <vector>

namespace Numbers
{
    int __gcd(int a, int b)
    {
        return 0 == b ? a : __gcd(b, a % b);
    }

    int __gcd1(int a, int b)
    {
        while (b != 0) {
            a = a % b;
            std::swap(a, b);
        }
        return a;
    }

    void GreatestCommonDivisor()
    {
        for (const auto& [values, expected] : std::vector<std::pair<std::pair<int, int>,int>> {
                {{9, 3}, 3},
                {{3, 9}, 3},
                {{0, 0}, 0},
                {{8, 6}, 2},
                {{128, 40}, 8},
                {{48, 18}, 6},
        })
        {
            std::cout << __gcd(values.first, values.second) << "  " <<  __gcd1(values.first, values.second)
                << " -->  " << expected << std::endl;
        }
    }
}

namespace Numbers
{
    int reverse_number(int number)
    {
        int16_t sign = number > 0 ? 1: -1;
        number = number * sign;
        int reverse_number = 0;
        while (0 != number) {
            reverse_number = reverse_number * 10 + number % 10;
            number = number / 10;
        }
        return reverse_number * sign;
    }

    void ReverseNumber()
    {
        for (const auto [num, expected]: std::vector<std::pair<int, int>> {
                {123,321},
                {-123,-321}
        })
        {
            std::cout << reverse_number(num) << "  " << expected << std::endl;
        };
    }
}


void UnikieInterviews::TestAll()
{
    // ::GreatestCommonDivisor();
    Numbers::ReverseNumber();

}