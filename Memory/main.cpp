/**============================================================================
Name        : OpenCV.cpp
Created on  : 10.01.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Memory C++ project
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    return EXIT_SUCCESS;
}

