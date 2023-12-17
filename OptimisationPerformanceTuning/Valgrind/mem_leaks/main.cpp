//============================================================================
// Name        : main.cpp
// Created on  : 15.09.2021
// Author      : Andrei Tokmakov
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Tests C++ project
//============================================================================


#include <stdio.h>
#include <stdlib.h>


#include <optional>
#include <iostream>
#include <string>
#include <string_view>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>

#include <exception>
#include <thread>
#include <future>
#include <mutex>
#include <syncstream>

#include <numeric>

#include <utility>
#include <vector>
#include <any>
#include <list>
#include <forward_list>
#include <deque>
#include <map>
#include <algorithm>
#include <array>
#include <version>
#include <concepts>

#include <format>
#include <expected>
#include <iostream>
#include <string_view>


int* allocate_some(size_t number)
{
    return new int(number);
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    std::cout << "Tests" << std::endl;

    for (int i = 0; i < 10; ++i)
    {
        allocate_some(10);
    }


    return EXIT_SUCCESS;
}

