//============================================================================
// Name        : FalseSharingExperiments.h
// Created on  : 24.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : FalseSharingExperiments
//============================================================================

#include "FalseSharingExperiments.h"

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

#define START_TIME_MEASURE auto start = std::chrono::high_resolution_clock::now();
#define STOP_TIME_MEASURE  { auto end = std::chrono::high_resolution_clock::now(); \
                           auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
						   std::cout << "Result: " << duration << " microseconds" << std::endl;}


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
        std::vector<std::thread> threads {};
        for (T& entry: store)
            threads.emplace_back(doWork, std::ref(entry.counter));

        START_TIME_MEASURE;
        std::for_each(threads.begin(), threads.end(), [](std::thread& job ) { job.join(); });
        STOP_TIME_MEASURE;

        // for (size_t i = 0; i < THREADS_COUNT; ++i)
        //    std::cout << "    vars[" << i << "]: " << vars[i].var << std::endl;
    }

    void SingleThreadTest() {
        std::atomic<int> v {0};

        START_TIME_MEASURE;
        for (size_t i = 0; i < THREADS_COUNT; ++i) {
            doWork(v);
        }
        STOP_TIME_MEASURE;
    }

    void ParallelThreads_SameVariable()
    {
        std::atomic<int> var {0};

        std::vector<std::thread> threads {};
        for (size_t i = 0; i < THREADS_COUNT; ++i)
            threads.emplace_back(doWork, std::ref(var));

        START_TIME_MEASURE;
        std::for_each(threads.begin(), threads.end(), [](std::thread&T ) { T.join(); });
        STOP_TIME_MEASURE;
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
#define START_TIME_MEASURE auto start = std::chrono::high_resolution_clock::now();
#define STOP_TIME_MEASURE  { auto end = std::chrono::high_resolution_clock::now(); \
                           auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
						   std::cout << "Result: " << duration << " microseconds" << std::endl;}

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

        START_TIME_MEASURE;
        std::for_each(threads.begin(), threads.end(), [](std::thread& job ) { job.join(); });
        STOP_TIME_MEASURE;
    }

    void Benchmark()
    {
        test<Stats>();
        test<StatsAligned>();
    }
};

namespace FalseSharingExperiments::DemoThree
{
    struct Foo {
        int x {0};
        int y {0};
    };

    struct FooAligned {
        alignas(std::hardware_destructive_interference_size) int x{0};
        alignas(std::hardware_destructive_interference_size) int y{0};
    };


    template<typename T>
    void task(T& data, int id) {
        for (int i = 0; i < 1'000'000'000; ++i) {
            if (i % 2 == 0) {
                data.x += id;
            } else {
                data.y += id;
            }
        }
    }

    template<typename T>
    void test()
    {
        T x, y;

        START_TIME_MEASURE;
        std::jthread t1(task<T>, std::ref(x), 1),
                     t2(task<T>, std::ref(y), 2);
        STOP_TIME_MEASURE;
    }

    void Benchmark()
    {
        test<Foo>();
        test<FooAligned>();
    }
}

void FalseSharingExperiments::TEST_ALL()
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
