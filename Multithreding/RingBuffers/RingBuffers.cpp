/**============================================================================
Name        : RingBuffers.cpp
Created on  : 27.03.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RingBuffers.cpp
============================================================================**/

#include "RingBuffers.hpp"

#include <iostream>
#include <format>
#include <print>
#include <syncstream>
#include <cassert>

#include <array>
#include <vector>

#include <atomic>
#include <thread>
#include <chrono>

#include "Testing.hpp"
#include "DateTimeUtilities.hpp"
#include "PerfUtilities.hpp"

namespace
{
    using DateTimeUtilities::getCurrentTime;

#define LOG  std::osyncstream { std::cout } << '[' << getCurrentTime() << "] "
}

namespace
{
    constexpr uint32_t fast_modulo(const uint32_t n, const uint32_t d) noexcept {
        return n & (d - 1);
    }

    constexpr bool is_pow_of_2(const int value) noexcept {
        return (value && !(value & (value - 1)));
    }
};

namespace ring_buffers::impl_1
{
    template <typename T, std::size_t N>
    class RingBuffer
    {
        using size_type = size_t;

        std::array<T, N> buffer;

        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> head { 0 };
        alignas(std::hardware_destructive_interference_size) std::size_t headCached{ 0 };
        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> tail { 0 };
        alignas(std::hardware_destructive_interference_size) std::size_t tailCached{ 0 };

    public:

        bool push(const T& value) noexcept
        {
            const size_type headLocal = head.load(std::memory_order_relaxed);
            size_type next_head = headLocal + 1;
            if (next_head == buffer.size()) [[unlikely]] {
                next_head = 0;
            }
            if (next_head == tailCached) [[unlikely]] {
                tailCached = tail.load(std::memory_order_acquire);
                if (next_head == tailCached) {
                    return false;
                }
            }
            buffer[headLocal] = value;
            head.store(next_head, std::memory_order_release);
            return true;
        }

        bool pop(T& value) noexcept
        {
            const size_type tailLocal = tail.load(std::memory_order_relaxed);
            if (tailLocal == headCached) [[unlikely]] {
                headCached = head.load(std::memory_order_acquire);
                if (tailLocal == headCached) {
                    return false;
                }
            }
            value = buffer[tailLocal];
            size_type next_tail = tailLocal + 1;
            if (next_tail == buffer.size()) [[unlikely]] {
                next_tail = 0;
            }
            tail.store(next_tail, std::memory_order_release);
            return true;
        }
    };
}

namespace ring_buffers::impl_2
{
    template<typename T>
    concept Queueable = std::default_initializable<T> && std::move_constructible<T>;

    template<Queueable Ty, std::size_t Capacity>
    class RingBuffer
    {
        using size_type  = size_t;
        using value_type = Ty;
        using collection_type = std::vector<value_type>;

        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");
        static_assert(is_pow_of_2(Capacity), "ERROR: Capacity must be a power of 2");

        std::array<value_type, Capacity> m_buffer;

        alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> readIndex { 0 };
        alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> writeIndex { 0 };

    public:

        RingBuffer() = default;

        RingBuffer(const RingBuffer&) = delete;
        RingBuffer& operator=(const RingBuffer&) = delete;
        RingBuffer(RingBuffer&&) = delete;
        RingBuffer& operator=(RingBuffer&&) = delete;

        template<typename U>
            requires std::is_convertible_v<value_type, std::decay_t<U>>
        bool push(U&& element)
        {
            const size_type idxWrite = writeIndex.load(std::memory_order_relaxed);
            const size_type idxWriteNext = (idxWrite + 1) & (Capacity - 1);

            if (idxWriteNext != readIndex.load(std::memory_order_acquire))
            {
                m_buffer[idxWrite] = std::forward<U>(element);
                writeIndex.store(idxWriteNext, std::memory_order_release);
                return true;
            }
            return false;
        }

