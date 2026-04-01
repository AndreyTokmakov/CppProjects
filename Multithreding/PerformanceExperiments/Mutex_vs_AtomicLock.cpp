/**============================================================================
Name        : Mutex_vs_AtomicLock.cpp
Created on  : 22.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Mutex_vs_AtomicLock.cpp
============================================================================**/

#include "Mutex_vs_AtomicLock.h"

#include "Multithreading_Performance.h"

#include <iostream>
#include <string>

#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <future>
#include <syncstream>
#include <concepts>

#include "PerfUtilities.hpp"

namespace Mutex_vs_AtomicLock
{
    template<typename Ty>
    concept Lockable = requires(Ty& lk)
    {
        { lk.lock() } -> std::same_as<void>;
        { lk.unlock() } -> std::same_as<void>;
    };

    class SpinLockBasic
    {
        std::atomic_flag flag {false};

    public:
        void lock() {
            while (flag.test_and_set(std::memory_order_acquire)) {}
        }

        void unlock() {
            flag.clear(std::memory_order_release);
        }
    };

    class SpinLockBasicCas
    {
        std::atomic<bool> isLocked;

    public:
        void lock() {
            bool expected = false;
            while(!isLocked.compare_exchange_weak(expected, true, std::memory_order_acquire)) {
                expected = false;
            }
        }

        void unlock() {
            isLocked.store(false, std::memory_order_release);
        }
    };

    struct SpinLockFast
    {
        void lock()
        {
            for (uint8_t timeout = 0;
                 flag.load(std::memory_order_relaxed) || flag.exchange(1, std::memory_order_acquire);
                 ns.tv_nsec = static_cast<int>(++timeout))
            {
                nanosleep(&ns, nullptr);
            }
        }

        void unlock() noexcept {
            flag.store(0, std::memory_order_release);
        }

        ~SpinLockFast() {
            unlock();
        }

    private:
        alignas(std::hardware_destructive_interference_size) std::atomic<uint32_t> flag {0 };
        alignas(std::hardware_destructive_interference_size) timespec ns { .tv_sec=0, .tv_nsec=1 };
    };



    template<Lockable LockType>
    void benchmark(const std::string_view name,
                   const uint16_t threadsCount,
                   const uint64_t testIterations)
    {
        LockType lock;

        uint64_t counter { 0 };
        auto task = [&] {
            for (uint64_t idx  = 0; idx < testIterations; ++idx) {
                lock.lock();
                ++counter;
                lock.unlock();
            }
        };

        const PerfUtilities::ScopedTimer timer { name };
        std::vector<std::jthread> tasks;
        for (uint16_t it = 0; it < threadsCount; ++it) {
            tasks.emplace_back(task);
        }
    }
};


void Mutex_vs_AtomicLock::benchmarks()
{
    constexpr uint16_t threadsCount = 16;
    constexpr uint64_t testIterations = 2'000'000;

    benchmark<SpinLockBasic>("SpinLockBasic", threadsCount, testIterations);
    benchmark<SpinLockBasicCas>("SpinLockBasicCas", threadsCount, testIterations);
    benchmark<SpinLockFast>("SpinLockFast", threadsCount, testIterations);
    benchmark<std::mutex>("std::mutex", threadsCount, testIterations);

    // SpinLockBasic    :  1.18731 seconds.
    // SpinLockBasicCas :  1.02128 seconds.
    // SpinLockFast     :  0.0758979 seconds.
    // std::mutex       :  0.491975 seconds.
}