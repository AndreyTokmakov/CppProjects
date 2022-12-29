//============================================================================
// Name        : ThreadPools.cpp
// Created on  : 28.10.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ThreadPools src class
//============================================================================

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
#include "ThreadPools.h"
#include "../Integer/Integer.h"
#include "../ThreadHelperUtilities/ThreadHelperUtilities.h"

namespace ThreadPools::Utilities {

    template<typename T>
    class ThreadsafeQueue {
    private:
        mutable std::mutex mutex;
        std::deque<T> qeque;
        std::condition_variable updated;

    public:
        ThreadsafeQueue() {
        }

        void push(T&& new_value) {
            {
                std::lock_guard<std::mutex> lock(mutex);
                qeque.push_back(std::move(new_value));
            }
            updated.notify_one();
        }

        template <typename... Args>
        void emplace(Args&& ... args) {
            {
                std::lock_guard<std::mutex> lock(mutex);
                qeque.emplace_back(std::forward<Args>(args)...);
            }
            updated.notify_one();
        }

        /*template <typename... Args>
        void AddRange(Args&&... args) {
            std::lock_guard<std::mtx> lock(mtx);
            (elements.push_back(args), ...);
        }*/

        void wait_and_pop(T& value) {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == qeque.empty();
            });
            value = std::move(qeque.front());
            qeque.pop_front();
        }

        template<class _Rep, class _Period>
        bool wait_for_and_pop(T& value, const std::chrono::duration<_Rep, _Period>& _Rel_time) {
            std::unique_lock<std::mutex> lock(mutex);
            bool ok = updated.wait_for(lock, _Rel_time, [this] {
                return false == qeque.empty();
            });
            if (false == ok)
                return false;
            value = std::move(qeque.front());
            qeque.pop_front();
            return true;
        }

        T&& wait_and_pop() {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == qeque.empty();
            });
            auto&& entry = qeque.front();
            qeque.pop_front();
            return std::move(entry);
        }

        bool try_pop(T& value) {
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

        bool empty() const {
            std::lock_guard<std::mutex> lock(mutex);
            return qeque.empty();
        }

        bool size() const {
            std::lock_guard<std::mutex> lock(mutex);
            return qeque.size();
        }
    };
};

namespace ThreadPools::Pool1 {

    template<typename Task = std::function<void()>>
    class thread_pool {
    private:
        /** **/
        Utilities::ThreadsafeQueue<Task> work_queue;
        /** Run switch: **/
        std::atomic_bool run {true};
        /** **/
        std::vector<std::thread> threads;

        /** Maximum number of request handler workers: **/
        static inline const unsigned int thread_count { std::thread::hardware_concurrency() };

    private:
        void worker_thread() {
            Task task;
            while (true == run) {
                auto result = work_queue.wait_for_and_pop(task, std::chrono::milliseconds(2000));
                // THREAD_INFO << "result = " << std::boolalpha << result << std::endl;
                if (true == result) {
                    task();
                }
            }
        }

        void worker_thread_old() {
            Task task;
            while (true == run) {
                work_queue.wait_and_pop(task);
                task();
            }
        }

    public:
        thread_pool(){
            try {
                for(size_t i = 0; i < thread_count; ++i) {
                    threads.emplace_back(&thread_pool::worker_thread, this);
                }
            }
            catch (...) {
                run = false;
                throw;
            }
        }

        ~thread_pool() {
            // run = false;
            /** Join threads: **/
            for (auto& T : threads)
                T.join();
        }

        template<typename FunctionType>
        void submit(FunctionType f) {
            work_queue.push(Task(f));
        }
    };


    //----------------------------------------------------------------------------------//

    void RunTest() {
        auto job1 = []() {
            THREAD_INFO << "Starting job" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            THREAD_INFO << "Job done" << std::endl;
        };

        auto job2 = []() {
            THREAD_INFO << "Starting job" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
            THREAD_INFO << "Job done" << std::endl;
        };

        thread_pool pool;
        pool.submit(job1);
        pool.submit(job2);

        std::this_thread::sleep_for(std::chrono::seconds(30));

        pool.submit(job1);
    }
};

void ThreadPools::TEST_ALL() {
    Pool1::RunTest();
};