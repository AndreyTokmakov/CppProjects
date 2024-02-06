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
    constexpr std::string_view dataFilePath {
        "/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/FinTechMarketProjects/data/orders1.dat"
    };
    constexpr std::string_view dataFilePathPart {
        "/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/FinTechMarketProjects/data/orders_part.csv"
    };
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

    void split_to(const std::string &str,
                  std::vector<std::string_view>& params,
                  const size_t partsExpected = 10,
                  const std::string& delimiter = std::string {";"})
    {
        params.clear();
        size_t pos = 0, prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos) {
            params.emplace_back(str.data() + prev, pos - prev);
            prev = pos + delimiter.length();
        }
        params.emplace_back(str.data() + prev, str.length() - prev);
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
    // DataParsers::readData(Paths::dataFilePathPart);

    std::string text { "123456789" };

    std::string_view view (text.data() + 2, 2);
    std::cout << view << std::endl;

}