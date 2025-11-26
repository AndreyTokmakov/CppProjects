/**============================================================================
Name        : Testing.hpp
Created on  : 26.11.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Testing.hpp
============================================================================**/

#ifndef CPPPROJECTS_TESTING_HPP
#define CPPPROJECTS_TESTING_HPP

#include <expected>
#include <iostream>
#include <source_location>
#include <print>

namespace testing
{
    enum class Action
    {
        Continue,
        Terminate
    };

    constexpr void performAction(const Action action)
    {
        if ( Action::Terminate == action)
        {
            std::terminate();
        }
    }

    // TODO: Add concepts to 'Ty'
    template<typename Ty>
    constexpr void AssertEqual(const Ty& expected,
                               const Ty& actual,
                               const Action action = Action::Terminate,
                               const std::source_location& location = std::source_location::current())
    {
        if (expected != actual)
        {
            std::println(std::cerr, "Assertation failed: {} != {}\n\tFile: {}\n\tFunction: {}\n\tLine: {}",
                expected, actual, location.file_name(), location.function_name(), location.line());
            performAction(action);
        }
    }

    // TODO: Add concepts to 'Ty'
    template<typename Ty>
    constexpr void AssertEqual(const Ty& expected,
                               const Ty& actual,
                               const std::string_view message,
                               const Action action = Action::Terminate,
                               const std::source_location& location = std::source_location::current())
    {
        if (expected != actual)
        {
            std::println(std::cerr, "{}: {} != {}\n\tFile: {}\n\tFunction: {}\n\tLine: {}",
                message, expected, actual, location.file_name(), location.function_name(), location.line());
            performAction(action);
        }
    }

    // TODO: Add concepts to 'Ty'
    template<typename Ty>
    constexpr void AssertNotNull(const Ty* actual,
                                 const Action action = Action::Terminate,
                                 const std::source_location& location = std::source_location::current())
    {
        if (nullptr == actual)
        {
            std::println(std::cerr, "Assertion failed (Actual value is null)\n\tFile: {}\n\tFunction: {}\n\tLine: {}",
                location.file_name(), location.function_name(), location.line());
            performAction(action);
        }
    }

    // TODO: Add concepts to 'Ty'
    template<typename Ty>
    constexpr void AssertNotNull(const Ty* actual,
                                 const std::string_view message,
                                 const Action action = Action::Terminate,
                                 const std::source_location& location = std::source_location::current())
    {
        if (nullptr == actual)
        {
            std::println(std::cerr, "{}\n\tFile: {}\n\tFunction: {}\n\tLine: {}",
                message, location.file_name(), location.function_name(), location.line());
            performAction(action);
        }
    }

    // TODO: Add concepts to 'Ty'
    template<typename Ty>
    constexpr void AssertIsNull(const Ty* actual,
                                const Action action = Action::Terminate,
                                const std::source_location& location = std::source_location::current())
    {
        if (nullptr != actual)
        {
            std::println(std::cerr, "Assertion failed (Actual value is not null)\n\tFile: {}\n\tFunction: {}\n\tLine: {}",
                location.file_name(), location.function_name(), location.line());
            performAction(action);
        }
    }

    // TODO: Add concepts to 'Ty'
    template<typename Ty>
    constexpr void AssertIsNull(const Ty* actual,
                                const std::string_view message,
                                const Action action = Action::Terminate,
                                const std::source_location& location = std::source_location::current())
    {
        if (nullptr != actual)
        {
            std::println(std::cerr, "{}\n\tFile: {}\n\tFunction: {}\n\tLine: {}",
                message, location.file_name(), location.function_name(), location.line());
            performAction(action);
        }
    }

    constexpr void AssertTrue(const bool condition,
                              const Action action = Action::Terminate,
                              const std::source_location& location = std::source_location::current())
    {
        if (!condition)
        {
            std::println(std::cerr, "Assertation failed:\n\tFile: {}\n\tFunction: {}\n\tLine: {}",
                location.file_name(), location.function_name(), location.line());
            performAction(action);
        }
    }

    constexpr void AssertTrue(const bool condition,
                              const std::string_view message,
                              const Action action = Action::Terminate,
                              const std::source_location& location = std::source_location::current())
    {
        if (!condition)
        {
            std::println(std::cerr, "{}:\n\tFile: {}\n\tFunction: {}\n\tLine: {}",
                    message, location.file_name(), location.function_name(), location.line());
            performAction(action);
        }
    }

    constexpr void AssertFalse(const bool condition,
                               const Action action = Action::Terminate,
                               const std::source_location& location = std::source_location::current())
    {
        if (condition)
        {
            std::println(std::cerr, "Condition is True (False expected):\n\tFile: {}\n\tFunction: {}\n\tLine: {}",
                    location.file_name(), location.function_name(), location.line());
            performAction(action);
        }
    }

    constexpr void AssertFalse(const bool condition,
                               const std::string_view message,
                               const Action action = Action::Terminate,
                               const std::source_location& location = std::source_location::current())
    {
        if (condition)
        {
            std::println(std::cerr, "{}:\n\tFile: {}\n\tFunction: {}\n\tLine: {}",
                    message, location.file_name(), location.function_name(), location.line());
            performAction(action);
        }
    }
}

#endif //CPPPROJECTS_TESTING_HPP