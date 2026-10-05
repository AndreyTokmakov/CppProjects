/**============================================================================
Name        : Function_Ref.cpp
Created on  : 05.10.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Function_Ref.cpp
============================================================================**/

#include "FunctionObjects.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <print>

namespace
{
    using payload = std::optional<std::string>;
    using size_type = uint32_t;

    [[maybe_unused]]
    [[nodiscard]]
    payload retryFunPtr(const size_type times,
                        payload (*action)())
    {
        for (size_t i = 0; i < times; ++i) {
            if (const payload result = action()) {
                return result;
            }
        }
        return std::nullopt;
    }

    template <std::invocable Function>
    [[nodiscard]]
    payload retryTemplate(const size_type times,
                          Function&& action)
    {
        for (size_type i = 0; i < times; ++i) {
            if (const payload result = action(); result.has_value()) {
                return result;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]]
    payload retryFunction(const size_type times,
                          const std::function<payload()>& action)
    {
        for (size_type i = 0; i < times; ++i) {
            if (const payload result = action()) {
                return result;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]]
    payload retryFunctionPtr(const size_type times,
                             const std::function_ref<payload()> action)
    {
        for (size_t i = 0; i < times; ++i) {
            if (const payload result = action()) {
                return result;
            }
        }
        return std::nullopt;
    }

    /** =========================================================================== **/

    payload retry(size_type& times, const size_type maxAttempts = 3)
    {
        ++times;
        // std::cout << "retry(" << times << " -> " << times + 1 << ")\n";
        if (maxAttempts > times)
            return std::nullopt;
        return "Success";
    }

    void testRetryByPtr(const size_type times = 3)
    {
        size_type attempts = 0;

        [[maybe_unused]]
        auto action = [&]() -> payload {
            return retry(attempts);
        };

        /*
        const payload res = retryFunPtr(times, action);        // This doesn’t work because the lambda captures attempts.
        std::cout << res.value_or("Failed") << std::endl;  // A stateful lambda cannot be passed where a
        */                                                 // regular function pointer is expected
    }

    void testRetryTemplate(const size_type times = 3)
    {
        size_type attempts = 0;
        auto action = [&]() -> payload {
            return retry(attempts);
        };

        const payload res = retryTemplate(times, action);
        std::cout << res.value_or("Failed") << std::endl;
    }

    void testRetryFunction(const size_type times = 3)
    {
        size_type attempts = 0;
        auto action = [&]() -> payload {
            return retry(attempts);
        };

        const payload res = retryFunction(times, action);
        std::cout << res.value_or("Failed") << std::endl;
    }

    void testRetryFunctionPtr(const size_type times = 3)
    {
        size_type attempts = 0;
        auto action = [&]() -> payload {
            return retry(attempts);
        };

        const payload res = retryFunctionPtr(times, action);
        std::cout << res.value_or("Failed") << std::endl;
    }

    void test_Different_Callback_Types()
    {
        constexpr size_type times = 3;

        testRetryByPtr(times);
        testRetryTemplate(times);
        testRetryFunction(times);
        testRetryFunctionPtr(times);
    }
}


namespace
{
    void task() noexcept
    {
        std::cout << "Fast Task" << std::endl;
    }

    void Function_Ref_Remove_Inderection()
    {
        // We create a function_ref that promises not to throw
        std::function_ref<void() noexcept> funcRef = task;

        // Mirroring core-language function pointer behavior, this conversion is allowed.
        // Today, 'proxyFuncRef' directly copies the internal pointers of 'funcRef', flattening the reference!
        const std::function_ref<void()> proxyFuncRef = funcRef;

        proxyFuncRef(); // Calls task() directly, bypassing 'funcRef' entirely.
    }
}

void FunctionObjects::Function_Ref::TestAll()
{
    // test_Different_Callback_Types();
    Function_Ref_Remove_Inderection();
}
