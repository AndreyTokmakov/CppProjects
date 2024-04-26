/**============================================================================
Name        : RingBuffer.cpp
Created on  : 25.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RingBuffer.cpp
============================================================================**/

#include "RingBuffer.h"

#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <atomic>

namespace
{
    constexpr bool is_pow_of_2(int value) noexcept {
        return (value && !(value & value - 1));
    }
}

namespace RingBuffer
{
    template<typename T, size_t Capacity>
    struct RingBuffer
    {
        using value_type = T;
        using size_type = size_t;

        static_assert(!std::is_same_v<value_type, void>, "ERROR: Type of the RingBuffer can not be void");
        static_assert(0 != Capacity, "Please try a little bigger buffer");
        static_assert(is_pow_of_2(Capacity), "Capacity shall be power of 2");

        // struct alignas(sizeof(value_type)) Placeholder {};
        // std::unique_ptr<Placeholder[]> buffer { std::make_unique<Placeholder[]>(Capacity) };

        size_type writePos {0};
        size_type readPos {0};
        bool overflow { false };

        std::array<value_type, Capacity> buffer {};

        bool read(value_type& output) noexcept
        {
            if ((overflow && writePos >= readPos) || (!overflow && readPos >= writePos))
                return false;

            output = buffer[readPos++];

            if (readPos >= Capacity) {
                readPos = 0;
                overflow = false;
            }

            return true;
        }

        void add(value_type value) noexcept
        {
            buffer[writePos++] = value;

            if (writePos >= Capacity) {
                writePos = 0;
                overflow = true;
            }
        }
    };
}


namespace RingBuffer::Tests
{
    template<typename T, size_t Size>
    void get(RingBuffer<T, Size> &ring) {
        int value{0};
        bool res = ring.read(value);

        std::cout << std::boolalpha << res << " | " << value << std::endl;
    }

    void Test() {
        RingBuffer<int, 4> ring;

        get(ring);

        ring.add(1);
        ring.add(2);

        get(ring);

        for (int i = 3; i < 30; ++i) {
            ring.add(i);
            get(ring);
        }
    }
}


namespace RingBuffer_WithAtomics
{
    template<typename T>
    struct Queue
    {
        using value_type = T;
        using size_type = size_t;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> readIdx {0};
        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> writeIdx {0};

        size_type capacity {0};

        std::vector<value_type> buffer;

        explicit Queue(size_type capacity = 1'000): capacity {capacity}
        {
            buffer.resize(capacity);
        }

        bool read(value_type& output) noexcept
        {
            const size_type readTo = readIdx.load(std::memory_order_relaxed);
            if (readIdx == writeIdx.load(std::memory_order_acquire)) {
                return false;
            }
            output = buffer[readTo];
            size_type nextReadIdx = readTo + 1;
            if (nextReadIdx == buffer.size()) {
                nextReadIdx = 0;
            }
            readIdx.store(nextReadIdx, std::memory_order_release);
            return true;
        }

        bool add(value_type value) noexcept
        {
            const size_type writeTo = writeIdx.load(std::memory_order_relaxed);
            size_type nextWriteIdx = writeTo + 1;
            if (nextWriteIdx == buffer.size()) {
                nextWriteIdx = 0;
            }
            if (nextWriteIdx == readIdx.load(std::memory_order_acquire)) {
                return false;
            }
            buffer[writeTo] = value;
            writeIdx.store(nextWriteIdx, std::memory_order_release);
            return true;
        }
    };
}


void RingBuffer::TestAll()
{
    // Tests::Test();

    //RingBuffer_WithAtomics

}