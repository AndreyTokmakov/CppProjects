/**============================================================================
Name        : indirect.cpp
Created on  : 14.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : indirect.cpp
============================================================================**/

#include "Indirect.hpp"
#include "PImpl_Example.hpp"

#include <print>
#include <vector>

namespace
{
    struct Data
    {
        explicit Data(const int value) : value { value } {}
        std::strong_ordering operator<=>(const Data&) const = default;

        int value;
    };
}


namespace
{
    void create()
    {
        std::indirect<Data> data { std::in_place, 42 };
        std::println("create: {}", data->value);
    }

    void access()
    {
        std::indirect<Data> data { std::in_place, 42 };

        data->value = 10;
        data->value = 20;

        std::println("access: {}", data->value);
    }

    void copy()
    {
        std::indirect<Data> first { std::in_place, 42 };
        std::indirect<Data> second = first;

        second->value = 100;

        std::println("copy: first = {}, second = {}",
                     first->value, second->value);
    }

    void move()
    {
        std::indirect<Data> first { std::in_place, 42 };
        std::indirect<Data> second = std::move(first);

        std::println("move: second = {}, first valueless = {}",
        second->value, first.valueless_after_move());
    }

    void compare()
    {
        std::indirect<Data> first { std::in_place, 42 };
        std::indirect<Data> second { std::in_place, 42 };
        std::indirect<Data> third { std::in_place, 100 };

        std::println("compare: first == second: {}, first < third: {}",
                     first == second, first < third);
    }

    void vector()
    {
        std::vector<std::indirect<Data>> values;

        values.emplace_back(std::in_place, 10);
        values.emplace_back(std::in_place, 20);
        values.emplace_back(std::in_place, 30);

        auto copiedValues = values;

        copiedValues[0]->value = 100;

        std::println("vector: original = {}, copied = {}",
                     values[0]->value, copiedValues[0]->value);
    }

    void allocator()
    {
        std::indirect<Data> data { std::in_place, 42 };

        auto allocator = data.get_allocator();

        static_assert(std::is_same_v<std::allocator_traits<decltype(allocator)>::value_type,Data>);

        std::println("allocator: value = {}", data->value);
    }

    void swap()
    {
        std::indirect<Data> first { std::in_place, 10 };
        std::indirect<Data> second { std::in_place, 20 };

        first.swap(second);

        std::println("swap: first = {}, second = {}",
                     first->value, second->value);
    }

    void uniquePtrVsIndirect()
    {
        std::unique_ptr<Data> uniqueFirst = std::make_unique<Data>(42);
        std::unique_ptr<Data> uniqueSecond = std::make_unique<Data>(*uniqueFirst);

        uniqueSecond->value = 100;

        std::println("unique_ptr: first = {}, second = {}",
                     uniqueFirst->value, uniqueSecond->value);

        std::indirect<Data> indirectFirst { std::in_place, 42 };
        std::indirect<Data> indirectSecond = indirectFirst;

        indirectSecond->value = 100;

        std::println("indirect: first = {}, second = {}",
                     indirectFirst->value, indirectSecond->value);

        // unique_ptr: first = 42, second = 100
        // indirect: first = 42, second = 100
    }
}

namespace
{
    struct Portfolio
    {
        std::vector<std::indirect<Data>> items;
    };

    void use_case_demo()
    {
        Portfolio first;

        first.items.emplace_back(std::in_place, 100);
        first.items.emplace_back(std::in_place, 200);
        first.items.emplace_back(std::in_place, 300);

        // INFO: Deep copy of all Data objects.
        //       Will fail if "std::vector<std::unique_ptr<Data>> items"
        Portfolio second = first;

        // Modify the copied portfolio.
        second.items[0]->value = 999;

        std::println("First portfolio:");

        for (const auto& item : first.items) {
            std::println("  Value: {}", item->value);
        }

        std::println("Second portfolio:");

        for (const auto& item : second.items) {
            std::println("  Value: {}", item->value);
        }
    }
}

void memory::indirect::TestAll()
{
    // create();
    // access();
    // copy();
    // move();
    // compare();
    // vector();
    // allocator();
    // swap();

    uniquePtrVsIndirect();

    use_case_demo();

    // pimpl_example::demo();
}
