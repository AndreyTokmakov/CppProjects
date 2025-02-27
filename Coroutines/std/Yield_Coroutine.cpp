/**============================================================================
Name        : Yield_Coroutine.cpp
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

};


void Coroutines::Yield_Coroutine::TestAll()
{
    ReturnType rt = coro_func();
    std::cout << rt.getValue() << std::endl;
}