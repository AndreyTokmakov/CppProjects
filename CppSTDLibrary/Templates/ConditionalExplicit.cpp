/**============================================================================
Name        : ConditionalExplicit.cpp
Created on  : 18.09.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ConditionalExplicit.cpp
============================================================================**/

#include "ConditionalExplicit.h"

#include <concepts>

namespace ConditionalExplicit
{
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

    void CastTest()
    {
        SafeIntegral a{2};  // SafeIntegral<int>
        SafeIntegral b{2u}; // SafeIntegral<unsigned>

        // a = b; // Will not compile, unsafe conversion
        a = SafeIntegral<int>{b}; // OK explicit conversion

        SafeIntegral<long> c = b; // OK assuming sizeof(long) > sizeof(unsigned)
        c = a; // OK
    }
}

void ConditionalExplicit::TestAll() {
    CastTest();
}
