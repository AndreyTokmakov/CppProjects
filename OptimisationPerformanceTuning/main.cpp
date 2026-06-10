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
#include "Branches/Remove_Branches_Builtin_Unreachable.h"
#include "AoS_vs_SoA/AoS_vs_SoA.h"
#include "Data_Prefetching/DataPrefetching.hpp"
#include "Loop_Unrolling/Loop_Unrolling.hpp"

#include <iostream>
#include <print>
#include <string_view>
#include <vector>
#include <thread>
#include <algorithm>
#include <numeric>
#include <unistd.h>

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


namespace caches
{
    size_t getCacheSizeL1_I() {
        return ::sysconf(_SC_LEVEL1_ICACHE_SIZE);
    }

    size_t getCacheSizeL1_D() {
        return ::sysconf(_SC_LEVEL1_DCACHE_SIZE);
    }

    size_t getCacheSizeL2(){
        return ::sysconf(_SC_LEVEL2_CACHE_SIZE);
    }

    size_t getCacheSizeL3()
    {
        // Linux-specific; returns L3 size in bytes
        const int64_t size = ::sysconf(_SC_LEVEL3_CACHE_SIZE);
        return (size > 0) ? static_cast<size_t>(size) : 8 * 1024 * 1024;  // 8MB fallback
    }

    void printCPUCaches()
    {
        std::println("L1I {}", getCacheSizeL1_I());
        std::println("L1D {}", getCacheSizeL1_D());
        std::println("L2  {}", getCacheSizeL2());
        std::println("L3  {}", getCacheSizeL3());
    }

}


int main([[maybe_unused]] const int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // caches::printCPUCaches();
    // FalseSharingExperiments::TestAll();
    // Variables_MemoryLayout::TestAll();
    // ValgrindExperiments::TestAll();
    // CacheLineAccessEffects::TestAll();
    // OptimizationTricks::TestAll();
    // Numa::TestAll();
    // DoubleBuffering::TestAll();
    // DataPrefetching::TestAll();
    // AoS_vs_SoA::TestAll();
    // Remove_Branches_Builtin_Unreachable::TestAll();
    // BranchLessCodeExamples::TestAll();
    loop_unrolling::TestAll();

    return EXIT_SUCCESS;
}
