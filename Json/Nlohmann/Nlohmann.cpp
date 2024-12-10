/**============================================================================
Name        : Nlohmann.cpp
Created on  : 07.07.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Nlohmann.cpp
============================================================================**/

#include "Nlohmann.h"
#include "FileUtilities.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

namespace Nlohmann
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
        FileUtilities::ReadFile2String(R"(../../Json/data/snapshot.json)", jsonString);

        std::cout << json::parse(jsonString) << std::endl;
    }

    void ParseJson_File2()
    {
        std::cout << json::parse(std::ifstream (R"(../../Json/data/snapshot.json)")) << std::endl;
    }


    void Create_and_Parse_Json()
    {
        nlohmann::json jsonData;
        jsonData["marketId"] = 112233;
        jsonData["name"] = "TEST_1";

        const std::string jsonString = jsonData.dump();
        std::cout << jsonData << "\n" << jsonString << std::endl;

        const nlohmann::json jsonData_New = nlohmann::json::parse(jsonString);
        std::cout << jsonData_New << std::endl;

        // std::string_view value = jsonDataEx["name"].get<std::string_view>();
        // std::cout << value << std::endl;
    }


    void Create_and_Parse_Json_2()
    {
        nlohmann::json jsonData {
            { "marketId", 112233 },
            { "name", "TEST_1" },
        };

        const std::string jsonString = jsonData.dump();
        std::cout << jsonData << "\n" << jsonString << std::endl;

        const nlohmann::json jsonData_New = nlohmann::json::parse(jsonString);
        std::cout << jsonData_New << std::endl;
    }
}


namespace Nlohmann::Array
{
    void CreateArray()
    {
        nlohmann::json jsonArray = nlohmann::json::array({1,2,3});
        std::cout << jsonArray << std::endl;
    }
}



// INFO: https://www.studyplan.dev/pro-cpp/json

void Nlohmann::TestAll()
{
    // checkIsValid();
    // ParseJson_StringStream();

    // ParseJson_File();
    // ParseJson_File2();

    // Create_and_Parse_Json();
    Create_and_Parse_Json_2();

    // Array::CreateArray();
}