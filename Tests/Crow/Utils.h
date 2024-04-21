/**============================================================================
Name        : Utils.h
Created on  : 21.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Utils.h
============================================================================**/

#ifndef CPPPROJECTS_UTILS_H
#define CPPPROJECTS_UTILS_H

#include <string_view>
#include <chrono>

namespace Utils
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
};

#endif //CPPPROJECTS_UTILS_H
