/**============================================================================
Name        : Mutex_vs_AtomicLock.cpp
Created on  : 22.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Mutex_vs_AtomicLock.cpp
============================================================================**/

#include "Mutex_vs_AtomicLock.h"

#include "../Utilities/Utilities.h"
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

namespace Mutex_vs_AtomicLock
{
    template<typename T>
    concept Storage = requires(T store)
    {
        store.increment();
        store.get_value();
    };

    struct SynchStorage
    {
        std::atomic_flag flag = ATOMIC_FLAG_INIT ;
        unsigned long counter { 0 };

    public:
        void lock()
        {
            while (flag.test_and_set(std::memory_order_acquire)) {
                // THREAD_INFO << "Locked"  << std::endl;
            }
        }
        void unlock() {
            flag.clear(std::memory_order_release);
        }

        void increment()
        {
            lock();
            counter++;
            unlock();
        }

        [[nodiscard]]
        unsigned long get_value() const noexcept {
            return counter;
        }
    };


    struct SynchStorage_Mutex
    {
        std::mutex mtx;
        unsigned long counter{ 0 };

    public:

        void increment() {
            std::lock_guard<std::mutex> lock(mtx);
            counter++;
        }

        [[nodiscard]]
        unsigned long get_value() const noexcept {
            return counter;
        }
    };

    void benchmark(Storage auto& storage,
                   std::string_view message)
    {
        constexpr int thread_max = 8, iter_max = 100'000;
        auto worker = [&]()->void {
            for (int i = 0; i < iter_max; i++) {
                storage.increment();
            }
        };

        Utilities::ScopedTimer timer {message};

        std::vector<std::future<void>> tasks;
        for (auto i = 0; i < thread_max; ++i)
            tasks.emplace_back(std::async(worker));
        for (const auto& fut : tasks)
            fut.wait();

        std::osyncstream {std::cout } << "Counter = " << storage.get_value() << std::endl;
    }
};


void Mutex_vs_AtomicLock::benchmarks()
{
    SynchStorage atomicStorage;
    SynchStorage_Mutex storage;
    benchmark(atomicStorage, "AtomicLockStorage");
    benchmark(storage, "MutexSynchStorage");
}