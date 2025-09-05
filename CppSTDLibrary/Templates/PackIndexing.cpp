/**============================================================================
Name        : PackIndexing.cpp
Created on  : 26.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PackIndexing.cpp
============================================================================**/

#include "PackIndexing.hpp"

namespace PackIndexing
{
#if 0
    template <typename... T>
    constexpr auto first(T... values) -> T...[0] {
        return values...[0];
    }

    template <typename... T>
    constexpr auto last(T... values) -> T...[sizeof...(values)-1] {
        return values...[sizeof...(values)-1];
    }

    void PackIndexingDemo()
    {
        // first(); // ill-formed: invalid index 0 for pack 'T' of size 0
        static_assert(first(1, 2, 3, 4, 5) == 1);
        static_assert(last(1, 2, 3, 4, 5) == 5);
        static_assert(first(1, 2, 3, 4, 5) + last(1, 2, 3, 4, 5) == 6);
    }
#endif
}


void PackIndexing::TestAll()
{
    // PackIndexing::PackIndexingDemo();
}
