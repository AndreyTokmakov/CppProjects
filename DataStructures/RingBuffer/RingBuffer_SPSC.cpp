/**============================================================================
Name        : RingBuffer_SPSC.cpp
Created on  : 25.12.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RingBuffer_SPSC.cpp
============================================================================**/

#include "RingBuffer_SPSC.h"

#include <iostream>
#include <string_view>
#include <vector>

#include <atomic>
#include <thread>
#include <future>



namespace RingBuffer_SPSC
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

        void put(const value_type& value)
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
        }

        bool get(value_type& value)
        {
            size_type readIdx = idxRead.load(std::memory_order::relaxed);
            if (!overflow && idxWrite == readIdx) {
                return false;
            }

            if (readIdx >= buffer.size()) {
                readIdx = 0;
                overflow = false;
            }

            value = std::move(buffer[readIdx++]);
            idxRead.store(readIdx, std::memory_order::release);
            return true;
        }

        void printState()
        {
            std::cout << "idxRead: " << idxRead << ", idxWrite: " << idxWrite << std::endl;

            std::cout << "[";
            for (auto v: buffer)
                std::cout << v << " ";
            std::cout << "]\n";
        }
    };
}


namespace RingBuffer_SPSC::TestUtils
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
            ringBuff.printState();
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
}

namespace RingBuffer_SPSC::MultithreadedTests
{
    void Test()
    {
        RingBuffer<std::string> buffer(32);

        auto produce = [&buffer](const std::string& text) {
            int64_t count = 0, n = 0;
            while (true) {
                buffer.put(std::string { text });
                if (10'000 == ++count) {
                    ++n;
                    //std::cout << count * n << std::endl;
                    count = 0;
                }
            }
        };

        auto consume = [&buffer]() {
            int64_t count = 0, n = 0;
            std::string str;
            while (true) {
                if (buffer.get(str)) {
                    if (100'000 == ++count) {
                        ++n;
                        std::cout << count * n << std::endl;
                        count = 0;
                    }
                }
            }
        };

        auto producer = std::async(produce, std::string(128, 'X'));
        auto consumer = std::async(consume);

        producer.wait();
        consumer.wait();
    }
}


namespace RingBuffer_SPSC::Tests
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

    void get_after_overlapped()
    {
        EXIT_IF_FAILURES_EXISTS;
        RingBuffer<int> buffer(3);

        for (int i = 1; i <= 5; ++i)
            buffer.put(i);

        getAndCompare(buffer, 3);
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

}

void RingBuffer_SPSC::TestAll()
{
    MultithreadedTests::Test();

#if 0
    Tests::get_empty_buffer();
    Tests::get_add_one();
    Tests::get_multiple_size_1();
    Tests::get_multiple_size_3();
    Tests::get_after_overlapped();
    Tests::put_and_get_no_overlapping_1();
#endif
}