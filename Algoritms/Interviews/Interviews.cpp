//============================================================================
// Name        : Interviews.cpp
// Created on  : 25.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Interviews src class
//============================================================================

#include "Interviews.h"
#include <iostream>
// #include <format>

namespace Interviews {
    // In the Gregorian calendar, a year is a leap year in two cases:
    // either it is a multiple of 4, but it is not a multiple of 100, or it is a multiple of 400.
    // Implement a method that calculates the number of leap years from the beginning of
    // our era (the first year) to a given year, inclusive.
    // In fact, the Gregorian calendar was introduced much later, but here,
    // for simplicity, we extend its effect to our entire era.

    int leapYearCount(int year) {
        int k4 = year / 4;
        int k100 = year / 100;
        int k400 = year / 400;
        return k4 - k100 + k400;
    }

    void CountLeapYears() {
        [[maybe_unused]]
        int a = leapYearCount(2020);
        // std::cout << std::format("There was {} leap year in the range 0 - 2000", a) << std::endl;
    }
};

void Interviews::TEST_ALL() 
{
    CountLeapYears();
};