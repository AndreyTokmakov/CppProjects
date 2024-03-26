/**============================================================================
Name        : MinMaxStack.cpp
Created on  : 26.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MinMaxStack.cpp
============================================================================**/

#include "MinMaxStack.h"

#include <iostream>
#include <vector>
#include <list>
#include <concepts>

/** For tests **/
#include <algorithm>
#include <random>


namespace MinMaxStack
{
    template<typename T>
    concept Comparable = requires(const T& a, const T& b)
    {
        { a < b }  -> std::same_as<bool>;
        { a <= b } -> std::same_as<bool>;
        { b < a }  -> std::same_as<bool>;
        { b <= a } -> std::same_as<bool>;

        { a > b }  -> std::same_as<bool>;
        { a >= b } -> std::same_as<bool>;
        { b > a }  -> std::same_as<bool>;
        { b >= a } -> std::same_as<bool>;

        { a == b } -> std::same_as<bool>;
        { a == b } noexcept -> std::convertible_to<bool>;

        { b == a } -> std::same_as<bool>;
        { b == a } noexcept -> std::convertible_to<bool>;
    };


    template<Comparable _Ty>
    struct MinMaxStack
    {
        using value_type = _Ty;
        static_assert(!std::is_same_v<value_type, void>,
                      "Type of the Objects in the stack can not be void");

        std::vector<value_type> store;
        std::list<value_type> minStore;

    public:
        MinMaxStack & push(value_type value) noexcept
        {
            store.push_back(value);
            if (minStore.empty() || value <= minStore.back())
                minStore.push_back(value);
            else if (value >= minStore.front())
                minStore.push_front(value);
            return *this;
        }

        void pop() noexcept
        {
            if (store.empty())
                return;
            if (store.back() == minStore.back())
                minStore.pop_back();
            else if (store.back() == minStore.front())
                minStore.pop_front();
            store.pop_back();
        }

        [[nodiscard]]
        value_type top() const noexcept
        {
            if (!store.empty())
                return store.back();
            throw std::runtime_error("Stack is empty");
        }

        [[nodiscard]]
        value_type min() const noexcept
        {
            if (!minStore.empty())
                return minStore.back();
            throw std::runtime_error("Stack is empty");
        }

        [[nodiscard]]
        value_type max() const noexcept
        {
            if (!minStore.empty())
                return minStore.front();
            throw std::runtime_error("Stack is empty");
        }
    };
};


namespace MinMaxStack::Test
{

    int32_t randomIntegerInRange(int32_t from, int32_t until)
    {
        std::random_device randomDevice{};
        std::mt19937 generator(randomDevice());
        return std::uniform_int_distribution<int>{from, until}(generator);
    }

    std::vector<int> getRandomValues(size_t size = 10)
    {
        std::vector<int> values;
        values.reserve(size);
        for (size_t idx = 0; idx < size; ++idx)
        {
            values.push_back(randomIntegerInRange(0, size * 2));
        }
        return values;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& stream,
                             const MinMaxStack<T>& stack)
    {
        for (const T& value: stack.store)
            std::cout << value << ' ';
        return stream;
    }


    void TestOne()
    {
        MinMaxStack<int> stack;
        stack.push(10).push(12).push(6).push(4).push(5);

        for (int i = 0; i < 3; ++i) {
            std::cout << "Min: " << stack.min() << " Max: " << stack.max() << ". Top: " << stack.top()
                << " | " << stack << std::endl;
            stack.pop();
        }

        stack.push(1).push(3);

        for (int i = 0; i < 4; ++i) {
            std::cout << "Min: " << stack.min() << " Max: " << stack.max() << ". Top: " << stack.top()
                      << " | " << stack << std::endl;
            stack.pop();
        }
    }

    void GenericTest()
    {
        // Doing 10 tests
        for (int i = 0; i < 10; ++i)
        {
            // Getting random size value and generating input collection of 'size' random values
            const int size {randomIntegerInRange(0, 100)};
            std::vector<int> values {getRandomValues(size)};

            MinMaxStack<int> stack;
            for (const auto v: values)
                stack.push(v);

            // pop-ing elements from 'values' and 'stack' one by one
            // each time checking MIN_MAX values both in the 'values' and in the 'stack'
            for (int n = 0; n < size; ++n)
            {
                const auto [min, max] = std::minmax_element(values.cbegin(), values.cend());
                if (*min != stack.min())
                {
                    std::cerr << "Error. Min values mismatch: " << stack.min() << " != " << *min << std::endl;
                    return;
                }
                if (*max != stack.max())
                {
                    std::cerr << "Error. Max values mismatch: " << stack.max() << " != " << *max << std::endl;
                    return;
                }

                values.pop_back();
                stack.pop();
            }

            std::cout << "Test for size = " << size << " OK " << std::endl;
        }
    }
}


void MinMaxStack::TestAll()
{
    // Test::TestOne();
    Test::GenericTest();
};