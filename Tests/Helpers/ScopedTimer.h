/**============================================================================
Name        : ScopedTimer.h
Created on  : 06.11.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities
============================================================================**/

#ifndef CPPPROJECTS_SCOPEDTIMER_H
#define CPPPROJECTS_SCOPEDTIMER_H

#include <iostream>
#include <chrono>
#include <iomanip>

struct ScopedTimer
{
    const std::string_view benchmarkName;
    const std::chrono::high_resolution_clock::time_point start {
            std::chrono::high_resolution_clock::now()
    };

    explicit ScopedTimer(std::string_view info) :
            benchmarkName {info} {
    }

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer(ScopedTimer&&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
    ScopedTimer& operator=(ScopedTimer&&) = delete;

    ~ScopedTimer()
    {
        const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

        std::cout << std::left << std::setw(14) << benchmarkName << ":  ";
        std::cout << time_span.count() << " seconds.\n";
    }
};

#endif //CPPPROJECTS_SCOPEDTIMER_H
