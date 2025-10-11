/**============================================================================
Name        : Servers.cpp
Created on  : 11.10.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Servers.cpp
============================================================================**/

#include "Servers.hpp"

void Servers::TestAll(const std::vector<std::string_view>& args)
{
    // runDebugServer(args);
    runReqRepServer(args);
}