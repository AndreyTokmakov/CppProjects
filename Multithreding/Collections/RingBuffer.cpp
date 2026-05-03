/**============================================================================
Name        : RingBuffer.cpp
Created on  : 14.12.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RingBuffer.cpp
============================================================================**/

#include "Collections.h"

#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <syncstream>


namespace Collections::RingBuffer
{
    template<typename T>
    struct RingBuffer
    {
        using size_type = uint32_t;
        using value_type = T;
        using collection_type = std::vector<value_type>;

        std::atomic<size_type> idxRead { 0 };
        std::atomic<size_type> idxWrite { 0 };
        std::atomic<bool> overflow {false };
        const size_type capacity { 0 };
        collection_type buffer {};

        explicit RingBuffer(size_type size):
            capacity { size }, buffer(capacity) {
        }

       void put(const value_type& value)
       {
           size_type writeIdx = idxWrite.load(std::memory_order::relaxed);
           if (writeIdx == capacity) {
               writeIdx = 0;
               overflow = true;
           }

           size_type readIdx = idxRead.load(std::memory_order::relaxed);
           if (overflow && writeIdx == readIdx)
           {
               if (++readIdx >= capacity) {
                   readIdx = 0;
                   overflow = false;
               }
               idxRead.store(readIdx, std::memory_order::release);
           }

           buffer[writeIdx++] = std::move(value);
           idxWrite.store(writeIdx, std::memory_order::release);
           idxWrite.notify_one();
       }

        void put(value_type&& value)
        {
            size_type writeIdx = idxWrite.load(std::memory_order::relaxed);
            if (writeIdx == capacity) {
                writeIdx = 0;
                overflow = true;
            }

            size_type readIdx = idxRead.load(std::memory_order::relaxed);
            if (overflow && writeIdx == readIdx)
            {
                if (++readIdx >= capacity) {
                    readIdx = 0;
                    overflow = false;
                }
                idxRead.store(readIdx, std::memory_order::release);
            }

            buffer[writeIdx++] = std::move(value);
            idxWrite.store(writeIdx, std::memory_order::release);
            idxWrite.notify_one();
        }

        bool get(value_type& value)
        {
            size_type readIdx = idxRead.load(std::memory_order::relaxed);
            if (!overflow && idxWrite == readIdx) {
                return false;
            }

            if (readIdx >= capacity) {
                readIdx = 0;
                overflow = false;
            }

            value = std::move(buffer[readIdx++]);
            idxRead.store(readIdx, std::memory_order::release);
            return true;
        }

        void get_wait(value_type& value)
        {
            size_type readIdx = idxRead.load(std::memory_order::relaxed);
            if (!overflow && idxWrite == readIdx) {
                idxWrite.wait(readIdx);
            }

            if (readIdx >= capacity) {
                readIdx = 0;
                overflow = false;
            }

            value = std::move(buffer[readIdx++]);
            idxRead.store(readIdx, std::memory_order::release);
        }
    };
}

namespace Collections::RingBuffer::TestUtils
{
    size_t failuresCount { 0 };

    template<typename T>
    void print(const RingBuffer<T>& ringBuff)
    {
        std::cout << "idxRead: " << ringBuff.idxRead << ", idxWrite: " << ringBuff.idxWrite
                  << ", overflow: " << std::boolalpha << ringBuff.overflow << std::endl;
        for (const T& entry: ringBuff.buffer)
        {
            std::cout << entry << std::endl;
        }
    }

    template<typename T>
    void get(RingBuffer<T>& ringBuff)
    {
        int value { 0 };
        bool result = ringBuff.get(value);

        if (result) {
            std::cout <<  "value: " << value << std::endl;
        }
        else {
            std::cout << "Failed to get value" << std::endl;
        }
    }

    template<typename T>
    bool getAndCompare(RingBuffer<T>& ringBuff,
                       const T& valueExpected,
                       const bool resultExpected = true)
    {
        T valueActual { 0 };
        const bool resultActual = ringBuff.get(valueActual);

        if (resultActual != resultExpected)
        {
            std::cerr << "ERROR: Result expected: " << std::boolalpha << resultExpected
                      << ", actual: " << std::boolalpha << resultActual << std::endl;
            ++failuresCount;
            return false;
        }

        if (valueActual != valueExpected)
        {
            std::cerr << "ERROR: Value expected: " << valueExpected << ", actual: "  << valueActual << std::endl;
            ++failuresCount;
            return false;
        }

        return true;
    }

    template<typename T>
    bool getWaitAndCompare(RingBuffer<T>& ringBuff,
                           const T& valueExpected)
    {
        T valueActual { 0 };
        ringBuff.get_wait(valueActual);

        if (valueActual != valueExpected)
        {
            std::cerr << "ERROR: Value expected: " << valueExpected << ", actual: "  << valueActual << std::endl;
            ++failuresCount;
            return false;
        }

        return true;
    }
}

