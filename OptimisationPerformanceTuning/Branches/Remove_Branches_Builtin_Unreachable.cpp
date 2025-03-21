/**============================================================================
Name        : Remove_Branches_Builtin_Unreachable.cpp
Created on  : 14.03.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Remove_Branches_Builtin_Unreachable.cpp
============================================================================**/

#include <cstdint>
#include <cstddef>
#include <numeric>
#include <array>

#include "Remove_Branches_Builtin_Unreachable.h"

namespace Remove_Branches_Builtin_Unreachable
{
    uint8_t sum(const uint8_t *data, const size_t len)
    {
        return std::accumulate(data, data + len, static_cast<uint8_t>(0));
    }

    /**
    * The length of the array that we want to sum up is always a multiple of 32
    * (the number of 8-bit values that fit in a YMM register).
    * This constraint can be a natural property of our input, or we can artificially add zero values as
    * padding until we reach a length that is a multiple of 32.
    * The array is non-empty. By convention, we can say that the sum function must NOT be called on empty arrays.
    * We can express these constraints with __builtin_unreachable()2, a compiler built-in which tells the compiler
    * that a specific code location/code path is unreachable:
    */
    uint8_t sum_with_constraints(const uint8_t *data, const size_t len)
    {
        constexpr size_t U8_VALUES_PER_YMM_WORD = 32;
        if (len % U8_VALUES_PER_YMM_WORD != 0)
            __builtin_unreachable(); // `len` is always a multiple of 32.
        if (len == 0)
            __builtin_unreachable(); // `len` is never zero.

        return std::accumulate(data, data + len, static_cast<uint8_t>(0));
    }
};

// LINK: https://nicula.xyz/2025/02/23/unwanted-branches.html

void Remove_Branches_Builtin_Unreachable::TestAll()
{
    std::array<uint8_t, 32> data;

    sum(data.data(), data.size());
    sum_with_constraints(data.data(), data.size());

}