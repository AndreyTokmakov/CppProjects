/**============================================================================
Name        : Common.cpp
Created on  : 31.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Common.cpp
============================================================================**/

#include "Common.h"

namespace
{
    constexpr char FORMAT[] { "[%d-%02d-%02d %02d:%02d:%02d.%06ld] " };
}

namespace Common
{
    [[nodiscard]]
    std::string getCurrentTime(const std::chrono::time_point<std::chrono::system_clock>& timestamp) noexcept
    {
        const time_t time { std::chrono::system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(buffer.data(), FORMAT,
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                duration_cast<std::chrono::microseconds>(timestamp - time_point_cast<std::chrono::seconds>(timestamp)).count());
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
    }
}
