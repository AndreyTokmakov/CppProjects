//============================================================================
// Name        : PackagedTask.h
// Created on  : 14.09.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Packaged Task src class
//============================================================================

#include <iostream>
#include <syncstream>
#include <future>
#include <chrono>
#include <string>
#include <deque>
#include <functional>
#include <cmath>
#include <thread>

#include "PackagedTask.h"
#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << utilities::datetime::getCurrentTime() << " "

namespace PackagedTask
{
    class  DBDataFetcher {
    public:
        std::string operator()(const std::string& token) {
            LOG << "Starting job..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3u));
            std::string data = "Data From " + token;
            LOG << "Done" << std::endl;
            return data;
        }
    };

    int countdown(int from, int to) {
        for (int i = from; i != to; --i) {
            std::cout << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1u));
        }
        return from - to;
    }

    void Task_From_ClassParam()
    {
        {
            DBDataFetcher fetcher = DBDataFetcher();
            std::packaged_task<std::string(std::string)> task(std::move(fetcher));
            std::future<std::string> result = task.get_future();
            std::thread th(std::move(task), "Test_1");

            std::string data = result.get();
            th.join();
            LOG << data << std::endl;
        }
        std::cout << std::endl;
        {
            DBDataFetcher fetcher = DBDataFetcher();
            std::packaged_task<std::string(std::string)> task(std::move(fetcher));
            std::future<std::string> result = task.get_future();
            std::thread(std::move(task), "Test_2").detach();
            std::string data = result.get();
            LOG << data << std::endl;
        }
    }

    void Reset() {
        std::packaged_task<int(int, int)> task(countdown); // package task

        std::cout << "Countdown from 10 til 5: " << std::endl;
        std::future<int> future = task.get_future();
        task(10, 5);
        future.get();

        // re-use same task object:
        task.reset();
        future = task.get_future();
        std::thread(std::move(task), 4, 0).detach();
        std::cout << "countdown from 4 - 0: " << std::endl;
        future.get();
    }

    void TEST_PackagedTask()
    {
        std::packaged_task<int(int, int)> tsk(PackagedTask::countdown); // Creating the packaged_task
        std::future<int> futureResult = tsk.get_future(); // Get the future
        std::thread threadHandle(std::move(tsk), 10, 0);  // Spawn thread to count down from 10 to 0

        // Do something while waiting for function to set future
        std::chrono::milliseconds span(100);
        while (futureResult.wait_for(span) == std::future_status::timeout)
            std::cout << ". " << std::flush;

        std::cout << "Getting the result\n";
        int value = futureResult.get();
        std::cout << "The countdown lasted for " << value << " seconds." << std::endl;
        threadHandle.join();
    }


    void PackagedTask_Create_and_Run()
    {
        std::packaged_task<std::string(size_t timeout)> task([](size_t timeout) {
            LOG << "Task is started [timeout = " << timeout << "]: ";

            int ms_sleep = (static_cast<int>(timeout) * 1000) / 20;
            for (int i = 0; i < 20; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ms_sleep));
                std::cout << ". ";
            }
            std::cout << "\n";
            LOG << "Task is completed\n";
            return std::string("Task lasted " + std::to_string(timeout));
        });

        LOG << "Prepare future object." << std::endl;
        std::future<std::string> futureResult = task.get_future();

        LOG << "Sleep for 0.5 seconds" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500u));

        LOG << "* * * * * Running task * * * * *\n";
        std::jthread threadHandle(std::move(task), 4);

        LOG << "* * * * * After task * * * * *\n";
        auto value = futureResult.get();

        LOG << value << " seconds." << std::endl;
    }

    void Lambda_Task()
    {
        std::packaged_task<double(int, int)> task([](int a, int b) {
            LOG << "**** Executing task ******" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3u));
            LOG << "**** Task completed ******" << std::endl;
            return std::pow(a, b);
        });

        LOG << "Entered" << std::endl;
        std::future<double> result = task.get_future();

        LOG << "Running task... " << std::endl;
        task(2, 9);
        LOG << "After task [BLOCKED!!!!]" << std::endl;

        LOG << "Result: " << result.get() << std::endl;
    }

    void Bind_Task() {
        auto func = [](int x, int y) {
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            return std::pow(x, y);
        };
        // std::packaged_task<double()> task(std::bind(func, 2, 11));
        std::packaged_task<double()> task([func] { return func(2, 11); });
        std::future<double> result = task.get_future();

        LOG << "Before task... \n";
        task();
        LOG << "After task [BLOCKED!!!!]\n";
        LOG << "Result: " << result.get() << std::endl;
    }

    void Run_Task_Test() {
        auto sleep = []() {
            LOG << "Task started\n";
            std::this_thread::sleep_for(std::chrono::seconds(3u));
            LOG << "Task completed\n";
            return 1;
        };

        std::packaged_task<int()> task(sleep);
        std::future<int> fut = task.get_future();
        task();

        LOG << "You can see this after 1 second" << std::endl;
        LOG << fut.get() << std::endl;
    }

    void Valid() {

        auto launcher = [](std::packaged_task<int(int)>& task, int arg) {
            if (true == task.valid()) {
                std::future<int> future = task.get_future();
                std::thread(std::move(task), arg).detach();
                return future;
            }
            else {
                return std::future<int>();
            }
        };

        std::packaged_task<int(int)> task([](int x) {
            return x * 2;
        });
        std::future<int> fut = launcher(task, 25);
        LOG << "The double of 25 is " << fut.get() << std::endl;
    }

    void MakeReady_AtThreadExit() {
        auto worker = [](std::future<void>& output)-> void {
            std::packaged_task<void(bool&)> my_task{ [](bool& done) { done = true; } };

            auto result = my_task.get_future();
            bool done = false;

            my_task.make_ready_at_thread_exit(done); // execute task right away
            LOG << "Worker: done = " << std::boolalpha << done << std::endl;

            auto status = result.wait_for(std::chrono::seconds(0u));
            if (status == std::future_status::timeout)
                LOG << "Worker: result is not ready yet" << std::endl;

            output = std::move(result);
        };

        std::future<void> result;
        std::thread{ worker, std::ref(result) }.join();

        auto status = result.wait_for(std::chrono::seconds(0u));
        if (status == std::future_status::ready)
            LOG << "Main: result is ready" << std::endl;
    }



    void PackagedTask_Collection()
    {
        std::deque<std::packaged_task<void()>> tasks;

        tasks.emplace_back([]() {
            LOG << "Task 1 is started\n";
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            LOG << "Task 1 is completed\n";
        });

        tasks.emplace_back([]() {
            LOG << "Task 2 is started\n";
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            LOG << "Task 2 is completed\n";
        });

        tasks.emplace_back([]() {
            LOG << "Task 3 is started\n";
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            LOG << "Task 3 is completed\n";
        });

        while (false == tasks.empty()) {
            std::packaged_task<void()> task { std::move(tasks.front()) };
            tasks.pop_front();
            task();
        }
    }

    void PackagedTask_Collection_2()
    {
        auto work = [](int id) {
            LOG << "Task " << id << " is started\n";
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            LOG << "Task " << id << " is completed\n";
        };

        std::deque<std::packaged_task<void()>> tasks;
        tasks.emplace_back([work] { return work(1); });
        tasks.emplace_back([work] { return work(2); });
        tasks.emplace_back([work] { return work(3); });

        while (false == tasks.empty()) {
            std::packaged_task<void()> task{ std::move(tasks.front()) };
            tasks.pop_front();

            LOG << "Starting task in the separate thread.\n";

            std::future<void> result = task.get_future();
            std::thread task_thread(std::move(task));
            task_thread.join();
            result.get();

            LOG << "Task completed\n\n";
        }
    }
}


