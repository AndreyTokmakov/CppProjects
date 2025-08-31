/**============================================================================
Name        : CompileTimeProgramming.cpp
Created on  : 17.11.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CompileTimeProgramming.cpp
============================================================================**/

#include "CompileTimeProgramming.hpp"
#include "GetClassName_CompileTime.hpp"

#include <iostream>
#include <algorithm>
#include <string_view>

namespace CompileTimeProgramming
{
    template <int N>
    struct factorial
    {
        static const int value = N * factorial<N - 1>::value;
    };

    template <>
    struct factorial<0> {
        static const int value = 1;
    };


    constexpr int factorialFunc(const int value)
    {
        int result = 1;
        for (int i = 2; i <= value; i++)
            result *= i;
        return result;
    };


    void Factorial()
    {
        static_assert(1 == factorial<0>::value);
        static_assert(1 == factorial<1>::value);
        static_assert(2 == factorial<2>::value);
        static_assert(6 == factorial<3>::value);
        static_assert(24 == factorial<4>::value);

        static_assert(1 == factorialFunc(0));
        static_assert(1 == factorialFunc(1));
        static_assert(2 == factorialFunc(2));
        static_assert(6 == factorialFunc(3));
        static_assert(24 == factorialFunc(4));
    }
}


namespace StringCompileTimeValidation
{
    constexpr bool is_valid(char c)
    {
        constexpr std::string_view valid = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        return valid.find(c) != std::string_view::npos;
    }

    template<size_t N>
    struct Format
    {
        constexpr Format(const char (&str)[N]) {
            std::copy_n(str, N, value);
        }

        [[nodiscard]]
        constexpr bool isValid() const
        {
            if constexpr (N == 0){
                return false;
            }

            for (char ch : value) {
                if(ch != '\0' && !is_valid(ch)) {
                    return false;
                }
            }

            return true;
        }

        [[nodiscard]]
        constexpr std::string_view format() const {
            return value;
        }

        char value[N]{};
    };

    // Function template with a non-type template parameter
    template <Format Fmt>
    void processString()
    {
        static_assert(Fmt.isValid(), "String format is invalid!");
        std::cout << "Processing string: " << Fmt.format() << std::endl;
    }

    void compileTimeValidation()
    {
        processString<"Valid123">();  // This compiles successfully
        // processString<"Invalid@Char">();  // This would fail at compile-time
    }
}


void CompileTimeProgramming::TestAll()
{
    // Factorial();
    // StringCompileTimeValidation::compileTimeValidation();
    GetClassName_CompileTime::TestAll();
}