//============================================================================
// Name        : Processes.cpp
// Created on  : 02.06.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Processes
//============================================================================

#include "Processes.h"

#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <algorithm>
#include <filesystem>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstdint>
#include <charconv>
#include <vector>
#include <format>


namespace Processes
{
    void Test() {
        auto id = (intmax_t) getpid ();
        auto parentId = (intmax_t) getppid ();

        std::cout << "ID: " << id << ", Parent: " << parentId << std::endl;
    }

    void CreateProcess_Fork() {
        switch (int pid = fork()) {
            case -1:
                std::cout << "Fork() failed. Error = " << errno << '\n';
                return;
            case 0:
                std::cout << "Child:  My pid = " << getpid() << " returned pid = " << pid << '\n';
                return;
            default:
                std::cout << "Parent: My pid = " << getpid() << " returned pid = " << pid << '\n';
                return;
        }
    }

    void CreateProcess_Fork_2() {
        pid_t pid = fork();

        if(pid == 0) {
            std::cout << "Child => PPID: " << getppid() << ", PID: " << getpid() << std::endl;
            return;
        }
        else if(pid > 0) {
            std::cout << "Parent => PID: " << getpid() << std::endl;
            std::cout << "Waiting for child process to finish."<< std::endl;
            wait(nullptr);
            std::cout << "Child process finished.\n"<< std::endl;
        }
        else {
            std::cout << "Unable to create child process" << std::endl;
        }
    }
};

void test()
{
    std::array<int, 2> pipeFd {};
    auto& [readFd, writeFd] = pipeFd;

    constexpr std::string_view testData {"qwerty_12345"};

    if (pipe(pipeFd.data()) == -1) {
        std::cout << "pipe() failed. Error = " << errno << std::endl;
        return;
    }

    const pid_t procID = fork();
    if (-1 == procID) {
        std::cout << "fork() failed. Error = " << errno << std::endl;
        std::for_each(pipeFd.begin(), pipeFd.end(), [] (int fd) { ::close(fd); });
        return;
    }

    char buf {};
    if (procID == 0) {
        close(writeFd);
        while (read(readFd, &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);
        write(STDOUT_FILENO, "\n", 1);
        close(readFd);
        _exit(EXIT_SUCCESS);
    } else {
        close(readFd);
        write(writeFd, testData.data(), testData.size());
        close(writeFd);
        wait(nullptr);
        exit(EXIT_SUCCESS);
    }
}

void getProcessList()
{
    struct LinuxProcess
    {
        uint32_t ppid { 0 };
        std::string cmdline;
        std::filesystem::path procPath {};
        std::filesystem::path exePath {};

        // environ : Values of environment variables
        // cwd     : working directory
    };

    constexpr std::string_view dirPath { R"(/proc/)" };

    std::vector<LinuxProcess> processList {};
    for (uint32_t pid {0}; const auto& entry : std::filesystem::directory_iterator(dirPath))
    {
        const std::string_view name { entry.path().filename().string() };
        if (entry.is_directory()) {
            const auto [ptr, errCode] = std::from_chars(name.data(), name.data() + name.length(), pid);
            if (errCode == std::errc()) {
                processList.emplace_back(pid, std::string(), entry.path());
            }
        }
    }

    for (LinuxProcess& proc: processList)
    {
        const std::filesystem::path exePath { proc.procPath / "exe" };
        if (std::filesystem::exists(exePath) && is_symlink(exePath)) {
            proc.exePath = read_symlink(exePath);
        }

        const std::filesystem::path cmdLinePath { proc.procPath / "cmdline" };
        if (is_regular_file(cmdLinePath)) {
            if (std::ifstream file(cmdLinePath.string().data()); file.is_open() && file.good())
            {
                std::getline(file, proc.cmdline);
            }
        }
    }

    for (const LinuxProcess& proc: processList) {
        std::cout << proc.ppid << std::endl;
        std::cout << '\t' << proc.exePath << std::endl;
        std::cout << '\t' << proc.cmdline << std::endl;
    }
}


namespace Processes::ProcessFilesystem
{
    void Read_CmdLine()
    {
        std::filesystem::path path { R"(/proc/145928/cmdline)" };

        if (is_regular_file(path))
        {
            std::cout << path.string().data() << std::endl;
            if (std::ifstream file(path.string().data()); file.is_open() && file.good())
            {
                std::string line;
                std::getline(file, line);
                std::cout << line << std::endl;
            }
        }
    }

    void Read_Status()
    {
        std::filesystem::path path { R"(/proc/145928/status)" };

        if (is_regular_file(path))
        {
            std::cout << path.string().data() << std::endl;
            if (std::ifstream file(path.string().data()); file.is_open() && file.good())
            {
                std::string line;
                while (std::getline(file, line)) {
                    std::cout << line << std::endl;

                }
            }
        }
    }
}

void Processes::TestAll()
{
    // Test();

    // CreateProcess_Fork();

    // CreateProcess_Fork_2();

    // test();

    // getProcessList();

    // ProcessFilesystem::Read_CmdLine();
    ProcessFilesystem::Read_Status();
};