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
#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>

namespace FileUtilities
{
    constexpr size_t readBlockSize { 1024 };

    void PrintFileContent(const std::filesystem::path &filePath)
    {
        if (std::ifstream file(filePath); file.is_open() && file.good())
        {
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;
            }
        }
    }

    std::string ReadFile(const std::filesystem::path &filePath)
    {
        if (std::ifstream file(filePath); file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            size_t fileSize = file.tellg(), bytesRead = 0;
            file.seekg(0, std::ios_base::beg);

            std::string text(fileSize, '\0');
            while ((bytesRead += file.readsome(text.data() + bytesRead, readBlockSize)) < fileSize) { }
            return text;
        }
        return {};
    }

    bool ReadFile2String(const std::filesystem::path &filePath,
                         std::string& dst)
    {
        if (std::ifstream file(filePath); file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            size_t fileSize = file.tellg(), bytesRead = 0;
            file.seekg(0, std::ios_base::beg);

            dst.resize(fileSize);
            while ((bytesRead += file.readsome(dst.data() + bytesRead, readBlockSize)) < fileSize) { }
            return true;
        }
        return false;
    }

    std::size_t getFileSize(const std::filesystem::path &filePath)
    {
        if (std::ifstream file(filePath); file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            const size_t fileSize = file.tellg();
            file.seekg(0, std::ios_base::beg);
            return fileSize;
        }
        return std::string::npos;
    }

    std::size_t getFileSizeFS(const std::filesystem::path &filePath)
    {
        return std::filesystem::file_size(filePath);
    }

    int32_t WriteToFile(const std::filesystem::path& filePath,
                        const std::string& text,
                        std::ios_base::openmode mode)
    {
        if (std::ofstream file(filePath, mode); file.is_open() && file.good())
        {
            const int32_t pos = static_cast<int32_t>(file.tellp());
            file.write(text.data(), std::ssize(text));
            return static_cast<int32_t>(file.tellp()) - pos;
        }
        return -1;
    }

    int32_t WriteToFile(const std::filesystem::path& filePath,
                        const std::string& text)
    {
        return WriteToFile(filePath, text, std::ios_base::trunc);
    }

    int32_t AppendToFile(const std::filesystem::path& filePath,
                         const std::string& text)
    {
        return WriteToFile(filePath, text, std::ios_base::app);
    }
}


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
        FileUtilities::ReadFile2String(R"(../../JsonCPP/data/widget.json)", jsonString);

        std::cout << json::parse(jsonString) << std::endl;
    }
}


// git clone git@github.com:open-source-parsers/jsoncpp.git
// git clone git@github.com:nlohmann/json.git

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // JsonCpp::Test();

    // Nlohmann::checkIsValid();
    // Nlohmann::ParseJson_StringStream();
    Nlohmann::ParseJson_File();


    return EXIT_SUCCESS;
}

