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
        constexpr std::string_view file_path { R"(/home/andtokm/Projects/data/Nifti/segm_labels_3d.nii)"};

        std::ifstream ifile(file_path.data(), std::ios::binary);
        if (ifile.is_open()) {
            ifile.seekg(0, std::ios_base::end);
            const size_t fileSize = ifile.tellg();
            ifile.seekg(0, std::ios_base::beg);

            std::cout << "Size = " << fileSize << std::endl;
        }
        ifile.close();
    }

    void FileSize()
    {
        constexpr std::string_view file_path { R"(/home/andtokm/DiskS/Temp/TESTING_ROOT_DIR/test_run.log)"};
        std::cout << "File size = " << std::filesystem::file_size(file_path) << std::endl;
    }

    void ReadFileBlocks()
    {
        constexpr std::string_view file_path
                { R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)"};

        size_t bytesTotal = 0;
        std::ifstream ifile(file_path.data(), std::ios::binary);
        char buffer[1024 * 1024] {};
        if (ifile.is_open()) {
            std::streamsize bytesRead {0};
            while (0 < (bytesRead = ifile.readsome(buffer, std::size(buffer)))) {
                std::cout << bytesRead << std::endl;
                bytesTotal += bytesRead;
            }
        }
        ifile.close();

        std::cout << bytesTotal << std::endl;
    }
};


void Files::TestAll()
{
    // TestFileLength();
    FileSize();
    // ReadFileBlocks();
};


