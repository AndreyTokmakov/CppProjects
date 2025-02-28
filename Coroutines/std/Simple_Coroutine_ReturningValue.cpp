/**============================================================================
Name        : Simple_Coroutine_ReturningValue.cpp
Created on  : 28.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Simple Coroutine returning value
============================================================================**/

#include "Coroutines.h"

#include <coroutine>
#include <print>

namespace
{
    struct MyCoroutine
    {
        struct promise_type
        {
            int value;

            MyCoroutine get_return_object()
            {
                std::println("\t0. get_return_object");
                return MyCoroutine {this };
            }

            std::suspend_never initial_suspend()
            {
                std::println("\t1. initial_suspend");
                return {};
            }

            std::suspend_never final_suspend() noexcept
            {
                std::println("\t4. final_suspend");
                return {};
            }

            void return_value(const int v)
            {
                std::println("\t3. return_value");
                value = v;
            }

            void unhandled_exception()
            {
                std::terminate();
            }
        };

        promise_type* promise;

        explicit MyCoroutine(promise_type* p) : promise(p) {
            std::println("MyCoroutine()");
        }

        ~MyCoroutine() {
            std::println("~MyCoroutine()");
        }

        [[nodiscard]]
        int get() const {
            std::println("MyCoroutine::get()");
            return promise->value;
        }
    };

    MyCoroutine computeValue() {
        co_return 42;
    }

}

/// In this example, the coroutine computeValue returns an integer value using co_return.
/// The promise type manages the returned value, and the get_return_object method returns a ReturnValue
/// object that the caller can use to retrieve the result.

void Coroutines::Simple_Coroutine_ReturningValue::TestAll()
{
    std::println("Start of coroutine");
    const MyCoroutine result = computeValue();
    std::println("Computed Value: {}\nDone", result.get() );
}
