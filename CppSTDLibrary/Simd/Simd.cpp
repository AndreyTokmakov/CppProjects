/**============================================================================
Name        : Simd.cpp
Created on  : 17.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Simd.cpp
============================================================================**/

#include "Simd.h"

#include <iostream>
#include <string_view>
#include <vector>
#include <thread>

#include <experimental/simd>

namespace Simd
{
    void println(const std::string_view name, auto const& a)
    {
        std::cout << name << ": ";
        for (std::size_t i{}; i != std::size(a); ++i)
            std::cout << a[i] << ' ';
        std::cout << '\n';
    }

    template<class A>
    std::experimental::simd<int, A> my_abs(std::experimental::simd<int, A> x)
    {
        where(x < 0, x) = -x; // Set elements where x is negative to their absolute value
        return x;
    }
}

void Simd::TestAll()
{
    const std::experimental::native_simd<int> a = 1;
    println("a", a);

    const std::experimental::native_simd<int> b([](int i) { return i - 2; });
    println("b", b);

    const auto c = a + b;
    println("c", c);

    const auto d = my_abs(c);
    println("d", d);

    // a: 1 1 1 1
    // b: -2 -1 0 1
    // c: -1 0 1 2
    // d: 1 0 1 2
}