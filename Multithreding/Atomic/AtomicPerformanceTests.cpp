/**============================================================================
Name        : AtomicPerformanceTests.cpp
Created on  : 07.06.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AtomicPerformanceTests.cpp
============================================================================**/

#include "AtomicPerformanceTests.h"

#include <iostream>
#include <atomic>
#include <thread>
#include <iomanip>
#include <vector>


namespace Utils
{

    struct ScopedTimer
    {
        const std::string_view benchmarkName;
        const std::chrono::high_resolution_clock::time_point start {
                std::chrono::high_resolution_clock::now()
        };

        explicit ScopedTimer(std::string_view info) :
                benchmarkName {info} {
        }

        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer(ScopedTimer&&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;
        ScopedTimer& operator=(ScopedTimer&&) = delete;

        ~ScopedTimer()
        {
            const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

            std::cout << std::left << std::setw(19) << benchmarkName << ":  ";
            std::cout << time_span.count() << " seconds.\n";
        }
    };
}


namespace AtomicPerformanceTests
{
    constexpr int threadsMax {16};
    constexpr size_t iterCount { 10'000'000 };

    void AtomicBool_vs_AtomicFlag()
    {
        {
            std::atomic_bool flag = true;
            bool value = true;

            auto task = [&] {
                for (size_t idx = 0; idx < iterCount; ++idx)
                {
                    value = !flag.load(std::memory_order_relaxed);
                    flag.store(value, std::memory_order_relaxed);
                }
            };

            Utils::ScopedTimer timer{"Bool 1"};
            std::vector<std::jthread> jobs;
            for (int t = 0; t < threadsMax; ++t)
                jobs.emplace_back(task);
        }
        {
            std::atomic<bool> flag = true;
            bool value = true;

            auto task = [&] {
                for (size_t idx = 0; idx < iterCount; ++idx)
                {
                    value = !flag.load(std::memory_order_relaxed);
                    flag.store(value, std::memory_order_relaxed);
                }
            };

            Utils::ScopedTimer timer{"Bool 2"};
            std::vector<std::jthread> jobs;
            for (int t = 0; t < threadsMax; ++t)
                jobs.emplace_back(task);
        }
        {
            std::atomic<int32_t> flag = true;
            bool value = true;

            auto task = [&] {
                for (size_t idx = 0; idx < iterCount; ++idx)
                {
                    value = !flag.load(std::memory_order_relaxed);
                    flag.store(value, std::memory_order_relaxed);
                }
            };

            Utils::ScopedTimer timer{"Bool (Uint32)"};
            std::vector<std::jthread> jobs;
            for (int t = 0; t < threadsMax; ++t)
                jobs.emplace_back(task);
        }
        {
            std::atomic_flag flag = true;

            auto task = [&] {
                for (size_t idx = 0; idx < iterCount; ++idx)
                {
#if 1
                    flag.test_and_set();
#else
                    if (flag.test(std::memory_order_relaxed))
                    {
                        flag.clear(std::memory_order_relaxed);
                    } else {
                        flag.test_and_set(std::memory_order_relaxed);
                    }
#endif
                }
            };

            Utils::ScopedTimer timer{"Flag"};
            std::vector<std::jthread> jobs;
            for (int t = 0; t < threadsMax; ++t)
                jobs.emplace_back(task);
        }

        // Bool 1             :  0.0884118 seconds.
        // Bool 2             :  0.095649 seconds.
        // Bool (Uint32)      :  0.0693623 seconds.
        // Flag               :  2.65361 seconds.
    }
}

void AtomicPerformanceTests::TestAll()
{
    AtomicPerformanceTests::AtomicBool_vs_AtomicFlag();
}