/**============================================================================
Name        : UseCases_TaskCoordination.cpp
Created on  : 01.03.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Simple Coroutine Waitable
============================================================================**/

#include "Coroutines.h"
#include "Utilities.h"

#include <coroutine>
#include <print>
#include <chrono>
#include <thread>
#include <fstream>

namespace {
    using Utilities::getCurrentTime;
}

namespace
{
#if 0
    struct Task
    {
        struct promise_type
        {
            Task get_return_object()
            {
                std::println("[{}] get_return_object() --> Task()", getCurrentTime());
                return Task { this };
            }

            std::suspend_never initial_suspend()
            {
                std::println("[{}] initial_suspend()", getCurrentTime());
                return {};
            }

            std::suspend_never final_suspend() noexcept
            {
                std::println("[{}] final_suspend()", getCurrentTime());
                return {};
            }

            void return_void() {
                std::println("[{}] return_void()", getCurrentTime());
            }

            void unhandled_exception() {
                std::println("[{}] unhandled_exception()", getCurrentTime());
                std::terminate();
            }
        };

        promise_type* promise;

        Task(promise_type* p) : promise(p) {
        }

        struct Awaiter
        {
            std::chrono::milliseconds delay;

            explicit Awaiter(const std::chrono::milliseconds d) : delay { d } {
                std::println("[{}] Awaiter::Awaiter({})", getCurrentTime(), delay.count());
            }

            bool await_ready()
            {
                std::println("[{}] Awaiter::await_ready()", getCurrentTime());
                return false;
            }

            void await_suspend(std::coroutine_handle<Task::promise_type> handle)
            {
                std::println("[{}] Awaiter::await_suspend()", getCurrentTime());
                std::thread([handle, this]() {
                    std::this_thread::sleep_for(delay);
                    handle.resume();
                }).detach();
            }

            void await_resume() {

            }
        };

        Awaiter moveEntity(int id, int distance)
        {
            std::println("[{}] Entity {} moving {} units)", getCurrentTime(), id, distance);
            return Awaiter {std::chrono::milliseconds(500U * distance)};
        }

        Awaiter updateEntity(int id)
        {
            std::println("[{}] Entity {} updating", getCurrentTime(), id);
            return Awaiter {std::chrono::milliseconds(500U)};
        }
    };

    Task runSimulation()
    {
        co_await Task{}.moveEntity(1, 5);
        co_await Task{}.updateEntity(1);
        co_await Task{}.moveEntity(2, 3);
        co_await Task{}.updateEntity(2);
        co_await Task{}.moveEntity(1, 2);
    }
#endif

    /*
    struct Task
    {
        struct promise_type
        {
            Task get_return_object() { return Task{this}; }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            void return_void() {}
            void unhandled_exception() { std::terminate(); }
        };

        promise_type* promise;
        Task(promise_type* p) : promise(p) {}

        struct Awaiter {
            std::chrono::milliseconds delay;

            bool await_ready() { return false; }
            void await_suspend(std::coroutine_handle<> h) {
                std::thread([h, this]() {
                    std::this_thread::sleep_for(delay);
                    h.resume();
                }).detach();
            }
            void await_resume() {}
        };

        Awaiter moveEntity(int id, int distance) {
            return Awaiter{std::chrono::milliseconds(500u * distance)};
        }

        Awaiter updateEntity(int id) {
            return Awaiter{std::chrono::milliseconds(100u)};
        }
    };

    Task runSimulation()
    {
        co_await Task{}.moveEntity(1, 5);
        co_await Task{}.updateEntity(1);
        co_await Task{}.moveEntity(2, 3);
        co_await Task{}.updateEntity(2);
        co_await Task{}.moveEntity(1, 2);
    }
    */
}


void Coroutines::UseCases_TaskCoordination::TestAll()
{

}
