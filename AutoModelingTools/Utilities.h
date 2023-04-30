//============================================================================
// Name        : Utilities.h
// Created on  : 26.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Points
//============================================================================

#ifndef CPPEXPERIMENTS3_UTILITIES_H
#define CPPEXPERIMENTS3_UTILITIES_H

#include <string>
#include <array>
#include <string_view>
#include <algorithm>
#include <cstdint>

namespace Utilities {
    static constexpr size_t MAX_TEETH_COUNT { 32 };
    static constexpr size_t MAX_TEETH_NUM_PER_JAW_SIDE { MAX_TEETH_COUNT / 2 };

    // TODO: Check performance Sorted vs Unsorted
    // Teeth of the upper jaw according to the FDI notation
    static constexpr std::array<int16_t, MAX_TEETH_NUM_PER_JAW_SIDE> upperTooths {
            18,  17,  16,  15,  14,  13,  12,  11,
            21,  22,  23,  24,  25,  26,  27,  28
    };

    // TODO: Check performance Sorted vs Unsorted
    // Teeth of the lower jaw according to the FDI notation
    static constexpr std::array<int16_t, MAX_TEETH_NUM_PER_JAW_SIDE> lowerTooths {
            48,  47,  46,  45,  44,  43,  42,  41,
            31,  32,  33,  34,  35,  36,  37,  38
    };

    static constexpr uint_fast16_t MAX_LOWER_JAW_TOOTH = *std::max_element(lowerTooths.begin(), lowerTooths.end());
    static constexpr uint_fast16_t MIN_LOWER_JAW_TOOTH = *std::min_element(lowerTooths.begin(), lowerTooths.end());

    static constexpr uint_fast16_t MAX_UPPER_JAW_TOOTH = *std::max_element(upperTooths.begin(), upperTooths.end());
    static constexpr uint_fast16_t MIN_UPPER_JAW_TOOTH = *std::min_element(upperTooths.begin(), upperTooths.end());

    bool isLowerTooth([[maybe_unused]] uint_fast16_t id);
    bool isUpperTooth([[maybe_unused]] uint_fast16_t id);
};


#endif //CPPEXPERIMENTS3_UTILITIES_H
