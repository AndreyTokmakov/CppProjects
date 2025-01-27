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
                return ReturnType{ *this };
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



void Coroutines::TestAll()
{
    SimpleCoroutine::Test();
}