/**============================================================================
Name        : BlockingQueue.cpp
Created on  : 25.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : BlockingQueue.cpp
============================================================================**/

#include "BlockingQueue.h"

#include <iostream>
#include <vector>
#include <thread>

#include "../Utilities/Wrapper.h"
#include "../Utilities/Utilities.h"


namespace BlockingQueue::WithLock
{
    template<typename T>
    struct Queue
    {
        using value_type = T;
        using size_type = size_t;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        mutable std::mutex mutex;

        size_type writePos {0};
        size_type readPos {0};
        size_type capacity {0};

        std::vector<value_type> buffer;

        explicit Queue(size_type capacity = 1'000): capacity {capacity}
        {
            buffer.resize(capacity);
        }

        bool read(value_type& output) noexcept
        {
            std::lock_guard<std::mutex> lock {mutex};
            if (readPos >= writePos) {
                return false;
            }

            output = buffer[readPos++];
            return true;
        }

        void add(value_type value) noexcept
        {
            std::lock_guard<std::mutex> lock {mutex};


            if (writePos >= buffer.size())
            {
                buffer.resize(buffer.size() + capacity);
            }

            if (readPos > buffer.size() / 2)
            {
                writePos = writePos - readPos;
                readPos = 0;
            }

            buffer[writePos++] = value;
        }
    };
}


namespace BlockingQueue::WithAtomics
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


void BlockingQueue::TestAll()
{
    // using namespace WithLock;
    using namespace WithAtomics;


    Queue<int> queue {1000};
    constexpr int eventsCount {1'000'000};

    Utilities::ScopedTimer timer {"benchmark"};

    auto consume = [&]() {
        int eventsRead = 0, result;
        while (eventsCount > eventsRead)
        {
            if (queue.read(result)) {
                ++eventsRead;
            }
        }
    };

    auto produce = [&]() {
        for (int idx = 0; idx <= eventsCount; ++idx)
        {
            queue.add(idx);
        }
    };

    std::jthread consumer {consume};
    std::jthread producer {produce};

    consumer.join();
    producer.join();

    std::cout << queue.buffer.capacity() << std::endl;
}