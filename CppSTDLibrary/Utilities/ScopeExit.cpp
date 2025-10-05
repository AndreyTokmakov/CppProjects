/**============================================================================
Name        : ScopeExit.cpp
Created on  : 05.10.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ScopeExit
============================================================================**/

#include <print>
#include <experimental/scope>

#include "Utilities.hpp"

namespace
{
    void task(const bool fail)
    {
        std::println("task() function entered. Will fail = {}", fail);

        std::experimental::scope_exit onExit { [] {
           std::println("Cleaning up");
        }};

        if (fail) {
            throw std::runtime_error("Task failed");
        }

        std::println("task() function exit");
    }

    void test(const bool fail)
    {
        try {
            task(fail);
        }
        catch (const std::exception& exception) {
            std::println("Caught exception: {}", exception.what());
        }
    }

}

void Utilities::ScopeExit::TestAll()
{
    test(true);
    std::println("{}", std::string(180, '-'));
    test(false);

    /**
    task() function entered. Will fail = true
    Cleaning up
    Caught exception: Task failed
    ---------------------------------------------------------------------------------------------------------
    task() function entered. Will fail = false
    task() function exit
    Cleaning up
    **/
}
