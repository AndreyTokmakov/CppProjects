/**============================================================================
Name        : MinStack.cpp
Created on  : 28.09.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MinStack
============================================================================**/

#include "MinStack.h"




#include "MinStack.h"

#include <vector>
#include <iostream>
#include <list>

/*
Design a stack that supports push, pop, top operations and is able to return
the minimum element in the stack in constant time.
Implement the minStack class:

minStack() - initializes the object
void push(int val) - puts val at the top of the stack
void pop() - removes an element from the top of the stack
int top() - returns the one at the top of the stack

int getMin() - returns the minimum element in the stack

It is necessary to implement a solution with complexity O(1) in time for each of the listed functions.
*/

namespace MinStack::Two_Vectors
{
    /**
    the idea is to store the elements themselves in the vector "store" (stock-like) and  the vector "mins" stores
    - the last item added to the "store" IF it is LESS than the smallest item (mins.back())
    - or the smallest element - if the one that is being added now is larger than the minimum
    thereby making the sizes() of the vectors the same -> so that when deleting, just do pop_back() from both elements
    **/

    template<typename _Ty>
    class MinStack
    {
        using object_type = _Ty;
        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        std::vector<object_type> store;
        std::vector<object_type> mins;

    public:
        MinStack& push(int val)
        {
            store.push_back(val);
            if (mins.empty())
                mins.push_back(val);
            mins.push_back(mins.back() > val ? val: mins.back());
            return *this;
        }

        void pop() noexcept
        {
            store.pop_back();
            mins.pop_back();
        }

        [[nodiscard]]
        object_type top() const noexcept
        {
            return store.back();
        }

        [[nodiscard]]
        object_type getMin() const noexcept
        {
            return mins.back();
        }
    };


    void Tests()
    {
        MinStack<int> stack;
        stack.push(10).push(12).push(6).push(4).push(5);

        for (int i = 0; i < 3; ++i) {
            std::cout << "Min: " << stack.getMin() << ". Top: " << stack.top() << std::endl;
            stack.pop();
        }

        stack.push(1).push(3);


        for (int i = 0; i < 2; ++i) {
            std::cout << "Min: " << stack.getMin() << ". Top: " << stack.top() << std::endl;
            stack.pop();
        }
    }
};


namespace MinStack::Two_Vectors_GOOD
{
    template<typename _Ty>
    class MinStack
    {
        using value_type = _Ty;
        static_assert(!std::is_same_v<value_type, void>,
                      "Type of the Objects in the stack can not be void");

        std::vector<value_type> store;
        std::vector<value_type> minStore;

    public:
        MinStack& push(int value)
        {
            store.push_back(value);
            if (minStore.empty() || minStore.back() >= value)
                minStore.push_back(value);
            return *this;
        }

        void pop() noexcept
        {
            if (store.empty())
                return;
            if (store.back() == minStore.back())
                minStore.pop_back();
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
        value_type getMin() const noexcept
        {
            if (!minStore.empty())
                return minStore.back();
            throw std::runtime_error("Stack is empty");
        }
    };


    void Tests()
    {
        MinStack<int> stack;
        stack.push(10).push(12).push(6).push(4).push(5);

        for (int i = 0; i < 3; ++i) {
            std::cout << "Min: " << stack.getMin() << ". Top: " << stack.top() << std::endl;
            stack.pop();
        }

        stack.push(1).push(3);


        for (int i = 0; i < 2; ++i) {
            std::cout << "Min: " << stack.getMin() << ". Top: " << stack.top() << std::endl;
            stack.pop();
        }
    }
};


void MinStack::TestAll()
{
    Two_Vectors::Tests();

    std::cout << std::endl;

    Two_Vectors_GOOD::Tests();
};