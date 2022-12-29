//============================================================================
// Name        : main.cpp
// Created on  : 17.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Boost Project
//============================================================================

#include <iostream>
#include <cmath>
#include <gtest/gtest.h>

#include "Utils.h"
#include "Utils2.h"


double squareRoot(const double a) {
    double b = std::sqrt(a);
    if(b != b) { // nan check
        return -1.0;
    }else{
        return sqrt(a);
    }
}

TEST(SquareRootTest, PositiveNos) {
    ASSERT_EQ(6, squareRoot(36.0));
    ASSERT_EQ(18.0, squareRoot(324.0));
    ASSERT_EQ(25.4, squareRoot(645.16));
    ASSERT_EQ(0, squareRoot(0.0));
}

TEST(SquareRootTest, NegativeNos) {
    ASSERT_EQ(-1.0, squareRoot(-15.0));
    ASSERT_EQ(-1.0, squareRoot(-0.2));
    // ASSERT_EQ(-1.0, squareRoot(0.4));
}

TEST(UtilsTest1, NegativeNos) {
    Utils utils;
    ASSERT_EQ(utils.add(1, 1), 2);
}

TEST(UtilsTest2, NegativeNos) {
    Utils2 utils;
    ASSERT_EQ(utils.add2(1, 1), 2);
}

TEST(UtilsTest3, NegativeNos) {
    Utils2 utils;
    ASSERT_EQ(utils.add2(1, 1), 2);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}