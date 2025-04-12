/**============================================================================
Name        : RingBufferExAtomic.cpp
Created on  : 10.04.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RingBufferExAtomic.cpp
============================================================================**/

#include "RingBufferExAtomic.h"

#include <vector>
#include <cstdint>
#include <iostream>
#include <atomic>
#include <thread>

namespace
{
    constexpr uint32_t fast_modulo(const uint32_t n, const uint32_t d) noexcept {
        return n & (d - 1);
    }

    constexpr bool is_pow_of_2(const int value) noexcept {
        return (value && !(value & value - 1));
    }
}


namespace
{
    template<typename T, size_t Capacity>
    struct RingBuffer
    {
        using size_type = int_fast32_t;
        using value_type = T;
        using collection_type = std::vector<value_type>;
        // using collection_type = std::array<value_type, Capacity>;

        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");
        static_assert(is_pow_of_2(Capacity),  "ERROR: Capacity must be a power of 2");

        collection_type buffer {};
        std::atomic<size_type> idxWrite { 0 };
        std::atomic<size_type> idxRead { 0 };
        std::atomic<bool> full { false };

        RingBuffer(): buffer(Capacity) {
        }

        void put(value_type&& value)
        {
            size_type writeIdx = idxWrite.load(std::memory_order::relaxed);
            size_type readIdx = idxRead.load(std::memory_order::relaxed);

            buffer[writeIdx] = std::move(value);

            if (full.load(std::memory_order::relaxed)) {
                readIdx = fast_modulo(readIdx + 1, Capacity);
                idxRead.store(readIdx, std::memory_order::relaxed);
            }

            writeIdx = fast_modulo(writeIdx + 1, Capacity);
            idxWrite.store(writeIdx, std::memory_order::relaxed);
            full.store(readIdx == writeIdx, std::memory_order::relaxed);
        }

        bool get(value_type& value)
        {
            if (true == empty())
                return false;

            size_type readIdx = idxRead.load(std::memory_order::relaxed);
            value = std::move(buffer[readIdx]);
            full.store(false, std::memory_order::relaxed);

            readIdx = fast_modulo(readIdx + 1, Capacity);
            idxRead.store(readIdx, std::memory_order::relaxed);

            return true;
        }

        [[nodiscard]]
        bool empty() const noexcept
        {
            // TODO: Is 'std::memory_order::relaxed' OK ??
            return (!full.load(std::memory_order::relaxed) &&
                   (idxWrite.load(std::memory_order::relaxed) == idxRead.load(std::memory_order::relaxed)));
        }

        [[nodiscard]]
        bool isFull() const noexcept
        {
            // TODO: Is 'std::memory_order::relaxed' OK ??
            return full.load(std::memory_order::relaxed);
        }

        static size_type capacity() noexcept {
            return Capacity;
        }
    };
}

namespace Tests
{
    void SimpleTest()
    {
        RingBuffer<int, 16> buffer;
        for (int i = 1; i <= 32; ++i) {
            buffer.put(std::move(i));
        }

        int value = 0;
        while (buffer.get(value)) {
            std::cout << value << " ";
        }
    }

    void MultithreadedTest()
    {
        RingBuffer<int, 1024> buffer;
        std::vector<int> readValues;
        readValues.reserve(1024 * 1024);

        auto produce = [&buffer] {
            for (int i = 0; i < 1024 * 2; ++i)
                buffer.put(decltype(i){i});
            std::cout << "Producer done: " << std::endl;
        };

        auto consume = [&buffer, &readValues] {
            int result = 0;
            for (int i = 0; i < 1024; ++i) {
                if (buffer.get(result)) {
                    readValues.push_back(result);
                }
                //else --i;
            }
        };

        {
            std::jthread producer{produce};
            std::jthread consumer{consume};
        }

        std::cout << readValues.size() << std::endl;
        std::cout << readValues.back() << std::endl;
    }
}


void RingBufferExAtomic::TestAll()
{
    // Tests::SimpleTest();
    Tests::MultithreadedTest();
}