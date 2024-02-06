/**============================================================================
Name        : DataParsers.cpp
Created on  : 06.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DataParsers
============================================================================**/

#include "DataParsers.h"

#include <iostream>
#include <string_view>
#include <vector>
#include <fstream>
#include <filesystem>

namespace Paths
{
    constexpr std::string_view dataFilePath {R"(../../FinTechMarketProjects/data/orders2.csv)"};
    constexpr std::string_view dataFilePathPart {R"(/home/andtokm/Projects/CppProjects/FinTechMarketProjects/data/orders_test1.dat)"};
    constexpr std::string_view testOrdersData {R"(/home/andtokm/Temp/orders.csv)"};
}
namespace
{
    [[nodiscard]]
    std::vector<std::string> split(const std::string &str,
                                   const size_t partsExpected = 10,
                                   const std::string& delimiter = std::string {";"})
    {
        std::vector<std::string> params{};
        params.reserve(partsExpected);
        size_t pos = 0, prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos) {
            params.emplace_back(str, prev, pos - prev);
            prev = pos + delimiter.length();
        }
        params.emplace_back(str, prev, str.length() - prev);
        return params;
    }
}

namespace DataParsers
{
    void readData(std::string_view path)
    {
        std::string line;
        if (std::fstream file {path.data()}; file.is_open() && file.good())
        {
            while (std::getline(file, line))
            {
                std::cout << line << std::endl;
            }
        }
    }
}


void DataParsers::TestAll()
{

    DataParsers::readData(Paths::dataFilePath);


}