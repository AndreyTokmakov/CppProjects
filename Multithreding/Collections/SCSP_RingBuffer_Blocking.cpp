/**============================================================================
Name        : SCSP_RingBuffer_Blocking.cpp
Created on  : 24.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SCSP_RingBuffer_Blocking.cpp
============================================================================**/

#include "SCSP_RingBuffer_Blocking.h"
#include "../Utilities/Utilities.h"

#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <syncstream>

namespace SCSP_RingBuffer_Blocking
{
    template<typename T>
    struct RingBuffer_Slow
    {
        using size_type = uint32_t;
        using value_type = T;
        using collection_type = std::vector<value_type>;

        std::atomic<size_type> idxRead { 0 };
        std::atomic<size_type> idxWrite { 0 };
        std::atomic<bool> overflow {false };
        const size_type capacity { 0 };
        collection_type buffer {};

        explicit RingBuffer_Slow(size_type size):
                capacity { size }, buffer(capacity) {
        }

        void put(const value_type& value)
        {
            size_type writeIdx = idxWrite.load(std::memory_order::relaxed);
            bool overflowLocal = overflow.load(std::memory_order::relaxed);

            if (writeIdx == capacity) {
                writeIdx = 0;
                overflowLocal = true;
            }

            if (overflowLocal && writeIdx == idxRead.load(std::memory_order::relaxed)) {
                idxRead.wait(writeIdx);
            }

            buffer[writeIdx] = value;
            idxWrite.store(writeIdx + 1, std::memory_order::release);
            overflow.store(overflowLocal, std::memory_order::release);
            idxWrite.notify_one();
        }

        void get(value_type& value)
        {
            size_type readIdx = idxRead.load(std::memory_order::relaxed);
            if (!overflow.load(std::memory_order::relaxed) && idxWrite.load(std::memory_order::relaxed) == readIdx) {
                idxWrite.wait(readIdx);
            }

            if (readIdx >= capacity) {
                readIdx = 0;
                overflow = false;
            }

            value = std::move(buffer[readIdx++]);
            idxRead.store(readIdx, std::memory_order::release);
            idxRead.notify_one();
        }
    };

    template<typename T>
    struct RingBuffer_Fast
    {
        using value_type = T;
        using size_type = uint32_t;

        alignas(std::hardware_destructive_interference_size) size_type head { 0 };

        const size_type capacity { 0 };
        std::vector<value_type> buffer;
        std::atomic<size_type> size { 0 };

        alignas(std::hardware_destructive_interference_size) size_type tail { 0 };

        static_assert(!std::is_same_v<T, void>, "Type of the Queue can not be void");

        explicit RingBuffer_Fast(size_type capacity) :
                capacity { capacity },
                buffer (capacity) {
        }

        void put(const value_type &value)
        {
            if (size.load(std::memory_order_relaxed) >= capacity) {
                size.wait(capacity, std::memory_order::relaxed);
            }

            tail = tail < capacity ? tail : 0;
            buffer[tail++] = value;
            size.fetch_add(1, std::memory_order_release);
            size.notify_one();
        }

        void get(value_type &value)
        {
            if (size.load(std::memory_order_relaxed) == 0) {
                size.wait(0, std::memory_order::relaxed);
            }

            head = head < capacity ? head : 0;
            value = std::move(buffer[head++]);
            size.fetch_sub(1, std::memory_order_release);
            size.notify_one();
        }
    };
}

namespace SCSP_RingBuffer_Blocking::Tests
{
    void Producer_Consumer_Test()
    {
        // RingBuffer_Slow<int> rfBuffer {10 };
        RingBuffer_Fast<int> rfBuffer {10 };
        std::vector<int> results;

        auto produce = [&](int size) {
            for (int i = 0; i < size; ++i) {
                rfBuffer.put(i);
                // std::osyncstream { std::cout } << i << " ==> pushed " << std::endl;
                // std::this_thread::sleep_for(std::chrono::milliseconds(10u));
            }
        };

        auto consume = [&](int size) {
            int result { 0 };
            for (int i = 0; i < size; ++i) {
                rfBuffer.get(result);
                results.push_back(i);
                // std::osyncstream { std::cout } << i << " <== popped " << std::endl;
            }
        };


        constexpr int events { 10'000'000 };
        Utilities::ScopedTimer timer { "Test" };

        std::vector<std::jthread> tasks;
        tasks.emplace_back(consume, events);
        tasks.emplace_back(produce, events);
    }
}

void SCSP_RingBuffer_Blocking::TestAll()
{

    Tests::Producer_Consumer_Test();


    /*
    RingBuffer_Fast<int> rfBuffer {3 };

    int res;
    rfBuffer.pop(res);

    for (int i = 0; i < 10; ++i)
    {
        rfBuffer.push(i);
        std::cout << i << std::endl;
    }*/


}