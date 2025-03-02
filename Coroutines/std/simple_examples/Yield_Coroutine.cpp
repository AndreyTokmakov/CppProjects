/**============================================================================
Name        : Yield_Coroutine.cpp
Created on  : 27.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Yield_Coroutine.h
============================================================================**/

#include "Coroutines.h"
#include "Utilities.h"

#include <coroutine>
#include <print>
#include <chrono>
#include <thread>

namespace
{
    using Utilities::getCurrentTime;
}

namespace
{
    using namespace std::string_literals;

    struct ReturnType
    {
        struct promise_type
        {
            std::string output_data { };

            ReturnType get_return_object() noexcept {
                std::println("[{}] \tpromise_type::get_return_object()", getCurrentTime());
                return ReturnType {  *this };
            }

            void return_void() noexcept {
                std::println("[{}] \tpromise_type::return_void()", getCurrentTime());
            }

            /// Will be called from instruction 'co_yield "Hello from the coroutine\n"s;'
            std::suspend_always yield_value(std::string msg) noexcept {
                std::println("[{}] \tpromise_type::yield_value('{}') ==> std::suspend_always()", getCurrentTime(), msg);
                output_data = std::move(msg);
                return {};
            }

            std::suspend_always initial_suspend() noexcept {
                std::println("[{}] \tpromise_type::initial_suspend() ==> std::suspend_always()", getCurrentTime());
                return {};
            }

            std::suspend_always final_suspend() noexcept {
                std::println("[{}] \tpromise_type::final_suspend() ==> std::suspend_always()", getCurrentTime());
                return {};
            }

            void unhandled_exception() noexcept {
                std::println("[{}] \tpromise_type::unhandled_exception()", getCurrentTime());
            }
        };

        std::coroutine_handle<promise_type> handle{};

        explicit ReturnType(promise_type& promise) :
                handle { std::coroutine_handle<promise_type>::from_promise(promise)}
        {
            std::println("[{}] ReturnType::ReturnType()", getCurrentTime());
        }

        ~ReturnType() noexcept
        {
            if (handle) {
                handle.destroy();
            }
            std::println("[{}] ReturnType::~ReturnType()", getCurrentTime());
        }

        [[nodiscard]]
        std::string getValue() const noexcept
        {
            std::println("[{}] ReturnType::getValue()", getCurrentTime());
            if (!handle.done()) {
                handle.resume();
            }
            return std::move(handle.promise().output_data);
        }
    };

    ReturnType createCoroutine()
    {
        std::println("[{}] createCoroutine() entered", getCurrentTime());

        co_yield "Hello from the coroutine"s;

        std::println("[{}] createCoroutine() after co_yield ..", getCurrentTime());

        co_return;
    }

};


void Coroutines::Simple::Yield_Coroutine::TestAll()
{
    ReturnType rt = createCoroutine();
    std::println("[{}] main function()", getCurrentTime());
    const auto returnedValue =  rt.getValue();

    std::println("[{}] main returned value: {}", getCurrentTime(),returnedValue);
}