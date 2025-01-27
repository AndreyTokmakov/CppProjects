/**============================================================================
Name        : Coroutines.cpp
Created on  : 27.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Coroutines.cpp
============================================================================**/

#include "Coroutines.h"

#include <iostream>
#include <string_view>
#include <future>
#include <semaphore>
#include <chrono>
#include <coroutine>
#include <iostream>

#include "../Utilities/Utilities.h"

namespace Coroutines::SimpleCoroutine
{
    struct ReturnType
    {
        struct promise_type
        {
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
        };

        explicit ReturnType(promise_type& promise) {
            std::cout << "ReturnType()" << std::endl;
        }

        ~ReturnType() noexcept {
            std::cout << "~ReturnType()" << std::endl;
        }
    };

    ReturnType coro_func() {
        co_return;
    }

    void Test()
    {
        ReturnType rt = coro_func();
    }
}

namespace Coroutines::Yield_Coroutine
{
    using namespace std::string_literals;

    struct ReturnType
    {
        struct promise_type
        {
            std::string output_data { };

            ReturnType get_return_object() noexcept
            {
                std::cout << "get_return_object" << std::endl;
                return ReturnType {  *this };
            }

            void return_void() noexcept
            {
                std::cout << "return_void" << std::endl;
            }

            /// Will be called from instruction 'co_yield "Hello from the coroutine\n"s;'
            std::suspend_always yield_value(std::string msg) noexcept
            {
                std::cout << "yield_value" << std::endl;
                output_data = std::move(msg);
                return {};
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
        };

        std::coroutine_handle<promise_type> handle{};

        explicit ReturnType(promise_type& promise) :
            handle { std::coroutine_handle<promise_type>::from_promise(promise)}
        {
            std::cout << "return_type()" << std::endl;
        }

        ~ReturnType() noexcept
        {
            if (handle) {
                handle.destroy();
            }
            std::cout << "~return_type()" << std::endl;
        }

        std::string getValue() noexcept
        {
            std::cout << "get()" << std::endl;
            if (!handle.done()) {
                handle.resume();
            }
            return std::move(handle.promise().output_data);
        }
    };

    ReturnType coro_func()
    {
        co_yield "Hello from the coroutine\n"s;
        co_return;
    }

    void Test()
    {
        ReturnType rt = coro_func();
        std::cout << rt.getValue() << std::endl;
    }
}

namespace Coroutines::Waiting_Coroutine
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

            auto await_transform(const std::string&) noexcept
            {
                struct awaiter
                {
                    promise_type& promise;

                    [[nodiscard]]
                    bool await_ready() const noexcept {
                        std::cout << "await_ready()" << std::endl;
                        return true;
                    }

                    [[nodiscard]]
                    std::string await_resume() const noexcept {
                        std::cout << "await_resume()" << std::endl;
                        return std::move(promise.input_data);
                    }

                    void await_suspend(std::coroutine_handle<promise_type>) const noexcept {
                        std::cout << "await_suspend" << std::endl;
                    }
                };
                return awaiter(*this);
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

    void Test()
    {
        ReturnType rt = coro_func();
        rt.putValue("Hello from main\n"s);
    }
}

void Coroutines::TestAll()
{
    // SimpleCoroutine::Test();
    // Yield_Coroutine::Test();
    Waiting_Coroutine::Test();
}