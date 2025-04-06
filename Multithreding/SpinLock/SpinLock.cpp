/**============================================================================
Name        : SpinLock.cpp
Created on  : 22.10.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SpinLock.cpp
============================================================================**/

#include "SpinLock.h"

#include <iostream>
#include <algorithm>
#include <string_view>
#include <vector>
#include <random>
#include <chrono>
#include <format>

#include <thread>
#include <future>
#include <mutex>
#include <syncstream>

namespace
{
    std::string timeString()
    {
        std::string buffer;
        buffer.reserve(32);
        std::format_to(std::back_inserter(buffer), "{:%Y-%m-%d %H:%M:%OS}", std::chrono::system_clock::now());
        buffer.shrink_to_fit();
        return buffer;
    }

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


namespace SpinLock::Impl
{
    class SpinLock_AtomicFlag
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

        ~SpinLock_AtomicFlag() {
            flag.clear(std::memory_order_release);
        }
    };

    /*
    class SpinLock_AtomicFlag_Wait
    {
        std::atomic_flag flag {false};

    public:
        void lock() {
            flag.wait(false);
        }

        void unlock() {
            flag.clear(std::memory_order_release);
        }

        ~SpinLock_AtomicFlag_Wait() {
            flag.clear(std::memory_order_release);
        }
    };*/

    class SpinLock_Bool
    {
        std::atomic<bool> isLocked;

    public:
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


        ~SpinLock_Bool() {
            isLocked.store(false, std::memory_order_release);
        }
    };

    void SpinLock_Tests()
    {
        constexpr int threadsMax { 8 };
        constexpr uint64_t iterCount { 10'000'000 };

        auto validate = [] (size_t actual, size_t expected){
            std::cout << actual << " = " << expected << std::endl;
            if (actual == expected) {
                std::cout << "OK\n";
            } else {
                std::cout << "Wrong: " << actual << " != " << expected << std::endl;
            }
        };

        auto test = [] (auto task){
            std::vector<std::jthread> jobs;
            for (int t = 0; t < threadsMax; ++t)
                jobs.emplace_back(task);
        };

        {
            uint64_t counter = 0;
            auto task = [&counter] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    counter++;
                }
            };

            test(task);
            validate(counter, threadsMax * iterCount);
        }

        {
            std::mutex mtx;
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    std::lock_guard<std::mutex> lock{mtx};
                    ++counter;
                }
            };

            test(task);
            validate(counter, threadsMax * iterCount);
        }

        {
            SpinLock_AtomicFlag spinLock{};
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            test(task);
            validate(counter, threadsMax * iterCount);
        }

        {
            SpinLock_Bool spinLock{};
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            test(task);
            validate(counter, threadsMax * iterCount);
        }

        /*
        {
            SpinLock_AtomicFlag_Wait spinLock {};
            uint64_t counter = 0;

            auto task = [&] {
                for (size_t idx  = 0; idx < iterCount; ++idx) {
                    spinLock.lock();
                    ++counter;
                    spinLock.unlock();
                }
            };

            test(task);
            validate(counter, threadsMax * iterCount);
        }*/
    }
}



// https://leetcode.com/problems/print-in-order/submissions/
/**

public class Foo {
    public void first() { print("first"); }
    public void second() { print("second"); }
    public void third() { print("third"); }
}

The same instance of Foo will be passed to three different threads.
Thread A will call first(), thread B will call second(), and thread C will call third().
Design a mechanism and modify the program to ensure that second() is executed
after first(), and third() is executed after second().
 */
namespace SpinLock::SwitchingThreads_SpinLock
{
    struct Worker
    {
        std::atomic<uint32_t> turnSwitch;

        explicit Worker(uint32_t turn = 1): turnSwitch { turn } {
        }

        void waitForOrder_SpinLock(uint32_t order) {
            while (turnSwitch.load(std::memory_order_acquire) != order) {
            }
        }

        void first()
        {
            std::osyncstream {std::cout} << timeString() <<  ": First" << std::endl;
            turnSwitch.store(2, std::memory_order_release); // turnSwitch.store(2) or turnSwitch = 2
        }

