/**============================================================================
Name        : ThreadSafeQueue_CV_vs_RingBuffer.cpp
Created on  : 20.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ThreadSafeQueue_CV_vs_RingBuffer.cpp
============================================================================**/

#include "ThreadSafeQueue_CV_vs_RingBuffer.h"

#include <iostream>
#include <string_view>
#include <thread>
#include <condition_variable>
#include <vector>
#include <deque>
#include <future>
#include <syncstream>
#include <format>
#include <chrono>
#include <list>

#include "../Utilities/Utilities.h"

namespace ThreadSafeQueue_CV_vs_RingBuffer
{
    template<typename T>
    class Queue_Deque
    {
        using value_type = T;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        mutable std::mutex mutex;
        std::deque<value_type> data_queue;
        std::condition_variable updated;

    public:
        Queue_Deque() = default;

        void push(value_type new_value)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                data_queue.push_back(std::move(new_value));
            }
            updated.notify_one();
        }

        template <typename... Args>
        void emplace(Args&& ... args)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                data_queue.emplace_back(std::forward<Args>(args)...);
            }
            updated.notify_one();
        }

        void wait_and_pop(value_type& value)
        {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == data_queue.empty();
            });
            value = std::move(data_queue.front());
            data_queue.pop_front();
        }

        bool wait_for_and_pop(value_type& value)
        {
            std::unique_lock<std::mutex> lock(mutex);
            bool ok = updated.wait_for(lock, std::chrono::seconds (5u), [this] {
                return false == data_queue.empty();
            });
            if (!ok)
                return false;
            value = std::move(data_queue.front());
            data_queue.pop_front();
            return true;
        }

        value_type&& wait_and_pop()
        {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == data_queue.empty();
            });
            auto&& entry = data_queue.front();
            data_queue.pop_front();
            return std::move(entry);
        }

        bool try_pop(value_type& value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (data_queue.empty())
                return false;
            value = std::move(data_queue.front());
            data_queue.pop_front();
            return true;
        }

        bool empty() const noexcept
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.empty();
        }

        size_t size() const noexcept
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.size();
        }
    };

    template<typename T>
    class Queue_List_Atomic
    {
        using value_type = T;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        mutable std::mutex mutex;
        std::deque<value_type> storage;
        std::atomic<uint32_t> counterAdded { 0 };
        uint32_t counterReads { 0 };

    public:
        Queue_List_Atomic() = default;

        void push(value_type new_value)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                storage.push_back(std::move(new_value));
            }
            counterAdded.fetch_add(1, std::memory_order::relaxed);
            counterAdded.notify_one();
        }

        void wait_and_pop(value_type& value)
        {
            counterAdded.wait(counterReads, std::memory_order::relaxed);
            {
                std::lock_guard<std::mutex> lock(mutex);
                value = std::move(storage.front());
                storage.pop_front();
            }
            ++counterReads;
        }
    };


    template<typename T>
    struct RingBufferQueue
    {
        using size_type = size_t;
        using value_type = T;
        using collection_type = std::vector<value_type>;

        size_type idxRead { 0 };
        size_type idxWrite { 0 };
        bool overlapped { false };
        collection_type buffer {};
        mutable std::mutex mutex;
        std::condition_variable updated;

        explicit RingBufferQueue(size_t size): idxRead { 0 }, idxWrite { 0 }, overlapped { false } {
            buffer.resize(size);
        }

        void put(const value_type& value)
        {
            {
                std::lock_guard<std::mutex> lock { mutex };
                if (idxWrite == buffer.size()) {
                    idxWrite = 0;
                    overlapped = true;
                }
                if (overlapped && idxWrite == idxRead) {
                    ++idxRead;
                }
                buffer[idxWrite++] = value;
            }

            // std::osyncstream { std::cout } << "put (" << value << ") idxWrite: "
            //          << idxWrite << ", idxRead: " << idxRead << ", overlapped: " << std::boolalpha << overlapped << std::endl;
            updated.notify_one();
        }

        bool get(value_type& value)
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (!overlapped && idxWrite == idxRead) {
                //std::osyncstream { std::cout } << "Wait" << std::endl;
                updated.wait(lock, [this] {
                    return not (false == overlapped && idxWrite == idxRead);
                });
                // std::osyncstream { std::cout } << "Done" << std::endl;
            }
            else {
                // std::osyncstream { std::cout } << "NO Wait" << std::endl;
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


namespace ThreadSafeQueue_CV_vs_RingBuffer::Dequeue_Tests
{
    void benchmark()
    {
        Queue_Deque<int> dQueue;

        auto produce = [&dQueue](int32_t count) {
            for (int32_t n = 0; n < count; ++n)
                dQueue.push(n);
        };

        auto consume = [&dQueue](int32_t count) {
            int result { 0 };
            while (count > 0) {
                if (dQueue.wait_for_and_pop(result)) {
                    --count;
                    //std::cout << result << " -> " << count << std::endl;
                }
            }
        };

        int32_t eventMax { 1'000'000 };
        Utilities::ScopedTimer timer { "Dequeue_Tests" };
        {
            std::jthread producer{produce, eventMax };
            std::jthread consumer{consume, eventMax };
        }
    }
}

namespace ThreadSafeQueue_CV_vs_RingBuffer::RFQueue_Tests
{
    void benchmark()
    {
        RingBufferQueue<int> rfQueue(1'000'000);

        auto produce = [&rfQueue](int32_t count) {
            for (int32_t n = 0; n < count; ++n) {
                rfQueue.put(n);
                // std::this_thread::sleep_for(std::chrono::milliseconds(1U));
            }
        };

        auto consume = [&rfQueue](int32_t count) {
            int result { 0 };
            while (count > 0) {
                if (rfQueue.get(result)) {
                    --count;
                    // std::osyncstream { std::cout } << result << " -> " << count << std::endl;
                }
            }
        };

        int32_t eventMax { 1'000'000 };
        Utilities::ScopedTimer timer { "RFQueue_Tests" };
        {
            std::jthread consumer{consume, eventMax };
            std::jthread producer{produce, eventMax };
        }
    }
}

namespace ThreadSafeQueue_CV_vs_RingBuffer::Queue_List_Atomic_Tests
{
    void benchmark()
    {
        Queue_List_Atomic<int> atomicListQueue;

        auto produce = [&atomicListQueue](int32_t count) {
            for (int32_t n = 0; n < count; ++n) {
                atomicListQueue.push(n);
                //std::this_thread::sleep_for(std::chrono::milliseconds(1U));
            }
        };

        auto consume = [&atomicListQueue](int32_t count)
        {
            int result { 0 };
            while (count > 0)
            {
                atomicListQueue.wait_and_pop(result);
                --count;
                //std::osyncstream { std::cout } << result << " -> " << count << std::endl;
            }
        };


        int32_t eventMax { 1'000'000 };
        Utilities::ScopedTimer timer { "Queue_List_Atomic" };
        {
            std::jthread consumer { consume, eventMax };
            std::jthread producer { produce, eventMax };
        }
    }
}

void ThreadSafeQueue_CV_vs_RingBuffer::TestAll()
{
    // RFQueue_Tests::benchmark();

    Dequeue_Tests::benchmark();
    Queue_List_Atomic_Tests::benchmark();
}