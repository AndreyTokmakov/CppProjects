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

namespace loop_unrolling_1
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

    void demo()
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
}

namespace loop_unrolling_2
{
    template <typename F, std::size_t... Is>
    void repeat_unrolled_impl(F&& f, std::index_sequence<Is...>)
    {
        ((f(), void(Is)), ...);
    }

    template <std::size_t Iterations, typename F>
    void repeat_unrolled(F&& f)
    {
        repeat_unrolled_impl(std::forward<F>(f), std::make_index_sequence<Iterations>{});
    }

    volatile int i = 0;

    void demo()
    {
        // repeat_unrolled<6>({ ++i; });
    }
}

namespace loop_unrolling_partial
{
    template <typename F, std::size_t... Offsets>
    void call_with_offsets(F&& f, std::index_sequence<Offsets...>) {
        ((f(), void(Offsets)), ...);
    }

    template <std::size_t Iterations, std::size_t UnrollFactor, typename F>
    void partial_unroll(F&& f)
    {
        constexpr std::size_t full_blocks = Iterations / UnrollFactor;
        constexpr std::size_t remainder   = Iterations % UnrollFactor;

        for (std::size_t block = 0; block < full_blocks; ++block) {
            call_with_offsets(std::forward<F>(f), std::make_index_sequence<UnrollFactor>{});
        }

        for (std::size_t i = 0; i < remainder; ++i) {
            f();
        }
    }

    volatile int counter = 0;

    void demo()
    {
        // partial_unroll<101, 4>({ ++counter; });
    }
}

void loop_unrolling::TestAll()
{
    loop_unrolling_1::demo();
    // loop_unrolling_2::demo();
    // loop_unrolling_partial::demo();
}
