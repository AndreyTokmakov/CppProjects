//============================================================================
// Name        : ShellCommands.cpp
// Created on  : 04.06.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ShellCommands
//============================================================================

#include "ShellCommands.h"

#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <string_view>

#include "../common.h"

namespace ShellCommands
{
    std::string ExecuteShellCommand(std::string_view command)
    {
        const std::unique_ptr<FILE, decltype(&::pclose)> pipe(popen(command.data(), "r"), ::pclose);
        if (!pipe)
            return std::string {};

        std::array<char, 128> buffer {};
        std::string result;
        while (fgets(buffer.data(), buffer.size(), pipe.get()))
            result.append(buffer.data());
        return result;
    }


    std::string exec(std::string_view command) {
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.data(), "r"), pclose);
        if (!pipe)
            return std::string {};

        char buffer[128];
        std::string result;
        while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr)
            result.append(buffer);
        return result;
    }

    void TestCommand()
    {
        auto output1 = ExecuteShellCommand("ls -la");
        std::cout << output1 << std::endl;


        /*
        auto output2 = exec("ls -la");
        std::cout << output2 << std::endl;
        */
    }
};

void ShellCommands::TestAll()
{
    TestCommand();
};
