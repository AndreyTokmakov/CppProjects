/**============================================================================
Name        : Simple_Coroutine_Waitable_2.cpp
Created on  : 28.02.2025
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

/**

Coroutine Functions and Suspension Points
A coroutine function is a special type of function in C++ that can be suspended and resumed at specific points
during its execution.
This is achieved using three key keywords: co_await, co_yield, and co_return.

1. co_await:  This keyword is used to suspend the execution of the coroutine until a particular
              condition is met or an asynchronous operation is completed.

2. co_yield:  This keyword allows the coroutine to produce a value and suspend its execution.
              It can be resumed later, continuing from the point after co_yield.

3. co_return: This keyword is used to return a value from the coroutine and finalize its execution.

Custom Awaitable Types

In C++, there are two built-in types like
-   std::suspend_always
-   std::suspend_never.

We create custom awaitable types that define specific suspension and resumption behavior:
An awaitable type is any type that implements the following methods:

1. await_ready()  : Determines if the coroutine should suspend or continue without suspension.

2. await_suspend(): Defines what happens when the coroutine is suspended.
                    This can involve storing the coroutine handle, initiating an asynchronous operation,
                    or interacting with other coroutines.

3. await_resume() : Defines what happens when the coroutine is resumed, often returning a value or performing
                    some final action before execution continues.

In this example, 'AwaiterTimer' is a custom awaitable type that suspends the coroutine for a specified duration.
The coroutine resumes automatically after the specified time, allowing you to integrate time-based delays
or other asynchronous tasks into your coroutine workflow.

**/

namespace {
    using Utilities::getCurrentTime;
}

namespace
{
    struct TaskPromise
    {
        struct promise_type
        {
            TaskPromise get_return_object()
            {
                std::println("[{}] get_return_object(), thread_id: {}", getCurrentTime(), std::this_thread::get_id());
                return TaskPromise {std::coroutine_handle<promise_type>::from_promise(*this)};
            }

            std::suspend_never initial_suspend() {
                std::println("[{}] initial_suspend", getCurrentTime());
                return {};
            }

            std::suspend_never final_suspend() noexcept {
                std::println("[{}] final_suspend()", getCurrentTime());
                return {};
            }

            void return_void() {
                std::println("[{}] return_void", getCurrentTime());
            }

            void unhandled_exception() {
                std::terminate();
            }

            size_t data = 0;
        };

        std::coroutine_handle<promise_type> handle;
    };

    struct Awaiter
    {
        std::chrono::milliseconds duration;

        explicit Awaiter(const std::chrono::milliseconds d) : duration(d) {
            std::println("[{}] Awaiter::Awaiter({})", getCurrentTime(), duration.count());
        }

        bool await_ready() noexcept
        {
            /** Called immediately before the coroutine is suspended
             *  Allows as such, for some reason, to decide not to suspend after all
             *  Returns true → coroutine is NOT suspended
             *  Typically : return false;
             *  Use case : suspension depends on some data availability
            **/
            std::println("[{}] Awaiter::await_ready(), thread_id: {}", getCurrentTime(), std::this_thread::get_id());
            return false;
        }

        void await_suspend(std::coroutine_handle<TaskPromise::promise_type> handle) noexcept
        {
            /** Called immediately after the coroutine is suspended
             *  Will get called if await_ready() return False
             *  Parameter: the handle of the coroutine that was suspended
             *  In the body you can either return an other coroutine_handle type to change the call execution
             *  Or you ca return nothing
            **/
            std::println("[{}] Awaiter::await_suspend(), thread_id: {}", getCurrentTime(), std::this_thread::get_id());
            auto thread = std::thread([=]() {
                std::this_thread::sleep_for(duration);
                handle.promise().data = 1;
                handle.resume();
            });
            thread.join();
        }

        void await_resume() noexcept
        {
            /** Called when the coroutine is resumed (after a successful suspension)
             *  It is the final result of expression 'co_await ...'
             *  It could return a value or nothing
             *  Can return a value : The value the co_await expression yields
            **/
            std::println("[{}] Awaiter::await_resume(), thread_id: {}", getCurrentTime(), std::this_thread::get_id());
        }
    };

    TaskPromise task_func()
    {
        std::println("[{}] task_func() step 1, thread_id: {}", getCurrentTime(), std::this_thread::get_id());
        co_await Awaiter { std::chrono::seconds(3u) };
        std::println("[{}] task_func() step 2, thread_id: {}", getCurrentTime(), std::this_thread::get_id());
    }

    void test()
    {
        std::println("[{}] main(0), thread_id:  {}", getCurrentTime(), std::this_thread::get_id());
        auto promise = task_func();

        auto data = promise.handle.promise().data;
        std::println("[{}] main(1), data: {}, thread_id: {}", getCurrentTime(), data, std::this_thread::get_id());

        promise.handle.resume();
        std::println("[{}] main(2), data: {}, thread_id: {}", getCurrentTime(), data, std::this_thread::get_id());

        /**
        [2025-03-01 11:18:29.300655] main(), thread_id:  139670308555648
        [2025-03-01 11:18:29.300725] get_return_object(), thread_id: 139670308555648
        [2025-03-01 11:18:29.300728] main(1), data: 0, thread_id: 139670308555648
        [2025-03-01 11:18:29.300730] task_func() step 1, thread_id: 139670308555648
        [2025-03-01 11:18:29.300733] await_ready(), thread_id: 139670308555648
        [2025-03-01 11:18:29.300734] await_suspend(), thread_id: 139670308555648
        [2025-03-01 11:18:30.300893] await_resume(), thread_id: 139670308542208
        [2025-03-01 11:18:30.301025] task_func() step 2, thread_id: 139670308542208
        [2025-03-01 11:18:30.301190] main(2), data: 0, thread_id: 139670308555648
        */
    }
}


void Coroutines::Simple_Coroutine_Waitable_2::TestAll()
{
    test();
}
