//============================================================================
// Name        : ThreadPoolOne.h
// Created on  : 20.03.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ThreadPoolOne
//============================================================================

#include "ThreadPoolOne.h"

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

namespace ThreadPoolOne {

    class ThreadPool {
        using UniqueFunction = std::packaged_task<void()>;

        struct State
        {
            std::mutex mtx;
            std::queue<UniqueFunction> work_queue;
            bool aborting { false };
        };

        State state {};
        std::vector<std::thread> workers;
        std::condition_variable updated;

    public:
        explicit ThreadPool(size_t size) {
            for (size_t i = 0; i < size; ++i) {
                workers.emplace_back([this]() { worker_loop(); });
            }
        }

        void enqueue_task(UniqueFunction task)
        {
            {
                std::unique_lock<std::mutex> lock {state.mtx};
                state.work_queue.push(std::move(task));
            }
            updated.notify_one();
        }

        ~ThreadPool()
        {
            {
                std::unique_lock<std::mutex> lock {state.mtx};
                state.aborting = true;
            }
            updated.notify_all();
            for (std::thread &t: workers) {
                t.join();
            }
        }

    public:
        template<class F>
        auto async(F&& func)
        {
            using ResultType = std::invoke_result_t<std::decay_t<F>>;
            std::packaged_task<ResultType()> pt(std::forward<F>(func));
            std::future<ResultType> future = pt.get_future();
            UniqueFunction task ([pt = std::move(pt)]() mutable { pt(); });
            enqueue_task(std::move(task));
            return future;
        }

    private:
        void worker_loop()
        {
            while (true)
            {
                std::unique_lock<std::mutex> lock {state.mtx};
                while (state.work_queue.empty() && !state.aborting) {
                    updated.wait(lock);
                }
                if (state.aborting)
                    break;

                assert(!state.work_queue.empty());
                UniqueFunction task = std::move(state.work_queue.front());
                state.work_queue.pop();
                lock.unlock();

                // INFO: Run task
                task();
            }
        }
    };
}

void ThreadPoolOne::TEST_ALL()
{
    std::atomic<int> sum(0);
    ThreadPool tp(4);
    std::vector<std::future<int>> futures;
    for (int i = 0; i < 60000; ++i) {
        auto f = tp.async([i, &sum]() {
            sum += i;
            return i;
        });
        futures.push_back(std::move(f));
    }
    std::cout << futures[42].get() << std::endl;
    std::cout << "sum = " << sum << std::endl; // INFO: 1799970000

    // assert(futures[42].get() == 42);
    // assert(903 <= sum && sum <= 1799970000);
}
