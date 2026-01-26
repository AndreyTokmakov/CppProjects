/**============================================================================
Name        : McMp_LockFree_Queue_vs_CV_Queue.cpp
Created on  : 26.01.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : McMp_LockFree_Queue_vs_CV_Queue.cpp
============================================================================**/

#include "McMp_LockFree_Queue_vs_CV_Queue.hpp"

#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <vector>
#include <deque>

#include "../Utilities/Utilities.h"

namespace mpmc_cv_queue
{
    template<typename T>
    class Queue
    {
        using value_type = T;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        mutable std::mutex mutex;
        std::deque<value_type> data_queue;
        std::condition_variable updated;

    public:
        Queue() = default;

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


        template<class _Rep, class _Period>
        bool wait_for_and_pop(value_type& value,
                              const std::chrono::duration<_Rep, _Period>& _Rel_time)
        {
            std::unique_lock<std::mutex> lock(mutex);
            bool ok = updated.wait_for(lock, _Rel_time, [this] {
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

        std::shared_ptr<value_type> try_pop()
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (data_queue.empty())
                return std::shared_ptr<value_type>();
            std::shared_ptr<value_type> result =
                    std::make_shared<value_type>(std::move(data_queue.front()));
            data_queue.pop_front();
            return result;
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
}

namespace mpmc_lockfree_queue
{

    template<typename T, size_t Capacity>
    class Queue
    {
        using value_type = T;
        using size_type  = size_t;

        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");
        static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of two");

        struct alignas(64) Cell
        {
            std::atomic<size_type> seq;
#if 0
            std::aligned_storage_t<sizeof(value_type), alignof(value_type)> storage{};
#else
            alignas(value_type) std::byte storage[sizeof(value_type)]{};
#endif
};

    public:

        Queue()
        {
            for (size_type i = 0; i < Capacity; ++i)
                buffer[i].seq.store(i, std::memory_order_relaxed);
        }

        ~Queue() {
            value_type tmp;
            while (try_pop(tmp)) {}
        }

        bool try_push(const value_type& value) {
            return emplace(value);
        }

        bool try_push(value_type&& value) {
            return emplace(std::move(value));
        }

        bool try_pop(value_type& out)
        {
            Cell* cell;
            size_type pos = dequeuePos.load(std::memory_order_relaxed);

            while (true)
            {
                cell = &buffer[pos & mask_];
                const size_type seq = cell->seq.load(std::memory_order_acquire);
                const intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);

                if (diff == 0) {
                    if (dequeuePos.compare_exchange_weak(pos, pos + 1,std::memory_order_relaxed))
                        break;
                } else if (diff < 0) {
                    return false; // empty
                } else {
                    pos = dequeuePos.load(std::memory_order_relaxed);
                }
            }

            value_type* data = reinterpret_cast<value_type*>(&cell->storage);
            out = std::move(*data);
            data->~value_type();

            cell->seq.store(pos + Capacity, std::memory_order_release);
            return true;
        }

    private:

        template<typename U>
        bool emplace(U&& value)
        {
            Cell* cell;
            size_type pos = enqueuePos.load(std::memory_order_relaxed);

            while (true)
            {
                cell = &buffer[pos & mask_];
                const size_type seq = cell->seq.load(std::memory_order_acquire);
                const intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);

                if (diff == 0) {
                    if (enqueuePos.compare_exchange_weak(pos, pos + 1,std::memory_order_relaxed))
                        break;
                } else if (diff < 0) {
                    return false; // full
                } else {
                    pos = enqueuePos.load(std::memory_order_relaxed);
                }
            }

            new (&cell->storage) value_type(std::forward<U>(value));
            cell->seq.store(pos + 1, std::memory_order_release);
            return true;
        }

    private:
        static constexpr size_t mask_ = Capacity - 1;

        alignas(64) Cell buffer [Capacity];
        alignas(64) std::atomic<size_type> enqueuePos { 0 };
        alignas(64) std::atomic<size_type> dequeuePos { 0 };
    };
}



namespace benchmarks
{
    using ValueType = int;

    constexpr uint64_t eventsMax = 10'000'000;
    constexpr uint16_t producersCount = 2;
    constexpr uint16_t consumersCount = 8;

    void mpmc_cv_queue_test()
    {
        std::vector<std::jthread> workers;
        mpmc_cv_queue::Queue<ValueType> queue;

        std::atomic<uint64_t> eventsProduced {0 };
        std::atomic<uint64_t> eventsConsumed {0 };

        auto produce = [&] {
            while (eventsMax > eventsProduced.fetch_add(1, std::memory_order_relaxed)) {
                queue.push(1);
            }
            // std::osyncstream { std::cout } << getCurrentTime() << " Producer done\n";
        };

        auto consume = [&]
        {
            ValueType value;
            while (eventsMax > eventsConsumed.fetch_add(1, std::memory_order_relaxed)) {
                queue.wait_and_pop(value);
            }
            // std::osyncstream { std::cout } << getCurrentTime() << " Consumer done\n";
        };

        Utilities::ScopedTimer timer { "CV Queue" };
        for (uint16_t n = 0; n < producersCount; ++n) {
            workers.emplace_back(produce);
        }
        for (uint16_t n = 0; n < consumersCount; ++n) {
            workers.emplace_back(consume);
        }

        workers.clear();
        // std::osyncstream { std::cout } << getCurrentTime() << " | " << queue.size() << " events queued.\n";
    }

    void mpmc_lockfree_queue_test()
    {
        std::vector<std::jthread> workers;
        mpmc_lockfree_queue::Queue<ValueType, 1024> queue;

        std::atomic<uint64_t> eventsProduced {0 };
        std::atomic<uint64_t> eventsConsumed {0 };

        auto produce = [&] {
            while (eventsMax > eventsProduced.fetch_add(1, std::memory_order_relaxed)) {
                while (!queue.try_push(1)) {} // spin
            }
            // std::osyncstream { std::cout } << getCurrentTime() << " Producer done\n";
        };

        auto consume = [&]
        {
            uint64_t eventsConsumedLocal = 0;
            ValueType value;
            while (true) {
                if (queue.try_pop(value)){
                    eventsConsumedLocal = eventsConsumed.fetch_add(1, std::memory_order_relaxed);
                }
                else {
                    eventsConsumedLocal = eventsConsumed.load( std::memory_order_relaxed);
                }

                if (eventsConsumedLocal >= eventsMax) {
                    break;
                }
            }
            // std::osyncstream { std::cout } << getCurrentTime() << " Consumer done\n";
        };

        Utilities::ScopedTimer timer { "LockFree Queue" };
        for (uint16_t n = 0; n < producersCount; ++n) {
            workers.emplace_back(produce);
        }
        for (uint16_t n = 0; n < consumersCount; ++n) {
            workers.emplace_back(consume);
        }

        workers.clear();
        //std::osyncstream { std::cout } << getCurrentTime() << " Done events queued.\n";
    }
}

void McMp_LockFree_Queue_vs_CV_Queue::TestAll()
{
    benchmarks::mpmc_cv_queue_test();
    benchmarks::mpmc_lockfree_queue_test();

    // CV Queue      :  8.00447 seconds.
    // LockFree Queue:  0.955784 seconds.

}
