/**============================================================================
Name        : PerformanceTests.cpp
Created on  : 04.06.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PerformanceTests.cpp
============================================================================**/

#include "PerformanceTests.h"
#include "FileUtilities.h"

#include "json/json.h"
#include <nlohmann/json.hpp>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"


#include <fstream>
#include <iostream>


namespace
{
    struct ScopedTimer
    {
        const std::string_view benchmarkName;
        const std::chrono::high_resolution_clock::time_point start {
                std::chrono::high_resolution_clock::now()
        };

        explicit ScopedTimer(std::string_view info) :
                benchmarkName {info} {
        }

        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer(ScopedTimer&&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;
        ScopedTimer& operator=(ScopedTimer&&) = delete;

        ~ScopedTimer()
        {
            const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

            std::cout << std::left << std::setw(14) << benchmarkName << ":  ";
            std::cout << time_span.count() << " seconds.\n";
        }
    };
}

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

    constexpr std::string_view paramName {"lastUpdateId"};
    if (document.HasMember(paramName.data()) && document[paramName.data()].IsString())
    {
        const std::string patientTag = document[paramName.data()].GetString();
        std::cout << paramName << " = " << patientTag << std::endl;
    }
    else {
        std::cout << "Not found\n";
    }
}

namespace Parse
{
    using json = nlohmann::json;

    void Parse_Nlohmann()
    {
        std::string jsonString;
        FileUtilities::ReadFile2String(R"(../../JsonCPP/data/snapshot.json)", jsonString);

        ScopedTimer timer { "Nlohmann"};
        for (int i = 0; i < 1'0'000; ++i) {
            json::parse(jsonString);
        }
    }

    void Parse_RapidJson()
    {
        std::string jsonString;
        FileUtilities::ReadFile2String(R"(../../JsonCPP/data/snapshot.json)", jsonString);

        ScopedTimer timer { "RapidJson"};
        for (int i = 0; i < 1'0'000; ++i) {
            rapidjson::Document document;
            document.Parse(jsonString.data());
        }
    }

    void Test()
    {
        std::string jsonString;
        FileUtilities::ReadFile2String(R"(../../JsonCPP/data/snapshot.json)", jsonString);

        // std::cout << jsonString << std::endl;

        rapidjson::Document document;
        // .Parse(jsonString.data());

        if (std::ifstream ifs(R"(../../JsonCPP/data/snapshot.json)"); ifs.is_open() && ifs.good())
        {
            rapidjson::IStreamWrapper isw(ifs);
            document.ParseStream(isw);
        }

        std::cout << document["lastUpdateId"].GetString() << std::endl;
    }
}

void PerformanceTests::TestAll()
{
    // ReadAndParse();

    // Parse::Parse_Nlohmann();
    // Parse::Parse_RapidJson();

    Parse::Test();
}