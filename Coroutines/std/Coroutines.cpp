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
#include <semaphore>
#include <chrono>
#include <coroutine>
#include <generator>
#include <utility>

namespace Coroutines::String_to_Integer_Parser
{
    using namespace std::string_literals;
    using namespace std::chrono_literals;

    template <typename Out, typename In>
    struct AsyncParser
    {
        struct promise_type
        {
            std::optional<In> input_data { };
            Out output_data { };

            AsyncParser get_return_object() noexcept {
                return AsyncParser { *this };
            }

            void return_void() noexcept {
            }

            std::suspend_always initial_suspend() noexcept {
                return {};
            }

            std::suspend_always final_suspend() noexcept {
                return {};
            }

            void unhandled_exception() noexcept {
            }

            std::suspend_always yield_value(int64_t num) noexcept {
                output_data = num;
                return {};
            }

            auto await_transform(char) noexcept {
                struct awaiter {
                    promise_type& promise;

                    [[nodiscard]] bool await_ready() const noexcept {
                        return promise.input_data.has_value();
                    }

                    [[nodiscard]] char await_resume() const noexcept {
                        //assert (promise.input_data.has_value());
                        return *std::exchange(promise.input_data, std::nullopt);
                    }

                    void await_suspend(std::coroutine_handle<promise_type>) const noexcept {
                    }
                };

                return awaiter(*this);

            }
        };

        std::coroutine_handle<promise_type> handle{};

        explicit AsyncParser(promise_type& promise):
                handle{ std::coroutine_handle<promise_type>::from_promise(promise)} {
        }

        ~AsyncParser() noexcept
        {
            if (handle) {
                handle.destroy();
            }
        }

        void put(char c) {
            handle.promise().input_data = c;
            if (!handle.done()) {
                handle.resume();
            }
        }

        int64_t get() {
            if (!handle.done()) {
                handle.resume();
            }
            return handle.promise().output_data;
        }
    };

    AsyncParser<int64_t, char> parseStringAsynch()
    {
        while (true)
        {
            char c = co_await char{ };
            int64_t number { }, sign { 1 };

            if (c != '-' && c != '+' && !std::isdigit(c)) {
                continue;
            }

            if (c == '-') {
                sign = -1;
            }
            else if (std::isdigit(c)) {
                number = number * 10 + c - '0';
            }

            while (true) {
                c = co_await char{};
                if (std::isdigit(c)) {
                    number = number * 10 + c - '0';
                }
                else {
                    break;
                }
            }

            co_yield number * sign;
        }
    }

    int64_t parseString(const std::string& str)
    {
        int64_t num { 0 }, sign { 1 };

        std::size_t c = 0;
        while (c < str.size()) {
            if (str[c] == '-') {
                sign = -1;
            }
            else if (std::isdigit(str[c])) {
                num = num * 10ll + (str[c] - '0');
            }
            else if (str[c] == '#') {
                break;
            }
            ++c;
        }

        return num * sign;
    }

    void Test()
    {
        const std::string num1 = "-123454321#"s;
        const std::string num2 = "-123454321#98765#-20241337#+31415#"s;

        // parse full string
        std::cout << parseString(num1) << std::endl;

        // parse the first half of the string
        const std::size_t sz = num1.size();
        AsyncParser<int64_t, char> num_parser = parseStringAsynch();
        for (std::size_t i = 0; i < sz / 2; ++i) {
            num_parser.put(num1[i]);
        }

        std::cout << "parsing number...\n"s;

        // parse the second half of the string
        for (std::size_t i = sz / 2; i < sz; ++i) {
            num_parser.put(num1[i]);
        }

        // print the parsed number
        std::cout << num_parser.get() << std::endl;

        // now let's parse a string containing several numbers
        for (char c : num2)
        {
            num_parser.put(c);

            /// do something...

            std::this_thread::sleep_for(std::chrono::milliseconds(100u));

            std::cout << num_parser.get() << std::endl;
            /*
            if (c == '#') {
                std::cout << num_parser.get() << std::endl;
            }*/
        }
    }
}

namespace Coroutines::UseCases
{
    struct Task
    {
        struct promise_type
        {
            Task get_return_object() noexcept
            {
                std::cout << "get_return_object" << std::endl;
                return Task { *this };
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

        explicit Task(promise_type&) {
            std::cout << "Task()" << std::endl;
        }

        ~Task() noexcept {
            std::cout << "~Task()" << std::endl;
        }
    };


    std::vector<int> data = {1, 2, 3, 4, 5};

    Task perform_heavy_computation()
    {
        std::cout << "perform_heavy_computation()" << std::endl;
        for (int i : data) {
            // Perform heavy computation on data[i] asynchronously
            std::cout << "Processing " << i << std::endl;
            co_await std::suspend_always {};
            std::cout << "Processed " << i << std::endl;
        }
    }

    void Task_Based_Parallelism()
    {
        std::cout << "Task_Based_Parallelism()" << std::endl;
        Task task = perform_heavy_computation();
    }
}

// https://medium.com/@AlexanderObregon/understanding-c-coroutine-implementation-8e6e5a2c3edd
// https://www.youtube.com/watch?v=V6UAO6niuYM

// https://habr.com/ru/articles/519464/

// C++20 Coroutine: Under The Hood
// https://dev.to/visheshpatel/c-20-coroutine-under-the-hood-3071?ysclid=m7qi746mr7506895977


void Coroutines::TestAll()
{
    // Simple::Coroutine_Lifecycle_CoAwait::TestAll();
    // Simple::Coroutine_Lifecycle_CoReturn::TestAll();

    // Simple::Awaiter_Lifecycle_Steps::TestAll();

    // Simple::Returning_Coroutine::TestAll();
    // Simple::Returning_Coroutine_2::TestAll();

    // Simple::Resuming_Coroutine_1::TestAll();

    // Simple::Awaiter_and_Awaitable::TestAll();
    // Simple::Waitable_Coroutine::TestAll();
    // Simple::Waitable_Coroutine_2::TestAll();
    // Simple::Waitable_Coroutine_Update_Promise_State::TestAll();

    // Simple::Yield_Coroutine::TestAll();
    // Simple::Yield_Coroutine_Values_from_List::TestAll();

    // Simple::Multiple_Awaiters_Resolution::TestAll();
    // Simple::Multiple_Awaiters_Resolution_2::TestAll();

    // Generators::TestAll();

    // Experiments::TestAll();
    // Experiments::Waitable_Coroutine_With_Mutex::TestAll();
    // Experiments::Calculating_Average::TestAll();
    // Experiments::PinBall_Game::TestAll();
    Experiments::Event_Processor::TestAll();
    // Experiments::Generic_TaskBased_Coroutine::TestAll();
    // Experiments::FileReader::TestAll();        // <------------- Not working
    // Experiments::TaskCoordination::TestAll();     // <------------- Not working

    // String_to_Integer_Parser::Test();
}