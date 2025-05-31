/**============================================================================
Name        : CheckingPolicy.cpp
Created on  : 23.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CheckingPolicy.cpp
============================================================================**/

#include <iostream>

namespace CheckingPolicy
{
    struct Unchecked
    {
        void operator() ([[maybe_unused]] size_t idx, [[maybe_unused]] size_t sizeMax) {}
    };

    struct Checked
    {
        void operator() (size_t idx, size_t sizeMax) {
            if (idx >= sizeMax)
                throw std::runtime_error("OPPSSSS");
        }
    };

    template<typename T,
             size_t Size,
             typename CheckingPolicy = Unchecked>
    struct Array
    {
        using value_type = T;

        value_type buffer[Size] {};

        value_type& operator[](size_t idx)
        {
            CheckingPolicy{}(idx, Size);
            return buffer[idx];
        };
    };
}

// NOTE: Idea taken from the Andreas Fertig's video: https://www.youtube.com/watch?v=emh-6dXTGcQ

void CheckingPolicy_Test()
{
    using namespace CheckingPolicy;

    Array<int, 2> values {1, 2};
    std::cout << values[3] << std::endl;

    Array<int, 2, Checked> valuesChecked {1, 2};
    std::cout << valuesChecked[3] << std::endl;

    /** Output:
     * 0
     *
     * terminate called after throwing an instance of 'std::runtime_error'
         what():  OPPSSSS
     */
}