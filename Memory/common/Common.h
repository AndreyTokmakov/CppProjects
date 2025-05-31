/**============================================================================
Name        : Common.h
Created on  : 31.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Common.h
============================================================================**/

#ifndef CPPPROJECTS_COMMON_H
#define CPPPROJECTS_COMMON_H

#include <chrono>
#include <syncstream>

namespace Common
{
    using namespace std::chrono;

    constexpr char FORMAT[] { "[%d-%02d-%02d %02d:%02d:%02d.%06ld] " };

    [[nodiscard]]
    std::string getCurrentTime(const std::chrono::time_point<std::chrono::system_clock>& timestamp
            = std::chrono::system_clock::now()) noexcept;

#define LOG std::osyncstream { std::cout } << Common::getCurrentTime() << "[" \
        << std::this_thread::get_id() << "] "

}

#endif //CPPPROJECTS_COMMON_H
