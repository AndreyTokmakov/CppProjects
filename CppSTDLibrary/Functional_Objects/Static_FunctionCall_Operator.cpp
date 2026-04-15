/**============================================================================
Name        : Static_FunctionCall_Operator.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Static_FunctionCall_Operator.cpp
============================================================================**/

#include "FunctionObjects.hpp"

#include <iostream>
#include <vector>
#include <string_view>
#include <ranges>
#include <print>


namespace FunctionObjects::Static_FunctionCall_Operator
{
    void example()
    {
        struct Sanitize
        {
            static std::string operator()(const std::string_view sv) {
                // strip whitespace, normalize encoding, etc.
                return std::string{sv};
            }
        };

        struct IsValid {
            static bool operator()(const std::string_view sv) {
                return !sv.empty() && sv.size() < 256;
            }
        };

        const std::vector<std::string_view> inputs {};

        // Every layer in this pipeline is free of 'this' overhead
        const std::vector<std::string> results = inputs
            | std::views::transform(Sanitize{})
            | std::views::filter(IsValid{})
            | std::ranges::to<std::vector>();
    }


    void functionPointerConversion()
    {
        struct Add {
            static int operator()(const int a, const int b) {
                return a + b;
            }
        };

        // Direct compatibility with C-style APIs
        using BinaryOp = int(*)(int, int);
        const BinaryOp op = &Add::operator();  // Clean, no conversion shim

        std::println("Add {} + {} = {}", 2, 3, op(2, 3) );
    }
}

/**
* The Problem Before C++23
* Functors — classes that overload operator() — are the backbone of the STL.
* But there's always been an invisible cost hiding in plain sight.
*
* // Pre-C++23: operator() must be a non-static member function
* struct IsEven {
*   bool operator()(int n) const {
*       return n % 2 == 0;
*       }
*  };
*
*  Looks harmless. But at the ABI level, the compiler sees this:
*
*  bool operator()(const IsEven* this, int n);  // What actually gets generated
*
*  That this pointer is completely useless — the function never touches it.
*  Yet it occupies a register, bloats calling conventions, and in certain contexts — particularly without LTO,
*  across translation units, or in unoptimized builds — can inhibit optimizations that you'd otherwise take for granted.
**/


// INFO : https://medium.com/@sagar.necindia/cpp23-static-operator-zero-cost-stateless-functors-performance-2cd676c528a1
void FunctionObjects::Static_FunctionCall_Operator::TestAll()
{
    // example();
    functionPointerConversion();
}