namespace Collections::RingBuffer::Tests
{
    using namespace TestUtils;

#define EXIT_IF_FAILURES_EXISTS if (failuresCount > 0) return;
#define PRINT_RESULT if (failuresCount == 0) std::cout << "All test passed" << std::endl;

    void get_empty_buffer()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(3);
        getAndCompare(buffer, 0, false);
    }

    void get_add_one()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(3);

        buffer.put(123);
        getAndCompare(buffer, 123);
    }

    void get_wait_add_one()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(3);

        buffer.put(123);
        getWaitAndCompare(buffer, 123);
    }

    void get_multiple_size_1()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(1);

        for (int i = 0; i < 10; ++i)
        {
            buffer.put(i * 2);
            getAndCompare(buffer, i * 2);
        }
    }

    void get_wait_multiple_size_1()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(1);

        for (int i = 0; i < 10; ++i)
        {
            buffer.put(i * 2);
            getWaitAndCompare(buffer, i * 2);
        }
    }

    void get_multiple_size_3()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(3);

        for (int i = 0; i < 10; ++i)
        {
            buffer.put(i * 2);
            getAndCompare(buffer, i * 2);
        }
    }

    void get_wait_multiple_size_3()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(3);

        for (int i = 0; i < 10; ++i)
        {
            buffer.put(i * 2);
            getWaitAndCompare(buffer, i * 2);
        }
    }

    void get_after_overlapped()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(3);

        for (int i = 1; i <= 5; ++i)
            buffer.put(i);

        getAndCompare(buffer, 3);
    }

    void get_wait_after_overlapped()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(3);

        for (int i = 1; i <= 5; ++i)
            buffer.put(i);

        getWaitAndCompare(buffer, 3);
    }

    void put_and_get_no_overlapping_1()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(10);

        for (int i = 1; i <= 10; ++i)
            buffer.put(i);

        for (int i = 1; i <= 10; ++i)
            getAndCompare(buffer, i);
    }

    void put_wat_and_get_no_overlapping_1()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(10);

        for (int i = 1; i <= 10; ++i)
            buffer.put(i);

        for (int i = 1; i <= 10; ++i)
            getWaitAndCompare(buffer, i);
    }
}

namespace Collections::RingBuffer::MultithreadedTests
{
    using namespace TestUtils;

#define EXIT_IF_FAILURES_EXISTS if (failuresCount > 0) return;
#define PRINT_RESULT if (failuresCount == 0) std::cout << "All test passed" << std::endl;


    void simple_test()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(10);

        auto produce = [&buffer](const std::string& name) {
            std::osyncstream { std::cout } << name << " started" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100u));
            buffer.put(123);
            std::osyncstream { std::cout } << name << " done" << std::endl;
        };

        auto consume = [&buffer](const std::string& name) {
            std::osyncstream { std::cout } << name << " started" << std::endl;
            int v {0};
            buffer.get_wait(v);
            std::osyncstream { std::cout } << name << " done. value = " << v << std::endl;
        };

        std::vector<std::jthread> tasks;
        tasks.emplace_back(produce, "Producer-1");
        tasks.emplace_back(consume, "Consumer-1");
        tasks.emplace_back(consume, "Consumer-2");
    }


    void Producer_Consumer_Test()
    {
        RingBuffer<int> buffer(10);
        std::vector<int> results;

        auto produce = [&](int size) {
            for (int i = 0; i < size; ++i) {
                buffer.put(i);
                std::osyncstream { std::cout } << i << " ==> pushed " << std::endl;
                //std::this_thread::sleep_for(std::chrono::milliseconds(10u));
            }
            std::osyncstream { std::cout } << "producer done" << std::endl;
        };

        auto consume = [&](int size) {
            int result { 0 };
            for (int i = 0; i < size; ++i) {
                buffer.get_wait(result);
                results.push_back(i);
                std::osyncstream { std::cout } << i << " <== popped " << std::endl;
            }
        };


        constexpr int events { 30 };
        std::vector<std::jthread> tasks;
        tasks.emplace_back(consume, events);
        tasks.emplace_back(produce, events);


        /*
        std::this_thread::sleep_for(std::chrono::seconds(1u));
        std::cout << "Done. size = " << results.size() << std::endl;

        for (int v: results)
            std::cout << v << ' ';
        std::cout << std::endl;*/
    }
}



void Collections::RingBuffer::TestAll()
{
    using namespace Tests;

#if 0
    Tests::get_empty_buffer();
    Tests::get_add_one();
    Tests::get_wait_add_one();
    Tests::get_multiple_size_1();
    Tests::get_wait_multiple_size_1();
    Tests::get_multiple_size_3();
    Tests::get_wait_multiple_size_3();
    Tests::get_after_overlapped();
    Tests::put_and_get_no_overlapping_1();
    Tests::put_wat_and_get_no_overlapping_1();
#endif

    // MultithreadedTests::simple_test();
    MultithreadedTests::Producer_Consumer_Test();

    PRINT_RESULT;
}