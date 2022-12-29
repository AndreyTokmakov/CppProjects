/**============================================================================
Name        : Metaprogramming.cpp
Created on  : 25.12.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Metaprogramming C++ library src
============================================================================**/

#include "Metaprogramming.h"

#include <iostream>
#include <type_traits>

namespace Metaprogramming::Conjunction
{
    template<typename T, typename... Ts>
    std::enable_if_t<std::conjunction_v<std::is_same<T, Ts>...>>
    check_types_are_same(T, Ts...)
    {
        std::cout << "all types in pack are T\n";
    }

    template<typename T, typename... Ts>
    std::enable_if_t<!std::conjunction_v<std::is_same<T, Ts>...>>
    check_types_are_same(T, Ts...)
    {
        std::cout << "not all types in pack are T\n";
    }

    void CheckTypesAreSame()
    {
        check_types_are_same(1, 2, 3);
        check_types_are_same(1, 2, "hello!");
    }
}

namespace Metaprogramming::Negation
{
    static_assert(std::is_same<std::bool_constant<false>, std::negation<std::bool_constant<true>>::type>::value, "");
    static_assert(std::is_same<std::bool_constant<true>, std::negation<std::bool_constant<false>>::type>::value, "");

    void Test()
    {
        std::cout << std::boolalpha;
        std::cout << std::negation<std::bool_constant<true>>::value << '\n';
        std::cout << std::negation<std::bool_constant<false>>::value << '\n';
    }
}

void Metaprogramming::TestAll()
{
    // Conjunction::CheckTypesAreSame();
    Negation::Test();
}