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


void SpinLock::TestAll()
{
    // SwitchingThreads_SpinLock::singleThreadTest();
    // SwitchingThreads_SpinLock::multiThreadTest();

    Impl::SpinLock_Tests();
}