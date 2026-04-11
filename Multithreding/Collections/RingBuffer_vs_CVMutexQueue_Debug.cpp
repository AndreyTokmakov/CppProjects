/**============================================================================
Name        : RingBuffer_vs_CVMutexQueue_Debug.cpp
Created on  : 14.12.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "Collections.h"
#include "PerfUtilities.hpp"

#include <iostream>
#include <vector>
#include <deque>
#include <memory>
#include <array>
#include <atomic>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <syncstream>


namespace Collections::RingBuffer_vs_CVMutexQueue_Debug
{
    template<typename T,
            uint16_t Capacity = std::numeric_limits<uint16_t>::max()>
    struct RingBuffer final
    {
        using size_type  = uint16_t;
        using value_type = T;
        using collection_type = std::vector<value_type>;


        // alignas(std::hardware_destructive_interference_size)
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

        template<typename ... Types>
        void emplace([[maybe_unused]] Types&& ... params)
        {
            //new (&buffer[idxWriteCached]) value_type { std::forward<Types>(params)... };
            idxWriteCached = idxWrite.fetch_add(1, std::memory_order::release) + 1;
        }

        [[nodiscard]]
        bool try_read_next([[maybe_unused]] value_type& result)
        {
            if (idxRead == idxWrite.load(std::memory_order::acquire)) {
                return false;
            }

            //result = std::move(buffer[idxRead++]);
            return true;
        }
    };
}

namespace Collections::RingBuffer_vs_CVMutexQueue_Debug
{
    template<typename T>
    struct LFQueue final
    {
        using size_type = size_t;

        explicit LFQueue(size_type num_elems) : store(num_elems, T()) {
        }

        T* getNextToWriteTo() noexcept
        {
            return &store[next_write_index_];
        }

        void updateWriteIndex() noexcept
        {
            next_write_index_ = (next_write_index_ + 1) % store.size();
            num_elements_++;
        }

        T* getNextToRead()  noexcept
        {
            return (size() ? &store[next_read_index_] : nullptr);
        }

        void updateReadIndex() noexcept
        {
            next_read_index_ = (next_read_index_ + 1) % store.size(); // wrap around at the end of container size.
            num_elements_--;
        }

        [[nodiscard]]
        size_type size() const noexcept {
            return num_elements_.load();
        }

        /// Deleted default, copy & move constructors and assignment-operators.
        LFQueue() = delete;

        LFQueue(const LFQueue &) = delete;
        LFQueue(const LFQueue &&) = delete;

        LFQueue &operator=(const LFQueue &) = delete;
        LFQueue &operator=(const LFQueue &&) = delete;

    private:
        /// Underlying container of data accessed in FIFO order.
        std::vector<T> store;

        /// Atomic trackers for next index to write new data to and read new data from.
        std::atomic<size_type> next_write_index_ = {0};
        std::atomic<size_type> next_read_index_ = {0};
        std::atomic<size_type> num_elements_ = {0};
    };
}

namespace Collections::RingBuffer_vs_CVMutexQueue_Debug
{
    template<typename T>
    class BlockingQueue
    {
        using value_type = T;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        static constexpr std::chrono::duration<int64_t, std::ratio<1, 1000>> waitTimeout { std::chrono::seconds(5U) };

        mutable std::mutex mutex;
        std::deque<value_type> data_queue;
        std::condition_variable updated;

    public:

        void push(value_type&& new_value)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                data_queue.push_back(std::move(new_value));
            }
            updated.notify_one();
        }

        bool pop(value_type& value)
        {
            std::unique_lock<std::mutex> lock(mutex);
            bool ok = updated.wait_for(lock, waitTimeout, [this] {
                return false == data_queue.empty();
            });
            if (!ok)
                return false;
            value = std::move(data_queue.front());
            data_queue.pop_front();
            return true;
        }

        [[nodiscard]]
        bool empty() const noexcept
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.empty();
        }

        [[nodiscard]]
        size_t size() const noexcept
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.size();
        }
    };
}

namespace Collections::RingBuffer_vs_CVMutexQueue_Debug::Tests
{
    constexpr int32_t evtCount = 10'000'000;

    template<typename T, size_t Size>
    struct Data
    {
        T value { 0 };
        std::array<char, Size> buffer {};
    };

    // using Type = int;
    using Type = Data<int, 4>;
    // using Type = Data<int, 128>;
    // using Type = Data<int, 1024>;
    // using Type = Data<int, 1024 * 4>;


    void simple_test_lf_queue()
    {
        LFQueue<Type> buffer(1024);
        Type* next_write = nullptr;
        for (int i = 0; i < 10; ++i) {
            next_write = buffer.getNextToWriteTo();
            next_write->value = i;
            buffer.updateWriteIndex();
        }

        Type* result;
        while (buffer.size()) {
            result = buffer.getNextToRead();
            std::cout << result->value << std::endl;
            buffer.updateReadIndex();
        }
    }

    void simple_test_ring_buffer()
    {
        RingBuffer<Type> buffer;
        for (int i = 0; i < 10; ++i) {
            buffer.emplace(i);
        }

        Type result;
        while (buffer.try_read_next(result)) {
            std::cout << result.value << std::endl;
        }
    }


    void multithreaded_buffer_test(bool warmUp = false)
    {
        RingBuffer<Type> buffer;
        const PerfUtilities::ScopedTimer timer { "multithreaded_buffer_test", warmUp};
        std::jthread producer ([&buffer] {
            for (int i = 0; i < evtCount; ++i) {
                // buffer.put(Type {i});
                buffer.emplace(i);
            }
        });

        std::jthread consumer ([&buffer] {
            int count { 0 };
            Type result;
            while (true) {
                count += buffer.try_read_next(result);
                if (count >= evtCount) {
                    break;
                }
            }
        });
    }

    void multithreaded_lf_queue_test(bool warmUp = false)
    {
        LFQueue<Type> buffer(256 * 256);
        const PerfUtilities::ScopedTimer timer { "multithreaded_lf_queue_test", warmUp};
        std::jthread producer ([&buffer] {
            Type* next_write = nullptr;
            for (int i = 0; i < evtCount; ++i) {
                next_write = buffer.getNextToWriteTo();
                next_write->value = i;
                buffer.updateWriteIndex();
            }
        });

        std::jthread consumer ([&buffer] {
            int count { 0 };
            [[maybe_unused]] Type* result;
            while (true) {
                if (buffer.size()) {
                    result = buffer.getNextToRead();
                    buffer.updateReadIndex();
                    ++count;
                    if (count >= evtCount) {
                        break;
                    }
                }
            }
        });
    }

    void multithreaded_queue_test(const bool warmUp = false)
    {
        BlockingQueue<Type> queue;

        const PerfUtilities::ScopedTimer timer { "multithreaded_queue_test", warmUp };
        std::jthread producer ([&queue] {
            for (int i = 0; i < evtCount; ++i) {
                queue.push(Type {i});
            }
        });

        std::jthread consumer ([&queue] {
            int count { 0 };
            Type result;
            while (true) {
                queue.pop(result);
                ++count;
                if (count >= evtCount) {
                    break;
                }
            }
        });
    }
}

void Collections::RingBuffer_vs_CVMutexQueue_Debug::TestAll()
{
    // Tests::simple_test_lf_queue();
    // Tests::simple_test_ring_buffer();

    // Tests::multithreaded_queue_test(true);
    Tests::multithreaded_buffer_test(true);
    Tests::multithreaded_lf_queue_test(true);

    // Tests::multithreaded_queue_test();
    Tests::multithreaded_buffer_test();
    Tests::multithreaded_lf_queue_test();
}