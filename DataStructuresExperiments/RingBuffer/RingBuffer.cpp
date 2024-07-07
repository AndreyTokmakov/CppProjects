/**============================================================================
Name        : RingBuffer.cpp
Created on  : 25.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RingBuffer.cpp
============================================================================**/

#include "RingBuffer.h"
#include "PertTools.h"

#include <iostream>
#include <vector>
#include <array>
#include <memory>

#include <atomic>
#include <thread>

namespace
{
    constexpr bool is_pow_of_2(int value) noexcept {
        return (value && !(value & value - 1));
    }


    template<typename T>
    std::ostream& operator<<(std::ostream& stream,
                             const std::vector<T>& vector)
    {
        for (const auto& v: vector)
            stream << v << ' ';
        return stream;
    }
}

namespace RingBuffer::WithMutex
{
    template<typename T, size_t Capacity>
    struct BlockingRingBuffer
    {
        using value_type = T;
        using size_type = size_t;

        static_assert(!std::is_same_v<value_type, void>, "ERROR: Type of the RingBuffer can not be void");
        static_assert(0 != Capacity, "Please try a little bigger buffer");
        // static_assert(is_pow_of_2(Capacity), "Capacity shall be power of 2");

        size_type writePos {0};
        size_type readPos {0};
        bool overflow { false };
        std::mutex mtx;

        std::array<value_type, Capacity> buffer {};

        bool read(value_type& output) noexcept
        {
            std::lock_guard<std::mutex> lock {mtx};

            if ((overflow && writePos >= readPos) || (!overflow && readPos >= writePos))
                return false;

            output = buffer[readPos++];

            if (readPos >= Capacity) {
                readPos = 0;
                overflow = false;
            }

            return true;
        }

        bool add(value_type value) noexcept
        {
            std::lock_guard<std::mutex> lock {mtx};

            buffer[writePos++] = value;

            if (writePos >= Capacity) {
                writePos = 0;
                overflow = true;
            }
            return true;
        }
    };
}


namespace RingBuffer::WithAtomics
{
    template<typename T, size_t Capacity>
    struct QueueOne
    {
        using value_type = T;
        using size_type = size_t;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> tail {0};
        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> head {0};

        std::vector<value_type> buffer;
        bool overlapped {false};

        explicit QueueOne()
        {
            buffer.resize(Capacity);
        }

        // TODO: Prefetch ??
        bool read(value_type& output) noexcept
        {
            size_type readIdx = tail.load(std::memory_order_relaxed);
            if (!overlapped && readIdx == head.load(std::memory_order_relaxed)) {
                return false;
            }

            output = buffer[readIdx];
            if (++readIdx == Capacity) {
                overlapped = false;
                readIdx = 0;
            }

            tail.store(readIdx, std::memory_order_release);
            return true;
        }

        // TODO: Prefetch ??
        bool add(value_type value) noexcept
        {
            size_type idx = head.load(std::memory_order_relaxed);
            buffer[idx] = value;

            if (++idx == Capacity) {
                overlapped = true;
                idx = 0;
            }

            head.store(idx, std::memory_order_release);
            return true;
        }
    };

    template<typename T, size_t Capacity>
    struct QueueOneX
    {
        using value_type = T;
        using size_type = size_t;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        alignas(std::hardware_destructive_interference_size) size_type tail {0};
        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> head {0};

        std::vector<value_type> buffer;
        bool overlapped {false};

        explicit QueueOneX()
        {
            buffer.resize(Capacity);
        }

        // TODO: Prefetch ??
        bool read(value_type& output) noexcept
        {
            if (!overlapped && tail == head.load(std::memory_order_relaxed)) {
                return false;
            }

            output = buffer[tail];
            if (++tail == Capacity) {
                overlapped = false;
                tail = 0;
            }

            return true;
        }

        // TODO: Prefetch ??
        bool add(value_type value) noexcept
        {
            size_type idx = head.load(std::memory_order_relaxed);
            buffer[idx] = value;

            if (++idx == Capacity) {
                overlapped = true;
                idx = 0;
            }

            head.store(idx, std::memory_order_release);
            return true;
        }
    };

    template<typename T, size_t Capacity>
    struct QueueTwo
    {
        using value_type = T;
        using size_type = size_t;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        struct IndexData
        {
            std::atomic<size_t> atomicIdx {0};
            size_t idx {0} ;

            void store()
            {
                atomicIdx.store(idx, std::memory_order_release);
            }
        };

        alignas(std::hardware_destructive_interference_size) IndexData tail {};
        alignas(std::hardware_destructive_interference_size) IndexData head {};

        std::vector<value_type> buffer;
        bool overlapped {false};

        explicit QueueTwo()
        {
            buffer.resize(Capacity);
        }

