//============================================================================
// Name        : BitwiseOperation.cpp
// Created on  : 07.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Bitwise operation src
//============================================================================

#include <iostream>
#include <string>
#include <array>
#include <numeric>
#include <functional>
#include <bitset>
#include <cstdint>

#include "../Helpers/Helpers.h"
#include "BitwiseOperation.h"


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


namespace BitwiseOperation
{
    using namespace BitUtils;


    void Modulo_Tests()
    {
        static_assert(9 == modulo(9, 64));
        static_assert(2 == modulo(66, 64));
        static_assert(66 == modulo(66, 1024));
        static_assert(26 == modulo(1050, 1024));
        static_assert(66 == modulo(66, 1024));
    }


    void ShowBitsTests()
    {
        uint16_t val = 5;
        show_bits(val);
    }

    void BaseTests()
    {
        // a = 5(00000101), b = 9(00001001)
        uint16_t a = 5, b = 9;

        show_bits(a);
        show_bits(b);

        // The result is 00000001 w4
        std::cout << "a & b  = " << (a & b) << std::endl;
        show_bits(a & b);

        // The result is 00001101
        std::cout << "\na | b  = " << (a | b) << std::endl;
        show_bits(a | b);


        // The result is 00001100
        std::cout << "\na ^ b  = " << (a ^ b) << std::endl;
        std::cout << "a ^ b  = "; show_bits(a ^ b);

        // The result is 11111010
        std::cout << "\n~a   = " << (~a) << std::endl;
        std::cout << "~a  = "; show_bits(~a);

        // The result is 00010010
        std::cout << "\nb<<1   = " << (b << 1) << std::endl;
        std::cout << "b<<1  = "; show_bits(b << 1);

        {
            std::cout << "'>>' operation.\n";
            int x = 170;
            std::cout << "\n\nBefore: " << x << " : "; show_bits(x);
            std::cout << "   x >> 1 :\n";
            int result = (x >> 1);
            std::cout << "After : " << result << "  : "; show_bits(result);
        }

        {
            std::cout << "'>>' operation.\n";
            int x = 170;
            std::cout << "\n\nBefore: " << x << " : "; show_bits(x);
            std::cout << "   x >> 2 :\n";
            int result = (x >> 2);
            std::cout << "After : " << result << "  : "; show_bits(result);
        }
    }

    void BaseTests1() {
        unsigned int value = 1;

        show_bits(value);
        for (int i = 1; i < ((sizeof(int) * 8)); i++) {
            value <<= 1;
            show_bits(value);
        }
    }


    int __Find_Element_Occured_Only_once(int arr[], int length)
    {
        int res = 0, i;
        for (i = 0; i < length; i++) {
            res ^= arr[i];
            // std::cout << arr[i] << "   " << res << std::endl;
        }
        return res;
    }


    void Print_Integer_Value_Bits_Test() {
        int value = 234;

        std::cout << value << "  ===>  ";
        for (int i = (sizeof(value) * 8) - 1; i >= 0; i--) {
            int val = (value& (1u << i));
            std::cout << val << " --> " << (val ? '1' : '0') << std::endl;
        }
        std::cout << std::endl;
    }

    void IsOdd_Test()
    {
        for (int i = 0; i < 10; i++) {
            std::cout << i << " is odd ? = " << BitUtils::is_odd(i) << std::endl;
        }
    }

    void IsEven_Test()
    {
        for (int i = 0; i < 10; i++) {
            std::cout << i << " is even ? = " << BitUtils::is_even(i) << std::endl;
        }
    }

    void  Find_Element_Occured_Only_once_Test()
    {
        {
            int arr[] = { 12, 12, 14, 90, 14, 14, 14, 11, 23,23, 22,22, 90 };
            int n = sizeof(arr) / sizeof(arr[0]);
            int result = __Find_Element_Occured_Only_once(arr, n);
            std::cout << "The odd occurring element is " << result << std::endl;
        }
    }

