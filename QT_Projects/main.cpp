//============================================================================
// Name        : QT_Projects.cpp
// Created on  : 
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : QT_Projects
//============================================================================

#include <iostream>
#include <string_view>
#include <vector>

#include "Experiments/Experiments.h"
#include "Widgets/Widgets.h"

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    Experiments::TestAll(argc, argv);
    // Widgets::TestAll(argc, argv);


    return EXIT_SUCCESS;
}

