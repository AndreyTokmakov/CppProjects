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

namespace PerformanceExperiments::CV_vs_Atomic
{
    struct RunnerBase
    {
        static constexpr int MaxCountTimes {1'000'000};

        virtual void ping() noexcept = 0;
        virtual void pong() noexcept = 0;
        virtual bool start() noexcept = 0;

        virtual ~RunnerBase() = default;
    };

    struct CVEngine: RunnerBase
    {
        std::condition_variable cv;
        std::mutex mtx;
        bool pingDone { false };
        bool pongDone { false };

        void ping() noexcept override
        {
            int counter = 0;
            while (counter <= MaxCountTimes)
            {
                {
                    std::unique_lock lck { mtx };
                    cv.wait(lck, [this]() {
                        return pingDone;
                    });
                    pingDone = false;
                    pongDone = true;
                    ++counter;
                }
                cv.notify_one();
            }
        }

        void pong() noexcept override
        {
            int counter = 0;
            while (counter<MaxCountTimes)
            {
                {
                    std::unique_lock lck { mtx };
                    cv.wait(lck, [this](){
                        return pongDone;
                    });
                    pingDone = true;
                    pongDone = false;
                    ++counter;
                }
                cv.notify_one();
            }
        }

        bool start() noexcept override
        {
            {
                std::unique_lock lck { mtx };
                pingDone = true;
            }
            cv.notify_one();
            return true;
        }
    };

    struct AtomicEngine: RunnerBase
    {
        std::atomic_flag pass;
        std::atomic_int counter;

        void ping() noexcept override
        {
            int counterLocal = 0;
            while (counterLocal <= MaxCountTimes) {
                pass.wait(false);
                pass.clear();
                ++counterLocal;
                ++counter;
                pass.notify_one();
                // std::osyncstream {std::cout}  << "ping: " << counterLocal << std::endl;
            }
            // std::osyncstream {std::cout} << "ping: done!!!" << std::endl;
        }

        void pong() noexcept override
        {
            int counterLocal = 0;
            while (counterLocal < MaxCountTimes) {
                pass.wait(true);
                pass.test_and_set();
                ++counterLocal;
                pass.notify_one();
                // std::osyncstream {std::cout}  << "pong: " << counterLocal << std::endl;
            }
            // std::osyncstream {std::cout} << "pong: done!!!" << std::endl;
        }

        bool start() noexcept override
        {
            pass.test_and_set();
            pass.notify_one();
            return true;
        }
    };

    void RunBenchmark()
    {
        {
            Utils::ScopedTimer timer {"Atomic variable     "};
            if (auto engine = new AtomicEngine(); engine->start()) {
                std::jthread ping = std::jthread(&AtomicEngine::ping, engine);
                std::jthread pong = std::jthread(&AtomicEngine::pong, engine);
            }
        }

        {
            Utils::ScopedTimer timer {"Conditional variable"};
            if (auto engine = new CVEngine(); engine->start()) {
                std::jthread ping = std::jthread(&CVEngine::ping, engine);
                std::jthread pong = std::jthread(&CVEngine::pong, engine);
            }
        }
        // Duration: 0.171744 seconds
        // Duration: 2.54261 seconds
    }
};



namespace PerformanceExperiments::SpinLock_vs_Mutex
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

    void RunBenchmark()
    {
        constexpr int threadsMax {16};
        constexpr size_t iterCount { 10'000'000 };
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
                Utils::ScopedTimer timer {"Mutex"};
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
                Utils::ScopedTimer timer{"SpinLock"};
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
                Utils::ScopedTimer timer {"SpinLock2"};
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
                Utils::ScopedTimer timer {"SpinLock2_Int"};
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
                Utils::ScopedTimer timer {"SpinLock2_Int_Timer"};
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
                Utils::ScopedTimer timer {"SpinLock2_Timer_Ex"};
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
                Utils::ScopedTimer timer {"SpinLock3"};
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
                Utils::ScopedTimer timer {"SpinLock4"};
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
                Utils::ScopedTimer timer {"spin_mutex_M2"};
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
}

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
                Utils::ScopedTimer timer {"Mutex"};
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
                Utils::ScopedTimer timer {"atomic"};
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
                Utils::ScopedTimer timer {"atomic"};
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
                Utils::ScopedTimer timer {"atomic"};
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
                Utils::ScopedTimer timer {"atomic"};
                std::vector<std::jthread> jobs;
                for (int t = 0; t < threadsMax; ++t)
                    jobs.emplace_back(task);
            }
        }

        /// Result: 686003 microseconds   mutext
        /// Result: 186516 microseconds   ++counter
        /// Result: 185476 microseconds   fetch_add
        /// Result: 712774 microseconds   compare_exchange_strong
        /// Result: 717862 microseconds   compare_exchange_weak
    }
}

void PerformanceExperiments::TestAll()
{
    CV_vs_Atomic::RunBenchmark();
    // SpinLock_vs_Mutex::RunBenchmark();
    // AtomicCounter_vs_Mutex::RunBenchmark();
};
