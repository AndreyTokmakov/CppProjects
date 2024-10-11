/**============================================================================
Name        : OptimizationTricks.cpp
Created on  : 21.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : OptimizationTricks.cpp
============================================================================**/

#include "OptimizationTricks.h"
#include <iostream>
#include <vector>
#include <cstdint>


namespace BoundsChecking
{
    constexpr int size = 10;

    void check_original(int index)
    {
        if (index < 0 || index >= size) {
            std::cout << "\tError: Index out of range.\n";
        }
        else {
            std::cout << "\tOK.\n";
        }
    }

    /**
    A possible negative value of i will appear as a large positive number when i is
    interpreted as an unsigned integer and this will trigger the error condition. Replacing two
    comparisons by one makes the code faster because testing a condition is relatively
    expensive, while the type conversion generates no extra code at all.
    */

    void check_new(int index)
    {
        if ((unsigned int)index >= (unsigned int)size) {
            std::cout << "\tError: Index out of range.\n";
        }
        else {
            std::cout << "\tOK.\n";
        }
    }


    void Tests()
    {
        for (const int idx: std::vector<int>{ 1, 14, -1,9, 5, 10, 0})
        {
            std::cout << idx << std::endl;
            check_original(idx);
            check_new(idx);
        }
    }
}

namespace BitwiseOperators_CheckingMultipleValues
{
    namespace Original
    {
        enum class Weekdays
        {
            Monday,
            Tuesday,
            Wednesday,
            Thursday,
            Friday,
            Saturday,
            Sunday,
        };

        void checkDay(Weekdays day)
        {
            if (day == Weekdays::Tuesday || day == Weekdays::Wednesday || day == Weekdays::Friday) {
                std::cout << "DoThisThreeTimesAWeek()\n";
            }
            else {
                std::cout << "NOT DoThisThreeTimesAWeek()\n";
            }
        }

        void test()
        {
            checkDay(Weekdays::Monday);
            checkDay(Weekdays::Tuesday);
            checkDay(Weekdays::Wednesday);
        }
    }

    namespace New
    {
        enum class Weekdays
        {
            Monday = 0x01,
            Tuesday = 0x02,
            Wednesday = 0x04,
            Thursday = 0x08,
            Friday = 0x10,
            Saturday = 0x20,
            Sunday = 0x40
        };

        void checkDay(Weekdays day)
        {
            if (static_cast<int>(day) & (static_cast<int>(Weekdays::Tuesday) |
                                         static_cast<int>(Weekdays::Wednesday) |
                                         static_cast<int>(Weekdays::Friday))) {
                std::cout << "DoThisThreeTimesAWeek()\n";
            }
            else {
                std::cout << "NOT DoThisThreeTimesAWeek()\n";
            }
        }

        void test()
        {
            checkDay(Weekdays::Monday);
            checkDay(Weekdays::Tuesday);
            checkDay(Weekdays::Wednesday);
        }
    }


    void Tests()
    {
        Original::test();
        std::cout << std::endl;
        New::test();
    }
}

namespace ModuloWithoutDivision
{
    // Duration in CPU cycles:
    // Addition       : ~ 1 cycle (could be even less, while all operations are still performed in
    //                             a whole number of cycles, some of them can be performed in parallel)
    // Multiplication : ~ 6 cycles
    // Division       : ~ 30 to 60 cycles.


    /**
     * Works in case if d is power of 2
     **/
    inline constexpr uint32_t moduloFast(const uint32_t n, const uint32_t d) noexcept {
        return n & (d - 1);
    };


    void Tests()
    {
        static_assert(0 == moduloFast(4, 4));
        static_assert(3 == moduloFast(15, 4));
        static_assert(0 == moduloFast(4, 2));
    }
}

void OptimizationTricks::TestAll()
{
    // BoundsChecking::Tests();
    BitwiseOperators_CheckingMultipleValues::Tests();
    // ModuloWithoutDivision::Tests();
}