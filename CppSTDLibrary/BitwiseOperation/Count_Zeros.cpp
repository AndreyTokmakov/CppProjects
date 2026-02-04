/**============================================================================
Name        : Count_Zeros.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Count_Zeros.cpp
============================================================================**/

#include <iostream>
#include <string>
#include <array>
#include <numeric>
#include <functional>
#include <bitset>
#include <cstdint>

#include "BitwiseOperation.h"

namespace bit_utils
{
    constexpr inline uint8_t charBit { 8 };
    constexpr inline uint8_t maxSize { sizeof(u_int16_t) * charBit };

    [[nodiscard]]
    static constexpr bool isSet(const uint16_t mask, const uint16_t index) noexcept {
        return mask & (1u << index);
    }

    [[nodiscard]]
    static constexpr bool isNotSet(const uint16_t mask, const uint16_t index) noexcept {
        return not isSet(mask, index);
    }

    static constexpr void unsetBit(uint16_t& mask, const uint16_t bit) noexcept {
        mask &= ~(1 << bit);
    };

    static constexpr void setBit(uint16_t& mask, const uint16_t bit) noexcept {
        mask |= (1 << bit);
    }

    void printBits(const uint16_t mask)
    {
        std::cout << mask << "  ===>  ";
        for (int i = (sizeof(uint16_t) * charBit) - 1; i >= 0; --i)
            std::cout << (mask & (1u << i) ? '1' : '0');
        std::cout << std::endl;
    }
}



namespace count_zeros
{
    uint16_t find_from(const uint16_t mask, const uint16_t start)
    {
        const uint16_t filtered = mask & ~((uint16_t{1} << start) - 1);
        return filtered ? std::countr_zero(filtered) : 16;
    }

    uint16_t find_last_le(const uint16_t mask, const uint16_t start)
    {
        const uint16_t filtered = mask & ~((uint16_t{1} << start) - 1);
        return filtered ? std::countl_zero(filtered) : 16;
    }

    void numberZeros_FromRight()
    {
        /// std::countr_zero(x) → возвращает количество подряд идущих нулей справа

        const uint16_t mask = 0b0000'0000'0100'0000;
        bit_utils::printBits(mask);

        {
            const int first = std::countr_zero(mask);
            std::cout << first << std::endl;
        }
        {
            const int first = find_from(mask, 0);
            std::cout << first << std::endl;
        }
    }

    void numberZeros_FromLeft()
    {
        /// std::countl_zero(x) → возвращает количество подряд идущих нулей слева
        const uint16_t mask = 0b0000'0000'0100'0000;
        bit_utils::printBits(mask);

        const int first = find_last_le(mask, 0);
        std::cout << first << std::endl;
    }

    // 64  ===>  0000000001000000
    // 6
    // 6
    // 64  ===>  0000000001000000
    // 9
}


void BitwiseOperation::Count_Zeros_Demo()
{
    count_zeros::numberZeros_FromLeft();
    count_zeros::numberZeros_FromLeft();
}