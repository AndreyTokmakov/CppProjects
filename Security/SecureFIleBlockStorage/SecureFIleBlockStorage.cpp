/**============================================================================
Name        : SecureFIleBlockStorage.cpp
Created on  : 01.12.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SecureFIleBlockStorage.cpp
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>
#include <chrono>

#include "SecureFIleBlockStorage.h"
#include "../utils/FileUtilities.h"

namespace SecureFIleBlockStorage
{
    const std::filesystem::path txtFilePath { R"(../../Security/data/text_file.txt)"};

    constexpr size_t blockSize { 32 };

    struct Header
    {
        uint64_t timestamp { 0 };
        uint64_t length { 0 };
        uint64_t hash { 0 };
        std::string data { 0 };
    };

    void Split_Content()
    {
        const std::string text = FileUtilities::ReadFile(txtFilePath);

        std::vector<std::string> parts;
        const size_t dataSize { text.size() };
        size_t pos = 0, bytesRemaining = blockSize;
        while (blockSize == bytesRemaining)
        {
            bytesRemaining = std::min(dataSize - pos, blockSize);
            parts.emplace_back(text, pos, bytesRemaining);
            pos += blockSize;
        }

        std::cout << text << std::endl;
        for (const std::string& str: parts)
            std::cout << str;
        std::cout << std::endl;
    }

    void Split_Content_AddHeaders()
    {
        const std::string text = FileUtilities::ReadFile(txtFilePath);

        const uint64_t timeStamp = std::chrono::system_clock::now().time_since_epoch().count();
        std::vector<Header> parts;
        const size_t dataSize { text.size() };
        size_t pos = 0, bytesRemaining = blockSize;
        while (blockSize == bytesRemaining)
        {
            bytesRemaining = std::min(dataSize - pos, blockSize);

            Header& hdr = parts.emplace_back(timeStamp, bytesRemaining);
            hdr.data.assign(text, pos, bytesRemaining);
            hdr.hash = std::hash<std::string>{}(hdr.data);

            pos += blockSize;
        }

        std::cout << text << std::endl;
        for (const Header& hdr: parts)
        {
            std::cout << hdr.length << " | " << hdr.hash << " | " << hdr.data << std::endl;
        }
        std::cout << std::endl;
    }
}

void SecureFIleBlockStorage::TestAll()
{
    // Split_Content();
    Split_Content_AddHeaders();
}