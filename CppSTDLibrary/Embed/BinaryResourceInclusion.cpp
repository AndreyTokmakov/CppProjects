/**============================================================================
Name        : BinaryResourceInclusion.cpp
Created on  : 23.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : BinaryResourceInclusion.cpp
============================================================================**/

#include "BinaryResourceInclusion.hpp"

#include <iostream>

namespace Embed
{
    static constexpr unsigned char data[] = {
#embed "/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/CppSTDLibrary/data/test_file.txt"
            ,'\0' // null terminator
    };

    static constexpr unsigned char message[] =
    {
#embed "/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/CppSTDLibrary/data/message.txt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g', '\n')
                    ,'\0' // null terminator
    };

    void test()
    {
        std::cout << data << std::endl;
        std::cout << message << std::endl;
    }
}


void Embed::TestAll()
{
    Embed::test();
}
