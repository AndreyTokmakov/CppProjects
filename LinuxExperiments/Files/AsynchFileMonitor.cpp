/**============================================================================
Name        : AsynchFileMonitor.cpp
Created on  : 15.10.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AsynchFileMonitor.cpp
============================================================================**/

#include "AsynchFileMonitor.hpp"
#include "FileUtilities.hpp"

#include <iostream>
#include <filesystem>
#include <print>
#include <format>

namespace
{
    constexpr std::filesystem::path testDataDir() noexcept
    {
        return std::filesystem::current_path() / "../../resources";
    }
}




namespace AsynchFileMonitor
{

}

void AsynchFileMonitor::TestAll()
{
    const std::filesystem::path testFile = testDataDir() / "test_file.txt";
    const std::string str = FileUtilities::ReadFile(testFile);

    std::cout << str << std::endl;

}