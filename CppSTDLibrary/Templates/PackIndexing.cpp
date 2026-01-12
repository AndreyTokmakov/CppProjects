/**============================================================================
Name        : PackIndexing.cpp
Created on  : 26.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PackIndexing.cpp
============================================================================**/

#include "Templates.hpp"

namespace PackIndexing
{

// INFO: https://compiler-explorer.com/


#if 0
    template <typename... T>
    constexpr auto first(T... values) -> T...[0] {
        return values...[0];
    }

    template <typename... Ts>
    constexpr auto last(Ts ... values) -> Ts ... [sizeof ... (values) - 1] {
        return values ... [sizeof ... (values) - 1];
    }

    template<size_t... Is, typename... Args>
    constexpr auto select(Args&&... args)
    {
        auto tup = std::forward_as_tuple(args...);
        return std::make_tuple(std::get<Is>(tup)...);
    }

    void PackIndexingDemo()
    {
        // first(); // ill-formed: invalid index 0 for pack 'T' of size 0
        static_assert(first(1, 2, 3, 4, 5) == 1);
        static_assert(last(1, 2, 3, 4, 5) == 5);
        static_assert(first(1, 2, 3, 4, 5) + last(1, 2, 3, 4, 5) == 6);

        static_assert(select<2,1,0>(1, 2, 3, 4, 5) == std::tuple{3,2,1});
    }
#endif
}


void Templates::PackIndexing::TestAll()
{
    // PackIndexing::PackIndexingDemo();
}
