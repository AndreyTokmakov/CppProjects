/**============================================================================
Name        : Files.cpp
Created on  : 06.11.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Files
============================================================================**/

#include "Files.h"
#include "FileUtilities.h"

#include <iostream>
#include <fstream>
#include <random>

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

    void Experiments()
    {
        namespace fs = std::filesystem;

        // Create a backup folder if it doesn't exist
        const fs::path backupFolder = "/tmp/backup";
        if (!fs::exists(backupFolder))
            create_directory(backupFolder);


        // Check if there is sufficient space
        /*
        if (fs::space(backup_folder).available < fs::file_size(file))
            throw std::runtime_error("Not enough space for backup.");
        */

        const fs::space_info spaceInfo = fs::space(backupFolder);
        std::cout << "Space info : " << backupFolder << std::endl;
        std::cout << "\tavailable: " << spaceInfo.available << std::endl;
        std::cout << "\tfree     : " << spaceInfo.free << std::endl;
        std::cout << "\tcapacity : " << spaceInfo.capacity << std::endl;

    }

    void demo_perms(std::filesystem::perms p)
    {
        using std::filesystem::perms;
        auto show = [=](char op, perms perm)
        {
            std::cout << (perms::none == (perm & p) ? '-' : op);
        };
        show('r', perms::owner_read);
        show('w', perms::owner_write);
        show('x', perms::owner_exec);
        show('r', perms::group_read);
        show('w', perms::group_write);
        show('x', perms::group_exec);
        show('r', perms::others_read);
        show('w', perms::others_write);
        show('x', perms::others_exec);
        std::cout << '\n';
    }


    void testFilePermissions()
    {
        namespace fs = std::filesystem;

        // Create a backup folder if it doesn't exist
        const fs::path backupFolder = "/tmp/backup";
        if (!fs::exists(backupFolder))
            create_directory(backupFolder);

        const fs::path filePath = backupFolder / "test.txt";
        std::cout << filePath << std::endl;
        if (std::ofstream file {filePath.c_str()}; file.is_open()) // create file
        {
            std::cout << "Created file with permissions: ";
            demo_perms(std::filesystem::status(filePath).permissions());

            /*
            std::filesystem::permissions(
                    "test.txt",
                    std::filesystem::perms::owner_all | std::filesystem::perms::group_all,
                    std::filesystem::perm_options::add
            );

            std::cout << "After adding u+rwx and g+rwx:  ";
            demo_perms(std::filesystem::status("test.txt").permissions());

            std::filesystem::remove("test.txt");
            */
        }
    }
};

namespace FileUtilities_Tests
{
    const std::string testFilePath { R"(/home/andtokm/DiskS/Temp/Folder_For_Testing/test_file.txt)" };

    void ReadFile()
    {
        std::string text = FileUtilities::ReadFile(testFilePath);
        std::cout << text << std::endl;
    }

    void ReadFile2String()
    {
        std::string text;
        FileUtilities::ReadFile2String(testFilePath, text);
        std::cout << text << std::endl;
    }

    void FileSize()
    {
        std::cout << FileUtilities::getFileSize(testFilePath) << std::endl;
        std::cout << FileUtilities::getFileSizeFS(testFilePath) << std::endl;
    }
}


void Files::TestAll()
{
    // TestFileLength();

    // ReadFileBlocks();

    // Experiments();
    // testFilePermissions();

    // FileUtilities_Tests::ReadFile();
    FileUtilities_Tests::ReadFile2String();
    // FileUtilities_Tests::FileSize();

};


