//============================================================================
// Name        : main.cpp
// Created on  : 17.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Boost Project
//============================================================================

#include <iostream>
#include "Utils.h"


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    std::cout << "Main app...\n";

    std::cout << Utils().add(1, 1) << std::endl;

    return EXIT_SUCCESS;
}