/**============================================================================
Name        : Assume.cpp
Created on  : 26.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Assume.cpp
============================================================================**/

#include "Assume.h"

#include <cstring>
#include <vector>
#include <cmath>
#include <cassert>

namespace Assume
{
    void callWithNotNull(int* p, size_t n)
    {
        assert(p);
        assert(n > 0);

        [[assume(p != nullptr)]];
        [[assume(n > 0)]];
    }

    void sum(const int* data, size_t n)
    {
        [[assume(n % 8 == 0)]];
        for (size_t i = 0; i < n; i += 8)
        {
            // компилятор может безопасно развернуть цикл
            __builtin_prefetch(data + i + 64);
            // ...
        }
    }

    int fast_read(const int* a, size_t i, size_t n)
    {
        [[assume(i < n)]];
        return a[i];
    }


    enum class Msg
    {
        A,
        B,
        C
    };

    void handle(Msg m)
    {
        [[assume(m == Msg::A || m == Msg::B || m == Msg::C)]];
        switch (m) {
            case Msg::A: /*...*/ break;
            case Msg::B: /*...*/ break;
            case Msg::C: /*...*/ break;
        }
    }


    void tests()
    {
        constexpr int array[]  {1,2,3,4,6};
        fast_read(array, 2, 5);
    }

    void calculate(int x)
    {
        [[assume(x > 0)]];  // Promise: x is always positive here
                            // Compiler now optimizes all downstream code knowing x > 0
    }

    int divide_by_positive(const int value, const int divisor)
    {
        [[assume(divisor > 0)]];
        return value / divisor;
    }

    void loop_optimization(std::vector<int> v)
    {
        [[assume(v.size() % 4 == 0)]];
        // Promise: size is always a multiple of 4

        for (size_t i = 0; i < v.size(); i += 4) {
            // Removes the barrier of generating a scalar remainder loop, enabling SIMD auto-vectorization.
            // Whether the compiler fully vectorizes still depends on target architecture, surrounding code, and
            // optimization level.
            v[i]   *= 2;
            v[i+1] *= 2;
            v[i+2] *= 2;
            v[i+3] *= 2;
        }
    }

    void fast_copy(char* dest,
                   char const* src,
                   const size_t n)
    {
        [[assume(reinterpret_cast<__uint64_t>(dest) % 64 == 0)]];
        [[assume(reinterpret_cast<__uint64_t>(src)  % 64 == 0)]];

        // May allow the compiler to generate aligned
        // load/store instructions instead of unaligned fallbacks
        std::memcpy(dest, src, n);
    }
}

void Assume::TestAll()
{
    calculate(123);

    loop_optimization({});

    divide_by_positive(10, 2);
};