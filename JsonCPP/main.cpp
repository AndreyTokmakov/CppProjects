//============================================================================
// Name        : JsonCPP.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : JsonCPP C++ project
//============================================================================

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>

#include <thread>
#include <future>

#include <mutex>
#include <atomic>

#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>

#include <vector>
#include <any>
#include <list>
#include <map>
#include <algorithm>
#include <array>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <span>

#include "json/json.h"
#include <fstream>
#include <iostream>

void Test()
{
    Json::Value root;
    std::ifstream file(R"(/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/JsonCPP/data/widget.json)");

    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, file, &root, &errs)) {
        std::cout << errs << std::endl;
        // return EXIT_FAILURE;
    }
    std::cout << root << std::endl;
}



int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);



    // TODO: Do the parsing


    return EXIT_SUCCESS;
}

