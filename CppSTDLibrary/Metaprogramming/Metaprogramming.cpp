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

namespace ConditionalExplicit
{
    /**
     * Since C++20, the explicit specifier can be conditional. This allows for generic code that
     * permits implicit conversions in safe cases.
     *
     * In this example, the wrapper for integral types permits implicit conversion only when the
     * destination type can represent the full range of the source type.
    **/

    template<std::integral Src, std::integral Dst>
    constexpr bool is_safe_conversion =
            // Signed -> Signed where sizeof(Src) <= sizeof(Dst)
            (std::is_signed_v<Src> && std::is_signed_v<Dst> && sizeof(Src) <= sizeof(Dst)) ||
            // Unsigned -> Unsigned where sizeof(Src) <= sizeof(Dst)
            (std::is_unsigned_v<Src> && std::is_unsigned_v<Dst> && sizeof(Src) <= sizeof(Dst)) ||
            // Unsigned -> Signed where sizeof(Src) < sizeof(Dst)
            (std::is_unsigned_v<Src> && std::is_signed_v<Dst> && sizeof(Src) < sizeof(Dst));

    template <std::integral T>
    struct SafeIntegral {
        constexpr SafeIntegral() : value{} {}
        constexpr SafeIntegral(T src) : value(src) {}

        //  Either conversion operator, or conversion constructor
        /*
            template<std::integral Dst>
            constexpr explicit(!is_safe_conversion<T, Dst>)
            operator SafeIntegral<Dst>() const { return val_; }
        */
        template <std::integral Src>
        constexpr explicit(!is_safe_conversion<Src, T>)
        SafeIntegral(SafeIntegral<Src> src) : value (src.value) {

        }

        T value {};
    };

    void Test()
    {
        SafeIntegral a{2};  // SafeIntegral<int>
        SafeIntegral b{2u}; // SafeIntegral<unsigned>

        // a = b; // Will not compile, unsafe conversion
        a = SafeIntegral<int>{b}; // OK explicit conversion

        SafeIntegral<long> c = b; // OK assuming sizeof(long) > sizeof(unsigned)
        c = a; // OK
    }
}


void Metaprogramming::TestAll()
{
    // Conjunction::CheckTypesAreSame();
    // Negation::Test();

    ConditionalExplicit::Test();
}