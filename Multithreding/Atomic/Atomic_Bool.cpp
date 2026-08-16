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
#include <thread>
#include <string>
#include <chrono>
#include <future>
#include <format>
#include <syncstream>

#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << utilities::datetime::getCurrentTime() << " "


namespace Atomic_Bool
{

    void ExchangeTest()
    {
        {
            std::atomic<bool> flag {false };
            for (int i = 0; i < 2; ++i)
                std::cout << std::boolalpha << flag.exchange(true) << std::endl;
        }

        {
            std::atomic<bool> flag {true };
            for (int i = 0; i < 2; ++i)
                std::cout << std::boolalpha << flag.exchange(true) << std::endl;
        }
    }

    void spin_lock_func(std::atomic<bool>& flag)
    {
        while (flag.exchange(true, std::memory_order::acquire)) {
        }

        LOG << "Spint lock is done" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1u));

        LOG << "Releasing " << std::endl;
        flag.store(false, std::memory_order::release);
    }

    void SpinLock_Function_Test()
    {
        std::atomic<bool> flag {true };

        std::jthread workerOne([&] {
            LOG << "workerOne started" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1u));

            flag.store(false, std::memory_order::release);
            LOG << "workerOne done" << std::endl;
        });

        std::jthread workerTwo([&] {
            LOG << "workerTwo started" << std::endl;
            spin_lock_func(flag);
            LOG << "workerTwo done" << std::endl;
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
            LOG << std::format("task {} started", name) << std::endl;
            LockGuard lockGuard {lockMtx};
            std::this_thread::sleep_for(std::chrono::seconds(1u));
            LOG << std::format("task {} completed", name) << std::endl;
        };


        std::vector<std::jthread> tasks;
        tasks.emplace_back(task, "Task_1");
        tasks.emplace_back(task, "Task_2");
        tasks.emplace_back(task, "Task_3");
    }
}

void Atomic_Bool::TestAll()
{
    ExchangeTest();
    // SpinLock_Function_Test();
    // SpinLock::Multiple_Threads_Test();
}