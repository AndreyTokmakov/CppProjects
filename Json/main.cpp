//============================================================================
// Name        : JsonCPP.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : JsonCPP C++ project
//============================================================================

// cd Project/third_party
// git clone git@github.com:open-source-parsers/jsoncpp.git
// git clone git@github.com:nlohmann/json.git
// git clone git@github.com:Tencent/rapidjson.git

#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <filesystem>

#include "RapidJson/RapidJson.h"
#include "Nlohmann/Nlohmann.h"
#include "FileUtilities.h"
#include "PerformanceTests.h"

#include "json/json.h"




namespace JsonCpp
{
    void Test()
    {

        Json::Value root;
        std::ifstream file(R"(../../JsonCPP/data/widget.json)");

        Json::CharReaderBuilder builder;
        JSONCPP_STRING errs;
        if (!parseFromStream(builder, file, &root, &errs)) {
            std::cout << errs << std::endl;
            // return EXIT_FAILURE;
        }
        std::cout << root << std::endl;
    }
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // RapidJson::TestAll();
    Nlohmann::TestAll();
    // JsonCpp::Test();


    // PerformanceTests::TestAll();

    return EXIT_SUCCESS;
}

