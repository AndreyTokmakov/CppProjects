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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstdint>
#include <charconv>
#include <vector>

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
        std::filesystem::path procPath {};
        std::filesystem::path exePath {};
    };

    constexpr std::string_view dirPath { R"(/proc/)" };

    std::vector<LinuxProcess> processList {};
    for (uint32_t pid {0}; const auto& entry : std::filesystem::directory_iterator(dirPath))
    {
        const std::string_view name { entry.path().filename().string() };
        if (entry.is_directory()) {
            const auto [ptr, errCode] = std::from_chars(name.data(), name.data() + name.length(), pid);
            if (errCode == std::errc()) {
                processList.emplace_back(pid, entry.path());
            }
        }
    }

    for (LinuxProcess& proc: processList)
    {
        const std::filesystem::path exePath { proc.procPath / "exe" };
        if (std::filesystem::exists(exePath) && is_symlink(exePath)) {
            proc.exePath = read_symlink(exePath);
            std::cout << proc.ppid << "  " << read_symlink(exePath) << std::endl;
        }
    }
}

void Processes::TestAll()
{
    // Test();

    // CreateProcess_Fork();

    // CreateProcess_Fork_2();

    // test();

    getProcessList();
};