/**============================================================================
Name        : Queue.cpp
Created on  : 08.06.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Queue.cpp
============================================================================**/

#include "Queue.h"

#include <iostream>
#include <algorithm>
#include <numeric>

#include <vector>
#include <array>
#include <deque>


#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include <syncstream>

namespace QueueCV
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

        /*
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
        */
    };
}



namespace QueueAtomic
{
    template<typename T>
    class Queue
    {
        using value_type = T;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        mutable std::mutex mutex;
        std::deque<value_type> data_queue;
        std::atomic<int32_t> size {false};

    public:
        Queue() = default;

        void wait_and_pop(value_type& value)
        {
            size.wait(0, std::memory_order_relaxed);
            const int32_t sizeUpdated = size.fetch_sub(1, std::memory_order_relaxed);


            /*
            while (true)
            {
                //std::osyncstream {std::cout} << "wait_and_pop: " << std::this_thread::get_id() << " entered\n";
                size.wait(0, std::memory_order_relaxed);


                const int32_t sizeUpdated = size.fetch_sub(1, std::memory_order_relaxed);
                if (sizeUpdated <= 0) {
                    size.store(0, std::memory_order_relaxed);
                    // std::osyncstream {std::cout} << "wait_and_pop: " << std::this_thread::get_id() << " continue: " << sizeUpdated << "\n";
                    continue;
                } else {
                    // std::osyncstream {std::cout} << "wait_and_pop: " << std::this_thread::get_id() << " PROCESSING: " << sizeUpdated << "\n";
                    return;
                }
            }*/
        }

        void push(value_type new_value)
        {
            size.fetch_add(1, std::memory_order_relaxed);
            // size.notify_one();
            size.notify_all();
        }

        /*
        void push(value_type new_value)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                data_queue.push_back(std::move(new_value));
            }
            updated.test_and_set(std::memory_order_release);
            updated.notify_one();
        }

        template <typename... Args>
        void emplace(Args&& ... args)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                data_queue.emplace_back(std::forward<Args>(args)...);
            }
            updated.test_and_set();
            updated.notify_one();
        }

        template <typename... Args>
        void AddRange(Args&&... args) {
            std::lock_guard<std::mtx> lock(mtx);
            (elements.push_back(args), ...);
        }

        void wait_and_pop(value_type& value)
        {
            updated.wait(false, std::memory_order_acquire);
            updated.clear();

            {
                std::lock_guard<std::mutex> lock(mutex);
                value = std::move(data_queue.front());
                data_queue.pop_front();
            }
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
            updated.wait(false, std::memory_order_acquire);
            updated.clear();

            {
                std::lock_guard<std::mutex> lock(mutex);
                auto&& entry = data_queue.front();
                data_queue.pop_front();
                return std::move(entry);
            }
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
        */
    };
}

void Queue::TestAll()
{
    QueueAtomic::Queue<int> queue {};
    // QueueCV::Queue<int> queue {};

    std::once_flag onceInfoFlag;
    std::atomic<int32_t> readsCount {0};
    constexpr int32_t maxCount = 10000000, consumersCount = 100;
    std::array<int32_t, consumersCount> counters {};

    auto printInfo = [&] ()
    {
        //std::osyncstream {std::cout}  << "Consumer [" << std::this_thread::get_id() << "] finished | Counter = " << counter << "\n";
        std::osyncstream {std::cout} << readsCount.load(std::memory_order_relaxed) << std::endl;
        const int32_t total = std::accumulate(counters.begin(), counters.end(), 0);
        std::osyncstream {std::cout} << "Total: " << total << std::endl;
        for (int32_t id = 0; id < consumersCount; ++id)
            std::osyncstream {std::cout} << counters[id] << " ";
        std::osyncstream {std::cout} << std::endl;
    };

    auto producer = [&queue]() {
        for (int32_t idx = 0; idx < maxCount; ++idx) {
            // std::this_thread::sleep_for(std::chrono::microseconds (1));
            queue.push(1);
        }
    };

    auto consumer = [&](const int32_t jobId) {
        int value {};
        while (true) {
            queue.wait_and_pop(value);
            const int32_t counter = readsCount.fetch_add(1, std::memory_order_relaxed);

            if (counter >= maxCount- 1)
            {
                std::call_once(onceInfoFlag, printInfo);
                return;
            }
            ++counters[jobId];
            // std::osyncstream {std::cout}  << "Consumer [" << std::this_thread::get_id() << "] | Counter = " << counter << "\n";
        }
    };

    std::vector<std::jthread> pool;
    pool.emplace_back(producer);
    for (int32_t id = 0; id < consumersCount; ++id) {
        pool.emplace_back(consumer, id);
    }
    pool.clear();
}