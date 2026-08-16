/**============================================================================
Name        : AtomicFlag.cpp
Created on  : 24.10.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AtomicFlag.cpp
============================================================================**/

#include "AtomicFlag.h"

#include <iostream>
#include <atomic>
#include <thread>
#include <string_view>
#include <chrono>
#include <format>
#include <syncstream>
#include "DateTimeUtilities.hpp"

namespace
{
    using utilities::datetime::getCurrentTime;

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "
}

namespace AtomicFlag::BasicTests
{
    void TestAndSet()
    {
        std::cout << std::boolalpha;

        {
            std::atomic_flag flag { ATOMIC_FLAG_INIT };

            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: false, flag --> true
            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: false, flag --> true
            flag.clear();
            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: false, flag --> true
            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: true , flag --> true
        }

        std::cout << std::endl;

        {
            std::atomic_flag flag{true};

            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: true,  flag --> true
            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: true,  flag --> true
            flag.clear();
            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: false, flag --> true
            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: true,  flag --> true
        }
    }
}


namespace AtomicFlag::SpinLock
{

    class SpinLock {
    private:
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

    class Waiting_SpinLock
    {
        std::atomic_flag flag {false};

    public:
        void lock() {
            flag.wait(true, std::memory_order_acquire);
            flag.test_and_set();
        }

        void unlock() {
            flag.clear(std::memory_order_release);
            flag.notify_all();
        }
    };


    void Test()
    {
        SpinLock mtx {};

        auto consumer = [&]()
        {
            LOG << " Consumer: Started\n";
            std::lock_guard lock {mtx};

            LOG << " Consumer: Done\n\n";
        };

        auto producer = [&]()
        {
            LOG << " Producer: Started\n";
            std::lock_guard lock {mtx};

            std::this_thread::sleep_for(std::chrono::seconds(1UL));
            LOG << " Producer: Done\n";
        };

        for (int i = 0; i < 3; ++i)
        {
            std::jthread a(producer);
            std::jthread b(consumer);
            std::this_thread::sleep_for(std::chrono::milliseconds (10UL));
        }
    }

    void SpinLock_CounterTest()
    {
        size_t counter {0};
        SpinLock spinLock {};

        auto task = [&]()
        {
            for (int i = 0; i < 10'000; ++i)
            {
                spinLock.lock();
                counter++;
                spinLock.unlock();
            }
        };

        {
            std::vector<std::jthread> jobs;
            for (int t = 0; t < 128; ++t)
                jobs.emplace_back(task);
        }

        std::cout << counter << std::endl;
    }

    void SpinLock_WaitingTest_1()
    {
        uint16_t counter {0};
        Waiting_SpinLock spinLock {};

        auto task = [&]()
        {
            spinLock.lock();
            LOG << " " << ++counter << std::endl;
            // std::this_thread::sleep_for(std::chrono::milliseconds (1));
            spinLock.unlock();
        };

        std::vector<std::jthread> jobs;
        for (int t = 0; t < 20; ++t)
            jobs.emplace_back(task);
    }

    void SpinLock_WaitingTest_2()
    {
        size_t counter {0};
        Waiting_SpinLock spinLock {};

        auto task = [&]()
        {
            for (int i = 0; i < 10'000; ++i)
            {
                spinLock.lock();
                counter++;
                spinLock.unlock();
            }
        };

        {
            std::vector<std::jthread> jobs;
            for (int t = 0; t < 128; ++t)
                jobs.emplace_back(task);
        }

        std::cout << counter << std::endl;
    }
}

namespace AtomicFlag::Waiting
{

    void Notify_and_Wait()
    {
        std::string data { "INITIAL"};
        std::atomic_flag atomicFlag {false};

        auto producer = [&]() {
            LOG << " Producer: Started" << std::endl;

            for (int i = 0; i < 3; ++i)
            {
                data = "NEW VALUE + " + getCurrentTime();

                atomicFlag.test_and_set();

                std::this_thread::sleep_for(std::chrono::seconds(1UL));

                atomicFlag.notify_one();
                LOG << " Producer: notify_one " << std::endl;

                atomicFlag.wait(true);
            }

            LOG << " Producer: done " << std::endl;
        };

        auto consumer = [&] {
            LOG << " Consumer: Started" << std::endl;

            for (int i = 0; i < 3; ++i)
            {
                atomicFlag.wait(false);
                LOG << " Consumer: data = " << data << std::endl;

                atomicFlag.clear();
                atomicFlag.notify_one();
            }

            LOG << " Consumer: done" << std::endl;
        };

        std::jthread t2(consumer);
        std::jthread t1(producer);
    }
}


// TODO: Wait()
// TODO: Notify()

void AtomicFlag::TestAll()
{
    BasicTests::TestAndSet();
    // Waiting::Notify_and_Wait();
    // SpinLock::Test();
    // SpinLock::SpinLock_CounterTest();
    // SpinLock::SpinLock_WaitingTest_1();
    // SpinLock::SpinLock_WaitingTest_2();    // TODO: Error
};