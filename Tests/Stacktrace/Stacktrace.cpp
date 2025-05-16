/**============================================================================
Name        : Stacktrace.cpp
Created on  : 16.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++Stacktrace
============================================================================**/

#include "Stacktrace.h"

#include <iostream>
#include <stacktrace>
#include <format>
#include <print>

namespace Stacktrace::experiments
{
    void testSimple()
    {
        auto stacktrace = std::stacktrace::current();
        for (const auto& entry: stacktrace) {
            std::cout
                << "description: " << entry.description()
                << " | source_file: " << entry.source_file()
                << " | source_line: " << entry.source_line()
                << " | native_handle: " << entry.native_handle()
                << std::endl;
        }
    }

    void foo()
    {
        auto trace = std::stacktrace::current();
        for (const auto& entry: trace) {
            std::cout << "Description: " << entry.description() << std::endl;
            std::cout << "file: " << entry.source_file() << std::endl;
            std::cout << "line: " << entry.source_line() << std::endl;
            std::cout << "------------------------------------" << std::endl;
        }
    }

    void Test2()
    {
        foo();
    }

}


namespace Stacktrace::exceptions
{
    void check(const int n)
    {
        if ((n & 1) == 1) {
            throw std::runtime_error("That's odd\n" + std::to_string(std::stacktrace::current()));
        }
    }

    void run_check(const int n = 0)
    {
        check(n);
        check(n + 1);

        /**
        terminate called after throwing an instance of 'std::runtime_error'
          what():  That's odd
           0# Stacktrace::TestAll() at :0
           1# main at :0
           2# __libc_start_call_main at ../sysdeps/nptl/libc_start_call_main.h:58
           3# __libc_start_main_impl at ../csu/libc-start.c:392
           4# _start at :0
           5#
        **/
    }
}

namespace Stacktrace::exceptions2
{
    struct stack_runtime_error final : public std::runtime_error
    {
        explicit stack_runtime_error(const std::string& what_arg,
                                     const std::stacktrace& trace = std::stacktrace::current())
            : std::runtime_error(std::format("{}\n{}", what_arg, trace)) {

        }
    };

    void check(const int n)
    {
        if ((n & 1) == 1) {
            throw stack_runtime_error("That's odd");
        }
    }

    void run_check(const int n = 0)
    {
        check(n);
        check(n + 1);

        /**
        terminate called after throwing an instance of 'Stacktrace::exceptions2::stack_runtime_error'
        what():  That's odd
        0# Stacktrace::exceptions2::check(int) [clone .cold] at :0
        1# main at :0
        2# __libc_start_call_main at ../sysdeps/nptl/libc_start_call_main.h:58
        3# __libc_start_main_impl at ../csu/libc-start.c:392
        4# _start at :0
        5#
        **/
    }
}


namespace Stacktrace::exceptions3
{
#define dynamic_assert(...) dynamic_assert_impl(std::stacktrace::current(), __VA_ARGS__)

    struct stack_runtime_error final : public std::runtime_error
    {
        explicit stack_runtime_error(const std::string& what_arg,
                                     const std::stacktrace& trace = std::stacktrace::current())
            : std::runtime_error(std::format("{}\n{}", what_arg, trace)) {

        }
    };

    template<class... Args>
    void dynamic_assert_impl(const std::stacktrace& st,
                             const bool cond,
                             std::format_string<Args...> fmt, Args&&... args) {
        if (!cond) {
            throw stack_runtime_error(std::format(fmt, std::forward<Args>(args)...), st);
        }
    }

    void run_check(int x)
    {
        dynamic_assert(x >= 0, "Negative input: {}", x);

        /** terminate called after throwing an instance of 'Stacktrace::exceptions3::stack_runtime_error'
            what():  Negative input: -1
            0# Stacktrace::TestAll() at :0
            1# main at :0
            2# __libc_start_call_main at ../sysdeps/nptl/libc_start_call_main.h:58
            3# __libc_start_main_impl at ../csu/libc-start.c:392
            4# _start at :0
            5#                                                                                        **/
    }
}

void Stacktrace::TestAll()
{
    // experiments::testSimple();
    // experiments::Test2();

    // exceptions::run_check();
    // exceptions2::run_check();
    exceptions3::run_check(-1);
}