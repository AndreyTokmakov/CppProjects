/**============================================================================
Name        : InplaceVector.cpp
Created on  : 17.05.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : InplaceVector.cpp
============================================================================**/

#include "InplaceVector.hpp"

#include <iostream>
#include <print>
#include <inplace_vector>

namespace inplace_vector_tests
{
    void basics()
    {
        using Type = int;
        constexpr uint32_t Capacity { 32 };


        const std::inplace_vector<Type, 32> values;
        std::println("Size: {}, Capacity: {}", values.size(), values.capacity());
        std::println("Size of: {} == {}", sizeof(values), sizeof(Type) * Capacity);

    }

    void unchecked_emplace_back()
    {
        std::inplace_vector<std::pair<std::string, std::string>, 2> fauna;
        std::string dog{"\N{DOG}"};

        fauna.unchecked_emplace_back("\N{CAT}", dog);
        fauna.unchecked_emplace_back("\N{CAT}", std::move(dog));
        // fauna.unchecked_emplace_back("BUG", "BUG"); // undefined behavior: no space
        std::println("fauna = {}", fauna);
    }

    void try_emplace_back()
    {
        using Type = int;
        std::inplace_vector<Type, 3> values;

        auto printResult = []<typename T>(const std::optional<T&> opt) {
            if (opt.has_value()) {
                std::println("Result: {}", opt.value());
            } else {
                std::println("Result: None");
            }
        };
        for (const int v: {1, 2, 3, 4, 5}) {
            const std::optional<Type&> item = values.try_emplace_back(v);
            printResult(item);
        }

        /*
        Result: 1
        Result: 2
        Result: 3
        Result: None
        Result: None
        */
    }
}

void inplace_vector::TestAll()
{
    inplace_vector_tests::basics();

    // inplace_vector_tests::try_emplace_back();
    // inplace_vector_tests::unchecked_emplace_back();
}
