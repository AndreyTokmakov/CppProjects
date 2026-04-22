/**============================================================================
Name        : SourceLocation_RangePipelines.cpp
Created on  :
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Source location  src
============================================================================**/

#include <iostream>
#include <numeric>
#include <print>
#include <format>
#include <source_location>
#include <chrono>
#include <string_view>
#include <atomic>
#include <cstdio>
#include <ranges>
#include <functional>
#include <numeric>
#include <algorithm>

#include "SourceLocation.hpp"

namespace
{
    struct StepStats
    {
        std::source_location loc;
        std::string_view     label;

        // Using atomics so multiple threads draining the same range don't corrupt
        std::atomic<uint64_t> total_ns{0};
        std::atomic<uint64_t> call_count{0};

        void record(const std::chrono::nanoseconds elapsed) noexcept
        {
            total_ns.fetch_add(static_cast<uint64_t>(elapsed.count()), std::memory_order_relaxed);
            call_count.fetch_add(1, std::memory_order_relaxed);
        }

        void report() const
        {
            const uint64_t calls = call_count.load(std::memory_order_relaxed);
            const uint64_t ns = total_ns.load(std::memory_order_relaxed);
            std::println(stderr,
                "[{}:{}] {:30}  total={}µs  calls={}  avg={:.1f}µs",
                loc.file_name(), loc.line(),
                label.data(),
                static_cast<unsigned long long>(ns / 1000),
                static_cast<unsigned long long>(calls),
                (calls != 0U) ? static_cast<double>(ns) / calls / 1000.0 : 0.0
            );
        }
    };

    // Wraps a callable F, recording timing into a StepStats reference on each call.
    template<typename Func>
    struct TimedTransformFn
    {
        Func inner;
        StepStats& stats;  // non-owning reference — stats must outlive the view

        template<typename T>
        auto operator()(T&& val) const
        {
            const std::chrono::time_point start = std::chrono::steady_clock::now();
            // Forward to the real transform
            decltype(auto) result = std::invoke(inner, std::forward<T>(val));
            const std::chrono::time_point  end = std::chrono::steady_clock::now();
            stats.record(end - start);
            return result;
        }
    };

    template<typename Pred>
    struct TimedFilterPred
    {
        Pred inner;
        StepStats& stats;

        template<typename T>
        bool operator()(T&& val) const
        {
            const std::chrono::time_point start = std::chrono::steady_clock::now();
            // Forward to the real FILTER
            const bool result = std::invoke(inner, std::forward<T>(val));
            const std::chrono::time_point end = std::chrono::steady_clock::now();
            stats.record(end - start);
            return result;
        }
    };

    // Factory — source_location captured here at the call site
    template<typename F>
    auto timed_transform(F&& fn,
                         StepStats& stats,
                         const std::source_location loc = std::source_location::current())
    {
        stats.loc = loc;   // stamp the location into the stats bucket
        return std::views::transform(TimedTransformFn<std::decay_t<F>>{ std::forward<F>(fn), stats });
    }


    template<typename Pred>
    auto timed_filter(Pred&& pred,
                      StepStats& stats,
                      const std::source_location loc = std::source_location::current())
    {
        stats.loc = loc;
        return std::views::filter(TimedFilterPred<std::decay_t<Pred>>{ std::forward<Pred>(pred), stats });
    }


    void demo()
    {
        std::vector<int> data(100'000);
        std::ranges::iota(data, 0);

        // One StepStats per pipeline stage — lives as long as the pipeline runs
        StepStats s_filter1 { .label = "filter(is_valid)"         };
        StepStats s_xform1  { .label = "transform(normalize)"     };
        StepStats s_filter2 { .label = "filter(above_threshold)"  };
        StepStats s_xform2  { .label = "transform(expensive)"     };

        auto is_valid        = [](int x) { return x % 2 == 0; };
        auto normalize       = [](int x) { return x / 2; };
        auto above_threshold = [](int x) { return x > 1000; };
        auto expensive       = [](int x) {
            // Simulate work
            volatile int acc = 0;
            for (int i = 0; i < 50; ++i) acc += x * i;
            return acc;
        };

        // Pipeline — source_location is snapshotted on each of these lines
        auto pipeline = data
            | timed_filter   (is_valid,        s_filter1)   // line captured here
            | timed_transform(normalize,       s_xform1)    // and here
            | timed_filter   (above_threshold, s_filter2)   // and here
            | timed_transform(expensive,       s_xform2);   // and here

        // Drain it
        long long sum = 0;
        for (const auto v : pipeline) {
            sum += v;
        }

        std::cout << "sum = " << sum << "\n\n";

        // Report — each line tells you exactly where in source the bottleneck is
        s_filter1.report();
        s_xform1.report();
        s_filter2.report();
        s_xform2.report();
    }
}

void SourceLocation::SourceLocation_RangePipelines::TestAll()
{
    demo();

    // [SourceLocation_RangePipelines.cpp:140] filter(is_valid)                total=2185µs  calls=100000  avg=0.0µs
    // [SourceLocation_RangePipelines.cpp:141] transform(normalize)            total=2160µs  calls=98999  avg=0.0µs
    // [SourceLocation_RangePipelines.cpp:142] filter(above_threshold)         total=1099µs  calls=50000  avg=0.0µs
    // [SourceLocation_RangePipelines.cpp:143] transform(expensive)            total=1550µs  calls=48999  avg=0.0µs
}