        bool pop(value_type& value) noexcept
        {
            const size_type idxRead = readIndex.load(std::memory_order_relaxed);
            if (idxRead == writeIndex.load(std::memory_order_acquire)) {
                return false;
            }

            value = std::move(m_buffer[idxRead]);
            readIndex.store((idxRead + 1 ) & (Capacity - 1), std::memory_order_release);
            return true;
        }
    };
}


/** My impl (from HFT project) **/
namespace ring_buffers::impl_3
{
    template<typename Ty, uint32_t Capacity>
    struct RingBuffer
    {
        using size_type  = uint32_t;
        using value_type = Ty;
        using collection_type = std::vector<value_type>;

        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");
        static_assert(is_pow_of_2(Capacity), "ERROR: Capacity must be a power of 2");

        RingBuffer(): buffer(Capacity) {
        }

        // TODO: Support for copy ??? --> bool push(value_type& item) --> std::move(item);
        [[nodiscard]]
        bool push(const value_type& item)
        {
            const size_type headCurrent = head.load(std::memory_order::relaxed);
            const size_type headNext = fast_modulo(headCurrent + 1, Capacity);
            if (headNext == tail.load(std::memory_order::acquire)) {
                return false;
            }

            // buffer[headCurrent] = std::move(item);
            buffer[headCurrent] = item;
            head.store(headNext, std::memory_order::release);

            return true;
        }

        [[nodiscard]]
        bool pop(value_type& item)
        {
            const size_type tailCurrent = tail.load(std::memory_order::relaxed);
            if (tailCurrent == head.load(std::memory_order::acquire)) {
                return false;
            }

            item = std::move(buffer[tailCurrent]);
            tail.store(fast_modulo(tailCurrent + 1, Capacity), std::memory_order::release);

            return true;
        }

        [[nodiscard]]
        size_type size() const noexcept {
            return head.load(std::memory_order::relaxed) - tail.load(std::memory_order::relaxed);
        }

        [[nodiscard]]
        size_type empty() const noexcept
        {
            // TODO: can 'acquire' be replaced with 'relaxed' ?
            return head.load(std::memory_order::acquire) == tail.load(std::memory_order::acquire);
        }

        [[nodiscard]]
        size_type full() const noexcept
        {
            return size() == Capacity;
        }

    private:

        std::vector<value_type> buffer;

        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> tail {0};
        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> head {0};
    };
}

/** My impl (from HFT project) **/
namespace ring_buffers::impl_3_size
{
    template<typename Ty>
    struct RingBuffer
    {
        using size_type  = uint32_t;
        using value_type = Ty;
        using collection_type = std::vector<value_type>;

        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        explicit RingBuffer(const size_type capacity):
            capacity { capacity }, buffer(capacity)
        {
            assert(is_pow_of_2(capacity) == true && "Capacity must be a power of 2");
        }

        bool push(const value_type& value)
        {
            const size_type headLocal = head.load(std::memory_order::relaxed);
            const size_type headNext = fast_modulo(headLocal + 1, capacity);

            if (headNext == tail.load(std::memory_order::acquire)) {
                return false;
            }

            buffer[headLocal] = value;
            head.store(headNext, std::memory_order::release);

            return true;
        }

        [[nodiscard]]
        bool pop(value_type& item)
        {
            const size_type tailLocal = tail.load(std::memory_order::relaxed);
            if (tailLocal == head.load(std::memory_order::acquire)) {
                return false;
            }

            item = std::move(buffer[tailLocal]);
            tail.store(fast_modulo(tailLocal + 1, capacity), std::memory_order::release);
            return true;
        }

        [[nodiscard]]
        size_type size() const noexcept {
            return head.load(std::memory_order::relaxed) - tail.load(std::memory_order::relaxed);
        }

        [[nodiscard]]
        size_type empty() const noexcept
        {
            // TODO: can 'acquire' be replaced with 'relaxed' ?
            return head.load(std::memory_order::acquire) == tail.load(std::memory_order::acquire);
        }

        [[nodiscard]]
        size_type full() const noexcept
        {
            return size() == capacity;
        }

    private:

        size_type capacity { 0 };
        std::vector<value_type> buffer;

        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> tail {0};
        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> head {0};
    };
}

