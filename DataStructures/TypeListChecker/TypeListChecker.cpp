/**============================================================================
Name        : TypeListChecker.cpp
Created on  : 05.09.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TypeListChecker.cpp
============================================================================**/

#include "TypeListChecker.hpp"

#include <iostream>
#include <utility>


namespace
{
    template <typename ... Ts>
    struct TypeList1
    {
        template <typename T>
        static consteval bool includes()
        {
            return (std::is_same_v<T, Ts> || ...);
        }
    };

    template <typename ... Ts>
    struct TypeList2
    {
        template <typename T>
        static consteval bool includes()
        {
            return std::disjunction_v<std::is_same<T, Ts>...>;
        }
    };


    template <typename ... Types>
    struct Helper
    {
        using AllowedTypes1 = TypeList1<Types...>;
        // OR
        using AllowedTypes2 = TypeList2<Types...>;

        template<typename T>
        void foo(const T&)
        {
            static_assert(AllowedTypes1::template includes<T>(), "T must be one of Types");
            // OR
            static_assert(AllowedTypes2::template includes<T>(), "T must be one of Types");
        }
    };

}

void TypeListChecker::TestAll()
{
    Helper<uint32_t, std::string, float> helper;

    // helper.foo(1);    // < -- Type not allowed
    helper.foo(1U);
    // helper.foo("");;  // < -- Type not allowed
    helper.foo(std::string(""));

}