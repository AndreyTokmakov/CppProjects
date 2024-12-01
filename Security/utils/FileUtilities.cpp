/**============================================================================
Name        : FileUtilities.cpp
Created on  : 21.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FileUtilities.cpp
============================================================================**/

#include "FileUtilities.h"

#include <iostream>
#include <fstream>

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

    std::vector<uint8_t> ReadFileAsBytes(const std::filesystem::path &filePath)
    {
        if (std::fstream file(filePath , std::ios::in | std::ios::binary); file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            size_t fileSize = file.tellg(), bytesRead = 0;
            file.seekg(0, std::ios_base::beg);

            std::vector<uint8_t> data (fileSize);
            while ((bytesRead += file.readsome(reinterpret_cast<char *>(data.data() + bytesRead), readBlockSize)) < fileSize) { }
            return data;
        }
        return {};
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

    int32_t WriteToFileBytes(const std::filesystem::path& filePath,
                             const std::vector<uint8_t>& data,
                             std::ios_base::openmode mode)
    {
        if (std::fstream file(filePath, mode); file.is_open() && file.good())
        {
            const int32_t pos = static_cast<int32_t>(file.tellp());
            file.write(reinterpret_cast<const char *>(data.data()), std::ssize(data));
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