namespace ring_buffers::tests
{
    template<typename Ty>
    concept IRingBuffer = requires(Ty ring_buffer, int64_t value)
    {
        { ring_buffer.push(value) } -> std::same_as<bool>;
        { ring_buffer.pop(value) } -> std::same_as<bool>;
    };

    void test_basic_push_pop(IRingBuffer auto& ringBuffer)
    {
        int64_t value = 0;
        testing::AssertTrue(ringBuffer.push(1));
        testing::AssertTrue(ringBuffer.pop(value));
        testing::AssertEqual(1L, value);
    }

    void test_empty_pop(IRingBuffer auto& ringBuffer)
    {
        int64_t value = 0;
        testing::AssertFalse(ringBuffer.pop(value));
    }

    void test_full_push(IRingBuffer auto& ringBuffer)
    {
        testing::AssertTrue(ringBuffer.push(1));
        testing::AssertTrue(ringBuffer.push(2));
        testing::AssertFalse(ringBuffer.push(3));
    }

    void test_fifo_order(IRingBuffer auto& ringBuffer)
    {
        ringBuffer.push(1);
        ringBuffer.push(2);
        ringBuffer.push(3);

        int64_t item = 0;

        ringBuffer.pop(item);
        testing::AssertEqual(1L, item);

        ringBuffer.pop(item);
        testing::AssertEqual(2L, item);

        ringBuffer.pop(item);
        testing::AssertEqual(3L, item);
    }

    void test_wrap_around(IRingBuffer auto& ringBuffer)
    {
        int64_t item = 0;

        testing::AssertTrue(ringBuffer.push(1));
        testing::AssertTrue(ringBuffer.push(2));

        testing::AssertTrue(ringBuffer.pop(item));
        testing::AssertEqual(1L, item);

        testing::AssertTrue(ringBuffer.push(3));
        testing::AssertTrue(ringBuffer.push(4));
        testing::AssertFalse(ringBuffer.push(5));

        testing::AssertTrue(ringBuffer.pop(item));
        testing::AssertEqual(2L, item);

        testing::AssertTrue(ringBuffer.pop(item));
        testing::AssertEqual(3L, item);
    }

    void test_alternating_push_pop(IRingBuffer auto& ringBuffer)
    {
        int64_t item = 0;

        testing::AssertTrue(ringBuffer.push(1));
        testing::AssertTrue(ringBuffer.pop(item));
        testing::AssertEqual(1L, item);

        testing::AssertTrue(ringBuffer.push(2));
        testing::AssertTrue(ringBuffer.pop(item));
        testing::AssertEqual(2L, item);
    }

    void test_large_sequence(IRingBuffer auto& ringBuffer)
    {
        for (int i = 0; i < 1000; ++i)
            testing::AssertTrue(ringBuffer.push(i));

        for (int64_t i = 0; i < 1000; ++i) {
            int64_t item = 0;
            testing::AssertTrue(ringBuffer.pop(item));
            testing::AssertEqual(i, item);
        }
    }

