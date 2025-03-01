/**============================================================================
Name        : Coroutines.cpp
Created on  : 07.07.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Coroutines
============================================================================**/

#include "Coroutines.h"


#include <iostream>
#include <string_view>
#include <format>
#include <coroutine>
#include <thread>
#include <print>
#include <mutex>
#include <thread>
#include <generator>

namespace
{
    template<typename ...Args>
    void print(Args&&... args) {
        (std::cout << ... << std::forward<Args>(args)) << std::endl;
    }
}


namespace Coroutines::Generators
{
    std::generator<int> generateNumbers(int begin, int end)
    {
        for (int i = begin; i < end; ++i) {
            co_yield i;
        }
    }

    void TestGenerator()
    {
        std::generator<int> numbers = generateNumbers(0, 10);

        std::cout << std::string(160, '=') << std::endl;
        for (int counter = 0; int n: numbers) {
            std::print("{0} ", n);
            if (++counter >= 5)
                break;
        }
        std::cout << '\n' << std::string(160, '=') << std::endl;
        for (int counter = 0; int n: numbers) {
            std::print("{0} ", n);
            if (++counter >= 5)
                break;
        }
        std::cout << '\n' <<std::string(160, '=') << std::endl;

    }
}

namespace SimpleExample0
{
    struct Task
    {
        struct promise_type
        {
            Task get_return_object() { return {}; }
            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void return_void() {}
            void unhandled_exception() {}
        };
    };

    Task create_task()
    {
        std::cout << "Coroutine start\n";
        co_await std::suspend_always{};
        std::cout << "Coroutine resumed\n";
    }

    void Test()
    {
        Task r = create_task();
        std::cout << "Main function continues\n";
    }
}

namespace SimpleExample
{
    struct ReturnType
    {
        struct promise_type
        {
            // the compiler looks for a type with the exact name promise_type inside the return type
            std::suspend_never initial_suspend()
            {
                std::cout << "initial_suspend" << std::endl;
                return std::suspend_never {};
            }

            // gets executed before a coroutine starts execution
            std::suspend_never final_suspend() noexcept
            {
                std::cout << "final_suspend" << std::endl;
                return std::suspend_never{};
            }

            // gets executed when a coroutine finishes execution
            ReturnType get_return_object()
            {
                std::cout << "get_return_object" << std::endl;
                return ReturnType{};
            }

            // this is the first method gets called when the coroutine is called for the first time
            void unhandled_exception()
            {

            }
        };
    };

    ReturnType foo()
    {
        std::cout << "1 foo\n";
        co_await std::suspend_always(); // suspend
        std::cout << "2 foo\n"; // will never execute since coroutine suspends in the above line
    }

    void Test()
    {
        ReturnType r = foo();
    }
}

namespace SimpleExample2
{
    struct ReturnType
    {
        struct promise_type
        {
            // the compiler looks for a type with the exact name promise_type inside the return type
            std::suspend_never initial_suspend()
            {
                std::cout << "initial_suspend" << std::endl;
                return std::suspend_never {};
            }

            // gets executed before a coroutine starts execution
            std::suspend_never final_suspend() noexcept
            {
                std::cout << "final_suspend" << std::endl;
                return std::suspend_never {};
            }

            // gets executed when a coroutine finishes execution
            ReturnType get_return_object() {
                return ReturnType(std::coroutine_handle<promise_type>::from_promise(*this));
            }

            // this is the first method gets called when the coroutine is called for the first time
            void unhandled_exception()
            {

            }
        };

        explicit ReturnType(std::coroutine_handle<void> handle) : mHandle{handle} {
        }
        std::coroutine_handle<void> mHandle;
    };

    ReturnType foo()
    {
        std::cout << "1 foo\n";
        co_await std::suspend_always(); // suspend
        std::cout << "2 foo\n"; // will never execute since coroutine suspends in the above line
    }

    void Test()
    {
        ReturnType r = foo();
        r.mHandle.resume(); // equivalent to r.mHandle();
    }
}

namespace SimpleExample3
{
    struct Awaitable
    {
        bool await_ready() const noexcept {
            return false;
        }

        void await_suspend(std::coroutine_handle<> ) const noexcept {
            std::this_thread::sleep_for(std::chrono::seconds (1UL));
        }

        void await_resume() const noexcept {
        }
    };

    std::mutex mtx;

    struct Task
    {
        struct promise_type
        {
            promise_type() = default;
            // promise_type(std::mutex& m): mtx {m} {}

            std::suspend_always initial_suspend()
            {
                std::cout << "Acquiring lock (initial_suspend)\n";
                mtx.lock();
                std::cout << "Acquiring lock (exit)\n";
                return {};
            }

