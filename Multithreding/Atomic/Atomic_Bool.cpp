/**============================================================================
Name        : Atomic_Bool.cpp
Created on  : 10.09.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Atomic_Bool.cpp
============================================================================**/

#include "Atomic_Bool.h"

#include <iostream>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <chrono>
#include <future>
#include <cassert>
#include <syncstream>
#include <print>
#include <format>

namespace
{
    std::string timeString()
    {
        std::string buffer;
        buffer.reserve(32);
        std::format_to(std::back_inserter(buffer), "[{:%Y-%m-%d %H:%M:%OS}]", std::chrono::system_clock::now());
        buffer.shrink_to_fit();
        return buffer;
    }
}

#define PRINT std::osyncstream {std::cout} << timeString() << " "

namespace Atomic_Bool
{

    void ExchangeTest()
    {
        std::atomic<bool> flag {true };
        std::cout << flag.exchange(true) << std::endl;
        std::cout << flag.exchange(true) << std::endl;
    }

    void spin_lock_func(std::atomic<bool>& flag)
    {
        while (flag.exchange(true, std::memory_order::acquire)) {
        }

        PRINT << "Spint lock is done" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        PRINT << "Releasing " << std::endl;
        flag.store(false, std::memory_order::release);
    }

    void SpinLock_Function_Test()
    {
        std::atomic<bool> flag {true };

        std::jthread workerOne([&] {
            PRINT << "workerOne started" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));

            flag.store(false, std::memory_order::release);
            PRINT << "workerOne done" << std::endl;
        });

        std::jthread workerTwo([&] {
            PRINT << "workerTwo started" << std::endl;
            spin_lock_func(flag);
            PRINT << "workerTwo done" << std::endl;
        });
    }
};


namespace Atomic_Bool::SpinLock
{
    class SpinLock
    {
        std::atomic<bool> isLocked {false };

    public:

        explicit SpinLock(bool is_locked = false) : isLocked { is_locked } {
        }

        void lock() {
            while (isLocked.exchange(true, std::memory_order::acquire)) {
            }
        }

        void unlock() {
            isLocked.store(false, std::memory_order_release);
        }

        ~SpinLock() {
            isLocked.store(false, std::memory_order_release);
        }
    };


    struct LockGuard
    {
        SpinLock& spinLock;

        explicit LockGuard(SpinLock& spin_lock): spinLock { spin_lock } {
            spinLock.lock();
        }

        ~LockGuard() {
            spinLock.unlock();
        }
    };

    void Multiple_Threads_Test()
    {
        SpinLock lockMtx;

        auto task = [&lockMtx](const std::string& name) {
            PRINT << std::format("task {} started", name) << std::endl;
            LockGuard lockGuard {lockMtx};
            std::this_thread::sleep_for(std::chrono::seconds(1));
            PRINT << std::format("task {} completed", name) << std::endl;
        };


        std::vector<std::jthread> tasks;
        tasks.emplace_back(task, "Task_1");
        tasks.emplace_back(task, "Task_2");
        tasks.emplace_back(task, "Task_3");
    }
}

void Atomic_Bool::TestAll()
{
    // ExchangeTest();
    // SpinLock_Function_Test();

    SpinLock::Multiple_Threads_Test();
}