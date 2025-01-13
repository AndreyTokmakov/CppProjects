/**============================================================================
Name        : Atomic_Wait.cpp
Created on  : 13.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <string>
#include <chrono>
#include <future>
#include <syncstream>

#include "Atomic_Wait.h"
#include "../Utilities/Utilities.h"


namespace Atomic_Wait::Basics
{
    void Wait()
    {
        constexpr int initialValue { 0 };
        std::atomic<int> variable { initialValue };

        std::future<void> waiter = std::async(std::launch::async, [&variable]()
        {
            THREAD_INFO << "Waiting until variable changes its value: value = " << variable << std::endl;
            variable.wait(initialValue);
            THREAD_INFO << "Waiting Done!!!: value = " << variable << std::endl;
        });

        std::future<void> task = std::async(std::launch::async, [&variable]()
        {
            THREAD_INFO << "Starting task" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(2U));
            variable.store(initialValue);
            THREAD_INFO << "Task 1 completed: value = " << variable << std::endl;

            variable.notify_all();

            std::this_thread::sleep_for(std::chrono::seconds(2U));
            variable.store(initialValue  + 1);
            THREAD_INFO << "Task 2 completed: value = " << variable << std::endl;

            variable.notify_all();
        });
    }

    void Notify_One()
    {
        using varType = int32_t;
        /// using varType = int64_t; // BUG ---- Will not work as it should
        constexpr varType initialValue { 0 };
        std::atomic<varType> value{ initialValue };

        auto task = std::async([&value]() -> void
        {
            THREAD_INFO << "Starting task" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1U));

            value.store(initialValue + 1);
            value.notify_one();

            THREAD_INFO << "Task  completed: value = " << value << std::endl;
        });

        std::vector<std::future<void>> tasks;
        for (int i = 0; i < 2; ++i) {
            const std::string name { "Thread_" + std::to_string(i) };
            tasks.emplace_back(std::async([&value, name]() -> void {
                THREAD_INFO << "Waiting until value != 10 . . . " << std::endl;
                value.wait(initialValue);
                THREAD_INFO << name << " done" << std::endl;
            }));
        }
    }


    void Notify_All()
    {
        constexpr int initialValue { 0 };
        std::atomic<int> value { initialValue };

        auto task = std::async([&value]() -> void {
            THREAD_INFO << "Starting task" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(5U));

            value.store(initialValue + 1);
            value.notify_all();

            THREAD_INFO << "Task  completed: value = " << value << std::endl;
        });

        std::vector<std::future<void>> tasks;
        for (int i = 0; i < 2; ++i) {
            const std::string name{ "Thread_" + std::to_string(i) };
            tasks.emplace_back(std::async([&value, name]() -> void {
                THREAD_INFO << "Waiting until value != 10 . . . " << std::endl;
                value.wait(initialValue);
                THREAD_INFO << name << " done" << std::endl;
            }));
        }
    }
}


namespace Atomic_Wait::Ring_Buffer_Tests
{
    template<typename T>
    struct RingBuffer
    {
        using size_type = size_t;
        using value_type = T;
        using collection_type = std::vector<value_type>;

        std::atomic<size_type> idxRead { 0 };
        std::atomic<size_type> idxWrite { 0 };
        std::atomic<bool> overflow {false };
        collection_type buffer {};

        explicit RingBuffer(size_t size): idxRead { 0 }, idxWrite { 0 }, overflow { false } {
            buffer.resize(size);
        }

        void put(value_type&& value)
        {
            size_type writeIdx = idxWrite.load(std::memory_order::relaxed);
            if (writeIdx == buffer.size()) {
                writeIdx = 0;
                overflow = true;
            }

            size_type readIdx = idxRead.load(std::memory_order::relaxed);
            if (overflow && writeIdx == readIdx)
            {
                if (++readIdx >= buffer.size()) {
                    readIdx = 0;
                    overflow = false;
                }
                idxRead.store(readIdx, std::memory_order::release);
            }

            buffer[writeIdx++] = std::move(value);
            idxWrite.store(writeIdx, std::memory_order::release);
            idxWrite.notify_one();
        }


        void get_wait(value_type& value)
        {
            size_type readIdx = idxRead.load(std::memory_order::relaxed);
            if (!overflow && idxWrite == readIdx) {
                // std::osyncstream { std::cout } << "get_wait 1  | readIdx: " << readIdx << ", idxWrite: " << idxWrite << std::endl;
                idxWrite.wait(readIdx);
            }

            if (readIdx >= buffer.size()) {
                readIdx = 0;
                overflow = false;
            }

            value = std::move(buffer[readIdx++]);
            idxRead.store(readIdx, std::memory_order::release);
        }
    };

    struct RingBufferDebug
    {
        // using size_type = size_t;
        using size_type = uint32_t;
        using value_type = int;

        std::atomic<size_type> idxRead { 0 };
        std::atomic<size_type> idxWrite { 0 };

        void put(value_type&& value)
        {
            const size_type writeIdx = idxWrite.load(std::memory_order::relaxed);
            idxWrite.store(writeIdx + 1, std::memory_order::release);
            idxWrite.notify_one();
        }

        void get()
        {
            const size_type readIdx = idxRead.load(std::memory_order::relaxed);
            idxWrite.wait(readIdx);
        }
    };


    void WaitTest()
    {
        RingBuffer<int> buffer(10);

        auto produce = [&buffer](const std::string& name) {
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " started" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(4u));
            buffer.put(123);
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " done 1" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(4u));
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " done 2" << std::endl;
        };

        auto consume = [&buffer](const std::string& name) {
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " started" << std::endl;
            int v {0};
            buffer.get_wait(v);
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " done" << std::endl;
        };

        std::vector<std::jthread> tasks;
        tasks.emplace_back(produce, "Producer-1");
        tasks.emplace_back(consume, "Consumer-1");
        tasks.emplace_back(consume, "Consumer-2");
    }

    void WaitTest_Debug()
    {
        RingBufferDebug buffer;

        auto produce = [&](const std::string& name) {
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " started" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1u));
            buffer.put(1);
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " done " << std::endl;
        };

        auto consume = [&](const std::string& name) {
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " started" << std::endl;
            buffer.get();
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " done" << std::endl;
        };

        std::vector<std::jthread> tasks;
        tasks.emplace_back(consume, "Consumer-1");
        tasks.emplace_back(consume, "Consumer-2");
        tasks.emplace_back(produce, "Producer-1");
    }

    void WaitTest_Debug_1()
    {
        // using int_type = uint32_t;
        using int_type = int64_t;


        constexpr int_type initialValue { 0 };
        std::atomic<int_type> idxWrite { initialValue };

        auto produce = [&](const std::string& name)
        {
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " started" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1u));
            idxWrite.fetch_sub(initialValue + 1);
            idxWrite.notify_one();
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " done " << std::endl;
        };

        auto consume = [&](const std::string& name)
        {
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " started" << std::endl;
            idxWrite.wait(initialValue);
            std::osyncstream { std::cout } << Utilities::getCurrentTime() << " " << name << " done" << std::endl;
        };

        std::vector<std::jthread> tasks;
        tasks.emplace_back(consume, "Consumer-1");
        tasks.emplace_back(consume, "Consumer-2");
        tasks.emplace_back(produce, "Producer-1");
    }
}


void Atomic_Wait::TestAll()
{
    // Basics::Wait();
    Basics::Notify_One();
    // Basics::Notify_All();

    // Ring_Buffer_Tests::WaitTest();
    // Ring_Buffer_Tests::WaitTest_Debug();
    // Ring_Buffer_Tests::WaitTest_Debug_1();
}