        // TODO: Prefetch ??
        bool read(value_type& output) noexcept
        {
            // TODO: Remove atomic
            if (!overlapped && tail.idx == head.atomicIdx.load(std::memory_order_relaxed)) {
                return false;
            }

            output = buffer[tail.idx];
            if (++tail.idx == Capacity) {
                overlapped = false;
                tail.idx = 0;
            }

            tail.store();
            return true;
        }

        // TODO: Prefetch ??
        bool add(value_type value) noexcept
        {
            // TODO: Remove atomic
            buffer[head.idx] = value;

            if (++head.idx == Capacity) {
                overlapped = true;
                head.idx = 0;
            }

            head.store();
            return true;
        }
    };


    template<typename T, size_t Capacity>
    struct QueueThree
    {
        using value_type = T;
        using size_type = size_t;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> tail {0};
        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> head {0};
        std::atomic_flag overlapped {false};
        std::vector<value_type> buffer;

        explicit QueueThree()
        {
            buffer.resize(Capacity);
        }

        // TODO: Prefetch ??
        bool read(value_type& output) noexcept
        {
            size_type readIdx = tail.load(std::memory_order_acquire);
            if (!overlapped.test(std::memory_order_acquire) && readIdx == head.load(std::memory_order_acquire)) {
                return false;
            }

            output = buffer[readIdx];
            if (++readIdx == Capacity) {
                overlapped.clear(std::memory_order_release);
                readIdx = 0;
            }

            tail.store(readIdx, std::memory_order_release);
            return true;
        }

        // TODO: Prefetch ??
        bool add(value_type value) noexcept
        {
            // TODO: Try to use non-atomic here ?
            size_type writeIdx = head.load(std::memory_order_acquire);
            /*
            if (overlapped.test(std::memory_order_acquire) && writeIdx == tail.load(std::memory_order_relaxed)) {
                return false;
            }*/

            buffer[writeIdx] = value;

            if (++writeIdx == Capacity) {
                overlapped.test_and_set(std::memory_order_release);
                writeIdx = 0;
            }

            head.store(writeIdx, std::memory_order_release);
            return true;
        }
    };

    template<typename T, size_t Capacity>
    struct QueueFour
    {
        using value_type = T;
        using size_type = size_t;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        alignas(std::hardware_destructive_interference_size) std::atomic<size_t> writeIdx_ = {0};
        alignas(std::hardware_destructive_interference_size) size_t readIdxCache_ = 0;
        alignas(std::hardware_destructive_interference_size) std::atomic<size_t> readIdx_ = {0};
        alignas(std::hardware_destructive_interference_size) size_t writeIdxCache_ = 0;

        std::vector<value_type> buffer;


        explicit QueueFour()
        {
            buffer.resize(Capacity);
        }

        // TODO: Prefetch ??
        bool read(value_type& output) noexcept
        {
            const size_type readIdx = readIdx_.load(std::memory_order_relaxed);
            if (writeIdx_.load(std::memory_order_acquire) == readIdx) {
                return false;
            }

            output = buffer[readIdx];
            size_type nextReadIdx = readIdx + 1;
            if (nextReadIdx == Capacity) {
                nextReadIdx = 0;
            }
            readIdx_.store(nextReadIdx, std::memory_order_release);
            return true;
        }

        // TODO: Prefetch ??
        bool add(value_type value) noexcept
        {
            const size_type writeIdx = writeIdx_.load(std::memory_order_relaxed);
            auto nextWriteIdx = writeIdx + 1;
            if (nextWriteIdx == Capacity) {
                nextWriteIdx = 0;
            }

            if (nextWriteIdx == readIdxCache_) {
                readIdxCache_ = readIdx_.load(std::memory_order_acquire);
                if (nextWriteIdx == readIdxCache_) {
                    return false;
                }
            }

            buffer[writeIdx] = value;
            writeIdx_.store(nextWriteIdx, std::memory_order_release);
            return true;
        }
    };

    template<typename T, size_t Capacity>
    struct QueueDebug
    {
        using value_type = T;
        using size_type = size_t;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> tail {0};
        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> head {0};
        std::atomic_flag overlapped {false};
        std::vector<value_type> buffer;

        explicit QueueDebug()
        {
            buffer.resize(Capacity);
        }

        // TODO: Prefetch ??
        bool read(value_type& output) noexcept
        {
            size_type readIdx = tail.load(std::memory_order_acquire);
            if (!overlapped.test(std::memory_order_acquire) && readIdx == head.load(std::memory_order_acquire)) {
                return false;
            }

            output = buffer[readIdx];
            if (++readIdx == Capacity) {
                overlapped.clear(std::memory_order_release);
                readIdx = 0;
            }

            tail.store(readIdx, std::memory_order_release);
            return true;
        }