    void runAllUnitTests()
    {
        {
            impl_1::RingBuffer<int64_t, 4> ringBuffer;
            test_basic_push_pop(ringBuffer);
        }
        {
            impl_1::RingBuffer<int64_t, 4> ringBuffer;
            test_empty_pop(ringBuffer);
        }
        {
            impl_1::RingBuffer<int64_t, 2> ringBuffer;
            test_empty_pop(ringBuffer);
        }
        {
            impl_1::RingBuffer<int64_t, 4> ringBuffer;
            test_fifo_order(ringBuffer);
        }
        {
            impl_1::RingBuffer<int64_t, 4> ringBuffer;
            test_wrap_around(ringBuffer);
        }
        {
            impl_1::RingBuffer<int64_t, 2> ringBuffer;
            test_alternating_push_pop(ringBuffer);
        }
        {
            impl_1::RingBuffer<int64_t, 1024> ringBuffer;
            test_large_sequence(ringBuffer);
        }
        //---------------------------------------------------------
        {
            impl_2::RingBuffer<int64_t, 4> ringBuffer;
            test_basic_push_pop(ringBuffer);
        }
        {
            impl_2::RingBuffer<int64_t, 4> ringBuffer;
            test_empty_pop(ringBuffer);
        }
        {
            impl_2::RingBuffer<int64_t, 2> ringBuffer;
            test_empty_pop(ringBuffer);
        }
        {
            impl_2::RingBuffer<int64_t, 4> ringBuffer;
            test_fifo_order(ringBuffer);
        }
        {
            impl_2::RingBuffer<int64_t, 4> ringBuffer;
            test_wrap_around(ringBuffer);
        }
        {
            impl_2::RingBuffer<int64_t, 2> ringBuffer;
            test_alternating_push_pop(ringBuffer);
        }
        {
            impl_2::RingBuffer<int64_t, 1024> ringBuffer;
            test_large_sequence(ringBuffer);
        }
        //---------------------------------------------------------
        {
            impl_3::RingBuffer<int64_t, 4> ringBuffer;
            test_basic_push_pop(ringBuffer);
        }
        {
            impl_3::RingBuffer<int64_t, 4> ringBuffer;
            test_empty_pop(ringBuffer);
        }
        {
            impl_3::RingBuffer<int64_t, 2> ringBuffer;
            test_empty_pop(ringBuffer);
        }
        {
            impl_3::RingBuffer<int64_t, 4> ringBuffer;
            test_fifo_order(ringBuffer);
        }
        {
            impl_3::RingBuffer<int64_t, 4> ringBuffer;
            test_wrap_around(ringBuffer);
        }
        {
            impl_3::RingBuffer<int64_t, 2> ringBuffer;
            test_alternating_push_pop(ringBuffer);
        }
        {
            impl_3::RingBuffer<int64_t, 1024> ringBuffer;
            test_large_sequence(ringBuffer);
        }
        //---------------------------------------------------------
        {
            impl_3_size::RingBuffer<int64_t> ringBuffer { 4 };
            test_basic_push_pop(ringBuffer);
        }
        {
            impl_3_size::RingBuffer<int64_t> ringBuffer { 4 };
            test_empty_pop(ringBuffer);
        }
        {
            impl_3_size::RingBuffer<int64_t> ringBuffer { 2 };
            test_empty_pop(ringBuffer);
        }
        {
            impl_3_size::RingBuffer<int64_t> ringBuffer { 4 };
            test_fifo_order(ringBuffer);
        }
        {
            impl_3_size::RingBuffer<int64_t> ringBuffer { 4 };
            test_wrap_around(ringBuffer);
        }
        {
            impl_3_size::RingBuffer<int64_t> ringBuffer { 2 };
            test_alternating_push_pop(ringBuffer);
        }
        {
            impl_3_size::RingBuffer<int64_t> ringBuffer { 1024 };
            test_large_sequence(ringBuffer);
        }
    }
}

namespace ring_buffers::tests::multithreading
{
    void test_1(IRingBuffer auto& ringBuffer)
    {
        constexpr int64_t eventsCount {10'000'000}, initialValue {0};
        int64_t reads {0}, writes {0}, errors {0};

        // ScopedTimer timer {"benchmark"};
        auto consume = [&]
        {
            int previousValue = initialValue - 1;
            int64_t result {0};
            while (eventsCount > reads) {
                if (ringBuffer.pop(result)) {
                    if (previousValue + 1 != result) {
                        ++errors;
                    }
                    previousValue = result;
                    ++reads;
                }
            }
        };

        auto produce = [&]
        {
            for (int64_t idx = initialValue; idx <= eventsCount; ++idx){
                while (true) {
                    if (ringBuffer.push(idx)) {
                        ++writes;
                        break;
                    }
                }
            }
        };

        std::jthread consumer {consume}, producer {produce};
        consumer.join();
        producer.join();

        testing::AssertEqual(eventsCount, reads);
        testing::AssertEqual(eventsCount + 1, writes);
        testing::AssertTrue(0 == errors);
    }

