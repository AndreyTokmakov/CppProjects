/**============================================================================
Name        : TaskExecutor.cpp
Created on  : 06.05.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TaskExecutor.cpp
============================================================================**/

#include "TaskExecutor.hpp"

#include <iostream>
#include <string_view>
#include <vector>
#include <print>
#include <utility>
#include <functional>

#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "DateTimeUtilities.hpp"



namespace
{
    using DateTimeUtilities::getCurrentTime;

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "
}


namespace executor
{
    class Task
    {
        struct IConcept
        {
            virtual ~IConcept() = default;
            virtual void call() = 0;
        };

        template<typename Func>
        struct Model : IConcept
        {
            Func fn;

            explicit Model(const Func& func) : fn { func } {
            }

            explicit Model(Func&& func) : fn { std::forward<Func>(func) } {
            }

            void call() override {
                fn();
            }
        };

        std::unique_ptr<IConcept> impl { nullptr };

    public:
        template<typename F>
        Task(F&& f): impl { std::make_unique<Model<F>>(std::forward<F>(f)) } {
        }

        Task(Task&&) = default;
        Task& operator=(Task&&) = default;

        void operator()() const {
            impl->call();
        }
    };

    template<typename Func, typename... Args>
    Task makeTask(Func&& func, Args&&... args)
    {
        return Task([fn = std::forward<Func>(func), ... captured = std::forward<Args>(args)]() mutable {
            std::invoke(std::move(fn), std::move(captured)...);
        });
    }


    class Executor
    {
        std::queue<Task> queue;
        std::mutex mtx;
        std::condition_variable cv;
        std::stop_source stopSource;
        std::jthread worker;

    public:
        Executor()
        {
            worker = std::jthread(&Executor::run, this, stopSource);
        }

        ~Executor()
        {
            {
                std::lock_guard lock(mtx);
                auto _ = stopSource.request_stop();
            }
            cv.notify_all();
        }

        void submit(Task t)
        {
            {
                std::lock_guard lock(mtx);
                queue.push(std::move(t));
            }
            cv.notify_one();
        }

    private:

        void run(const std::stop_source& source)
        {
            while (!source.stop_requested())
            {
                Task task([]{});
                {
                    std::unique_lock lock(mtx);
                    cv.wait(lock, [&] {
                        return source.stop_requested() || !queue.empty();
                    });
                    if (source.stop_requested() && queue.empty())
                        return;
                    task = std::move(queue.front());
                    queue.pop();
                }
                task();
            }
        }
    };
}

namespace executor
{
    Executor executor;

    struct Worker
    {
        std::string data { "Worker_data"};

        void runTask()
        {
            executor.submit(makeTask([*this] {
                std::cout << "Data: " << data << std::endl;
             }));
        }
    };

    void demo()
    {
        Worker worker;

        executor.submit(makeTask([] {
            std::cout << "fire and forget\n";
        }));

        executor.submit(makeTask([](int a, int b) {
            std::cout << a + b << "\n";
        }, 2, 3));

        worker.runTask();

        std::this_thread::sleep_for(std::chrono::seconds{1});
    }
}


void task_executor::TestAll()
{
    executor::demo();
}
