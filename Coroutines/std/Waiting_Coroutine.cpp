/**============================================================================
Name        : Waiting_Coroutine.cpp
Created on  : 27.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Yield_Coroutine.h
============================================================================**/

#include "Coroutines.h"

#include <iostream>
#include <coroutine>
#include <generator>
#include <utility>

namespace
{
    using namespace std::string_literals;

    struct ReturnType
    {
        struct promise_type
        {
            std::string input_data { };

            ReturnType get_return_object() noexcept
            {
                std::cout << "get_return_object" << std::endl;
                return ReturnType { *this };
            }

            void return_void() noexcept
            {
                std::cout << "return_void" << std::endl;
            }

            std::suspend_always initial_suspend() noexcept
            {
                std::cout << "initial_suspend" << std::endl;
                return {};
            }

            std::suspend_always final_suspend() noexcept
            {
                std::cout << "final_suspend" << std::endl;
                return {};
            }

            void unhandled_exception() noexcept
            {
                std::cout << "unhandled_exception" << std::endl;
            }

            // when the coroutine calls co_await, the compiler will generate code to call a function in the promise
            // object called await_transform(), which has a parameter of the same type as the data the coroutine is waiting for.
            //
            // As its name implies, await_transform is a function that transforms any object (in our example, std::string)
            // into an awaitable object.
            //
            // std::string is not awaitable, hence the previous compiler error. await_transform() must return an awaiter object.
            // This is just a simple struct implementing a required interface for the awaiter to be usable by the compiler.

            auto await_transform(const std::string&) noexcept
            {
                std::cout << "await_transform()" << std::endl;

                struct MyAwaiter
                {
                    promise_type& promise;

                    // await_ready(): This is called to check whether the coroutine is suspended. If that is the case,
                    // it returns false. In our example, it always returns true to indicate the coroutine is not suspended.

                    [[nodiscard]]
                    bool await_ready() const noexcept {
                        std::cout << "await_ready()" << std::endl;
                        return true;
                    }

                    // await_resume(): This resumes the coroutine and generates the result of the co_await expression.

                    [[nodiscard]]
                    std::string await_resume() const noexcept {
                        std::cout << "await_resume()" << std::endl;
                        return std::move(promise.input_data);
                    }

                    // await_suspend(): In our simple awaiter, this returns void, meaning the control is passed to the
                    // caller and the coroutine is suspended. It’s also possible for await_suspend to return a Boolean.
                    // Returning true in this case is like returning void. Returning false means the coroutine is resumed.

                    void await_suspend(std::coroutine_handle<promise_type>) const noexcept {
                        std::cout << "await_suspend" << std::endl;
                    }
                };

                return MyAwaiter(*this);
            }
        };

        std::coroutine_handle<promise_type> handle{};

        explicit ReturnType(promise_type& promise) :
                handle { std::coroutine_handle<promise_type>::from_promise(promise) }
        {
            std::cout << "ReturnType()" << std::endl;
        }

        ~ReturnType() noexcept
        {
            if (handle) {
                handle.destroy();
            }
            std::cout << "~ReturnType()" << std::endl;
        }

        void putValue(std::string msg) const noexcept
        {
            handle.promise().input_data = std::move(msg);
            if (!handle.done()) {
                handle.resume();
            }
        }
    };

    ReturnType coro_func()
    {
        std::cout << co_await std::string{ };
        co_return;
    }
};


void Coroutines::Waiting_Coroutine::TestAll()
{
    ReturnType rt = coro_func();
    rt.putValue("Hello from main\n"s);
}