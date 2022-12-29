//============================================================================
// Name        : Files.cpp
// Created on  : 02.06.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Files
//============================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include <thread>


#include "Files.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>


namespace Files
{
    void OpenFile() {
        constexpr std::string_view filePath { R"(/home/andtokm/tmp/TEST_FILES/TestFile.txt)" };
        int fileHandle = open (filePath.data(), O_RDONLY);

        if (-1 == fileHandle) {
            std::cout << "Failed to open file. Error = " << errno << std::endl;
            return;
        } else {
            std::cout << "Filed opened" << std::endl;
        }

        if (-1 == close(fileHandle)) {
            std::cout << "Failed to close file. Error = " << errno << std::endl;
            return;
        } else {
            std::cout << "Filed closed" << std::endl;
        }
    }

    void FileSize() {
        constexpr std::string_view filePath { R"(/home/andtokm/tmp/TEST_FILES/TestFile.txt)" };
        std::unique_ptr<FILE, decltype(&fclose)> file (fopen(filePath.data(), "r"), fclose);
        if (!file) {
            std::cout << "Failed to open file. Error = " << errno << std::endl;
            return;
        }

        fseek(file.get(), 0L, SEEK_END);
        const size_t size = ftell(file.get());

        std::cout << "File size: " << size << " bytest\n";
    }

    void TemporaryFile() {
        std::string nameTemplate {R"(/tmp/somestringXXXXXX)"};

        const int fileHandle = mkstemp(nameTemplate.data());
        if (-1 == fileHandle) {
            std::cout << "Failed to open file. Error = " << errno << std::endl;
            return;
        } else {
            std::cout << "Filed opened" << std::endl;
        }

        std::cout << "Generated filename was: " << nameTemplate << std::endl;

        // std::this_thread::sleep_for(std::chrono::seconds(10));

        unlink(nameTemplate.data());

        if (-1 == close(fileHandle)) {
            std::cout << "Failed to close file. Error = " << errno << std::endl;
            return;
        } else {
            std::cout << "Filed closed" << std::endl;
        }
    }
};

void Files::TestAll()
{
    // OpenFile();
    // FileSize();
    TemporaryFile();
};