            std::suspend_always final_suspend() noexcept
            {
                std::cout << "Releasing lock (final_suspend)\n";
                mtx.unlock();
                return {};
            }

            Task get_return_object()
            {
                return Task {};
            }

            void return_void() {}
            void unhandled_exception() {}
        };

        Task operator co_await() {
            return *this;
        }
    };

    Task async_task()
    {
        std::cout << "Starting the async task ....\n";
        co_await Awaitable (); // Simulating some IO operation (explicit suspension)
        co_return;
    }

    void Test()
    {
        std::jthread t1([]{ async_task(); }), t2([]{ async_task(); });
    }
}




namespace Coroutines::DemoOne
{
    struct Task
    {
        // The coroutine level type
        struct promise_type {
            Task get_return_object() {
                return {};
            }

            std::suspend_never initial_suspend() {
                return {};
            }

            std::suspend_never final_suspend() noexcept {
                return {};
            }

            void return_void() {}

            void unhandled_exception() {}
        };
    };

    Task myCoroutine() {
        co_return; // make it a coroutine
    }

    void test()
    {
        [[maybe_unused]]
        Task x = myCoroutine();
    }
}

namespace Coroutines::DemoTwo
{
    struct MyCoroutine
    {
        struct promise_type
        {
            MyCoroutine get_return_object() {
                return MyCoroutine{std::coroutine_handle<promise_type>::from_promise(*this)};
            }

            [[nodiscard]]
            std::suspend_always initial_suspend() const noexcept {
                return {};
            }

            [[nodiscard]]
            std::suspend_never final_suspend() const noexcept {
                return {};
            }

            void return_void() {}
            void unhandled_exception() {}
        };

        explicit MyCoroutine(std::coroutine_handle<promise_type> handle) : handle { handle } {
        }

        void destroy() const {
            print("MyCoroutine::destroy()");
            handle.destroy();
        }

        void resume() const {
            print("MyCoroutine::resume()");
            handle.resume();
        }

        std::coroutine_handle<promise_type> handle;
    };

    MyCoroutine myCoroutine() {
        co_return; /** make it a coroutine **/
    }

    MyCoroutine simpleCoroutine()
    {
        print("* * * Start coroutine * * *");
        co_await std::suspend_always{};
        print("* * * Resume coroutine * * * ");
    }

    MyCoroutine simpleCoroutineFixed()
    {
        MyCoroutine::promise_type p {};
        [[maybe_unused]]  MyCoroutine coroutine = p.get_return_object();

        try {
            co_await p.initial_suspend();
            print("* * * Start coroutine * * *");
            co_await std::suspend_always{};
            print("* * * Resume coroutine * * * ");
        } catch(...) {
            print(" --> got the unhandled_exception <-- ");
            p.unhandled_exception();
        }
        co_await p.final_suspend();
    }

    void test()
    {
        [[maybe_unused]]
        MyCoroutine c = myCoroutine();
        c.resume();
        // c.destroy();
    }

    void test2()
    {
        MyCoroutine coroutine = simpleCoroutine();
        print("Coroutine is not executed yet");

        coroutine.resume();
        print("Suspend coroutine");
        coroutine.resume();

        coroutine.destroy();
    }

    void testFixed()
    {
        MyCoroutine coroutine = simpleCoroutineFixed();
        print("Coroutine is not executed yet");

        coroutine.resume();
        print("Suspend coroutine");
        coroutine.resume();

        coroutine.destroy();
    }
}

namespace Coroutines::DemoThree
{
    // The caller-level type
    struct Generator
    {
        // The coroutine level type
        struct promise_type
        {
            int current_value {0};
            using Handle = std::coroutine_handle<promise_type>;

            Generator get_return_object() {
                return Generator{Handle::from_promise(*this)};
            }

            [[nodiscard]]
            std::suspend_always initial_suspend() const noexcept {
                return {};
            }

            [[nodiscard]]
            std::suspend_never final_suspend() const noexcept {
                return {};
            }

            [[nodiscard]]
            std::suspend_always yield_value(int value) noexcept {
                current_value = value;
                return {};
            }

            void unhandled_exception() { }
        };
        explicit Generator(promise_type::Handle coro) : coro_(coro) {
            // ...
        }

        ~Generator() {
            if (coro_)
                coro_.destroy();
        }

        // Make move-only
        Generator(const Generator&) = delete;
        Generator& operator=(const Generator&) = delete;

        Generator(Generator&& t) noexcept : coro_(t.coro_) {
            t.coro_ = {};
        }

        Generator& operator=(Generator&& t) noexcept {
            if (this == &t) return *this;
            if (coro_) coro_.destroy();
            coro_ = t.coro_;
            t.coro_ = {};
            return *this;
        }

        int get_next() {
            coro_.resume();
            return coro_.promise().current_value;
        }

