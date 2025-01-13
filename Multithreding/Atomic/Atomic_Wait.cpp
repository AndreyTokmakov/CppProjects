/**============================================================================
Name        : Atomic_Wait.cpp
Created on  : 13.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <string>
#include <chrono>
#include <future>
#include <syncstream>

#include "Atomic_Wait.h"
#include "../Utilities/Utilities.h"


namespace Atomic_Wait::Basics
{
    void Wait()
    {
        constexpr int initialValue { 0 };
        std::atomic<int> variable { initialValue };

        auto waiter = std::async(std::launch::async, [&variable]() -> void {
            THREAD_INFO << "Waiting until variable changes its value: value = " << variable << std::endl;
            variable.wait(initialValue);
            THREAD_INFO << "Waiting Done!!!: value = " << variable << std::endl;
        });

        auto task = std::async(std::launch::async, [&variable]() -> void {
            THREAD_INFO << "Starting task" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(2U));
            variable.store(initialValue);
            THREAD_INFO << "Task 1 completed: value = " << variable << std::endl;

            variable.notify_all();

            std::this_thread::sleep_for(std::chrono::seconds(2U));
            variable.store(initialValue  + 1);
            THREAD_INFO << "Task 2 completed: value = " << variable << std::endl;

            variable.notify_all();
        });
    }

    void Notify_One()
    {
        constexpr int initialValue { 0 };
        std::atomic<int> value{ initialValue };

        auto task = std::async([&value]() -> void {
            THREAD_INFO << "Starting task" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(5U));

            value.store(initialValue + 1);
            value.notify_one();

            THREAD_INFO << "Task  completed: value = " << value << std::endl;
        });

        std::vector<std::future<void>> tasks;
        for (int i = 0; i < 2; ++i) {
            const std::string name { "Thread_" + std::to_string(i) };
            tasks.emplace_back(std::async([&value, name]() -> void {
                THREAD_INFO << "Waiting until value != 10 . . . " << std::endl;
                value.wait(initialValue);
                THREAD_INFO << name << " done" << std::endl;
            }));
        }
    }


    void Notify_All()
    {
        constexpr int initialValue { 0 };
        std::atomic<int> value { initialValue };

        auto task = std::async([&value]() -> void {
            THREAD_INFO << "Starting task" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(5U));

            value.store(initialValue + 1);
            value.notify_all();

            THREAD_INFO << "Task  completed: value = " << value << std::endl;
        });

        std::vector<std::future<void>> tasks;
        for (int i = 0; i < 2; ++i) {
            const std::string name{ "Thread_" + std::to_string(i) };
            tasks.emplace_back(std::async([&value, name]() -> void {
                THREAD_INFO << "Waiting until value != 10 . . . " << std::endl;
                value.wait(initialValue);
                THREAD_INFO << name << " done" << std::endl;
            }));
        }
    }
}


void Atomic_Wait::TestAll()
{
    Basics::Wait();
    // Basics::Notify_One();
    // Basics::Notify_All();

    // std::cout << Utilities::timeString() << std::endl;
    // std::cout << Utilities::getCurrentTime() << std::endl;
    // std::cout << Utilities::getCurrentTimeOld() << std::endl;
}
