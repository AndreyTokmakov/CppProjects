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
#include "CustomType_ToAndFromJson.h"

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


namespace Nlohmann::JsonParsing
{
    namespace sys
    {
        enum class SystemType: uint8_t
        {
            TypeOne,
            TypeTwo
        };

        struct SystemInformation
        {
            int32_t id { 0 };
            std::string name;
            SystemType type { SystemType::TypeOne };
        };

        void parser_sys_info(const nlohmann::json& json, SystemInformation& system_information)
        {
            json.at("id").get_to(system_information.id);
            json.at("name").get_to(system_information.name);
            json.at("sys_type").get_to(system_information.type);
        }

        NLOHMANN_JSON_SERIALIZE_ENUM(SystemType,
        {
            { SystemType::TypeOne, "TypeOne" },
            { SystemType::TypeTwo, "TypeTwo" }
        })
    }
}

namespace nlohmann
{
    template <typename T>
    struct adl_serializer<std::optional<T>> {
        static void from_json(const json& json, std::optional<T>& opt) {
            if (json.is_null()) {
                opt = std::nullopt;
            } else {
                opt = json.get<T>();
            }
        }
    };

    template <>
    struct adl_serializer<Nlohmann::JsonParsing::sys::SystemInformation> {
        static void from_json(const json& json, Nlohmann::JsonParsing::sys::SystemInformation& system_information) {
            parser_sys_info(json , system_information);
        }
    };
}

namespace Nlohmann::JsonParsing
{
    void test()
    {
        auto jsonString = R"({
            "id": 101, "name": "Param1", "sys_type": "TypeTwo"
        })";

        const nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        sys::SystemInformation system_information = jsonData.get<sys::SystemInformation>();

        std::cout << system_information.id << " "
                  << system_information.name << " "
                  << static_cast<int>(system_information.type) << "\n"
                  << std::endl;

        /*
        json one = "TypeOne";
        json two = "TypeTwo";

        {
            sys::SystemType type = one.get<sys::SystemType>();
            std::cout <<  << std::endl;
        }
        {
            sys::SystemType type = two.get<sys::SystemType>();
            std::cout << static_cast<int>(type) << std::endl;
        }*/
    }
}



void Nlohmann::TestAll()
{
    CustomType_ToAndFromJson::TestAll();

    // checkIsValid();
    // ParseJson_StringStream();

    // ParseJson_File();
    // ParseJson_File2();

    // Create_and_Parse_Json();
    // Create_and_Parse_Json_2();

    // Array::CreateArray();

    // JsonParsing::test();
}