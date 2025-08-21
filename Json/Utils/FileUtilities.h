/**============================================================================
Name        : FileUtilities.h
Created on  : 04.06.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FileUtilities.h
============================================================================**/

#ifndef CPPPROJECTS_FILEUTILITIES_HPP
#define CPPPROJECTS_FILEUTILITIES_H

#include <filesystem>
#include <fstream>

namespace FileUtilities
{

    void PrintFileContent(const std::filesystem::path &filePath);

    std::string ReadFile(const std::filesystem::path &filePath);

    bool ReadFile2String(const std::filesystem::path &filePath,
                         std::string& dst);

    std::size_t getFileSize(const std::filesystem::path &filePath);

    std::size_t getFileSizeFS(const std::filesystem::path &filePath);

    int32_t WriteToFile(const std::filesystem::path& filePath,
                        const std::string& text,
                        std::ios_base::openmode mode);

    int32_t WriteToFile(const std::filesystem::path& filePath,
                        const std::string& text);

    int32_t AppendToFile(const std::filesystem::path& filePath,
                         const std::string& text);
}



#endif //CPPPROJECTS_FILEUTILITIES_HPP
