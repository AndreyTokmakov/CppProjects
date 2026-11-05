/**============================================================================
Name        : Constant_Wrapper.cpp
Created on  : 05.11.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Constant_Wrapper.cpp
============================================================================**/

#include "Constant_Wrapper.hpp"

#include <iostream>
#include <utility>


namespace constant_wrappers
{
    template<typename T>
    constexpr int get_element(T)
    {
        constexpr std::array<int, 3> arr = {10, 20, 30};
        return std::get<T::value>(arr);
    }

    template<double Factor>
    struct ScaledArray
    {
        std::array<double, 100> data{};

        double get(const int i) const
        {
            return data[i] * Factor;
        }
    };

    template<typename F>
    auto makeScaledArray(F) {
        return ScaledArray<F::value>{};
    }

    void demo()
    {
        constexpr auto a = std::cw<42>;      // constant_wrapper wrapping int 42
        constexpr auto b = std::cw<3.14>;    // wrapping double
        constexpr auto c = std::cw<'x'>;     // wrapping char

        // the verbose form — same types, same values
        constexpr std::constant_wrapper<42>   a2{};
        constexpr std::constant_wrapper<3.14> b2{};
        constexpr std::constant_wrapper<'x'>  c2{};

        constexpr int x = get_element(std::cw<1>); // x == 20
    }

    void testWithDouble()
    {
        // ScaledArray<2.5>
        const ScaledArray arr = makeScaledArray(std::cw<2.5>);
    }

    void arithmeticOperators()
    {
        constexpr std::constant_wrapper a = std::cw<10>;
        constexpr std::constant_wrapper b = std::cw<3>;

        constexpr auto sum  = a + b;   // constant_wrapper<13>
        constexpr auto diff  = a - b;   // constant_wrapper<7>
        constexpr auto prod= a * b;   // constant_wrapper<30>
        constexpr auto quot = a / b;   // constant_wrapper<3>
        constexpr auto mod   = a % b;   // constant_wrapper<1>

        static_assert(sum == std::cw<13>);
    }

    void comparisonOperators ()
    {
        // constant_wrapper<true>
        constexpr std::constant_wrapper cmp = (std::cw<5>) < (std::cw<10>);

        // implicit conversion to bool
        static_assert(cmp);
    }

    void computations()
    {
        // constant_wrapper<-42>
        constexpr auto neg = -std::cw<42>;

        // constant_wrapper<1>
        // constexpr auto inc = ++std::cw<0>;
    }
}


void constant_wrappers::TestAll()
{
    demo();
    testWithDouble();
    arithmeticOperators();
    comparisonOperators();

}
