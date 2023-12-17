//============================================================================
// Name        : main.cpp
// Created on  : 15.09.2021
// Author      : Andrei Tokmakov
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Tests C++ project
//============================================================================

#include <iostream>
#include <vector>
#include <fstream>

constexpr std::string_view path {R"(/home/andtokm/DiskS/Temp/Folder_For_Testing/test_run.log)"};

void OpenFile_Unsafe()
{
    FILE* file = fopen(path.data(), "w");

    // fclose(file);
}

void OpenFile_Safe()
{
    if (std::fstream file = std::fstream(path.data()); file.is_open() && file.good()) {

    }
}

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    OpenFile_Unsafe();
    OpenFile_Safe();

    return EXIT_SUCCESS;
}

