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

namespace Coroutines
{
    namespace demo_one
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

    namespace demo_two
    {
        struct Task
        {
            // The coroutine level type
            struct promise_type {
                using Handle = std::coroutine_handle<promise_type>;

                Task get_return_object() {
                    return Task{Handle::from_promise(*this)};
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

            explicit Task(promise_type::Handle coro) : coro_{coro} {
                // ....
            }

            void destroy() {
                coro_.destroy();
            }

            void resume() {
                std::cout << "demo_two::Task::resume()\n";
                coro_.resume();
            }

        private:
            promise_type::Handle coro_;
        };

        Task myCoroutine() {
            co_return; // make it a coroutine
        }

        void test()
        {
            [[maybe_unused]]
            Task c = myCoroutine();
            c.resume();
            // c.destroy();
        }
    }

    namespace demo_three
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
            auto c = myCoroutine();
            int x = 0;
            while ((x = c.get_next()) < 10) {
                std::cout << x << "\n";
            }
        }
    }
}


void Coroutines::TestAll()
{
    // demo_one::test();
    // demo_two::test();
    demo_three::test();
};

