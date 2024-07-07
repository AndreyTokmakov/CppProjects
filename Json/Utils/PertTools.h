/**============================================================================
Name        : PertTools.h
Created on  : 07.07.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PertTools.h
============================================================================**/

#ifndef CPPPROJECTS_PERTTOOLS_H
#define CPPPROJECTS_PERTTOOLS_H

#include <string_view>
#include <chrono>

namespace PertTools
{
    struct ScopedTimer
    {
        const std::string_view benchmarkName;
        const std::chrono::high_resolution_clock::time_point start { std::chrono::high_resolution_clock::now() };

        explicit ScopedTimer(std::string_view info) :
                benchmarkName {info} {
        }

        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer(ScopedTimer&&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;
        ScopedTimer& operator=(ScopedTimer&&) = delete;

        ~ScopedTimer();
    };
};

#endif //CPPPROJECTS_PERTTOOLS_H
