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

#include "SecureFIleBlockStorage.h"
#include "../utils/FileUtilities.h"

namespace SecureFIleBlockStorage
{
    const std::filesystem::path txtFilePath { R"(../../Security/data/text_file.txt)"};

    void Split_Content()
    {
        const std::string text = FileUtilities::ReadFile(txtFilePath);

        std::cout << text << std::endl;
    }
}

void SecureFIleBlockStorage::TestAll()
{
    Split_Content();
}