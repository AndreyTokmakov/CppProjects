/**============================================================================
Name        : Comparators.cpp
Created on  : 15.09.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Comparators.cpp
============================================================================**/

#include "Comparators.h"

#include <iostream>
#include <utility>

namespace Comparators
{
    template<typename T>
    concept Comparable = requires(T const& a, T const& b) {
        { a < b } -> std::same_as<bool>;
        { a > b } -> std::same_as<bool>;
        { a <= b } -> std::same_as<bool>;
        { a >= b } -> std::same_as<bool>;
        { a == b };
        { a == b } -> std::convertible_to<bool>;
        { a == b } noexcept -> std::convertible_to<bool>;
    };

    template<Comparable _Ty, typename _Cmp>
    struct Validator
    {
        _Cmp comparator {};

        [[nodiscard]]
        constexpr bool compare(const _Ty& a, const _Ty& b) const noexcept {
            return comparator (a, b);
        }
    };

    template<typename _Cmp>
    struct ValidatorEx
    {
        _Cmp comparator {};

        template<Comparable... Args>
        [[nodiscard]]
        constexpr bool compare(Args&&... args) const noexcept {
            return comparator (std::forward<Args>(args) ...);
        }
    };

    void test1()
    {
        Validator<int, std::less<>> validator {};

        static_assert(validator.compare(1, 2));
        static_assert(not validator.compare(11, 2));
    }

    void test2()
    {
        ValidatorEx<std::less<>> validator {};

        static_assert(validator.compare(1, 2));
        static_assert(not validator.compare(11, 2));
    }

    void test_lambda()
    {
        auto comparator = [](const auto& a, const auto& b, const auto& c) -> bool {
            return c > b && b > a;
        };

        ValidatorEx<decltype(comparator)> validator {};

        static_assert(validator.compare(1, 2, 3));
        static_assert(not validator.compare(1,1,1));
    }
};

void Comparators::TestAll()
{
    test1();
    test2();
    test_lambda();
};
