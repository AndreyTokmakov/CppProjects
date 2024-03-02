/**============================================================================
Name        : CacheLineAccessEffects.cpp
Created on  : 02.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CacheLineAccessEffects.cpp
============================================================================**/

#include "CacheLineAccessEffects.h"

#include <iostream>
#include <chrono>

namespace CacheLineAccessEffects
{
    using value_type = int;
    constexpr size_t length = 1024 * 1024 * 1024;
    constexpr size_t cacheLineSize = std::hardware_destructive_interference_size;
    constexpr size_t entriesInLine = cacheLineSize / sizeof(value_type);

    constexpr size_t N = length / entriesInLine;


    void sequentialAccess()
    {
        int *buffer = new int[length];
        const auto start = std::chrono::high_resolution_clock::now();

        for (size_t n = 0; n < N; ++n) {
            for (size_t m = 0; m < entriesInLine; ++m) {
                ++buffer[n * entriesInLine + m ];
                // std::cout << n * entriesInLine + m  << std::endl;
            }
        }

        const auto stop = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << (float) duration.count() << std::endl;

        delete[] buffer;
    }

    void sequentialAccess_DiffCacheLines()
    {
        int *buffer = new int[length];
        const auto start = std::chrono::high_resolution_clock::now();

        for (size_t m = 0; m < entriesInLine; ++m) {
            for (size_t n = 0; n < N; ++n)
            {
                ++buffer[n * entriesInLine + m ];
                // std::cout << n * entriesInLine + m  << std::endl;
            }
        }

        const auto stop = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << (float) duration.count() << std::endl;

        delete[] buffer;
    }
}


void CacheLineAccessEffects::TestAll()
{
    sequentialAccess();
    sequentialAccess_DiffCacheLines();
}