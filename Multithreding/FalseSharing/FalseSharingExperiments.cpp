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


    void doWork(std::atomic<int>& var) {
        for (size_t i = 0; i < ITER_COUNT; ++i) {
            ++var;
        }
    }

    void SingleThreadTest() {
        std::atomic<int> v {0};

        START_TIME_MEASURE;
        for (size_t i = 0; i < THREADS_COUNT; ++i) {
            doWork(v);
        }
        STOP_TIME_MEASURE;
    }

    void ParallelThreads_SameVariable() {
        std::atomic<int> var {0};

        std::vector<std::thread> threads {};
        for (size_t i = 0; i < THREADS_COUNT; ++i)
            threads.emplace_back(doWork, std::ref(var));

        START_TIME_MEASURE;
        std::for_each(threads.begin(), threads.end(), [](std::thread&T ) { T.join(); });
        STOP_TIME_MEASURE;
    }

    void ParallelThreads_DifferentVariable() {
        std::vector<std::atomic<int>> vars (THREADS_COUNT);
        for (size_t i = 0; i < THREADS_COUNT; ++i)
            vars[i] = std::atomic<int>{0}.load();

        std::vector<std::thread> threads {};
        for (size_t i = 0; i < THREADS_COUNT; ++i)
            threads.emplace_back(doWork, std::ref(vars[i]));

        START_TIME_MEASURE;
        std::for_each(threads.begin(), threads.end(), [](std::thread&T ) { T.join(); });
        STOP_TIME_MEASURE;

        /*
        for (size_t i = 0; i < THREADS_COUNT; ++i)
            std::cout << "    vars[" << i << "]: " << &vars[i] << std::endl;
        */
    }

    void ParallelThreads_DifferentCacheLines()
    {
        constexpr size_t CACHE_LINE_SIZE = 64;
        struct alignas(CACHE_LINE_SIZE) AlignedType {
            std::atomic<int> var {0};
        };

        std::vector<AlignedType> vars (THREADS_COUNT);
        for (size_t i = 0; i < THREADS_COUNT; ++i)
            vars[i].var = std::atomic<int>{0}.load();

        std::vector<std::thread> threads {};
        for (size_t i = 0; i < THREADS_COUNT; ++i)
            threads.emplace_back(doWork, std::ref(vars[i].var));

        START_TIME_MEASURE;
        std::for_each(threads.begin(), threads.end(), [](std::thread&T ) { T.join(); });
        STOP_TIME_MEASURE;

        /*
        for (size_t i = 0; i < THREADS_COUNT; ++i)
            std::cout << "    vars[" << i << "]: " << &vars[i] << std::endl;
        */
    }
};

void FalseSharingExperiments::TEST_ALL()
{
    Base::SingleThreadTest();
    Base::ParallelThreads_SameVariable();
    Base::ParallelThreads_DifferentVariable();
    Base::ParallelThreads_DifferentCacheLines();

};
