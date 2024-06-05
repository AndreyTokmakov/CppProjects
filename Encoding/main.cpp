/**============================================================================
Name        : Encoding.cpp
Created on  : 15.09.2021
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Encoding C++ project
============================================================================**/

#include <iostream>
#include <vector>
#include <string_view>

#include "Base64/Base64.h"
#include "URL/URLEncoding.h"
#include "Hex_to_Bytes/Hex_to_Bytes.h"

// TODO:
// 1. int <--> IP
// 2. String <--> Base64
// 3.

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Base64::TestAll(args);
    // URLEncoding::TestAll(args);
    Hex_to_Bytes::TestAll();


    return EXIT_SUCCESS;
}
