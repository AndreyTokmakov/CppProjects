/**============================================================================
Name        : Utilities.h
Created on  : 01.09.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities
============================================================================**/

#ifndef CPPPROJECTS_UTILITIES_H
#define CPPPROJECTS_UTILITIES_H

#include <string_view>
#include <chrono>

namespace Networking::Utilities {
    void TestAll();

    void ConnectToHostAndSleep(std::string_view hostName,
                               uint16_t port = 80,
                               const std::chrono::duration<int64_t>& timeout = std::chrono::seconds (0));
};

#endif //CPPPROJECTS_UTILITIES_H