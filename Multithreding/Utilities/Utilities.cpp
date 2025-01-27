/**============================================================================
Name        : Utilities.cpp
Created on  : 31.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities.cpp
============================================================================**/

#include <iostream>
#include <mutex>
#include <thread>
#include <syncstream>
#include <iomanip>
#include "Utilities.h"


namespace Utilities
{
    using namespace std::chrono;

    constexpr std::string_view formatSeconds { "{:%Y-%m-%d %H:%M:%OS}" };
    constexpr std::string_view formatMSeconds { "%d-%02d-%02d %02d:%02d:%02d.%06ld" };

    [[nodiscard]]
    std::string timeString(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
    {
        std::string buffer;
        buffer.reserve(32);
        std::format_to(std::back_inserter(buffer), formatSeconds, timestamp);
        buffer.shrink_to_fit();
        return buffer;
    }


    [[nodiscard]]
    std::string getCurrentTime(const time_point<system_clock>& timestamp)
    {
        const time_t time { std::chrono::system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(buffer.data(), formatMSeconds.data(),
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                    duration_cast<microseconds>(timestamp - time_point_cast<seconds>(timestamp)).count()
        );
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
    }
}


namespace Utilities
{
    bool setThreadCore(const uint32_t coreId) noexcept
    {
        cpu_set_t cpuSet {};
        CPU_ZERO(&cpuSet);
        CPU_SET(coreId, &cpuSet);
        return 0 == pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuSet);
    }
}


namespace Utilities
{
    const std::thread::id mainThreadId = std::this_thread::get_id();
    std::mutex print_mutex;

    ThreadInfo::ThreadInfo() {
        thisThreadId = std::this_thread::get_id();
    }

    std::ostream& operator<< (std::ostream& stream, const ThreadInfo& threadInfo)
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        stream << "[" << getCurrentTime() << "] Thread [";
        if (mainThreadId == threadInfo.thisThreadId) {
            stream << std::setiosflags(std::ios::left) << std::setw(15) << "Main";
        }
        else {
            stream << std::setiosflags(std::ios::left) << std::setw(5) << threadInfo.thisThreadId;
        }
        stream << "] ";
        stream.flush();;
        return stream;
    }
}

namespace Utilities
{
    ScopedTimer::~ScopedTimer()
    {
        const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

        std::cout << std::left << std::setw(14) << benchmarkName << ":  ";
        std::cout << time_span.count() << " seconds.\n";
    }
}