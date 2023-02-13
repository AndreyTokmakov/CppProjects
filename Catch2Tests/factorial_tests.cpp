//============================================================================
// Name        : factorial_tests.cpp
// Created on  : 15.09.2021
// Author      : Andrei Tokmakov
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Catch2 Factorial tests examples
//============================================================================

#include "test_controller.h"
#ifdef FACTORIAL_TESTS

#include <iostream>
#include <catch2/catch_test_macros.hpp>

constexpr uint32_t factorial( uint32_t number ) {
    return number <= 1 ? number : factorial(number-1) * number;
}

TEST_CASE( "Factorial of 0 is 1 (fail)", "[factorial 1]" ) {
    REQUIRE(factorial(0) == 0 );
}

TEST_CASE( "Factorials are computed", "[factorial 2]" ) {
    REQUIRE(factorial( 1) == 1);
    REQUIRE(factorial( 2) == 2);
    REQUIRE(factorial( 3) == 6);
    REQUIRE(factorial(10) == 3'628'800);
}

#endif