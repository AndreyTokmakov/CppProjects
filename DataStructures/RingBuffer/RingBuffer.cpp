/**============================================================================
Name        : RingBuffer.cpp
Created on  : 14.12.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RingBuffer.cpp
============================================================================**/

#include "RingBuffer.h"

#include <iostream>
#include <vector>



namespace RingBuffer
{
    template<typename T>
    struct RingBuffer
    {
        using size_type = size_t;
        using value_type = T;
        using collection_type = std::vector<value_type>;

        size_type idxRead { 0 };
        size_type idxWrite { 0 };
        collection_type buffer {};
        bool overlapped { false };
        // TODO: add consumer lost counter ???

        explicit RingBuffer(size_t size): idxRead { 0 }, idxWrite { 0 }, overlapped { false } {
            buffer.resize(size);
        }

        void put(const value_type& value)
        {
            if (idxWrite == buffer.size()) {
                idxWrite = 0;
                overlapped = true;
            }
            if (overlapped && idxWrite == idxRead) {
                ++idxRead;
            }
            buffer[idxWrite++] = value;
        }

        void put(value_type&& value)
        {
            if (idxWrite == buffer.size()) {
                idxWrite = 0;
                overlapped = true;
            }
            if (overlapped && idxWrite == idxRead) {
                ++idxRead;
            }
            buffer[idxWrite++] = std::move(value);
        }

        bool get(value_type& value)
        {
            if (!overlapped && idxWrite == idxRead) {
                return false;
            }

            if (idxRead == buffer.size()) {
                idxRead = 0;
                overlapped = false;
            }

            value = std::move(buffer[idxRead++]);
            return true;
        }
    };
}

namespace RingBuffer::TestUtils
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
}

namespace RingBuffer::Tests
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

// TODO: Border tests

void RingBuffer::TestAll()
{
    using namespace Tests;

    Tests::get_empty_buffer();
    Tests::get_add_one();
    Tests::get_multiple_size_1();
    Tests::get_multiple_size_3();
    Tests::get_after_overlapped();
    Tests::put_and_get_no_overlapping_1();


    PRINT_RESULT;
}