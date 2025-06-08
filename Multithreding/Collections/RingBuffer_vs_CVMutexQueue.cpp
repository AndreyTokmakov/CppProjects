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
        collection_type buffer {};

        explicit RingBuffer(): buffer(Capacity + 1)  {
            std::cout << "SIZE = " << buffer.size() << std::endl;
        }

        void put(const value_type& value)
        {
            std::cout << idxWriteCached << std::endl;
            buffer[idxWriteCached] = value;
            idxWriteCached = idxWrite.fetch_add(1, std::memory_order::release) + 1;
        }
    };

}

void Collections::RingBuffer_vs_CVMutexQueue::TestAll()
{
    RingBuffer<int> buffer;
    for (int i = 0; i < std::numeric_limits<uint16_t>::max() * 2; ++i) {
        buffer.put(i);
    }



}