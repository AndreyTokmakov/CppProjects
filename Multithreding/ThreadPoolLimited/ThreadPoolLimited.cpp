/**============================================================================
Name        : ThreadPoolLimited.cpp
Created on  : 05.02.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ThreadPool with limited queue capacity
============================================================================**/

#include "ThreadPoolLimited.h"

#include <iostream>
#include <syncstream>
#include <functional>

#include <thread>
#include <future>
#include <condition_variable>

#include <vector>
#include <deque>

namespace ThreadPoolLimited
{
    template<typename Task = std::function<void()>>
    struct thread_pool
    {
        // TODO: Refactor?? use template like std::func
        using TaskType = Task;

        mutable std::mutex mutex;

        std::deque<TaskType> queue;
        // std::list<TaskType> queue;

        std::condition_variable updated;

        std::atomic_bool run { true };

        std::vector<std::jthread> workers {};

        /** Maximum number of request handler workers: **/
        // static inline const size_t THREADS_COUNT { std::thread::hardware_concurrency() };
        static inline const size_t THREADS_COUNT { 3 };

        /** Maximum number of request handler workers: **/
        static inline const size_t MAX_CAPACITY { 1 };

        static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> TIMEOUT =
                std::chrono::milliseconds(2000);

        static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> POLL_TIMEOUT =
                std::chrono::milliseconds(250);

    private:

        template<class Rep, class Period>
        bool wait_for_and_pop(TaskType &task,
                              const std::chrono::duration<Rep, Period> &timeout) noexcept {
            std::unique_lock<std::mutex> lock(mutex);
            if (!updated.wait_for(lock, timeout, [this] { return !queue.empty(); }))
                return false;
            task = std::move(queue.front());
            queue.pop_front();
            lock.unlock();
            updated.notify_all();
            return true;
        }

    private:

        void worker_thread()
        {
            TaskType task;
            while (run) {
                if (auto result = wait_for_and_pop(task, TIMEOUT); result) {
                    task();
                }
            }
        }

    public:
        thread_pool()
        {
            try {
                for (size_t i = 0; i < THREADS_COUNT; ++i) {
                    workers.emplace_back(&thread_pool::worker_thread, this);
                }
            } catch (...) {
                run = false;
                throw;
            }
        }

        ~thread_pool() {
            run = false;
        }

        [[nodiscard("Its not for free")]]
        bool empty() const noexcept {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.empty();
        }

        [[nodiscard("Its not for free")]]
        size_t size() const noexcept {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.size();
        }

        void submit(TaskType &&new_value) noexcept
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (MAX_CAPACITY >= queue.size()) {
                while (!updated.wait_for(lock, POLL_TIMEOUT, [this] { return MAX_CAPACITY > queue.size(); })) { /** **/ }
            }
            queue.push_back(std::move(new_value));
            lock.unlock();
            updated.notify_one();
        }

        // TODO: Make it work!
        template<typename... Args>
        void emplace(Args &&... args) noexcept
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                queue.emplace_back(std::forward<Args>(args)...);
            }
            updated.notify_one();
        }
    };
}

void ThreadPoolLimited::TestAll()
{
    thread_pool pool;

    auto task = []() {
        std::osyncstream(std::cout) << "Starting job\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::osyncstream(std::cout) << "Job done\n";
    };

    std::vector<std::future<void>> workers;
    for (int i = 0; i < 25; ++i) {
        workers.emplace_back(std::async([&] { pool.submit(task); } ));
    }
    std::for_each(workers.cbegin(), workers.cend(), [](const auto &task) {
        task.wait();
    });
};

