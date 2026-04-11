/**============================================================================
Name        : SpinLock.cpp
Created on  : 22.10.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SpinLock.cpp
============================================================================**/


#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <future>
#include <mutex>
#include <syncstream>

#include <emmintrin.h> // _mm_pause()

#include "SpinLock.h"

#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << " "


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


    struct FastSpinLock
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

        inline void unlock() noexcept {
            flag.store(0, std::memory_order_release);
        }

        ~FastSpinLock() {
            unlock();
        }
    };

    struct FastSpinLock2
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

        inline void unlock() noexcept {
            isLocked.store(false, std::memory_order_release);
        }

        ~FastSpinLock2() {
            unlock();
        }
    };

    struct SpinLock_Experimental
    {
        std::atomic_flag isLocked {false };

        void lock()
        {
            int_fast32_t backOff { 0 };
            while (isLocked.test_and_set(std::memory_order_acquire)) {
                for (int_fast32_t i = 0; i < backOff; ++i) {
                    _mm_pause();
                }

                if (backOff < 1024) {
                    backOff *= 2;
                } else {
                    std::this_thread::yield();
                }
            }
        }

        inline void unlock() noexcept {
            isLocked.clear(std::memory_order_release);
        }

        ~SpinLock_Experimental() {
            unlock();
        }
    };
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
            LOG <<  ": First" << std::endl;
            turnSwitch.store(2, std::memory_order_release); // turnSwitch.store(2) or turnSwitch = 2
        }

        void second()
        {
            waitForOrder_SpinLock(2);
            LOG <<  ": Second" << std::endl;
            turnSwitch.store(3, std::memory_order_release);
        }

        void third()
        {
            waitForOrder_SpinLock(3);
            LOG <<  ": Third" << std::endl;
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
                const uint32_t sleepTime = getRandomInt(0, 5);
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



namespace SpinLock::Tests
{
    struct NullSpinLock
    {
        void lock() {}
        void unlock() {}
    };


// #pragma GCC push_options
// #pragma GCC optimize("O0")

    __attribute__((optimize("O0")))
    void basicCorrectnessTest()
    {
        constexpr uint32_t threadsCount { 16 };
        uint32_t counter = 0;

        // NullSpinLock lock;
        // Impl::SpinLock_Bool lock;
        // Impl::SpinLock_AtomicFlag lock;
        Impl::FastSpinLock lock;
        // Impl::FastSpinLock2 lock;
        // Impl::SpinLock_Experimental lock;

        auto callback = [&]() {
            for (int i = 0; i < 1'000'000; ++i) {
                lock.lock();
                ++counter;
                lock.unlock();
            }
        };

        std::vector<std::jthread> workers;
        for (uint32_t idx = 0; idx < threadsCount; ++idx) {
            workers.emplace_back(callback);
        }
        workers.clear();

        std::osyncstream {std::cout} << " counter = " << counter << std::endl;
    }

// #pragma GCC pop_options
}

void SpinLock::TestAll()
{
    Tests::basicCorrectnessTest();


    // SwitchingThreads_SpinLock::singleThreadTest();
    // SwitchingThreads_SpinLock::multiThreadTest();
}