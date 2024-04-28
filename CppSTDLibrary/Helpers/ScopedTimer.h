/**============================================================================
Name        : ScopedTimer.h
Created on  : 28.04.2028
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : ScopedTimer
============================================================================**/

#ifndef CPPPROJECTS_SCOPEDTIMER_H
#define CPPPROJECTS_SCOPEDTIMER_H

#include <chrono>
#include <iostream>

namespace Helpers
{
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

            std::cout << std::left << std::setw(19) << benchmarkName << ":  ";
            std::cout << time_span.count() << " seconds.\n";
        }
    };
}

#endif //CPPPROJECTS_SCOPEDTIMER_H
