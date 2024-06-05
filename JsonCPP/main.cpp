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

#include "FileUtilities.h"
#include "PerformanceTests.h"

#include "json/json.h"
#include <nlohmann/json.hpp>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"


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

namespace Nlohmann // INFO: https://www.studyplan.dev/pro-cpp/json
{

    using json = nlohmann::json;


    void checkIsValid()
    {
        // a valid JSON text
        auto valid_text = R"(
        {
            "numbers": [1, 2, 3]
        }
        )";

        auto invalid_text = R"(
        {
            "strings": ["extra", "comma", ]
        }
        )";

        std::cout << std::boolalpha
                  << json::accept(valid_text) << ' '
                  << json::accept(invalid_text) << '\n';
    }

    // INFO: https://www.studyplan.dev/pro-cpp/json
    void ParseJson_StringStream()
    {
        // JSON Lines (see https://jsonlines.org)
        std::stringstream input;
        input << R"({"name": "Gilbert", "wins": [["straight", "7♣"], ["one pair", "10♥"]]}
{"name": "Alexa", "wins": [["two pair", "4♠"], ["two pair", "9♠"]]}
{"name": "May", "wins": []}
{"name": "Deloise", "wins": [["three of a kind", "5♣"]]}
)";

        std::string line;
        while (std::getline(input, line))
        {
            std::cout << json::parse(line) << std::endl;
        }
    }


    void ParseJson_File()
    {
        std::string jsonString;
        FileUtilities::ReadFile2String(R"(../../JsonCPP/data/snapshot.json)", jsonString);

        std::cout << json::parse(jsonString) << std::endl;
    }

    void ParseJson_File2()
    {
        std::cout << json::parse(std::ifstream (R"(../../JsonCPP/data/snapshot.json)")) << std::endl;
    }
}


namespace RapidJson
{
    void ReadAndParse()
    {
        constexpr std::string_view jsonFile { R"(../../JsonCPP/data/snapshot.json)" };

        rapidjson::Document document;
        if (std::ifstream ifs(jsonFile.data()); ifs.is_open() && ifs.good())
        {
            rapidjson::IStreamWrapper isw(ifs);
            document.ParseStream(isw);
            // Can be removed: filebuf object is RAII
            ifs.close();
        }

        if (document.IsNull() || !document.IsObject())
        {
            std::cout << "Failed to read document " << jsonFile << std::endl;
            return;
        }

        std::cout << document.IsArray() << std::endl;

        constexpr std::string_view tagParamerName {"lastUpdateId"};
        if (document.HasMember(tagParamerName.data()) && document[tagParamerName.data()].IsString())
        {
            const std::string patientTag = document[tagParamerName.data()].GetString();
            std::cout << tagParamerName << " = " << patientTag << std::endl;
        }
        else {
            std::cout << "Not found\n";
        }
    }
}



int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // JsonCpp::Test();

    // Nlohmann::checkIsValid();
    // Nlohmann::ParseJson_StringStream();

    // Nlohmann::ParseJson_File();
    // Nlohmann::ParseJson_File2();

    RapidJson::ReadAndParse();

    // PerformanceTests::TestAll();

    return EXIT_SUCCESS;
}

