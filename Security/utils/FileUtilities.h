/**============================================================================
Name        : FileUtilities.h
Created on  : 21.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FileUtilities.h
============================================================================**/

#ifndef CPPPROJECTS_FILEUTILITIES_HPP
#define CPPPROJECTS_FILEUTILITIES_H

#include <filesystem>
#include <vector>

namespace FileUtilities
{
    void PrintFileContent(const std::filesystem::path& filePath);

    [[nodiscard]]
    std::string ReadFile(const std::filesystem::path& filePath);

    std::vector<uint8_t> ReadFileAsBytes(const std::filesystem::path &filePath);

    int32_t WriteToFile(const std::filesystem::path& filePath,
                        const std::string& text);

    int32_t WriteToFileBytes(const std::filesystem::path& filePath,
                             const std::vector<uint8_t>& data,
                             std::ios_base::openmode mode = std::ios::out | std::ios::binary);

    int32_t AppendToFile(const std::filesystem::path& filePath,
                         const std::string& text);

    bool ReadFile2String(const std::filesystem::path& filePath,
                         std::string& dst);

    [[nodiscard]]
    std::size_t getFileSize(const std::filesystem::path& filePath);

    [[nodiscard]]
    std::size_t getFileSizeFS(const std::filesystem::path& filePath);
};

#endif //CPPPROJECTS_FILEUTILITIES_HPP
