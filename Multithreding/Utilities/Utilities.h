/**============================================================================
Name        : Utilities.h
Created on  : 31.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities.h
============================================================================**/

#ifndef CPPPROJECTS_UTILITIES_H
#define CPPPROJECTS_UTILITIES_H

#include <iostream>
#include <chrono>
#include <thread>

#define THREAD_INFO std::cout << Utilities::ThreadInfo()

namespace Utilities
{
    [[nodiscard]]
    std::string timeString(const std::chrono::time_point<std::chrono::system_clock>& timestamp =
            std::chrono::system_clock::now());

    [[nodiscard]]
    std::string getCurrentTime(const std::chrono::time_point<std::chrono::system_clock>& timestamp =
            std::chrono::system_clock::now());

    [[nodiscard]]
    std::string getCurrentTimeOld();

    class ThreadInfo
    {
        std::thread::id thisThreadId;

    public:
        ThreadInfo();
        friend std::ostream& operator<< (std::ostream& stream, const ThreadInfo& threadInfo);
    };


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

    bool setThreadCore(const uint32_t coreId) noexcept;

    void TestAll();
};

#endif //CPPPROJECTS_UTILITIES_H
