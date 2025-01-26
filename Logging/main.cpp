/**============================================================================
Name        : main.cpp
Created on  : 25.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Logging tests
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>

#include "Quill/Quill.h"
#include "SPDLog/SPDLog.h"
#include "XTR/XTR.h"


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Quill::TestAll();
    XTR::TestAll();
    // SPDLog::TestAll();

    return EXIT_SUCCESS;
}
