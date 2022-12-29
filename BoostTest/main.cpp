//============================================================================
// Name        : main.cpp
// Created on  : 17.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Boost Project
//============================================================================

#include <iostream>
#include <memory>
#include <math.h>
#include <algorithm>

#include <thread>
#include <future>

#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <string>
#include <string_view>
#include <mutex>
#include <atomic>
#include <numeric>
#include <iomanip>


#include <boost/version.hpp>
#define BOOST_TEST_MODULE My Test
// #include <boost/test/included/unit_test.hpp>


namespace Utilites {

    void PrintBoostVersion()
    {
        std::cout << "Using Boost " << BOOST_VERSION / 100000     << "."  // major version
                << BOOST_VERSION / 100 % 1000 << "."  // minor version
                << BOOST_VERSION % 100                // patch level
                << std::endl;
    }

    /** Just to print version at the startup: **/
    const static int dummy = []() {
        PrintBoostVersion();
        return 0;
    }();
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{


    return EXIT_SUCCESS;
}
