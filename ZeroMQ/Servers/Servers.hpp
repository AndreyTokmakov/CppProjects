/**============================================================================
Name        : Servers.hpp
Created on  : 11.10.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Servers.hpp
============================================================================**/

#ifndef CPPPROJECTS_SERVERS_HPP
#define CPPPROJECTS_SERVERS_HPP

#include <string_view>
#include <vector>

namespace Servers
{
    void TestAll(const std::vector<std::string_view>& args);
    void runDebugServer(const std::vector<std::string_view>& args);
    void runReqRepServer(const std::vector<std::string_view>& args);
}

#endif //CPPPROJECTS_SERVERS_HPP