/**============================================================================
Name        : OptimisationPerformanceTuning.cpp
Created on  : 21.11.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Optimisation and Performance Tuning experiments
============================================================================**/

#include "Utilities.h"
#include "FalseSharing/FalseSharingExperiments.h"

#include <iostream>
#include <string_view>
#include <vector>
#include <thread>
#include <algorithm>
#include <numeric>

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // FalseSharingExperiments::TestAll();


    /*
    const size_t size = 100'000'000;

    int* data = new int[size];
    std::iota(data, data + size, 0);

    {
        Utilities::ScopedTimer timer { "DemoThree::test" };;
        for (int n = 0; n < 100'000; ++n)
        {
            uint64_t sum = 0;
            for (size_t i = 0; i < size; ++i)
                sum += data[i];
        }
    }

    delete [] data;
    */

    return EXIT_SUCCESS;
}
