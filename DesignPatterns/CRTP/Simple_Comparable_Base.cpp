/**============================================================================
Name        : Simple_Comparable_Base.cpp
Created on  : 31.05.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Simple_Comparable_Base
============================================================================**/

#include "CRTP.h"
#include <iostream>

namespace CRTP::Simple_Comparable_Base
{

    template<typename Derived>
    struct Equals
    {
        friend constexpr bool operator==(const Derived& a, const Derived& b)
        {
            return (!(a < b) && !(b < a));
        }
    };

    template<typename Derived>
    struct NotEquals
    {
        friend constexpr bool operator!=(const Derived& a, const Derived& b)
        {
            return (a < b) || (b < a);
        }
    };

    struct Object: public Equals<Object>, public NotEquals<Object>
    {
        int value { 0 };

        explicit constexpr Object(int v): value {v} {
        }

        constexpr bool operator<(const Object& obj) const
        {
            return value < obj.value;
        }
    };
}

void CRTP::Simple_Comparable_Base::TestAll()
{
    constexpr Object objOne {10}, objTwo {20}, objThree {10};

    static_assert(objOne == objThree);
    static_assert(!(objOne == objTwo));
    static_assert(objOne != objTwo);
}