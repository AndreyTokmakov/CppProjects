/**============================================================================
Name        : PertTools.cpp
Created on  : 07.07.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PertTools.cpp
============================================================================**/

#include "PertTools.h"

#include <iostream>
#include <iomanip>

namespace PertTools
{
    ScopedTimer::~ScopedTimer() {
        const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

        std::cout << std::left << std::setw(14) << benchmarkName << ":  ";
        std::cout << time_span.count() << " seconds.\n";
    }
};



