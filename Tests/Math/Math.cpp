/**============================================================================
Name        : Math.cpp
Created on  : 16.07.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Math test
============================================================================**/

#include "Math.h"

namespace Math
{
    int calc(int val)
    {
        int count = 0;
        while (val > 1) {
            val /= 2;
            ++count;
        }
        return count;
    }

    void Log2Test()
    {
        calc(59218);
    }
}

void Math::TestAll()
{
    Log2Test();
}

