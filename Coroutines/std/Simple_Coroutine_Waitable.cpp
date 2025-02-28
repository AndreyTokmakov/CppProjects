/**============================================================================
Name        : Simple_Coroutine_Waitable.cpp
Created on  : 28.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Simple Coroutine Waitable
============================================================================**/

#include "Coroutines.h"

#include <coroutine>
#include <print>
#include <chrono>
#include <thread>

namespace
{
    constexpr std::string_view formatMSeconds { "%d-%02d-%02d %02d:%02d:%02d.%06ld" };

    [[nodiscard]]
    std::string getCurrentTime(const std::chrono::time_point<std::chrono::system_clock>& timestamp =
                                std::chrono::system_clock::now())
    {
        const time_t time { std::chrono::system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(buffer.data(), formatMSeconds.data(),
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
            duration_cast<std::chrono::microseconds>(timestamp - time_point_cast<std::chrono::seconds>(timestamp)).count()
        );
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
    }
}

namespace
{
    /**
    Custom Awaitable Types
    In C++, coroutines are not limited to working with built-in types like std::suspend_always or std::suspend_never.
    You can create custom awaitable types that define specific suspension and resumption behavior.

    An awaitable type is any type that implements the following methods:
    1. await_ready()  : Determines if the coroutine should suspend or continue without suspension.

    2. await_suspend(): Defines what happens when the coroutine is suspended.
                        This can involve storing the coroutine handle, initiating an asynchronous operation,
                        or interacting with other coroutines.

    3. await_resume() : Defines what happens when the coroutine is resumed, often returning a value or performing
                        some final action before execution continues.
    */
    struct Timer
    {
        std::chrono::milliseconds duration;

        explicit Timer(const std::chrono::milliseconds d) : duration(d) {
            std::println("[{}] Timer::Timer({})", getCurrentTime(), duration.count());
        }

        [[nodiscard]]
        bool await_ready() const
        {
            std::println("[{}] Timer::await_ready()", getCurrentTime());
            return false;
        }

        void await_suspend(std::coroutine_handle<> h) const
        {
            std::println("[{}] Timer::await_suspend() entered", getCurrentTime());
            std::thread([h, this]() {
                std::this_thread::sleep_for(duration);
                h.resume();
            }).detach();
        }

        void await_resume() const {
            std::println("[{}] Timer::await_resume()", getCurrentTime());
        }
    };

    struct MyCoroutineTask
    {
        struct promise_type
        {
            MyCoroutineTask get_return_object() {
                std::println("[{}] get_return_object", getCurrentTime());
                return {};
            }

            std::suspend_never initial_suspend() {
                std::println("[{}] initial_suspend", getCurrentTime());
                return {};
            }

            std::suspend_never final_suspend() noexcept {
                std::println("[{}] initial_suspend", getCurrentTime());
                return {};
            }

            void return_void() {
                std::println("[{}] return_void", getCurrentTime());
            }

            void unhandled_exception() {
                std::terminate();
            }
        };

        MyCoroutineTask run()
        {
            std::println("[{}] Starting timer... ", getCurrentTime());
            co_await Timer{std::chrono::seconds(2)};
            std::println("[{}] Starting timer...", getCurrentTime());
            std::println("[{}] Timer finished.", getCurrentTime());
        }

        MyCoroutineTask() {
            std::println("[{}] MyCoroutineTask()", getCurrentTime());
        }

        ~MyCoroutineTask() {
            std::println("[{}] ~MyCoroutineTask()", getCurrentTime());
        }
    };
}


void Coroutines::Simple_Coroutine_Waitable::TestAll()
{
    MyCoroutineTask task;
    task.run();
    std::this_thread::sleep_for(std::chrono::seconds(3));
}
