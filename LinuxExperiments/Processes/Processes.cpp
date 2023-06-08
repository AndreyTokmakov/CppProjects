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
#include <format>

#include <array>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>


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

namespace Processes
{
    using namespace std::string_view_literals;
    using namespace std::string_literals;

    void test() {
        std::array<int, 2> pipeFd{};
        auto &[readFd, writeFd] = pipeFd;

        constexpr std::string_view testData{"qwerty_12345"};

        if (pipe(pipeFd.data()) == -1) {
            std::cout << "pipe() failed. Error = " << errno << std::endl;
            return;
        }

        const pid_t procID = fork();
        if (-1 == procID) {
            std::cout << "fork() failed. Error = " << errno << std::endl;
            std::for_each(pipeFd.begin(), pipeFd.end(), [](int fd) { ::close(fd); });
            return;
        }

        char buf{};
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

    struct LinuxProcess {
        uint32_t pid { 0 };
        uint32_t ppid { 0 };
        std::string name; // TODO: Initialize
        std::string cmdline;
        std::filesystem::path procPath {};
        std::filesystem::path exePath {};

        // environ : Values of environment variables
        // cwd     : working directory

        std::vector<LinuxProcess>::iterator parent;

        // std::vector<LinuxProcess*> children {};
        std::vector<std::vector<LinuxProcess>::iterator> children {};
    };

    void printProcTree(const LinuxProcess& process,
                       const std::string& padding = ""s)
    {
        std::cout << padding << process.pid << " [" << process.exePath << "] (" << process.name << ")\n";
        for (const auto child: process.children) {
            printProcTree(*child, padding + "       ");
        }
    }

    void readProcessList()
    {
        constexpr std::string_view dirPath{R"(/proc/)"};

        std::vector<LinuxProcess> processList {};
        for (uint32_t pid{0}; const auto &entry : std::filesystem::directory_iterator(dirPath))
        {
            const std::string_view name { entry.path().filename().string() };
            if (entry.is_directory()) {
                const auto [ptr, errCode] = std::from_chars(name.data(), name.data() + name.length(), pid);
                if (errCode == std::errc()) {
                    processList.emplace_back(pid, 0, ""s, ""s,
                                             entry.path(), "", processList.end());
                }
            }
        }

        for (LinuxProcess &proc: processList)
        {
            const std::filesystem::path exePath { proc.procPath / "exe" };
            if (std::filesystem::exists(exePath) && is_symlink(exePath)) {
                proc.exePath = read_symlink(exePath);
            }

            const std::filesystem::path cmdLinePath { proc.procPath / "cmdline" };
            if (is_regular_file(cmdLinePath)) {
                if (std::ifstream file(cmdLinePath.string().data()); file.is_open() && file.good()) {
                    std::getline(file, proc.cmdline);
                }
            }

            const std::filesystem::path statusPath { proc.procPath / "status" };
            if (is_regular_file(statusPath))
            {
                if (std::ifstream file(statusPath.string().data()); file.is_open() && file.good())
                {
                    size_t start{0}, end{0};
                    std::string line;
                    while (std::getline(file, line))
                    {
                        if (line.contains("Name:"))
                        {
                            start = line.find_first_not_of(' ', 6);
                            proc.name.assign(line.cbegin() + start, line.cend());
                        }
                        else if (line.contains("PPid:"))
                        {
                            start = line.find_first_not_of(' ', 6);
                            for (end = start; end < line.size(); ++end)
                                if (!std::isdigit(line[end]))
                                    break;

                            const auto [ptr, errCode] = std::from_chars(line.data() + start, line.data() + end, proc.ppid);
                            if (errCode != std::errc()) {
                                // TODO: Handle error
                            }
                        }

                        // name
                    }
                }
            }
        }

        // std::unordered_set<uint32_t> ids { 1 };
        // using TreeType = std::map<uint32_t, std::vector<LinuxProcess>::iterator>;
        using TreeType = std::unordered_map<uint32_t, std::vector<LinuxProcess>::iterator>;
        TreeType processTree {
                {processList.begin()->pid, processList.begin() }
        };

        for (auto procIter = processList.begin() + 1; procIter != processList.end(); ++procIter)
        {
            auto parent = processTree.find(procIter->ppid);
            if (processTree.end() == parent) {
                // std::cout << "Error: [pid: " << procIter->pid << ", ppid: " << procIter->ppid << "]\n";
                continue;
            }

            procIter->parent = parent->second;
            parent->second->children.push_back(procIter);
            processTree.emplace(procIter->pid, procIter);
        }


        auto process = processTree.find(1);
        if (processTree.end() != process) {
            printProcTree(*process->second);
        }
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
        std::filesystem::path path { R"(/proc/4780/status)" };

        if (is_regular_file(path))
        {
            if (std::ifstream file(path.string().data()); file.is_open() && file.good())
            {
                size_t start { 0 }, end { 0 };
                std::string line;
                while (std::getline(file, line))
                {
                    // std::cout << line << std::endl;
                    if (line.contains("Name:"))
                    {
                        start = line.find_first_not_of(' ', 6);
                        std::string name(line.cbegin() + start, line.cend());
                        std::cout << "name = " << name << std::endl;
                    }
                    else if (line.contains("PPid:"))
                    {
                        start = line.find_first_not_of(' ', 6);
                        for (end = start; end < line.size(); ++end)
                            if (!std::isdigit(line[end]))
                                break;

                        int ppid = 0;
                        const auto [ptr, errCode] = std::from_chars(line.data() + start, line.data() + end, ppid);
                        if (errCode == std::errc()) {
                            std::cout << "ppid = " << ppid << std::endl;
                        }
                    }
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

    readProcessList();

    // ProcessFilesystem::Read_CmdLine();
    // ProcessFilesystem::Read_Status();
};