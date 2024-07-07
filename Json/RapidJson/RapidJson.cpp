/**============================================================================
Name        : RapidJson.cpp
Created on  : 07.07.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RapidJson.cpp
============================================================================**/

#include "RapidJson.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"

#include <iostream>
#include <fstream>


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

    void Print_As_String()
    {
        constexpr std::string_view jsonFile { R"(../../Json/data/snapshot.json)" };

        rapidjson::Document document;
        if (std::ifstream ifs(jsonFile.data()); ifs.is_open() && ifs.good()) {
            rapidjson::IStreamWrapper isw(ifs);
            document.ParseStream(isw);
        } else {
            return;
        }

        rapidjson::StringBuffer strBuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);

        strBuf.Clear();
        document.Accept(writer);

        std::cout << strBuf.GetString() << std::endl;
    }
};

void RapidJson::TestAll()
{
    // ReadAndParse();
    Print_As_String();

}