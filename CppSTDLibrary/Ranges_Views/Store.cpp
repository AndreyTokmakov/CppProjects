/**============================================================================
Name        : Store.cpp
Created on  : 31.12.2025
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "Ranges.hpp"

#include <vector>
#include <ranges>
#include <print>


namespace details
{
    namespace views  = std::views;
    namespace ranges = std::ranges;

    struct Store
    {
        std::vector<int> data;

        void generate()
        {
            data = views::iota(0, 10) | ranges::to<std::vector<int>>();
        }

        [[nodiscard]]
        ranges::view auto getEvenNumbers() const
        {
            return data | views::filter([] (const int v) { return not (v & 1); } );
        }

        [[nodiscard]]
        ranges::view auto getOldNumbers() const
        {
            return data | views::filter([] (const int v) { return v & 1; } );
        }

        [[nodiscard]]
        ranges::view auto getAll() const
        {
            return views::all(data);
        }

        [[nodiscard]]
        ranges::view auto getLargeNumbers() const
        {
            return data | views::filter([&] (const int v) { return v >= 5;} );
        }

        [[nodiscard]]
        ranges::view auto getSubSet(const uint32_t start, const uint32_t count) const
        {
            return ranges::subrange(data.begin() + start,
                data.begin() + ranges::min(start + count, static_cast<uint32_t>(data.size())));
        }

        template<typename  Func>
        [[nodiscard]]
        ranges::view auto getMathing(Func&& pred)
        {
            /*if (data.empty()) {
                return views::empty<int>;
            }*/
            return data | views::filter(std::forward<Func>(pred));
        }
    };
}

void Ranges::Store::TestAll()
{
    details::Store store;
    store.generate();

    std::println("All    : {}", store.getAll());
    std::println("Even   : {}", store.getEvenNumbers());
    std::println("Odd    : {}", store.getOldNumbers());
    std::println("Large  : {}", store.getLargeNumbers());
    std::println("SubSet : {}", store.getSubSet(1, 5));
    std::println("Filter : {}", store.getMathing([](int v) {  return v >= 3 && v <= 5; }));
}

/**
All    : [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
Even   : [0, 2, 4, 6, 8]
Odd    : [1, 3, 5, 7, 9]
Large  : [5, 6, 7, 8, 9]
SubSet : [1, 2, 3, 4, 5]
Filter : [3, 4, 5]
**/