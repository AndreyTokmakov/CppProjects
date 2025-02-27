/**============================================================================
Name        : Calculating_Average.cpp
Created on  : 27.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Calculating average
============================================================================**/

#include "Calculating_Average.h"


#include <iostream>
#include <string_view>
#include <future>
#include <semaphore>
#include <chrono>
#include <coroutine>
#include <generator>
#include <utility>

namespace
{
    struct [[nodiscard]] Average
    {
        struct promise_type
        {
            Average get_return_object()
            {
                return Average { CoroHandle::from_promise(*this) };
            }

            std::suspend_always initial_suspend() {
                return std::suspend_always{};
            }

            void unhandled_exception() {
                std::terminate();
            }

            void return_value(const int& valueIn) {
                value = valueIn;
            }

            std::suspend_always final_suspend() noexcept {
                return std::suspend_always{};
            }

            int value;
        };


        using CoroHandle = std::coroutine_handle<promise_type>;

        explicit Average(const CoroHandle handle) : coroHandle { handle } {
        }

        ~Average()
        {
            if (coroHandle) {
                coroHandle.destroy();
            }
        }
        Average(const Average&) = delete;
        Average& operator=(const Average&) = delete;

        [[nodiscard]]
        bool calculate() const
        {
            if (!coroHandle || coroHandle.done()) {
                return false; // we are done
            }
            coroHandle.resume();
            return !coroHandle.done();
        }

        [[nodiscard]]
        int getResult() const
        {
            return coroHandle.promise().value;
        }

    private:

        CoroHandle coroHandle;
    };

    Average makeAverage(const std::vector<int>& numbers)
    {
        int sum {0};
        for (const auto& number : numbers)
        {
            std::cout << "Number crunching the next value"<< std::endl;
            sum += number;
            co_await std::suspend_always{};
        }

        std::cout << "Finally, all calculated." << std::endl;
        co_return sum / numbers.size();
    }

}


void Coroutines::Examples::Calculating_Average::TestAll()
{
    const std::vector<int> numbers {100, 200, 100, 200, 100, 200, 100, 200};
    const Average average = makeAverage(numbers);

    while (average.calculate())
    {
        std::cout << "More calculations needed." << std::endl;
    }
    std::cout << "Average : " << average.getResult() << std::endl;
}
