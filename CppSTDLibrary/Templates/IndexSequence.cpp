/**============================================================================
Name        : IndexSequence.cpp
Created on  : 23.11.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : IndexSequence.cpp
============================================================================**/

#include "Templates.hpp"

#include <tuple>
#include <utility>
#include <iostream>
#include <print>

namespace unpack_tuple
{
    void print3(const int a, const double b, const char* c)
    {
        std::println("a : {}, b: {}, c: {}", a, b, c);
    }

    template <typename F, typename Tuple, std::size_t... I>
    void apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
    {
        std::forward<F>(f)(std::get<I>(t)...);
    }

    template <typename F, typename Tuple>
    void apply(F&& f, Tuple&& t)
    {
        apply_impl(std::forward<F>(f),
                   std::forward<Tuple>(t),
                   std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{});
    }

    void test()
    {
        std::tuple<int, double, const char*> tup{42, 3.14, "hello"};
        apply(print3, tup);
    }
}

namespace unpack_tuple_2
{
    template <typename Tuple, std::size_t... I>
    void print_impl(const Tuple& t, std::index_sequence<I...>)
    {
        ((std::cout << std::get<I>(t) << " "), ...);
    }

    template <typename... Args>
    void print(const std::tuple<Args...>& t)
    {
        print_impl(t, std::index_sequence_for<Args...>{});
    }

    void test()
    {
        print(std::make_tuple(1, 2.0, "hi"));
        // 1 2 hi
    }
}

namespace create_array_compile_time
{
    template <std::size_t... I>
    consteval auto make_squares(std::index_sequence<I...>) {
        return std::array { (I * I)... };
    }

    void test()
    {
        constexpr std::array arr = make_squares(std::make_index_sequence<5>{});
        for (const auto v : arr)
            std::cout << v << " ";
    }
}

/** Perfect-forward arguments to constructor (tuple → constructor) **/
namespace perfect_forward_tuple_constructor
{
    struct Foo
    {
        Foo(const int a, const double b, const std::string& s)
        {
            std::println("Foo(a : {}, b: {}, c: {})", a, b, s);
        }
    };

    template <typename T, typename Tuple, std::size_t... I>
    T construct_from_tuple_impl(Tuple&& tup, std::index_sequence<I...>)
    {
        return T(std::get<I>(std::forward<Tuple>(tup))...);
    }

    template <typename T, typename Tuple>
    T construct_from_tuple(Tuple&& tup)
    {
        constexpr std::size_t N = std::tuple_size_v<std::decay_t<Tuple>>;
        return construct_from_tuple_impl<T>(std::forward<Tuple>(tup),
                                            std::make_index_sequence<N>{});
    }

    void test()
    {
        std::tuple<int, double, std::string> args = std::make_tuple(10, 2.5, "wow");
        [[maybe_unused]] Foo f = construct_from_tuple<Foo>(args);

        // Foo(a : 10, b: 2.5, c: wow)
    }
}

namespace compile_time_values_sequence
{
    template <std::size_t... I>
    void show(std::index_sequence<I...>) {
        ((std::cout << I << " "), ...);
    }

    void test()
    {
        show(std::make_index_sequence<8>{});
        // 0 1 2 3 4 5 6 7
    }
}

namespace unroll_data_processing_loop
{
    constexpr size_t SlotCount = 8;

    // HFT-style slot processor
    struct SlotProcessor
    {
        alignas(64) std::array<int, SlotCount> slots {};   // cache-line aligned

        void process_one(const std::size_t i)
        {
            // Simulate some hot-path processing
            slots[i] += 1;
        }
    };

    template <typename F, std::size_t... I>
    void unrolled_impl(F&& f, std::index_sequence<I...>)
    {
        // Fold expression expands into: (f(0), f(1), f(2), ...)
        ( (f(I), void()), ... );
    }

    template <std::size_t N, typename F>
    void unrolled(F&& f)
    {
        unrolled_impl(std::forward<F>(f), std::make_index_sequence<N>{});
    }

    /**
    * The compiler emits 8 inlined calls process_one(0), process_one(1) …
    * No loop counters. No conditional jumps.
    *
    * ✔ No loop-carried dependencies
    *   Better instruction-level parallelism → CPUs execute multiple ops per cycle.
    *
    * ✔ Perfectly predictable loads/stores
    *   Caches love linear prefetch-friendly access.
    *
    * ✔ Works great on pre-allocated arrays
    *   Very common in HFT engines (ring-buffers, book levels, recycling pools, etc.)
    */
    void test()
    {
        SlotProcessor proc;

        // Fully unrolled at compile time: equivalent to 8 manual calls
        unrolled<SlotCount>([&](const std::size_t i) {
            proc.process_one(i);
        });

        for (const auto v : proc.slots)
            std::cout << v << " ";

        // Output:
        //  1 1 1 1 1 1 1 1
    }
}


namespace Templates::IndexSequence::Apply_Custom
{
    template <typename F, typename Tuple, std::size_t... I>
    decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
    {
        return f(std::get<I>(std::forward<Tuple>(t))...);
    }

    template <typename F, typename Tuple>
    decltype(auto) Apply(F&& f, Tuple&& t)
    {
        constexpr std::size_t N =std::tuple_size_v<std::remove_reference_t<Tuple>>;
        return apply_impl(std::forward<F>(f),
                          std::forward<Tuple>(t),
                            std::make_index_sequence<N>{});
    }

    void test()
    {
        auto sum = [](int a, int b, int c){ return a + b + c;};
        auto t = std::make_tuple(1, 2, 3);
        int result = Apply(sum, t);
        std::cout << result << " "; // -> 6
    }
}

void Templates::IndexSequence::TestAll()
{
    // unpack_tuple::test();
    // unpack_tuple_2::test();

    Apply_Custom::test(); // <- N

    // create_array_compile_time::test();
    // perfect_forward_tuple_constructor::test();
    // compile_time_values_sequence::test();
    // unroll_data_processing_loop::test();
}