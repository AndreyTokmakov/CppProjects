/**============================================================================
Name        : Iterators.cpp
Created on  : 06.11.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Iterators
============================================================================**/

#include "Iterators.h"

#include <iostream>

namespace Iterators
{
    class IntIterator
    {
    private:
        int value{};

    public:
        explicit IntIterator(int v) : value{v} {
        }

        auto operator==(const IntIterator &it) const {
            return value == it.value;
        }

        auto operator!=(const IntIterator &it) const {
            return !(*this == it);
        }

        auto &operator*() const {
            return value;
        }

        auto &operator++() {
            ++value;
            return *this;
        }
    };

    void IterTest() {
        auto first = IntIterator{12}; // Start at 12
        auto last = IntIterator{16}; // Stop when equal to 16
        for (auto it = first; it != last; ++it) {
            std::cout << (*it) << " ";
        }
    }
};


void Iterators::TestAll()
{
    IterTest();
};