    void Swap_Bits()
    {
        unsigned int value = 12345;
        show_bits(value);

        unsigned int swaped = ~12345;
        show_bits(swaped);


        std::cout << "\nTest 2" << std::endl;

        unsigned char c = 170;
        std::cout << 170 << " = "; show_bits(c);

        c = static_cast<char>(~170);
        std::cout << "170 (swaped)" << " = "; show_bits(c);
    }

    void Swap_Bits_2() {

        unsigned char c = 'A';
        show_bits(c);

        c = ~c;
        show_bits(c);
    }

    void SetBit()
    {
        int num = 4, bit = 1;

        show_bits(num);
        set_bit(num, 33);
        show_bits(num);
    }

    void UnSetBit()
    {
        constexpr auto unset_bit = [](int& num, unsigned short pos) {
            num &= (~(1 << pos));
        };

        int num = 20, bit = 4;

        show_bits(num);

        unset_bit(num, bit);

        show_bits(num);
    }


    void Divide_By_2_Test()
    {
        for (const int v: {1, 2, 4, 6, 8, 10})
        {
            std::cout << v << " = " << BitUtils::divide_by_2(v) << std::endl;
        }
    }

    void Mid_Point_Test()
    {
        for (const auto [a, b]: std::vector<std::pair<int, int>>{
            {1, 2}, {2, 3}, {4,8}
        }) {
            std::cout << a << " + " << b <<  " = " << BitUtils::mid_point(a, b) << std::endl;
        }
    }

    void Multiplying_By_2()
    {
        for (const int v: { 1, 2, 4, 6, 8, 10 })
        {
            std::cout << v << " = " << BitUtils::multiply_by_2(v) << std::endl;
        }
    }

    void Upper_to_Lower_Case() {
        {
            char c = 'A';
            std::cout << c << " --> ";
            c |= ' ';
            std::cout << c << std::endl;
        }

        std::cout << "\nTest2:" << std::endl;
        {
            std::string text = "AAAAAABBBBBCCCCCDDDDDD";
            std::cout << text << " --> ";
            for (char & i : text) {
                i |= ' ';
            }
            std::cout << text << std::endl;
        }
    }

    void Lower_to_Upper_Case() {
        {
            char c = 'A';
            std::cout << c << " --> ";
            c |= ' ';
            std::cout << c << std::endl;
        }

        std::cout << "\nTest2:" << std::endl;
        {
            std::string text = "aaaaaaabbbbbbbbcccccccccc";
            std::cout << text << " --> ";
            for (int i = 0; i < text.length(); i++) {
                text[i] &= '_';
            }
            std::cout << text << std::endl;
        }
    }

    void Count_Set_bits() {
        constexpr auto counter = [](int v)-> unsigned short {
            unsigned short count = 0;
            for (int i = 0; i < (sizeof(int) * 8); ++i)
                if (v & (1 << i))
                    count++;
            return count;
        };

        {
            const int value = 7;
            std::cout << value << " has " << counter(value) << " bits set" << std::endl;
        }
        {
            const int value = 128;
            std::cout << value << " has " << counter(value) << " bits set" << std::endl;
        }

    }

    int __log2Base(int x)
    {
        int res = 0;
        while (x >>= 1)
            res++;
        return res;
    }

    void Find_Log_Base_2() {
        int val = 8;
        std::cout << val << " --> " << __log2Base(val) << std::endl;
    }

    int __isPow2(int x) {
        return (x && !(x & x - 1));
    }

    void isPowerof2_Test()
    {
        int val = 9;
        std::cout << val << " --> " << __isPow2(val) << std::endl;

        val = 12;
        std::cout << val << " --> " << __isPow2(val) << std::endl;

        val = 32;
        std::cout << val << " --> " << __isPow2(val) << std::endl;
    }


