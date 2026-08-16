/**============================================================================
Name        : SpinLock.cpp
Created on  : 07.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

#include "SpinLock.h"
#include "PerfUtilities.hpp"
#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << utilities::datetime::getCurrentTime() << " "

namespace
{
    struct SpinLock_AtomicFlag
    {
        std::atomic_flag flag {false};

        void lock()
        {
            // First thead has flag == true. So it will exit while loop at the first iteration
            while (flag.test_and_set(std::memory_order_acquire)) {
            }
        }

        void unlock()
        {
            flag.clear(std::memory_order_release);
        }

        ~SpinLock_AtomicFlag()
        {
            flag.clear(std::memory_order_release);
        }
    };


    struct SpinLock2
    {
        std::atomic<bool> isLocked;

        void lock()
        {
            bool expected = false;
            while(!isLocked.compare_exchange_weak(expected, true, std::memory_order_acquire)) {
                expected = false;
            }
        }

        void unlock() { isLocked.store(false, std::memory_order_release); }
        // ~SpinLock2() { isLocked.store(false, std::memory_order_release); }
    };

    struct SpinLock2_Int
    {
        alignas(std::hardware_destructive_interference_size) std::atomic<uint32_t> isLocked { 0 };

        void lock()
        {
            uint32_t expected = 0;
            while(!isLocked.compare_exchange_weak(expected, 1, std::memory_order_acquire)) {
                expected = 0;
            }
        }

        void unlock() { isLocked.store(0, std::memory_order_release); }
        // ~SpinLock2_Int() { isLocked.store(false, std::memory_order_release); }
    };

    struct SpinLock2_Int_Timer
    {
        alignas(std::hardware_destructive_interference_size) std::atomic<uint32_t> isLocked { 0 };

        void lock()
        {
            static const timespec ns {0, 1};
            uint32_t expected = 0;
            for (int i = 0; !isLocked.compare_exchange_weak(expected, 1, std::memory_order_acquire); ++i) {
                expected = 0;
                if (2 == i) /// to tune thread scheduler
                {
                    i = 0;
                    nanosleep(&ns, nullptr);
                }
            }
        }

        void unlock() { isLocked.store(0, std::memory_order_release); }
        // ~SpinLock2_Int() { isLocked.store(false, std::memory_order_release); }
    };

    struct alignas(std::hardware_destructive_interference_size) SpinLock2_Timer_Ex
    {
        std::atomic<uint32_t> isLocked { 0 };
        static constexpr timespec ns {0, 1};

        void lock()
        {
            uint32_t expected = 0;
            for (int i = 0; !isLocked.compare_exchange_weak(expected, 1,
                                                            std::memory_order_acq_rel,
                                                            std::memory_order_relaxed); ++i) {
                expected = 0;
                if (2 == i) /// to tune thread scheduler
                {
                    i = 0;
                    nanosleep(&ns, nullptr);
                    //std::this_thread::sleep_for(std::chrono::nanoseconds(1));
                }
            }
        }

        void unlock() { isLocked.store(0, std::memory_order_release); }
        // ~SpinLock2_Int() { isLocked.store(false, std::memory_order_release); }
    };

    struct SpinLock3
    {
        std::atomic<unsigned int> flag {0};

        void lock()
        {
            static const timespec ns {0, 1};
            for (int i = 0; flag.load(std::memory_order_relaxed) || flag.exchange(1, std::memory_order_acquire); ++i)
            {
                if (8 == i) /// to tune thread scheduler
                {
                    i = 0;
                    nanosleep(&ns, nullptr);
                }
            }
        }

        void unlock() { flag.store(0, std::memory_order_release); }
    };


    struct SpinLock4
    {
        /** Show worst performance with std::atomic<bool> . . . .**/
        // std::atomic<bool> flag {0};
        alignas(std::hardware_destructive_interference_size) std::atomic<uint32_t> flag {0 };

        void lock()
        {
            static const timespec ns {0, 1};
            for (int i = 0; flag.load(std::memory_order_relaxed) || flag.exchange(1, std::memory_order_acquire); ++i)
            {
                if (4 == i) /// to tune task scheduler
                {
                    i = 0;
                    nanosleep(&ns, nullptr);
                }
            }
        }

        void unlock() { flag.store(0, std::memory_order_release); }
    };

    class spin_mutex_M2
    {
    private:
        using flag = std::atomic<unsigned long long>;
    private:
        flag _f;
    public:
        spin_mutex_M2(): _f(0) {}
        ~spin_mutex_M2() = default;
        spin_mutex_M2(const spin_mutex_M2&) = delete;
        spin_mutex_M2(spin_mutex_M2&&) = delete;
        spin_mutex_M2& operator= (const spin_mutex_M2&) = delete;
        spin_mutex_M2& operator= (spin_mutex_M2&&) = delete;

        void lock()
        {
            unsigned long long expected = 0;
            do
            {
                expected = 0;
            }
            while (!_f.compare_exchange_weak(expected, 1, std::memory_order_relaxed, std::memory_order_relaxed));
        }

        bool try_lock()
        {
            unsigned long long expected = 0;
            return _f.compare_exchange_weak(expected, 1, std::memory_order_relaxed, std::memory_order_relaxed);
        }

        void unlock()
        {
            _f.store(0, std::memory_order_relaxed);
        }
    };

    void runSimulation()
    {
        constexpr int threadsMax {16};
        constexpr size_t iterCount { 1'000'000 };
#if 1
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
            SpinLock_AtomicFlag spinLock;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            {
                const utilities::perf::ScopedTimer timer{"SpinLock_AtomicFlag"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        {
            SpinLock2 spinLock;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"SpinLock2"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        {
            SpinLock2_Int spinLock;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"SpinLock2_Int"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }
#endif

        {
            SpinLock2_Int_Timer spinLock;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"SpinLock2_Int_Timer"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        {
            SpinLock2_Timer_Ex spinLock;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"SpinLock2_Timer_Ex"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }
        {
            SpinLock3 spinLock;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"SpinLock3"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        {
            SpinLock4 spinLock;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"SpinLock4"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        {
            spin_mutex_M2 spinLock;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount / 10; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"spin_mutex_M2"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        /// Mutex                :  0.636317 seconds.
        /// SpinLock_AtomicFlag  :  3.79414 seconds.
        /// SpinLock2            :  3.91013 seconds.
        /// SpinLock2_Int        :  2.45358 seconds.
        /// SpinLock2_Int_Timer  :  0.294999 seconds.
        /// SpinLock3            :  0.121038 seconds.
        /// SpinLock4            :  0.0963803 seconds.
    }
}



void SpinLock::benchmarks()
{
    runSimulation();
}