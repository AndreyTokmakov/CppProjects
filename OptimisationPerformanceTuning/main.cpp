/**============================================================================
Name        : OptimisationPerformanceTuning.cpp
Created on  : 21.11.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Optimisation and Performance Tuning experiments
============================================================================**/

#include "Numa/Numa.h"
#include "Utilities.h"
#include "FalseSharing/FalseSharingExperiments.h"
#include "Valgrind/ValgrindExperiments.h"
#include "CacheLineAccessEffects/CacheLineAccessEffects.h"
#include "SpecificOptimizationTopics/OptimizationTricks.h"
#include "DoubleBuffering/DoubleBuffering.h"
#include "BranchLessCodeExamples/BranchLessCodeExamples.h"

#include <iostream>
#include <string_view>
#include <vector>
#include <thread>
#include <algorithm>
#include <numeric>

namespace Variables_MemoryLayout
{
    struct point {
        int x;
        int y;
    };

    struct rectangle1 {
        bool flag1;
        bool flag2;
        point p1;
        point p2;
    };

    struct rectangle2 {
        point p1;
        bool flag1;
        point p2;
        bool flag2;
    };

    void TestAll()
    {
        std::cout << sizeof(point) << std::endl;
        std::cout << sizeof(rectangle1) << std::endl;
        std::cout << sizeof(rectangle2) << std::endl;
    }
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // FalseSharingExperiments::TestAll();
    // Variables_MemoryLayout::TestAll();
    // ValgrindExperiments::TestAll();
    // CacheLineAccessEffects::TestAll();
    OptimizationTricks::TestAll();
    // Numa::TestAll();
    // DoubleBuffering::TestAll();
    // BranchLessCodeExamples::TestAll();

    return EXIT_SUCCESS;
}
