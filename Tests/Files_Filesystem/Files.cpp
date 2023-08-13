/**============================================================================
Name        : Files.cpp
Created on  : 06.11.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Files
============================================================================**/

#include "Files.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace Files
{
    void TestFileLength()
    {
        constexpr std::string_view file_path
                { R"(/home/andtokm/DiskS/Temp/TESTING_ROOT_DIR/test_run.log)"};

        if (std::ifstream file(file_path.data(), std::ios::binary); file.is_open() && file.good()) {
            file.seekg(0, std::ios_base::end);
            const size_t fileSize = file.tellg();
            file.seekg(0, std::ios_base::beg);

            std::cout << "Size : " << fileSize << std::endl;
            std::cout << "Size : " << std::filesystem::file_size(file_path) << std::endl;
        }
    }


    void ReadFileBlocks()
    {
        constexpr std::string_view file_path
                { R"(/home/andtokm/DiskS/Temp/TESTING_ROOT_DIR/test_run.log)"};

        size_t bytesReadTotal { 0 };
        char buffer[1024 * 1024] {};
        if (std::ifstream file(file_path.data(), std::ios::binary); file.is_open() && file.good()) {
            std::streamsize bytesRead {0};
            while (0 < (bytesRead = file.readsome(buffer, std::size(buffer)))) {
                // std::cout << bytesRead << std::endl;
                bytesReadTotal += bytesRead;
            }
        }

        std::cout << "Bytes read: " << bytesReadTotal << std::endl;
        std::cout << "File size : " << std::filesystem::file_size(file_path) << std::endl;
    }
};


void Files::TestAll()
{
    TestFileLength();

    // ReadFileBlocks();
};


