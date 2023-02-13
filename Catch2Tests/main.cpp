/**============================================================================
Name        : Catch2Tests.cpp
Created on  : 15.09.2021
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Catch2Tests C++ project
============================================================================**/

#include <iostream>
#include <catch2/catch_session.hpp>

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);
    int result = Catch::Session().run( argc, argv );

    return EXIT_SUCCESS;
}
