/**============================================================================
Name        : PerformanceExperiments.cpp
Created on  : 26.02.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Multithreading performance experiments
============================================================================**/

#include "PerformanceExperiments.h"

#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <syncstream>
#include <iomanip>

#include "PerfUtilities.hpp"
#include "ConditionVariable_vs_Atomic.h"
#include "Mutex_vs_AtomicLock.h"
#include "SpinLock.h"

namespace PerformanceExperiments::AtomicCounter_vs_Mutex
{
    class SpinLock
    {
        std::atomic_flag flag {false};

    public:
        void lock() {
            // First thead has flag == true. So it will exit while loop at the first iteration
            while (flag.test_and_set(std::memory_order_acquire)) {
            }
        }

        void unlock() {
            flag.clear(std::memory_order_release);
        }

        ~SpinLock() {
            flag.clear(std::memory_order_release);
        }
    };

    void RunBenchmark()
    {
        constexpr int threadsMax {16};
        constexpr size_t iterCount { 1'000'000 };

        {
            std::mutex mtx;
            uint64_t counter = 0;
            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    std::lock_guard<std::mutex> lock{mtx};
                    ++counter;
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"Mutex"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        {
            std::atomic<uint64_t> counter = 0;
            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    ++counter;
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"atomic"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        {
            std::atomic<uint64_t> counter = 0;
            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    counter.fetch_add(1);
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"atomic"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        {
            std::atomic<uint64_t> counter = 0;
            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    uint64_t x = counter.load(std::memory_order_relaxed);
                    while (!counter.compare_exchange_strong(x, x + 1)) {}
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"atomic"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        {
            std::atomic<uint64_t> counter = 0;
            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    uint64_t x = counter.load(std::memory_order_relaxed);
                    while (!counter.compare_exchange_weak(x, x + 1)) {}
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"atomic"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        {
            std::atomic<uint64_t> counter = 0;
            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    uint64_t x = counter.load(std::memory_order_relaxed);
                    while (!counter.compare_exchange_weak(x, x + 1,
                                                            std::memory_order::relaxed, std::memory_order::relaxed))
                    {
                    }
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"atomic"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        /// Result: 1'686'003 microseconds   mutext
        /// Result:   266'516 microseconds   ++counter
        /// Result:   265'476 microseconds   fetch_add
        /// Result:   712'774 microseconds   compare_exchange_strong
        /// Result:   717'862 microseconds   compare_exchange_weak
        /// Result:   717'862 microseconds   compare_exchange_weak && CAS -> std::memory_order::relaxed,
    }
}


namespace PerformanceExperiments::Atomic_vs_Volatile
{
    void SingleThread()
    {
        std::atomic<int32_t> atomicCounter = 0;
        volatile int32_t volatileCounter = 0;

        constexpr size_t iterCount { 100'000'000 };

        {
            const utilities::perf::ScopedTimer timer {"atomicCounter"};
            for (size_t t = 0; t < iterCount; ++t)
                atomicCounter.fetch_add(1, std::memory_order::relaxed);
        }

        {
            const utilities::perf::ScopedTimer timer {"volatileCounter"};
            for (size_t t = 0; t < iterCount; ++t)
                volatileCounter += 1;
        }
    }


    void MultipleThreads()
    {
        constexpr int32_t iterCount { 10'000'000 };
        constexpr int32_t threadsMax { 32 };

        {
            std::atomic<int32_t> atomicCounter = 0;
            auto writer = [&] {
                for (size_t idx = 0; idx < iterCount; ++idx) {
                    atomicCounter.fetch_add(1, std::memory_order_relaxed);
                }
            };
            auto reader = [&]
            {
                for (int32_t idx = 0, val = 0; idx < iterCount; ++idx) {
                    val = atomicCounter.load(std::memory_order_relaxed);
                    (void)val; // -Wunused-but-set-variable
                }
            };

            const utilities::perf::ScopedTimer timer {"atomicCounter"};
            std::vector<std::jthread> jobs;
            for (int32_t t = 0; t < threadsMax / 2; ++t) {
                jobs.emplace_back(writer);
                jobs.emplace_back(reader);
            }
        }

        {
            volatile int32_t volatileCounter = 0;
            auto writer = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    volatileCounter += 1;
                }
            };
            auto reader = [&] {
                int32_t val = 0;
                for (int32_t idx = 0; idx < iterCount; ++idx) {
                    val = volatileCounter;
                }
                (void)val; // -Wunused-but-set-variable
            };

            const utilities::perf::ScopedTimer timer {"volatileCounter"};
            std::vector<std::jthread> jobs;
            for (uint32_t t = 0; t < threadsMax / 2; ++t) {
                jobs.emplace_back(writer);
                jobs.emplace_back(reader);
            }
        }
    }

    void RunBenchmark()
    {
        // SingleThread();
        MultipleThreads();
    }
}

void PerformanceExperiments::TestAll()
{
    // ConditionVariable_vs_Atomic::benchmarks();
    // SpinLock::benchmarks();
    Mutex_vs_AtomicLock::benchmarks();
    // AtomicCounter_vs_Mutex::RunBenchmark();
    // Atomic_vs_Volatile::RunBenchmark();
};
