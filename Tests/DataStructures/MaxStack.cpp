/**============================================================================
Name        : MaxStack.cpp
Created on  : 11.06.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : MaxStack
============================================================================**/

#include "MaxStack.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <set>



namespace MaxStack
{
    /* Problem:
    Implement a Max-Stack data structure that provides the following interface:

    O(1) complexity methods: top() and max()
    O(logn) complexity methods: push(), pop() and pop_max()
    */

    /* Solution:
    The requirements are already pointing us towards ordered containers due
    to the O(logn) complexity for pop(), pop_max() and push().

    For simplicity, we can keep our data twice, once sorted in the LIFO
    order and once ordered by value. When we pop() or pop_max(), we remove
    the element from both containers, and symmetrically, when we push(),
    we insert the element into both containers. The top() and max() are
    then O(1) queries into the corresponding containers.

    Finally, to keep the elements sorted in LIFO order, we need to keep
    track of an ID that we will assign to every element.
    */

    // Value with a position
    template <typename T>
    struct PV {
        uint64_t position;
        T value;
    };

    // Order by position (LIFO)
    struct by_key {
        template <typename PV>
        bool operator()(const PV& left, const PV& right) const {
            return left.position < right.position;
        }
    };

    // Order by value
    struct by_value {
        template <typename PV>
        bool operator()(const PV& left, const PV& right) const {
            if (left.value == right.value)
                return left.position < right.position;
            return left.value < right.value;
        }
    };

    template <typename T>
    struct MaxStack {
        void push(T x) {
            uint64_t next_pos = 0;
            if (!store_.empty())
                next_pos = store_.rbegin()->position + 1;
            // Construct an element with a unique ID
            auto v = PV<T>{next_pos, std::move(x)};
            // Insert into both containers
            store_.insert(v);
            max_.insert(std::move(v));
        }

        T pop() {
            if (store_.empty()) return T{};
            // Fetch the top element in LIFO order
            auto v = *store_.rbegin();
            // Remove the element from both containers
            max_.erase(v);
            store_.erase(v);
            return std::move(v.value);
        }

        T pop_max() {
            if (store_.empty()) return T{};
            // Fetch the top element by-value
            auto v = *max_.rbegin();
            // Remove the element from both containers
            max_.erase(v);
            store_.erase(v);
            return std::move(v.value);
        }

        T top() {
            if (store_.empty()) return T{};
            // Fetch the top element in LIFO order
            return store_.rbegin()->value;
        }

        T max() {
            if (store_.empty()) return T{};
            // Fetch the top element by-value
            return max_.rbegin()->value;
        }

    private:
        std::set<PV<T>,by_key> store_;
        std::set<PV<T>,by_value> max_;
    };


    void test() {
        MaxStack<int> stack;
        stack.push(10);
        assert(stack.max() == 10);
        assert(stack.top() == 10);
        stack.push(5);
        assert(stack.max() == 10);
        assert(stack.top() == 5);
        stack.push(11);
        assert(stack.max() == 11);
        assert(stack.top() == 11);
        assert(stack.pop_max() == 11);
        assert(stack.max() == 10);
        assert(stack.top() == 5);
        assert(stack.pop_max() == 10);
        assert(stack.max() == 5);
        assert(stack.top() == 5);
        assert(stack.pop() == 5);
        stack.push(5);
        stack.push(4);
        stack.push(3);
        stack.push(2);
        stack.push(1);
        assert(stack.top() == 1);
        assert(stack.max() == 5);
        assert(stack.pop() == 1);
        assert(stack.pop_max() == 5);
        assert(stack.top() == 2);
        assert(stack.max() == 4);
        assert(stack.pop() == 2);
        assert(stack.pop() == 3);
        assert(stack.pop() == 4);
        stack.push(5);
        stack.push(5);
        stack.push(5);
        stack.push(1);
        assert(stack.top() == 1);
        assert(stack.max() == 5);
        assert(stack.pop_max() == 5);
        assert(stack.pop_max() == 5);
        assert(stack.top() == 1);
        assert(stack.max() == 5);
        stack.push(5);
        assert(stack.pop() == 5);
        assert(stack.pop() == 1);
        assert(stack.pop() == 5);
    }
}

void MaxStack::TestAll()
{

}


