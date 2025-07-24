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


namespace BitUtils
{

    template<typename T>
    void show_bits(T x)
    {
        std::cout << x << "  ===>  ";
        for (int i = (sizeof(T) * 8) - 1; i >= 0; i--)
            std::cout << (x & (1u << i) ? '1' : '0');
        std::cout << std::endl;
    }

    template<typename T>
    void set_bit(T& num, uint32_t bit)
    {
        if (sizeof(T) * 8 > bit - 1 )
        {
            num |= (1 << (bit - 1));
        }
    }

    constexpr bool is_odd(const int value) noexcept {
        return value & 1;
    }

    constexpr bool is_even(const int value) noexcept {
        return ! (value & 1);
    }

    constexpr bool is_pow_of_2(const int value) noexcept {
        return (value && !(value & value - 1));
    }

    constexpr int divide_by_2(const int value) {
        return value >> 1 ;
    }

    constexpr int mid_point(const int a, const int b) noexcept {
        return (a + b) >> 1 ;
    }

    constexpr int multiply_by_2(const int value) {
        return value << 1 ;
    }

    /**
     * Works in case if d is power of 2
     **/
    inline constexpr uint32_t modulo(const uint32_t n, const uint32_t d) noexcept {
        return n & (d - 1);
    };
}


void BinManipulation::TestAll()
{
    // Detect_If_Two_Integers_Have_Opposite_Signs();
    // ComputeAbsoluteValue();

    for (int i = 0;i < 255; ++i) {
        BitUtils::show_bits(i);
    }

    // 128  ===>  10000000

}