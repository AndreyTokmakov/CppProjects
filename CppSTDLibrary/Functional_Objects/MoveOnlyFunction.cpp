/**============================================================================
Name        : MoveOnlyFunction.cpp
Created on  : 26.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MoveOnlyFunction.cpp
============================================================================**/

#include "FunctionObjects.hpp"

#include <functional>
#include <future>
#include <iostream>

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <print>
#include <thread>


// INFO: https://medium.com/@sagar.necindia/std-move-only-function-cpp23-callable-wrapper-no-copy-369e79e5baa0


namespace FunctionObjects::MoveOnlyFunction
{
    /**
    The Problem: std::function Forces Copyability
    Say you’ve got a unique resource — a database connection, a file handle, something wrapped in a std::unique_ptr.
    You want to create a callback that owns this resource:
    **/


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


    void SimpleExample_2()
    {
        auto register_callback = [](std::move_only_function<void()> cb) {
            cb();
        };
        auto register_callback_func = [](std::function<void()> cb) {
            cb();
        };

        std::unique_ptr<int> resource = std::make_unique<int>(42);
        auto callback = [res = std::move(resource)]() {
            std::cout << "Resource value: " << *res << "\n";
        };

        register_callback(std::move(callback)); // Compiles and works.
#if 0
        register_callback_func(std::move(callback)); // Compiles and works.
                                                     // error: static assertion failed: std::function target must be copy-constructible
#endif
        // Resource value: 42
    }
}

namespace FunctionObjects::MoveOnlyFunction
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


    private:

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

void FunctionObjects::MoveOnlyFunction::TestAll()
{
    // SimpleExample();
    SimpleExample_2();
    // TaskQueueTest();
}