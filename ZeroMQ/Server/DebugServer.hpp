/**============================================================================
Name        : DebugServer.h
Created on  : 02.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DebugServer.h
============================================================================**/

#ifndef CPPWORKPROJECTS_DEBUGSERVER_H
#define CPPWORKPROJECTS_DEBUGSERVER_H

#include <string_view>
#include <vector>

namespace DebugServer
{
    void TestAll(const std::vector<std::string_view>& args);
};

#endif //CPPWORKPROJECTS_DEBUGSERVER_H
