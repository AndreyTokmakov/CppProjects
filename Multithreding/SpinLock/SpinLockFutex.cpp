/**============================================================================
Name        : SpinLockFutex.cpp
Created on  : 27.06.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SpinLockFutex.cpp
============================================================================**/

#include "SpinLockFutex.h"

#include "../Utilities/Utilities.h"

#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <future>
#include <mutex>
#include <syncstream>

namespace
{
    struct SpinLockBest
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

        void unlock() {
            flag.store(0, std::memory_order_release);
        }

        ~SpinLockBest() {
            unlock();
        }
    };
}



namespace SpinLockFutex
{
    struct OnByteMutex
    {
        void lock()
        {
            if (state.exchange(locked, std::memory_order_acquire) == unlocked) {
                return;
            }
            while (state.exchange(sleeper, std::memory_order_acquire) != unlocked) {
                state.wait(sleeper, std::memory_order_relaxed);
            }
        }

        void unlock()
        {
            if (state.exchange(unlocked, std::memory_order_release) == sleeper) {
                state.notify_one();
            }
        }

    private:

        std::atomic<uint8_t> state { unlocked };

        static inline constexpr uint8_t unlocked { 0 };
        static inline constexpr uint8_t locked { 0b01 };
        static inline constexpr uint8_t sleeper { 0b10 };
    };
}

namespace Tests
{

// #pragma GCC push_options
// #pragma GCC optimize("O0")

    __attribute__((optimize("O0")))
    void basicTest()
    {
        constexpr uint32_t threadsCount { 16 };
        uint32_t counter = 0;

        // SpinLockFutex::OnByteMutex lock;
        SpinLockBest lock;

        auto callback = [&]() {
            for (int i = 0; i < 1'000'000; ++i) {
                //lock.lock();
                ++counter;
                //lock.unlock();
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

void SpinLockFutex::TestAll()
{
    Tests::basicTest();
}