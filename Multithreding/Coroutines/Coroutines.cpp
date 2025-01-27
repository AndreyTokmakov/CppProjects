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
#include <generator>
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

                struct awaiter
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

namespace Coroutines::Fibonacci_Sequence_Generator
{
    std::generator<int> fibonacci_generator()
    {
        int a { 0 }, b{  1 };
        while (true) {
            co_yield a;
            int c = a + b;
            a = b;
            b = c;
        }
    }

    std::generator<int> fibonacci_generator(int limit)
    {
        int a { 0 }, b{  1 };
        while  (limit--) {
            co_yield a;
            int c = a + b;
            a = b;
            b = c;
        }
    }

    void Test()
    {
        std::generator<int> fib = fibonacci_generator();

        int i = 0;
        for (auto f = fib.begin(); f != fib.end(); ++f) {
            if (i == 10) {
                break;
            }
            std::cout << *f << " ";
            ++i;
        }
        std::cout << std::endl;

        for (int f : fibonacci_generator(10)) {
            std::cout << f << " ";
        }
    }
}

namespace Coroutines::Fibonacci_Sequence_Generator_Ex
{
    using namespace std::string_literals;

    template <typename Out>
    struct SequenceGenerator
    {
        struct promise_type
        {
            Out output_data { };

            SequenceGenerator get_return_object() noexcept {
                return SequenceGenerator{ *this };
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
        };

        std::coroutine_handle<promise_type> handle{};

        explicit SequenceGenerator(promise_type& promise) :
            handle { std::coroutine_handle<promise_type>::from_promise(promise) } {
        }

        ~SequenceGenerator() noexcept
        {
            if (handle) {
                handle.destroy();
            }
        }

        void next() {
            if (!handle.done()) {
                handle.resume();
            }
        }

        int64_t value() {
            return handle.promise().output_data;
        }
    };

    SequenceGenerator<int64_t> fibonacci()
    {
        int64_t a{ 0 };
        int64_t b{ 1 };
        int64_t c{ 0 };

        while (true) {
            co_yield a;
            c = a + b;
            a = b;
            b = c;
        }
    }


    void Test()
    {
        SequenceGenerator<int64_t> fib = fibonacci();

        std::cout << "Generate ten Fibonacci numbers\n"s;

        for (int i = 0; i < 10; ++i) {
            fib.next();
            std::cout << fib.value() << " ";
        }
        std::cout << std::endl;

        std::cout << "Generate ten more\n"s;

        for (int i = 0; i < 10; ++i) {
            fib.next();
            std::cout << fib.value() << " ";
        }
        std::cout << std::endl;

        std::cout << "Let's do five more\n"s;

        for (int i = 0; i < 5; ++i) {
            fib.next();
            std::cout << fib.value() << " ";
        }
        std::cout << std::endl;

    }
}



void Coroutines::TestAll()
{
    // SimpleCoroutine::Test();
    // Yield_Coroutine::Test();
    // Waiting_Coroutine::Test();

    // Fibonacci_Sequence_Generator::Test();
    Fibonacci_Sequence_Generator_Ex::Test();
}