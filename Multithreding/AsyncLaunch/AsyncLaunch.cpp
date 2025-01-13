//============================================================================
// Name        : AsyncLaunch.cpp
// Created on  : 11.07.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : std::async src
//============================================================================

#include "AsyncLaunch.h"

#include <iostream>
#include <string>
#include <string_view>
#include <future>
#include <chrono>
#include "../Utilities/Utilities.h"


namespace AsyncLaunch {


    void Async_Default() {
        THREAD_INFO << "Main thread: Thread id: " << std::this_thread::get_id() << std::endl;

        auto asyncDefault = std::async([]()-> void {
            THREAD_INFO << "Second thread (id: " << std::this_thread::get_id() << "). Started." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(9u));
            THREAD_INFO << "Second thread (id: " << std::this_thread::get_id() << "). Ended." << std::endl;
        });


        THREAD_INFO << "Main thread: Before sleep." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5u));
        THREAD_INFO << "Main thread: Sleep ended." << std::endl;

        THREAD_INFO << "Main thread: Waiting for the second Thread..." << std::endl;
        asyncDefault.get();
        THREAD_INFO << "Main thread: Second done." << std::endl;
    }


    //--------------------------------------------------------------------------------

    void Simple_Asynch_Task() {
        auto taskToSleep = [](int timeout)-> void {
            THREAD_INFO << "Task started" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            THREAD_INFO << "Task completed" << std::endl;
        };

        int timeout = 3;
        std::future<void> future = std::async(std::launch::async, taskToSleep, timeout);
        THREAD_INFO << "You can see this immediately!" << std::endl;

        // However, the value of the future will be available after sleep has finished so future.get() can block up to 1 second.
        future.get();

        THREAD_INFO << "This will be shown " << timeout << " seconds later." << std::endl;
    }

    //==================================================================================

    template<typename Func, typename... Ts>
    auto runAsync(Func&& f, Ts&&... params) {
        return std::async(std::launch::async,
                          std::forward<Func>(f),
                          std::forward<Ts>(params)...);
    }

    void Simple_Asynch_Task_Function() {
        auto task = [](int timeout)-> void {
            THREAD_INFO << "Task started. Sleeping for " << timeout << " seconds\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            THREAD_INFO << "Task completed" << std::endl;
        };

        auto future = runAsync(task, 4);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        THREAD_INFO << "Back to main." << std::endl;
    }

    //==================================================================================

    void print_ten(std::string text, std::chrono::milliseconds duration) {
        for (int i = 0; i < 10; ++i) {
            std::this_thread::sleep_for(duration);
            std::cout << text;
        }
        std::cout << "\n";
    }

    void Asynch_vs_Defered() {
        auto some_task = [](unsigned long timeout)-> void {
            std::this_thread::sleep_for(std::chrono::milliseconds(50u));
            THREAD_INFO << "Task started. Sleeping for " << timeout << " seconds\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            THREAD_INFO << "Done!\n";
            THREAD_INFO << "\n";
        };

        constexpr unsigned long timeout = 5; // 5 seconds
        std::future<void> futureAsync = std::async(std::launch::async, some_task, timeout);

        THREAD_INFO << "After std::launch::async task. Point 1\n";
        std::this_thread::sleep_for(std::chrono::seconds(3u));
        THREAD_INFO << "After std::launch::async task. Point 2\n";

        futureAsync.wait();

        // std::launch::deferred
        // the task is executed on the calling thread the first time its result is requested (lazy evaluation)

        std::future<void> futureDeferred = std::async(std::launch::deferred, some_task, timeout);

        THREAD_INFO << "After std::launch::deferred task. Point 1\n";
        std::this_thread::sleep_for(std::chrono::seconds(3u));
        THREAD_INFO << "After std::launch::deferred task. Point 2\n";

        futureDeferred.get();
    }

    void Asynch_vs_Defered_2()
    {
        THREAD_INFO << "with launch::async:\n";
        std::future<void> foo = std::async(std::launch::async, print_ten, " * ", std::chrono::milliseconds(250u));
        std::future<void> bar = std::async(std::launch::async, print_ten, " $ ", std::chrono::milliseconds(250u));

        foo.get();
        bar.get();
        THREAD_INFO << "\n\n";

        THREAD_INFO << "with launch::deferred:" << std::endl;
        foo = std::async(std::launch::deferred, print_ten, " * ", std::chrono::milliseconds(250u));
        bar = std::async(std::launch::deferred, print_ten, " $ ", std::chrono::milliseconds(250u));

        foo.get();
        bar.get();
        THREAD_INFO << std::endl;;
    }


    void Asynch_vs_Defered_3()
    {
        auto task = [](unsigned int timeout, const std::string& name)-> void {
            THREAD_INFO << name << " entered" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            std::cout << std::endl;
            THREAD_INFO << name << " done." << std::endl;
        };

        THREAD_INFO << "Starting async task ....." << std::endl;
        auto async_task = std::async(std::launch::async, task, 3, "Asynch_Task");
        auto deferred_task = std::async(std::launch::deferred, task, 3, "Deffered_Task");

        std::chrono::milliseconds span(99u);
        while (async_task.wait_for(span) == std::future_status::timeout)
            std::cout << ". " << std::flush;

        THREAD_INFO << "Async task done. Deffered task should start" << std::endl;
        deferred_task.get();
        THREAD_INFO << "We've done." << std::endl;
    }

}

void AsyncLaunch::TEST_ALL() {
    Async_Default();

    // Simple_Asynch_Task();
    // Simple_Asynch_Task_Function();

    // Asynch_vs_Defered();
    // Asynch_vs_Defered_2();
    // Asynch_vs_Defered_3();
}
