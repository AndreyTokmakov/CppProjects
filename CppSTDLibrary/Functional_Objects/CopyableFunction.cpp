/**============================================================================
Name        : CopyableFunction.cpp
Created on  : 24.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CopyableFunction.cpp
============================================================================**/

#include "FunctionObjects.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <print>



namespace
{
    void demo()
    {
        class  Counter
        {
            int state { 0 };

        public:

            int operator()() {
                return ++state;
            }
        };

        // copyable_function<int()> means operator() is non-const
        std::copyable_function<int()> f = Counter{};
        f(); // OK, non-const call

        // If you want const, you say so explicitly:
        //
        //std::copyable_function<int() const> g = Counter{}; // ERROR!
        // Counter::operator() is not const-qualified

        std::copyable_function<int() const> const h = [] {
            return 42;
        };
    }
}

void FunctionObjects::CopyableFunction::TestAll()
{
    demo();
}
