/**============================================================================
Name        : FalseSharingExperiments.h
Created on  : 24.05.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FalseSharingExperiments
============================================================================**/

#include "FalseSharingExperiments.h"
#include "../Utilities.h"

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <string_view>
#include <mutex>
#include <atomic>
#include <numeric>
#include <chrono>
#include <algorithm>

namespace FalseSharingExperiments::Base
{
    constexpr size_t ITER_COUNT { 10'000'000 };
    constexpr size_t THREADS_COUNT { 12 };
    constexpr size_t CACHE_LINE_SIZE = 64;

    void doWork(std::atomic<int>& var) {
        for (size_t i = 0; i < ITER_COUNT; ++i) {
            ++var;
        }
    }

    struct NoAlignedType {
        std::atomic<int> counter {0};
    };

    struct alignas(CACHE_LINE_SIZE) AlignedType {
        std::atomic<int> counter {0};
    };

    struct alignas(std::hardware_destructive_interference_size) AlignedTypeEx {
        std::atomic<int> counter {0};
    };

    template<typename T>
    void test()
    {
        std::vector<T> store (THREADS_COUNT);
        Utilities::ScopedTimer timer { "Base::test" };
        {
            std::vector<std::jthread> threads{};
            for (T &entry: store)
                threads.emplace_back(doWork, std::ref(entry.counter));
        }

        // for (size_t i = 0; i < THREADS_COUNT; ++i)
        //    std::cout << "    vars[" << i << "]: " << vars[i].var << std::endl;
    }

    void SingleThreadTest() {
        std::atomic<int> v {0};

        Utilities::ScopedTimer timer { "Base::SingleThreadTest" };
        for (size_t i = 0; i < THREADS_COUNT; ++i) {
            doWork(v);
        }
    }

    void ParallelThreads_SameVariable()
    {
        std::atomic<int> var {0};

        std::vector<std::thread> threads {};
        for (size_t i = 0; i < THREADS_COUNT; ++i)
            threads.emplace_back(doWork, std::ref(var));

        Utilities::ScopedTimer timer { "Base::ParallelThreads_SameVariable" };
        std::for_each(threads.begin(), threads.end(), [](std::thread&T ) { T.join(); });
    }

    void ParallelThreads_DifferentVariable()
    {
        test<NoAlignedType>();
    }

    void ParallelThreads_DifferentCacheLines()
    {
        test<AlignedType>();
    }

    void ParallelThreads_DifferentCacheLinesEx()
    {
        test<AlignedTypeEx>();
    }
};


namespace FalseSharingExperiments::DemoTwo
{

    constexpr size_t ITER_COUNT { 10'000'000 };
    constexpr size_t COUNT { 12 };

    struct Stats {
        int a { 0 };
        int b { 0 };
        int c { 0 };
        int d { 0 };
    };

    struct StatsAligned {
        alignas(std::hardware_destructive_interference_size) int a { 0 };
        alignas(std::hardware_destructive_interference_size) int b { 0 };
        alignas(std::hardware_destructive_interference_size) int c { 0 };
        alignas(std::hardware_destructive_interference_size) int d { 0 };
    };


    template<typename T>
    void task(T& var)
    {
        for (size_t i = 0; i < ITER_COUNT; ++i) {
            for (size_t n = 0; n < ITER_COUNT; ++n) {
                ++var;
            }
        }
    }

    template<typename T>
    void test()
    {
        std::vector<T> store (COUNT);
        std::vector<std::thread> threads {};
        for (T& entry: store) {
            threads.emplace_back(task<int>, std::ref(entry.a));
            threads.emplace_back(task<int>, std::ref(entry.b));
            threads.emplace_back(task<int>, std::ref(entry.c));
            threads.emplace_back(task<int>, std::ref(entry.d));
        }

        Utilities::ScopedTimer timer { "Base::test" };;
        std::for_each(threads.begin(), threads.end(), [](std::thread& job ) { job.join(); });
    }

    void Benchmark()
    {
        test<Stats>();
        test<StatsAligned>();
    }
};

namespace FalseSharingExperiments::DemoThree
{
    constexpr size_t ITER_COUNT { 10'000'000 };
    constexpr size_t THREADS_COUNT { 32 };

    struct Foo {
        std::atomic_int64_t a {0};
        std::atomic_int64_t b {0};
        std::atomic_int64_t c {0};
        std::atomic_int64_t d {0};
    };

    struct FooAligned {
        alignas(std::hardware_destructive_interference_size) std::atomic_int64_t a {0};
        alignas(std::hardware_destructive_interference_size) std::atomic_int64_t b {0};
        alignas(std::hardware_destructive_interference_size) std::atomic_int64_t c {0};
        alignas(std::hardware_destructive_interference_size) std::atomic_int64_t d {0};
    };

    template<typename T>
    void task(T& var) {
        for (size_t i = 0; i < ITER_COUNT; ++i) {
            var += 1;
        }
    }

    template<typename T, bool warmUp = false>
    void test()
    {
        auto benchmark = [] {
            T obj;
            std::vector<std::jthread> threads{};
            for (size_t threadNum = 0 ; threadNum < THREADS_COUNT; ++threadNum) {
                if (0 == threadNum % 4)
                    threads.emplace_back(task<decltype(T::a)>, std::ref(obj.a));
                else if (1 == threadNum % 4)
                    threads.emplace_back(task<decltype(T::b)>, std::ref(obj.b));
                else if (2 == threadNum % 4)
                    threads.emplace_back(task<decltype(T::c)>, std::ref(obj.c));
                else
                    threads.emplace_back(task<decltype(T::d)>, std::ref(obj.d));
            }
            // std::cout << obj.x << ' ' << obj.y << ' ' << obj.z << '\n';
        };

        if constexpr (not warmUp)
        {
            Utilities::ScopedTimer timer { "DemoThree::test" };;
            benchmark();
        } else
        {
            benchmark();
        }

    }

    void Benchmark()
    {
        test<Foo, true>();
        test<FooAligned, true>();

        test<Foo>();
        test<FooAligned>();

        /// Result: 3696887 microseconds
        /// Result: 1266461 microseconds
    }
}

void FalseSharingExperiments::TestAll()
{

    /*
    Base::SingleThreadTest();
    Base::ParallelThreads_SameVariable();
    Base::ParallelThreads_DifferentVariable();   // <--- FalseSharing Problem
    Base::ParallelThreads_DifferentCacheLines();
    Base::ParallelThreads_DifferentCacheLinesEx();
    */

    // DemoTwo::Benchmark();

    DemoThree::Benchmark();
};
