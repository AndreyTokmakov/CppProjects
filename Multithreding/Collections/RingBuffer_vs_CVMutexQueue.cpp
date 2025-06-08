/**============================================================================
Name        : RingBuffer_vs_CVMutexQueue.cpp
Created on  : 14.12.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "Collections.h"

#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include <atomic>
#include <thread>
#include <chrono>
#include <syncstream>


namespace Collections::RingBuffer_vs_CVMutexQueue
{
    template<typename T,
             int_fast16_t Capacity = std::numeric_limits<uint16_t>::max()>
    struct RingBuffer
    {
        using size_type = uint16_t;
        using value_type = T;
        using collection_type = std::vector<value_type>;

        std::atomic<size_type> idxWrite { 0 };
        size_type idxWriteCached { 0 };
        size_type idxRead { 0 };
        collection_type buffer {};

        explicit RingBuffer(): buffer(Capacity + 1)  {
        }

        void put(const value_type& value)
        {
            buffer[idxWriteCached] = value;
            idxWriteCached = idxWrite.fetch_add(1, std::memory_order::release) + 1;
        }

        [[nodiscard]]
        bool try_read_next(value_type& result)
        {
            if (idxRead == idxWrite.load(std::memory_order::acquire)) {
                return false;
            }

            result = std::move(buffer[idxRead++]);
            return true;
        }
    };
}

namespace Collections::RingBuffer_vs_CVMutexQueue::Tests
{
    void simple_test()
    {
        RingBuffer<int> buffer;
        for (int i = 0; i < 10; ++i) {
            buffer.put(i);
        }

        int result { 0 };
        while (buffer.try_read_next(result)) {
            std::cout << result << std::endl;
        }
    }

    void multithreaded_test()
    {
        RingBuffer<int> buffer;
        std::jthread producer ([&buffer] {
           for (int i = 0; i < 10'000; ++i)
               buffer.put(i);
        });

        std::jthread consumer ([&buffer] {
            int result { 0 };
            while (buffer.try_read_next(result)) {
                // std::cout << result << std::endl;
            }
        });
    }
}

void Collections::RingBuffer_vs_CVMutexQueue::TestAll()
{
    // Tests::simple_test();
    Tests::multithreaded_test();


}