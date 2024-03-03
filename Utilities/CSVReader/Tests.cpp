/**============================================================================
Name        : Tests.cpp
Created on  : 03.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CSV Tests
============================================================================**/

#include "CSVReader.h"
#include "Tests.h"

namespace CSV_Reader_Tests
{
    using namespace CSVReader;

    void Test_ParseFile()
    {
        constexpr std::string_view csvFile { R"(../../Utilities/data/anime.csv)"};
        CSVData data = readCsv(csvFile);

        // for (const auto& [hdr, idx]: data.headers)
        //    std::cout << hdr << " = " << idx << std::endl;

        for (const Row& row: data.rows)
        {
            std::cout << row["anime_id"].asInt() << "    " <<  row["name"] << "  " << row["episodes"] << std::endl;
        }
    }
}


void Value_Tests()
{
    using namespace CSVReader;

    Value val {"123.45"};
    std::cout << val.asInt() << std::endl;
    // std::cout << val.asDouble() << std::endl;
}

void CSV_Reader_Tests::TestAll()
{
    // CSV_Reader_Tests::Test_ParseLine();
    // CSV_Reader_Tests::Test_ParseFile();
    // CSV_Reader_Tests::Value_Tests();

    Value_Tests();
}