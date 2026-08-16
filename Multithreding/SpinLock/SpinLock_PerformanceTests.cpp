/**============================================================================
Name        : SpinLock_PerformanceTests.cpp
Created on  : 26.06.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SpinLock_PerformanceTests.cpp
============================================================================**/

#include <atomic>
#include <thread>

#include "SpinLock_PerformanceTests.h"
#include "PerfUtilities.hpp"

namespace SpinLock_PerformanceTests::Impl
{
    struct SpinLock
    {
        std::atomic_flag flag {false};

        void lock() {
            // First thead has flag == true. So it will exit while loop at the first iteration
            while (flag.test_and_set(std::memory_order_acquire)) {
            }
        }

        void unlock() {
            flag.clear(std::memory_order_release);
        }

       ~SpinLock() {
           unlock();
       }
    };

    struct SpinLock_WithYield
    {
        std::atomic_flag flag {false};

        void lock() {
            while (flag.test_and_set(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
        }

        void unlock() {
            flag.clear(std::memory_order_release);
        }

        ~SpinLock_WithYield() {
            unlock();
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

        void unlock() {
            isLocked.store(false, std::memory_order_release);
        }

        ~SpinLock2() {
            unlock();
        }
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

        void unlock() {
            isLocked.store(0, std::memory_order_release);
        }

        ~SpinLock2_Int() {
            isLocked.store(false, std::memory_order_release);
        }
    };

    struct SpinLock2_Int_Timer
    {
        alignas(std::hardware_destructive_interference_size) std::atomic<uint32_t> isLocked { 0 };

        void lock()
        {
            static constexpr timespec ns {0, 1};
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

        void unlock() {
            isLocked.store(0, std::memory_order_release);
        }

        ~SpinLock2_Int_Timer() {
            isLocked.store(false, std::memory_order_release);
        }
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
                if (4 == i) /// to tune thread scheduler
                {
                    i = 0;
                    nanosleep(&ns, nullptr);
                    //std::this_thread::sleep_for(std::chrono::nanoseconds(1));
                }
            }
        }

        void unlock() {
            isLocked.store(0, std::memory_order_release);
        }

        ~SpinLock2_Timer_Ex() {
            unlock();
        }
    };

    struct SpinLock3
    {
        std::atomic<unsigned int> flag {0};

        void lock()
        {
            static const timespec ns {0, 1};
            for (int i = 0; flag.load(std::memory_order_relaxed) || flag.exchange(1, std::memory_order_acquire); ++i)
            {
                if (8 == i) /** to tune thread scheduler **/ {
                    i = 0;
                    nanosleep(&ns, nullptr);
                }
            }
        }

        void unlock() {
            flag.store(0, std::memory_order_release);
        }

        ~SpinLock3() {
            unlock();
        }
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
                if (4 == i) /** to tune task scheduler **/ {
                    i = 0;
                    nanosleep(&ns, nullptr);
                }
            }
        }

        void unlock() {
            flag.store(0, std::memory_order_release);
        }

        ~SpinLock4() {
            unlock();
        }
    };


    struct SpinLock4_1
    {
        alignas(std::hardware_destructive_interference_size) std::atomic<uint32_t> flag {0 };
        alignas(std::hardware_destructive_interference_size) timespec ns {.tv_sec=0, .tv_nsec=1 };

        void lock()
        {
            for (uint8_t timeout = 0;
                 flag.load(std::memory_order_relaxed) || flag.exchange(1, std::memory_order_acquire);
                 ns.tv_nsec = static_cast<int>(++timeout))
            {
                nanosleep(&ns, nullptr);
            }
        }

        inline void unlock() noexcept {
            flag.store(0, std::memory_order_release);
        }

        ~SpinLock4_1() {
            unlock();
        }
    };

    class SpinLockM2
    {
        std::atomic<uint32_t> flag {0};

    public:
        SpinLockM2() = default;
        SpinLockM2(const SpinLockM2&) = delete;
        SpinLockM2(SpinLockM2&&) noexcept = delete;
        SpinLockM2& operator= (const SpinLockM2&) = delete;
        SpinLockM2& operator= (SpinLockM2&&) = delete;

        void lock()
        {
            uint32_t expected = 0;
            do {
                expected = 0;
            } while (!flag.compare_exchange_weak(expected, 1, std::memory_order_relaxed, std::memory_order_relaxed));
        }

        bool try_lock()
        {
            uint32_t expected = 0;
            return flag.compare_exchange_weak(expected, 1, std::memory_order_relaxed, std::memory_order_relaxed);
        }

        void unlock() {
            flag.store(0, std::memory_order_relaxed);
        }

        ~SpinLockM2() {
            unlock();
        }
    };
}

namespace SpinLock_PerformanceTests::Tests
{
    using namespace Impl;


    void RunBenchmark()
    {
        constexpr int threadsMax { 8 };
        constexpr size_t iterCount { 5'000'000 };
#if 0
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
                ScopedTimer timer {"Mutex"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        {
            SpinLock spinLock;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            {
                ScopedTimer timer{"SpinLock"};
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
                ScopedTimer timer {"SpinLock2"};
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
                ScopedTimer timer {"SpinLock2_Int"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }
#endif


        auto benchmark = [&]<typename LockType> (std::string_view name) {
            LockType spinLock;
            uint64_t counter = 0;

            auto task = [&counter, &spinLock] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            const utilities::perf::ScopedTimer timer { name };
            std::vector<std::jthread> jobs;
            for (int t = 0; t < threadsMax; ++t)
                jobs.emplace_back(task);
        };

        benchmark.template operator()<SpinLock_WithYield>("SpinLock_WithYield");
        benchmark.template operator()<SpinLock2_Int_Timer>("SpinLock2_Int_Timer");
        benchmark.template operator()<SpinLock2_Timer_Ex>("SpinLock2_Timer_Ex");
        benchmark.template operator()<SpinLock3>("SpinLock3");
        benchmark.template operator()<SpinLock4>("SpinLock4");
        benchmark.template operator()<SpinLock4_1>("SpinLock4_1");
        benchmark.template operator()<SpinLockM2>("SpinLockM2");

        // SpinLock_WithYield  :  1.42483 seconds.
        // SpinLock2_Int_Timer :  0.498962 seconds.
        // SpinLock2_Timer_Ex  :  0.600027 seconds.
        // SpinLock3           :  0.675397 seconds.
        // SpinLock4           :  0.558371 seconds.
        // SpinLock4_1         :  0.52006 seconds.
        // spin_mutex_M2       :  8.13006 seconds.
    }

    void RunBenchmark_Fastest()
    {
        constexpr int threadsMax { 16 };
        constexpr size_t iterCount { 20'000'000 };

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
            SpinLock4_1 spinLock;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"SpinLock4_1"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        // SpinLock4     :  1.84052 seconds.
        // SpinLock4_1   :  1.86409 seconds.
    }

    void RunBenchmark_Basic_with_Yield()
    {
        constexpr int threadsMax { 16 };
        constexpr size_t iterCount { 1'000'000 };

        {
            SpinLock spinLock;
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
            SpinLock_WithYield spinLock;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            {
                const utilities::perf::ScopedTimer timer {"SpinLock4_1"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        // SpinLock4     :  3.89815 seconds.
        // SpinLock4_1   :  0.301509 seconds.
    }
}


void SpinLock_PerformanceTests::TestAll()
{
    Tests::RunBenchmark();
    // Tests::RunBenchmark_Fastest();
    // Tests::RunBenchmark_Basic_with_Yield();
}