    private:
        promise_type::Handle coro_;
    };

    Generator myCoroutine()
    {
        int x = 0;
        while (true) {
            co_yield x++;
        }
    }

    void test()
    {
        Generator c = myCoroutine();
        int x = 0;
        while ((x = c.get_next()) < 10) {
            std::cout << x << "\n";
        }
    }
}

namespace Coroutines::TTT
{
    struct MyCoroutine
    {
        struct promise_type
        {
            MyCoroutine get_return_object() {
                return std::coroutine_handle<promise_type>::from_promise(*this);
            }
            std::suspend_always initial_suspend() {
                return {};
            }
            std::suspend_always final_suspend() noexcept {
                return {};
            }
            void return_void() {}
            void unhandled_exception() {}
        };

        MyCoroutine(std::coroutine_handle<promise_type> handle): handle{handle} {
        }

        void resume() {
            handle.resume();
        }
        void destroy() {
            handle.destroy();
        }

        std::coroutine_handle<promise_type> handle;
    };
}


namespace DemoFour
{
    auto switch_to_new_thread(std::jthread& out)
    {
        struct awaitable
        {
            std::jthread* p_out;

            bool await_ready() { return false; }

            void await_suspend(std::coroutine_handle<> h)
            {
                std::jthread& out = *p_out;
                if (out.joinable())
                    throw std::runtime_error("Output jthread parameter not empty");
                out = std::jthread([h] { h.resume(); });
                // Potential undefined behavior: accessing potentially destroyed *this
                // std::cout << "New thread ID: " << p_out->get_id() << '\n';
                std::cout << "New thread ID: " << out.get_id() << '\n'; // this is OK
            }

            void await_resume() {}
        };
        return awaitable{&out};
    }

    struct task
    {
        struct promise_type
        {
            task get_return_object() { return {}; }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            void return_void() {}
            void unhandled_exception() {}
        };
    };

    task resuming_on_new_thread(std::jthread& out)
    {
        std::cout << "Coroutine started on thread: " << std::this_thread::get_id() << '\n';
        co_await switch_to_new_thread(out);
        // awaiter destroyed here
        std::cout << "Coroutine resumed on thread: " << std::this_thread::get_id() << '\n';
    }

    void Test()
    {
        std::jthread out;
        resuming_on_new_thread(out);
    }
}


namespace Coroutines::Example4
{
    // https://www.alibabacloud.com/blog/a-comprehensive-discussion-on-c%2B%2B20-coroutines_600889


    template <bool ReadyFlag>
    struct Awaiter
    {
        bool await_ready() noexcept
        {
            std::cout << "await_ready: " << std::boolalpha << ReadyFlag << std::endl;
            return ReadyFlag;
        }
        void await_resume() noexcept
        {
            std::cout << "await_resume" << std::endl;
        }
        void await_suspend(std::coroutine_handle<>) noexcept
        {
            std::cout << "await_suspend" << std::endl;
        }
    };

    struct TaskPromise
    {
        struct promise_type
         {
            TaskPromise get_return_object()
            {
                std::cout << "get_return_object" << std::endl;
                return TaskPromise{std::coroutine_handle<promise_type>::from_promise(*this)};
            }

            Awaiter<true> initial_suspend() noexcept
            {
                std::cout << "TaskPromise::promise_type::initial_suspend" << std::endl;
                return {};
            }

            Awaiter<true> final_suspend() noexcept
            {
                std::cout << "final_suspend" << std::endl;
                return {};
            }

            void unhandled_exception()
            {
                std::cout << "unhandled_exception" << std::endl;
            }

            void return_void() noexcept
            {
                std::cout << "return_void" << std::endl;
            }
        };

        void resume()
        {
            std::cout << "resume" << std::endl;
            handle.resume();
        }

        std::coroutine_handle<promise_type> handle;
    };

    TaskPromise task_func()
    {
        std::cout << "task first run" << std::endl;
        co_await Awaiter<false>{};
        std::cout << "task resume" << std::endl;
    }

    void test()
    {
        auto promise = task_func();
        promise.resume();

        /**  OUTPUT:
        get_return_object
        initial_suspend
        await_ready: 1
        await_resume
        task first run
        await_ready: 0
        await_suspend
        resume
        await_resume
        task resume
        return_void
        final_suspend
        await_ready: 1
        await_resume
         */
    }
}



void Coroutines::TestAll()
{
    // SimpleExample0::Test();
    // SimpleExample::Test();
    // SimpleExample2::Test();
    SimpleExample3::Test();

    // Example4::test();


    // DemoOne::test();
    // DemoTwo::test();
    // DemoTwo::test2();
    // DemoTwo::testFixed();
    // DemoThree::test();
    // DemoFour::Test();

    // Generators::TestGenerator();



};

