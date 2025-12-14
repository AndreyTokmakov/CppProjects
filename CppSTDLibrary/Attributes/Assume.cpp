/**============================================================================
Name        : Assume.cpp
Created on  : 26.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Assume.cpp
============================================================================**/

#include "Assume.h"

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
}

void Assume::TestAll()
{

};