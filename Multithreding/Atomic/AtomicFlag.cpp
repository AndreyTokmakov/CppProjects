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

namespace AtomicFlag::BasicTests
{
    void TestAndSet()
    {
        std::cout << std::boolalpha;

        {
            std::atomic_flag flag { ATOMIC_FLAG_INIT };

            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: false, flag --> true
            flag.clear();
            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: false, flag --> true
            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: true , flag --> true
        }

        std::cout << std::endl;

        {
            std::atomic_flag flag{true};

            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: true,  flag --> true
            flag.clear();
            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: false, flag --> true
            std::cout << flag.test_and_set(std::memory_order_acquire) << std::endl; // ret: true,  flag --> true
        }
    }
}


namespace AtomicFlag::SpinLock {

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


    void Test()
    {
        SpinLock mtx {};

        auto consumer = [&]()
        {
            std::osyncstream{std::cout} << timeString() << " Consumer: Started\n";
            std::lock_guard lock {mtx};

            std::osyncstream{std::cout} << timeString() << " Consumer: Done\n\n";
        };

        auto producer = [&]()
        {
            std::osyncstream{std::cout} << timeString() << " Producer: Started\n";
            std::lock_guard lock {mtx};

            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::osyncstream{std::cout} << timeString() << " Producer: Done\n";
        };

        for (int i = 0; i < 3; ++i)
        {
            std::jthread a(producer);
            std::jthread b(consumer);
            std::this_thread::sleep_for(std::chrono::milliseconds (10));
        }
    }
}

// TODO: Wait()
// TODO: Notify()

void AtomicFlag::TestAll()
{
    // BasicTests::TestAndSet();

    SpinLock::Test();
};