//============================================================================
// Name        : FDI.h
// Created on  : 10.12.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : FDI
//============================================================================

#ifndef CPPPROJECTS_FDI_H
#define CPPPROJECTS_FDI_H

#include <string>
#include <string_view>
#include <array>

namespace FDI {
    using namespace std::string_view_literals;
    using IdType = uint_fast16_t;

    static constexpr size_t MAX_TEETH_COUNT { 32 };
    static constexpr size_t MAX_TEETH_NUM_PER_JAW_SIDE { MAX_TEETH_COUNT / 2 };

    // TODO: Check performance Sorted vs Unsorted
    // Teeth of the upper jaw according to the FDI notation
    static constexpr std::array<IdType, MAX_TEETH_NUM_PER_JAW_SIDE> upperTeeth {
            18,  17,  16,  15,  14,  13,  12,  11,
            21,  22,  23,  24,  25,  26,  27,  28
    };

    // TODO: Check performance Sorted vs Unsorted
    // Teeth of the lower jaw according to the FDI notation
    static constexpr std::array<IdType, MAX_TEETH_NUM_PER_JAW_SIDE> lowerTeeth {
            48,  47,  46,  45,  44,  43,  42,  41,
            31,  32,  33,  34,  35,  36,  37,  38
    };

    static constexpr std::array<IdType, MAX_TEETH_COUNT> teethIds {
            18,  17,  16,  15,  14,  13,  12,  11, 21,  22,  23,  24,  25,  26,  27,  28,
            48,  47,  46,  45,  44,  43,  42,  41, 31,  32,  33,  34,  35,  36,  37,  38
    };

    // TODO: Do we need it
    static constexpr std::array<std::string_view, MAX_TEETH_COUNT> teethIdsStr {
            "18"sv,  "17"sv,  "16"sv,  "15"sv, "14"sv,  "13"sv,  "12"sv,  "11"sv,
            "21"sv,  "22"sv,  "23"sv,  "24"sv, "25"sv,  "26"sv,  "27"sv,  "28"sv,
            "48"sv,  "47"sv,  "46"sv,  "45"sv, "44"sv,  "43"sv,  "42"sv,  "41"sv,
            "31"sv,  "32"sv,  "33"sv,  "34"sv, "35"sv,  "36"sv,  "37"sv,  "38"sv
    };

    static constexpr IdType MAX_LOWER_JAW_TOOTH =
            *std::max_element(lowerTeeth.begin(), lowerTeeth.end());

    static constexpr IdType MIN_LOWER_JAW_TOOTH =
            *std::min_element(lowerTeeth.begin(), lowerTeeth.end());

    static constexpr IdType MAX_UPPER_JAW_TOOTH =
            *std::max_element(upperTeeth.begin(), upperTeeth.end());

    static constexpr IdType MIN_UPPER_JAW_TOOTH =
            *std::min_element(upperTeeth.begin(), upperTeeth.end());

    [[nodiscard("Make sure to handle return value")]]
    constexpr bool isLowerTooth(FDI::IdType id) noexcept {
        return id >= MIN_LOWER_JAW_TOOTH && id <= MAX_LOWER_JAW_TOOTH && (20 != id && 19 != id);
    }

    [[nodiscard("Make sure to handle return value")]]
    constexpr bool isUpperTooth(FDI::IdType id) noexcept {
        return id >= MIN_UPPER_JAW_TOOTH && id <= MAX_UPPER_JAW_TOOTH && (40 != id && 39 != id);
    }

    [[nodiscard("Make sure to handle return value")]]
    constexpr bool isLeftSideTooth(FDI::IdType id) noexcept {
        return (id >= 21 && id <= 28) || (id >= 31 && id <= 38);
    }

    [[nodiscard("Make sure to handle return value")]]
    constexpr bool isRightSideTooth(FDI::IdType id) noexcept {
        return (id >= 11 && id <= 28) || (id >= 41 && id <= 48);
    }

}
#endif //CPPPROJECTS_FDI_H
