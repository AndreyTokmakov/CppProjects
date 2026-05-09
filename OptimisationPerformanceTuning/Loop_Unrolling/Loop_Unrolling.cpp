/**============================================================================
Name        : Loop_Unrolling.cpp
Created on  : 09.05.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Loop_Unrolling.cpp
============================================================================**/

#include "Loop_Unrolling.hpp"

#include <array>
#include <iostream>
#include <utility>
#include <cstddef>

namespace
{
    template <typename F, std::size_t... Is>
    constexpr void unroll_impl(F&& f, std::index_sequence<Is...>) {
        // Fold expression: expands to f(0); f(1); f(2); ... at compile time.
        (f(std::integral_constant<std::size_t, Is>{}), ...);
    }

    template <std::size_t N, typename F>
    constexpr void unroll(F&& f) {
        unroll_impl(std::forward<F>(f), std::make_index_sequence<N>{});
    }
}

void loop_unrolling::TestAll()
{
    std::array<int, 4> data{10, 20, 30, 40};
    int sum = 0;

    unroll<4>([&](auto I) {
        // I is a compile-time constant! The index becomes a compile-time constant — the load from data is
        // still runtime (unless data itself is constexpr), but the offset is baked in.
        sum += data[I];
    });

    std::cout << sum << '\n'; // 100
}