    void Swap_Two_Numbers() {

        {
            int a = 11, b = 33;
            std::cout << a << " " << b << std::endl;
            a ^= b;
            b ^= a;
            a ^= b;
            std::cout << a << " " << b << std::endl;
        }
        {
            int a = 33, b = 500100;
            std::cout << a << " " << b << std::endl;
            a ^= b;
            b ^= a;
            a ^= b;
            std::cout << a << " " << b << std::endl;
        }
    }

    void InitVariable_BinaryForm() {
        {
            auto number = 0b011;
            std::cout << number << std::endl;
        }
        {
            auto number = 0b0111;
            std::cout << number << std::endl;
        }
    }

    void Check_Two_Numbers_Are_Equal()
    {
        int x = 10;
        int y = 10;
        if (!(x ^ y))
            std::cout << " x is equal to y " << std::endl;
        else
            std::cout << " x is NOT equal to y " << std::endl;
    }

    ///////////////////////////////////

    int BitString_To_Int(const std::string& str) {
        int num = 0, bit = 0;
        for (std::string::const_reverse_iterator iter = str.rbegin(); str.rend() != iter; iter++) {
            if ('1' == *iter)
                num |= (1 << bit);
            bit++;
        }
        return num;
    }

    std::string Int_To_BinString(int num) {
        if (0 == num)
            return "0";

        std::string str;
        for (int i = (sizeof(num) * 8) - 1; i >= 0; i--) {
            char b = (num& (1u << i)) ? '1' : '0';
            str.append(1, b);
        }
        return str.substr(str.find_first_of("1"));
    }

    void Sum_Two_BinStrins() {

        std::string a = "1101", b = "100";

        int v1 = BitString_To_Int(a);
        int v2 = BitString_To_Int(b);
        std::cout << Int_To_BinString(v1 + v2) << std::endl;

    }


    void Popcount()
    {
        for (std::uint8_t i : { 0, 0b11111111, 0b00011101 }) {
            std::cout << "popcount(0b" << std::bitset<8>(i) << ") = " << std::popcount(i) << '\n';
        }
    }

    void Count_Set_Bits()
    {
        for (const uint32_t mask: {0u,1u , 0b1111'0000u})
        {
            std::cout << mask << " --> " << std::popcount(mask) << std::endl;
        }
    }


    void Count_Zeros() {
        for (std::uint8_t i : { 0, 0b11100111, 0b00011100 }) {
            std::cout << "countl_zero(0b" << std::bitset<8>(i) << ") = " << std::countl_zero(i) << '\n';
            std::cout << "countr_zero(0b" << std::bitset<8>(i) << ") = " << std::countr_zero(i) << "\n\n";
        }
    }

    void Count_One_s() {

        // Returns the number of consecutive zero or one bits respectively in a given
        // value starting from the left, that is, starting with the most - significant bit

        for (std::uint8_t i : { 0, 0b11111111, 0b11100011 })
        {
            std::cout << "countr_one(0b" << std::bitset<8>(i) << ") = " << std::countr_one(i) << '\n';
            std::cout << "countl_one(0b" << std::bitset<8>(i) << ") = " << std::countl_one(i) << "\n\n";
        }
    }
}

void BitwiseOperation::TestAll()
{

    // InitVariable_BinaryForm();

    Modulo_Tests();

    // ShowBitsTests();
    // BaseTests();
    // BaseTests1();

    // Print_Integer_Value_Bits_Test();

    // IsOdd_Test();
    // IsEven_Test();

    // Find_Element_Occured_Only_once_Test();

    // Swap_Bits();
    // Swap_Bits_2();

    // SetBit();
    // UnSetBit();

    // Swap_Two_Numbers();

    // Divide_By_2_Test();
    // Mid_Point_Test();
    // Multiplying_By_2();
    // Check_Two_Numbers_Are_Equal();


    // Upper_to_Lower_Case();
    // Lower_to_Upper_Case();
    // Count_Set_bits();

    // Find_Log_Base_2();
    // isPowerof2_Test();

    // Sum_Two_BinStrins();


    // Bitset::Popcount();
    // Count_Set_Bits();

    // Count_Zeros();
    // Count_One_s();
};