    void test_2(IRingBuffer auto& ringBuffer)
    {
        constexpr int64_t eventsCount = 100000;
        std::atomic<bool> producer_done = false;
        const std::jthread producer([&]() {
            for (int i = 0; i < eventsCount; ) {
                if (ringBuffer.push(i)) {
                    ++i;
                }
            }
            producer_done = true;
        });

        const std::jthread consumer([&]() {
            int64_t recordsPoped = 0, value;
            while (!producer_done || recordsPoped < eventsCount) {
                if (ringBuffer.pop(value)) {
                    testing::AssertEqual(value, recordsPoped);
                    ++recordsPoped;
                }
            }

            testing::AssertEqual(eventsCount, recordsPoped);
        });
    }

    void runAllTests()
    {
        constexpr size_t capacity { 1024 };
        LOG << "Running tests ....\n";

        //--------------------------------------------
        {
            impl_1::RingBuffer<int64_t, capacity> ringBuffer;
            test_1(ringBuffer);
        }
        {
            impl_1::RingBuffer<int64_t, capacity> ringBuffer;
            test_2(ringBuffer);
        }
        //--------------------------------------------
        {
            impl_2::RingBuffer<int64_t, capacity> ringBuffer;
            test_1(ringBuffer);
        }
        {
            impl_2::RingBuffer<int64_t, capacity> ringBuffer;
            test_2(ringBuffer);
        }
        //--------------------------------------------
        {
            impl_3::RingBuffer<int64_t, capacity> ringBuffer;
            test_1(ringBuffer);
        }
        {
            impl_3::RingBuffer<int64_t, capacity> ringBuffer;
            test_2(ringBuffer);
        }
        //--------------------------------------------
        {
            impl_3_size::RingBuffer<int64_t> ringBuffer { capacity };
            test_1(ringBuffer);
        }
        {
            impl_3_size::RingBuffer<int64_t> ringBuffer { capacity };
            test_2(ringBuffer);
        }

        LOG << "All tests passed" << std::endl;
    }

}

namespace ring_buffers::tests::performance_tests
{
    void runTest(IRingBuffer auto& ringBuffer,
                 const int64_t eventsCount,
                 std::string_view name)
    {
        const PerfUtilities::ScopedTimer timer {name};
        auto consume = [&]
        {
            int64_t item {0}, itemsPopped {0};
            while (eventsCount >= itemsPopped) {
                if (ringBuffer.pop(item)) {
                    ++itemsPopped;
                }
            }
            // LOG << "itemsPopped: " << itemsPopped << std::endl;
        };

        auto produce = [&]
        {
            int64_t item { 0 }, itemsPushed { 0 };
            while (eventsCount >= itemsPushed) {
                if (ringBuffer.push(item)) {
                    ++itemsPushed;
                }
            }
            // LOG << "Produce " << itemsPushed << " items" << std::endl;
        };

        std::jthread consumer {consume}, producer {produce};
        consumer.join();
        producer.join();
    }

    void benchmark()
    {
        constexpr size_t capacity { 1024 }, eventsCount { 100'000'000 };

        {
            impl_1::RingBuffer<int64_t, capacity> ringBuffer;
            runTest(ringBuffer, eventsCount, "impl_1::RingBuffer");
        }
        {
            impl_2::RingBuffer<int64_t, capacity> ringBuffer;
            runTest(ringBuffer, eventsCount, "impl_2::RingBuffer");
        }
        {
            impl_3::RingBuffer<int64_t, capacity> ringBuffer;
            runTest(ringBuffer, eventsCount, "impl_3::RingBuffer");
        }
        {
            impl_3_size::RingBuffer<int64_t> ringBuffer { capacity };
            runTest(ringBuffer, eventsCount, "impl_3_size::RingBuffer");
        }

        // impl_1::RingBuffer:  0.6771 seconds.
        // impl_2::RingBuffer:  0.916864 seconds.
        // impl_3::RingBuffer:  1.14923 seconds.
        // impl_3_size::RingBuffer:  1.07055 seconds.
    }
}

void ring_buffers::TestAll()
{
    // tests::runAllUnitTests();
    // tests::multithreading::runAllTests();
    tests::performance_tests::benchmark();
}
