//============================================================================
// Name        : ThreadPoolTwo.h
// Created on  : 20.03.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ThreadPoolTwo
//============================================================================

#include "ThreadPoolTwo.h"

#include <iostream>
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <stdexcept>
#include <algorithm>
#include <deque>
#include <chrono>
#include <queue>
#include <cassert>
#include <sstream>
#include <iomanip>

namespace ThreadPoolTwo
{
    /*
    template<typename T>
    class ThreadsafeQueue {
    private:
        mutable std::mutex mutex;
        std::deque<T> qeque;
        std::condition_variable updated;

    public:
        ThreadsafeQueue() = default;

        void wait_and_pop(T &value) noexcept {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == qeque.empty();
            });
            value = std::move(qeque.front());
            qeque.pop_front();
        }

        template<class _Rep, class _Period>
        bool wait_for_and_pop(T &value, const std::chrono::duration<_Rep, _Period> &_Rel_time) noexcept {
            std::unique_lock<std::mutex> lock(mutex);
            bool ok = updated.wait_for(lock, _Rel_time, [this] {
                return false == qeque.empty();
            });
            if (!ok)
                return false;
            value = std::move(qeque.front());
            qeque.pop_front();
            return true;
        }

        T &&wait_and_pop() noexcept {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == qeque.empty();
            });
            auto &&entry = qeque.front();
            qeque.pop_front();
            return std::move(entry);
        }

        bool try_pop(T &value) noexcept {
            std::lock_guard<std::mutex> lock(mutex);
            if (qeque.empty())
                return false;
            value = std::move(qeque.front());
            qeque.pop_front();
            return true;
        }

        std::shared_ptr<T> try_pop() {
            std::lock_guard<std::mutex> lock(mutex);
            if (qeque.empty())
                return std::shared_ptr<T>();
            std::shared_ptr<T> result =
                    std::make_shared<T>(std::move(qeque.front()));
            qeque.pop_front();
            return result;
        }
    };
    */


    template<typename Task = std::function<void()>>
    class thread_pool {
    private:
        mutable std::mutex mutex;
        std::deque<Task>   queue;
        std::condition_variable updated;

        /** Run switch: **/
        std::atomic_bool run { true };

        /** **/
        std::vector<std::thread> threads {};

        /** Maximum number of request handler workers: **/
        static inline const size_t thread_count { std::thread::hardware_concurrency() };

        /** Maximum number of request handler workers: **/
        // static inline const size_t MAX_CAPACITY { std::thread::hardware_concurrency() * 2 };
        static inline constexpr size_t MAX_CAPACITY { 100 };

        static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> TIMEOUT =
                std::chrono::milliseconds(2000);

        static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> POLL_TIMEOUT =
                std::chrono::milliseconds(200);

    private:

        template<class Rep, class Period>
        bool wait_for_and_pop(Task &task,
                              const std::chrono::duration<Rep, Period> &timeout) noexcept {
            std::unique_lock<std::mutex> lock(mutex);
            if (!updated.wait_for(lock, timeout, [this] { return !queue.empty();}))
                return false;

            task = std::move(queue.front());
            queue.pop_front();
            return true;
        }

    private:

        void worker_thread()
        {
            Task task;
            while (run) {
                // THREAD_INFO << "result = " << std::boolalpha << result << std::endl;
                if (auto result = wait_for_and_pop(task, TIMEOUT); result) {
                    task();
                }
            }
        }

        /*
        void worker_thread_old() {
            Task task;
            while (run) {
                work_queue.wait_and_pop(task);
                task();
            }
        }*/

    public:
        thread_pool(){
            try {
                for (size_t i = 0; i < thread_count; ++i) {
                    threads.emplace_back(&thread_pool::worker_thread, this);
                }
            }
            catch (...) {
                run = false;
                throw;
            }
        }

        // TODO: Make it work!
        ~thread_pool() {
            run = false;
            /** Join threads: **/
            for (auto& T : threads)
                T.join();
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

        void submit(Task &&new_value) noexcept
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                queue.push_back(std::move(new_value));
            }
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


    struct synch_stream final : public std::stringstream {
    private:
        static inline std::mutex mtx;
        const static inline std::thread::id mainThreadId { std::this_thread::get_id() };

    private:
        static std::string getCurrentTime() noexcept {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
            return ss.str();
        }

    public:
        ~synch_stream() override {
            const auto currId = std::this_thread::get_id();

            // TODO: Prepare string before lock
            std::string info("[");
            info.append(getCurrentTime()).append("] Thread [");

            std::lock_guard<std::mutex> lock{ mtx };
            std::cout << info;
            if (mainThreadId == currId)
                std::cout << std::setiosflags(std::ios::left) << std::setw(9) << "Main";
            else
                std::cout << "Id: " << std::setiosflags(std::ios::left) << std::setw(5) << currId;

            std::cout << "] " << rdbuf();
            std::cout.flush();
        }
    };
}

void ThreadPoolTwo::TEST_ALL()
{
    thread_pool pool;

    for (int i = 0; i < 3; ++i) {
        pool.submit([]() {
            synch_stream() << "Starting job" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            synch_stream() << "Job done" << std::endl;
        });
    }

    // std::this_thread::sleep_for(std::chrono::seconds(30));

    // pool.submit(job1);
}
