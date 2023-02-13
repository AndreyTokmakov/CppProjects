//============================================================================
// Name        : test_sections_1.cpp
// Created on  : 15.09.2021
// Author      : Andrei Tokmakov
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Catch2 Test sections
//============================================================================

#include "test_controller.h"
#ifdef TEST_SECTIONS_1

#include <iostream>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "vectors can be sized and resized", "[vector]") {

    std::vector<int> v(5);

    REQUIRE(v.size() == 5);
    REQUIRE(v.capacity() >= 5);

    SECTION("resizing bigger changes size and capacity")
    {
        std::cout << "Session 1: Vector: [size: " << v.size() << ", capacity: " << v.capacity() << "]\n";

        v.resize( 10);

        REQUIRE(v.size() == 10);
        REQUIRE(v.capacity() >= 10);
    }

    SECTION("resizing smaller changes size but not capacity")
    {
        std::cout << "Session 2: Vector: [size: " << v.size() << ", capacity: " << v.capacity() << "]\n";

        v.resize( 0);

        REQUIRE(v.size() == 0);
        REQUIRE(v.capacity() >= 5);
    }

    SECTION("reserving bigger changes capacity but not size")
    {
        std::cout << "Session 3: Vector: [size: " << v.size() << ", capacity: " << v.capacity() << "]\n";

        v.reserve( 10 );

        REQUIRE(v.size() == 5);
        REQUIRE(v.capacity() >= 10);
    }

    SECTION("reserving smaller does not change size or capacity")
    {
        std::cout << "Session 4: Vector: [size: " << v.size() << ", capacity: " << v.capacity() << "]\n";

        v.reserve( 0 );

        REQUIRE(v.size() == 5);
        REQUIRE(v.capacity() >= 5);
    }
}

#endif