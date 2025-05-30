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

#include <tcmalloc/tcmalloc.h>

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    return EXIT_SUCCESS;
}

// git clone https://github.com/google/tcmalloc.git
// git clone https://github.com/abseil/abseil-cpp.git