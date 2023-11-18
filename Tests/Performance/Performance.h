/**============================================================================
Name        : Performance.h
Created on  : 01.01.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Performance
============================================================================**/

#ifndef CPPPROJECTS_PERFORMANCE_H
#define CPPPROJECTS_PERFORMANCE_H

#include <string_view>
#include <chrono>

namespace Performance {
    void TestAll();
};

namespace Performance::Utils
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

        ~ScopedTimer();
    };
}

#endif //CPPPROJECTS_PERFORMANCE_H
