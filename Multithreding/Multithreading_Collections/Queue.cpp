/**============================================================================
Name        : Queue.cpp
Created on  : 12.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Queue.cpp
============================================================================**/

#include "Queue.h"

#include <iostream>
#include <string_view>
#include <thread>
#include <condition_variable>
#include <vector>
#include <deque>

namespace QueueCV
{
    template<typename T>
    class Queue
    {
        mutable std::mutex mutex;
        std::deque<T> data_queue;
        std::condition_variable updated;

    public:
        Queue() = default;

        void push(T new_value) {
            std::lock_guard<std::mutex> lock(mutex);
            data_queue.push(std::move(new_value));
            updated.notify_one();
        }

        template <typename... Args>
        void emplace(Args&& ... args) {
            std::lock_guard<std::mutex> lock(mutex);
            data_queue.emplace_back(std::forward<Args>(args)...);
            updated.notify_one();
        }

        /*
        template <typename... Args>
        void AddRange(Args&&... args) {
            std::lock_guard<std::mtx> lock(mtx);
            (elements.push_back(args), ...);
        }
        */

        void wait_and_pop(T& value) {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == data_queue.empty();
            });
            value = std::move(data_queue.front());
            data_queue.pop_front();
        }


        template<class _Rep, class _Period>
        bool wait_for_and_pop(T& value, const std::chrono::duration<_Rep, _Period>& _Rel_time) {
            std::unique_lock<std::mutex> lock(mutex);
            bool ok = updated.wait_for(lock, _Rel_time, [this] {
                return false == data_queue.empty();
            });
            if (false == ok)
                return false;
            value = std::move(data_queue.front());
            data_queue.pop_front();
            return true;
        }

        T&& wait_and_pop() {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == data_queue.empty();
            });
            auto&& entry = data_queue.front();
            data_queue.pop_front();
            return std::move(entry);
        }

        bool try_pop(T& value) {
            std::lock_guard<std::mutex> lock(mutex);
            if (data_queue.empty())
                return false;
            value = std::move(data_queue.front());
            data_queue.pop_front();
            return true;
        }

        std::shared_ptr<T> try_pop()
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (data_queue.empty())
                return std::shared_ptr<T>();
            std::shared_ptr<T> result =
                    std::make_shared<T>(std::move(data_queue.front()));
            data_queue.pop_front();
            return result;
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.empty();
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.size();
        }
    };
}

void Queue::TestAll()
{

};