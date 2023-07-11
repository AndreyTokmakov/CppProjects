/**============================================================================
Name        : LinuxProcessManager.cpp
Created on  : 11.06.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : LinuxProcessManager C++ project
============================================================================**/

// #include <vector>
// #include <string_view>
// #include <functional>

#include "ProcessManager.h"

int main(int argc, char** argv)
{
    // const std::vector<std::string_view> args(argv + 1, argv + argc);
    ProcessManager().run(argc, argv);
    return 0;
}
