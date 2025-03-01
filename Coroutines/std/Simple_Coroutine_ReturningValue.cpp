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
    struct ReturnValue
    {
        struct promise_type
        {
            int value { 0 };

            ReturnValue get_return_object() {
                return ReturnValue { this };
            }

            std::suspend_never initial_suspend() {
                return {};
            }

            std::suspend_never final_suspend() noexcept {
                return {};
            }

            void return_value(const int val) {
                value = val;
            }

            void unhandled_exception() {
                std::terminate();
            }
        };

        promise_type* promise;

        explicit ReturnValue(promise_type* p) : promise(p) {
        }

        [[nodiscard]]
        int get() const {
            return promise->value;
        }
    };

    ReturnValue computeValue() {
        co_return 42;
    }
}

/// In this example, the coroutine computeValue returns an integer value using co_return.
/// The promise type manages the returned value, and the get_return_object method returns a ReturnValue
/// object that the caller can use to retrieve the result.

void Coroutines::Simple_Coroutine_ReturningValue::TestAll()
{
    std::println("Start of coroutine");
    ReturnValue result = computeValue();
    std::println("Computed Value: {}\nDone", result.get() );
}