namespace PackagedTask::CreateGenericTask
{
    template<typename Func, typename ... Args>
    std::packaged_task<std::invoke_result_t<Func, Args ... >()> createTask(Func&& func, Args&& ... args)
    {
        using ReturnType = std::invoke_result_t<Func, Args ... >;
        auto task = [fn = std::forward<Func>(func), ...args = std::forward<Args>(args)]() mutable {
            return std::invoke(std::move(fn), std::forward<Args>(args)...);
        };
        return std::packaged_task<ReturnType()>(std::move(task));
    }

    int getInt() {
        return 101;
    }

    std::string getString() {
        return "Hello World!";
    }

    void getVoid(const std::string_view text)
    {
        std::cout << text << std::endl;
    }

    void create_and_run_task()
    {
        using namespace std::string_view_literals;
        {
            auto task = CreateGenericTask::createTask(&getInt);
            std::future<int> fut = task.get_future();
            std::thread(std::move(task)).detach();
            std::cout << fut.get() << std::endl;
        }
        {
            auto task = CreateGenericTask::createTask(&getString);
            std::future<std::string> fut = task.get_future();
            std::thread(std::move(task)).detach();
            std::cout << fut.get() << std::endl;
        }
        {
            auto task = CreateGenericTask::createTask(&getVoid, "Hello from packaged task!"sv);
            const std::future<void> fut = task.get_future();
            std::thread(std::move(task)).detach();
            fut.wait();
        }

        // 101
        // Hello World!
        // Hello from packaged task!
    }
}

void PackagedTask::TEST_ALL()
{
    // Task_From_ClassParam();

    // PackagedTask_Create_and_Run();

    // TEST_PackagedTask();
    // Lambda_Task();
    // Bind_Task();
    // Run_Task_Test();

    // Reset();

    // Valid();

    // MakeReady_AtThreadExit();

    // PackagedTask_Collection();
    // PackagedTask_Collection_2();


    CreateGenericTask::create_and_run_task();
};
