/**============================================================================
Name        : ThreadSafe_BoundedQueue.cpp
Created on  : 25.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ThreadSafe_BoundedQueue.cpp
============================================================================**/

#include "ThreadSafe_BoundedQueue.hpp"

#include <iostream>
#include <queue>

#include <mutex>
#include <condition_variable>
#include <syncstream>

#include "DateTimeUtilities.hpp"

namespace
{
    using DateTimeUtilities::getCurrentTime;

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "
}


namespace thread_safe_bounded_queue
{
    template<class T>
    class BoundedQueue
    {
        std::mutex m;
        std::condition_variable not_full, not_empty;
        std::queue<T> q;
        const size_t cap;
        bool stopped{false};

    public:
        explicit BoundedQueue(size_t c) : cap(c) {}

        bool push(T v) {
            std::unique_lock lk(m);
            not_full.wait(lk, [&]{ return q.size() < cap || stopped; });
            if (stopped) return false;
            q.push(std::move(v));
            not_empty.notify_one();
            return true;
        }

        bool pop(T& out) {
            std::unique_lock lk(m);
            not_empty.wait(lk, [&]{ return !q.empty() || stopped; });
            if (q.empty()) return false;
            out = std::move(q.front());
            q.pop();
            not_full.notify_one();
            return true;
        }

        void stop() {
            std::lock_guard lk(m);
            stopped = true;
            not_full.notify_all();
            not_empty.notify_all();
        }
    };


}

void thread_safe_bounded_queue::TestAll()
{
    LOG << "Test" << std::endl;
}