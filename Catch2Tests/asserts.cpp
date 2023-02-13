/**============================================================================
Name        : Catch2Tests.cpp
Created on  : 05.02.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Asserts
============================================================================**/

#include "test_controller.h"
#ifdef ASSERTS

#include <iostream>
#include <catch2/catch_test_macros.hpp>

constexpr bool isOK() {
    return true;
}

TEST_CASE("Check_Tests", "")
{
    std::string str {"string value" };
    CHECK( str == "string value" );

    CHECK( isOK() );
}

TEST_CASE("Require_Tests", "")
{
    int val { 42 };
    REQUIRE( val == 42 );
}

#endif