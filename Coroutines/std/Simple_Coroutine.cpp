/**============================================================================
Name        : Simple_Coroutine.cpp
Created on  : 27.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Simple_Coroutine.cpp
============================================================================**/

#include "Coroutines.h"
#include <iostream>
#include <coroutine>

namespace
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

        explicit ReturnType(promise_type&) {
            std::cout << "ReturnType()" << std::endl;
        }

        ~ReturnType() noexcept {
            std::cout << "~ReturnType()" << std::endl;
        }
    };

    ReturnType coro_func() {
        co_return;
    }

}

void Coroutines::Simple_Coroutine::TestAll()
{
    ReturnType rt = coro_func();
};