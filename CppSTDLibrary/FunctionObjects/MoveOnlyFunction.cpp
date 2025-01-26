/**============================================================================
Name        : MoveOnlyFunction.cpp
Created on  : 26.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MoveOnlyFunction.cpp
============================================================================**/

#include "MoveOnlyFunction.h"

#include <functional>
#include <future>
#include <iostream>

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <print>
#include <thread>


namespace MoveOnlyFunction
{
    // https://en.cppreference.com/w/cpp/utility/functional/move_only_function
    void SimpleExample()
    {
        std::packaged_task<double()> packaged_task([](){ return 3.14159; });

        std::future<double> future = packaged_task.get_future();

        auto lambda = [task = std::move(packaged_task)]() mutable { task(); };

        // std::function<void()> function = std::move(lambda); // Error
        std::move_only_function<void()> function = std::move(lambda); // OK

        function();

        std::cout << future.get();

    }
}

namespace MoveOnlyFunction
{
    using Task = std::move_only_function<void()>;
    // using Task = std::function<void()>;

    struct TaskQueue
    {
        TaskQueue(): worker { &TaskQueue::processTasks, this } {
        }

        ~TaskQueue()
        {
            {
                std::lock_guard<std::mutex> lock {mtx};
                stop = true;
            }
            cv.notify_all();
        }

        void processTasks()
        {
            Task task;
            while (true)
            {
                {
                    std::unique_lock<std::mutex> lock{mtx};
                    cv.wait(lock, [this] { return stop || !tasks.empty(); });

                    if (stop || tasks.empty())
                        return;

                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        }

        void addTask(Task task)
        {
            {
                std::lock_guard<std::mutex> lock {mtx};
                tasks.push(std::move(task));
            }
            cv.notify_all();
        }


        std::queue<Task> tasks;
        std::mutex mtx;
        std::condition_variable cv;
        bool stop { false };
        std::jthread worker;
    };


    void TaskQueueTest()
    {
        TaskQueue taskQueue;

        {
            auto resource = std::make_unique<int>(12345);
            taskQueue.addTask([res = std::move(resource)] mutable {
                //std::print("Processing unique resource: {}\n", res.get());
                std::cout << "Processing unique resource (int): " << *res.get() << std::endl;
            });
        }

        {
            auto resource = std::make_unique<std::string>("Hello world");
            taskQueue.addTask([res = std::move(resource)] mutable {
                //std::print("Processing unique resource: {}\n", res.get());
                std::cout << "Processing unique resource (string): " << *res.get() << std::endl;
            });
        }

        std::this_thread::sleep_for(std::chrono::seconds(1u));
    }
}

void MoveOnlyFunction::TestAll()
{
    // SimpleExample();
    TaskQueueTest();
}