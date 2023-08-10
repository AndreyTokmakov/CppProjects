/**============================================================================
Name        : Cpp23_Features.cpp
Created on  : 26.07.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Cpp23_Features
============================================================================**/

#include "Cpp23_Features.h"

#include <iostream>
#include <string_view>
#include <array>
#include <vector>
#include <coroutine>


// C++ 23:
// #include <expected>

// #include <stacktrace>
// #include <flat_map>
// #include <flat_set>


namespace Cpp23_Features
{
    template <class T>
    int foo() {
        if constexpr (std::is_same_v<T, int>) {
            return 42;
        } else if constexpr (std::is_same_v<T, float>) {
            return 24;
        } else {
            static_assert(false, "T should be an int or a float");
        }
    }

    void Test()
    {
        foo<int>();
        // foo<std::string>();
    }

    struct S {
        static constexpr bool operator() (int x, int y) {
            return x < y;
        }
    };

    struct StaticArray
    {
        static inline constexpr std::array<int, 5> data { 1, 2, 3, 4 ,5 };

        static int operator[](const size_t index)
        {
            // static_assert(index > data.size());
            return data[index];
        }
    };

    void static_operator()
    {
        constexpr S s;
        static_assert (s (1, 2));

        constexpr bool result = S::operator()(1, 2);  // OK in C++23
        std::cout << std::boolalpha << result << std::endl;
    }


    void static_operator_lambda()
    {
        auto isEven = [](int i) static {return i % 2 == 0;};


        /** At the same time, you only declare a lambda static if it has no capture,
         *  if it has a capture then the compiler reminds you that you violated the rules: **/
        const int ratio = 0;
        // auto isDivisableBy = [ratio](int i) static {return i % ratio == 0;};
    }


    void static_operator__square_brackets()
    {
        int x = StaticArray{}[1];
        std::cout << x << std::endl;

        // static_assert(2 == StaticArray{}[1]);
    }

    //-----------------------------------------------------------------------------//

    int&& g(int&& x)
    {
        return x;
    }

    void test2()
    {
        [[maybe_unused]]
        const char arr[] = u8"hi";
    }

    /*
    struct Array
    {
        static constexpr std::array<int, 10> data{
                1, 2, 3, 4, 5, 6, 7, 8, 9, 10
        };

        int operator[](int idx, int = 5)
        {
            return data[idx];
        }
    };

    void Array_Operator_Default_Value()
    {
        Array array {};
        std::cout << array[5] << std::endl;
    }
    */

    struct A
    {
        int value { 123 };


        A& operator=(int v) {
            value = v;
            return *this;
        }

        explicit operator bool() {
            return true;
        }
    };

    void Wparentheses()
    {
        A a;
        if (a = 0) { // Warning Here

        }
    }


    struct T {};

    T fn()
    {
        T t;
        return std::move (T{});
    }
};

namespace Cpp23_Features::Multidimensional_Operator
{
    void simpleTest()
    {
        std::vector<std::vector<int>> numbers = {
                {1, 2, 3, 4, 5},
                {11, 12, 13, 14, 15}
        };

        // std::cout << numbers[0, 1] << '\n';

        std::cout << numbers[0][1] << '\n';
    }
}



void Cpp23_Features::TestAll()
{

    // static_operator();
    // static_operator_lambda();

    // static_operator__square_brackets();

    // test2();

    // Array_Operator_Default_Value();

    // Wparentheses();

    Multidimensional_Operator::simpleTest();
};
