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

namespace
{
    template<typename ...Args>
    void print(Args&&... args) {
        (std::cout << ... << std::forward<Args>(args)) << std::endl;
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
        MyCoroutine coroutine = p.get_return_object();

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

void Coroutines::TestAll()
{
    // DemoOne::test();

    // DemoTwo::test();
    // DemoTwo::test2();
    DemoTwo::testFixed();

    // DemoThree::test();
};