        // TODO: Prefetch ??
        bool add(value_type value) noexcept
        {
            // TODO: Try to use non-atomic here ?
            size_type writeIdx = head.load(std::memory_order_acquire);
            if (overlapped.test(std::memory_order_acquire) && writeIdx == tail.load(std::memory_order_acquire)) {
                return false;
            }

            buffer[writeIdx] = value;

            if (++writeIdx == Capacity) {
                overlapped.test_and_set(std::memory_order_release);
                writeIdx = 0;
            }

            head.store(writeIdx, std::memory_order_release);
            return true;
        }
    };
}

namespace RingBuffer::Tests
{
    template<typename T>
    void get(T& ring) {
        int value{0};
        bool res = ring.read(value);

        std::cout << std::boolalpha << res << " -> " << value << std::endl;
    }

    template<typename T, typename V>
    void add(T& ring, V value) {
        bool res = ring.add(value);
        std::cout << std::boolalpha << res << " -> " << value << std::endl;
    }

    void Test()
    {
        using namespace WithAtomics;
        using namespace WithMutex;

        QueueTwo<int, 3> ring{};

        add(ring, 1);
        add(ring, 2);
        add(ring, 3);

        std::cout << ring.buffer << std::endl;

        get(ring);
        get(ring);
    }

    void Benchmark()
    {
        using namespace WithAtomics;
        using namespace WithMutex;

        // QueueOne<int, 100'000> queue {};
        // QueueOneX<int, 100'000> queue {};
        // QueueTwo<int, 100'000> queue {};
        // QueueThree<int, 100'000> queue {}; // <-- OK
        // QueueFour<int, 100'000> queue {};

        // QueueDebug<int, 100'000> queue {};
        BlockingRingBuffer<int, 100'000> queue {};

        constexpr int eventsCount {1'000'000};
        size_t reads {0}, writes {0};

        PertTools::ScopedTimer timer {"benchmark"};

        auto consume = [&]() {
            int result;
            while (eventsCount > reads)
            {
                if (queue.read(result)) {
                    ++reads;
                }
            }
            std::cout << "consumer done\n";
        };

        auto produce = [&]() {
            for (int idx = 0; idx <= eventsCount; ++idx)
            {
                while (true) {
                    if (queue.add(idx))
                    {
                        ++writes;
                        break;
                    }
                }
            }
            std::cout << "producer done\n";
        };

        std::jthread consumer {consume};
        std::jthread producer {produce};

        consumer.join();
        producer.join();

        std::cout << "reads: " << reads << ", writes: " << writes << std::endl;
    }

    void Benchmark_Debug()
    {
        using namespace WithAtomics;
        using namespace WithMutex;

        // QueueOne<int, 100'000> queue {};
        // QueueOneX<int, 100'000> queue {};
        // QueueTwo<int, 100'000> queue {};
        // QueueThree<int, 100'000> queue {}; <-- OK
        // QueueFour<int, 100'000> queue {};

        // QueueDebug<int, 100'000> queue {};
        BlockingRingBuffer<int, 100'000> queue {};


        constexpr int eventsCount {10'000'000};
        size_t failedReads {0}, failedWrites {0};
        size_t okReads {0}, okWrites {0};

        PertTools::ScopedTimer timer {"benchmark"};

        auto consume = [&]() {
            int result;
            while (eventsCount > okReads)
            {
                if (queue.read(result)) {
                    ++okReads;
                } else {
                    ++failedReads;
                    if (failedReads > (eventsCount * 10))
                    {
                        std::cout << "Error: failedReads\n";
                        return;
                    }
                }
            }
            std::cout << "consumer OK\n";
        };

        auto produce = [&]() {
            for (int idx = 0; idx <= eventsCount; ++idx)
            {
                while (true)
                {
                    if (!queue.add(idx))
                    {
                        ++failedWrites;
                        if (failedWrites > eventsCount * 10)
                        {
                            std::cout << "Error: failedWrites\n";
                            return;
                        }
                    }
                    else {
                        ++okWrites;
                        break;
                    }
                }
            }
            std::cout << "producer OK\n";
        };

        std::jthread consumer {consume};
        std::jthread producer {produce};

        consumer.join();
        producer.join();

        std::cout << "okReads: " << okReads << ", okWrites: " << okWrites << std::endl;
        std::cout << "failedReads: " << failedReads << ", failedWrites: " << failedWrites << std::endl;
    }
}


void RingBuffer::TestAll()
{
    // Tests::Test();

    // RingBuffer_WithAtomics::Test();
    // Tests::Benchmark();
    Tests::Benchmark_Debug();


}