        void second()
        {
            waitForOrder_SpinLock(2);
            std::osyncstream {std::cout} << timeString() <<  ": Second" << std::endl;
            turnSwitch.store(3, std::memory_order_release);
        }

        void third()
        {
            waitForOrder_SpinLock(3);
            std::osyncstream {std::cout} << timeString() <<  ": Third" << std::endl;
        }
    };

    int32_t getRandomInt(int32_t from = 0, int32_t until = 100)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distribution(from, until);
        return distribution(gen);
    }

    void singleThreadTest()
    {
        Worker worker;

        worker.first();
        worker.second();
        worker.third();
    }

    void multiThreadTest()
    {
        Worker worker;

        std::vector<std::future<void>> jobs;
        for (size_t idx = 1; idx <= 3; ++idx) {
            jobs.emplace_back(std::async(std::launch::async, [&worker, idx]()
            {
                const int32_t sleepTime = getRandomInt(0, 5);
                std::this_thread::sleep_for(std::chrono::seconds(sleepTime));

                switch (idx) {
                    case 1: worker.first(); break;
                    case 2: worker.second(); break;
                    case 3: worker.third(); break;
                }
            }));
        }

        for (const auto& T: jobs)
            T.wait();
    }
}

namespace SpinLock::Compare_Diff_SpinLock_Implementations
{
    struct SpinLock
    {
        std::atomic_flag flag {false};

        void lock() {
            // First thead has flag == true. So it will exit while loop at the first iteration
            while (flag.test_and_set(std::memory_order_acquire)) {
            }
        }

        void unlock() { flag.clear(std::memory_order_release); }
        // ~SpinLock() { flag.clear(std::memory_order_release); }
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

    struct SpinLock4_1
    {
        alignas(std::hardware_destructive_interference_size) std::atomic<uint32_t> flag {0 };
        alignas(std::hardware_destructive_interference_size) timespec ns {0, 1};

        void lock()
        {

            for (uint8_t n = 0;flag.load(std::memory_order_relaxed) || flag.exchange(1, std::memory_order_acquire); ++n)
            {
                ns.tv_nsec = static_cast<int>(n);
                nanosleep(&ns, nullptr);
            }
        }

        void unlock() { flag.store(0, std::memory_order_release); }
    };

    class spin_mutex_M2
    {
    private:
        using flag = std::atomic<uint32_t>;
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
            uint32_t expected = 0;
            do {
                expected = 0;
            } while (!_f.compare_exchange_weak(expected, 1, std::memory_order_relaxed, std::memory_order_relaxed));
        }

        bool try_lock()
        {
            uint32_t expected = 0;
            return _f.compare_exchange_weak(expected, 1, std::memory_order_relaxed, std::memory_order_relaxed);
        }

        void unlock()
        {
            _f.store(0, std::memory_order_relaxed);
        }
    };

    void RunBenchmark()
    {
        constexpr int threadsMax { 16 };
        constexpr size_t iterCount { 1'000'000 };
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
                ScopedTimer timer {"SpinLock2_Int_Timer"};
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
                ScopedTimer timer {"SpinLock2_Timer_Ex"};
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
                ScopedTimer timer {"SpinLock3"};
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
                ScopedTimer timer {"SpinLock4"};
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
                ScopedTimer timer {"SpinLock4_1"};
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
                ScopedTimer timer {"spin_mutex_M2"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        /// Mutex              :  0.636317 seconds.
        /// SpinLock           :  3.79414 seconds.
        /// SpinLock2          :  3.91013 seconds.
        /// SpinLock2_Int      :  2.45358 seconds.
        /// SpinLock2_Int_Timer:  0.294999 seconds.
        /// SpinLock3          :  0.121038 seconds.
        /// SpinLock4          :  0.0963803 seconds.
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
                ScopedTimer timer {"SpinLock4"};
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
                ScopedTimer timer {"SpinLock4_1"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }
    }
}


void SpinLock::TestAll()
{
    // SwitchingThreads_SpinLock::singleThreadTest();
    // SwitchingThreads_SpinLock::multiThreadTest();

    // Impl::SpinLock_Tests();

    // Compare_Diff_SpinLock_Implementations::RunBenchmark();
    Compare_Diff_SpinLock_Implementations::RunBenchmark_Fastest();
}