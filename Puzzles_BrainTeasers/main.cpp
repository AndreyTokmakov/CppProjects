/**============================================================================
Name        : main.cpp
Created on  : 02.04.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Puzzles_BrainTeasers
============================================================================**/

#include <iostream>
#include <vector>
#include <string_view>
#include "Puzzles.h"

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Puzzles::False_Start();
    // Puzzles::No_Memory_CleanUp();
    // Puzzles::Class_Members_Creation_Order();
    // Puzzles::Lifetime_Global_and_Static_Variables();
    Puzzles::Assign_to_Function();

    return EXIT_SUCCESS;
}

