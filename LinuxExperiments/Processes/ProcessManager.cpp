/**============================================================================
Name        : ProcessManager.cpp
Created on  : 11.06.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Linux ProcessManager C++ experiments
============================================================================**/

#include "ProcessManager.h"

#include <iostream>
#include <string_view>
#include <filesystem>
#include <fstream>
#include <memory>

#include <vector>
#include <list>
#include <map>
#include <unordered_map>

#include <charconv>

// #include <format>

namespace ProcessManager
{
    using namespace std::string_view_literals;
    using namespace std::string_literals;

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
}

namespace ProcessManager::VectorAndMapDemo
{

    struct LinuxProcess
    {
        uint32_t pid { 0 };
        uint32_t ppid { 0 };
        std::string name;
        std::string cmdline;
        std::filesystem::path procPath {};
        std::filesystem::path exePath {};
        // environ : Values of environment variables
        // cwd     : working directory
        std::vector<LinuxProcess>::iterator parent;
        std::vector<std::vector<LinuxProcess>::iterator> children {};
    };

    struct LinuxProcesses
    {
        std::vector<LinuxProcess> processList;
        std::unordered_map<uint32_t, std::vector<LinuxProcess>::iterator> processMap;
    };

    std::vector<LinuxProcess> GetProcessList()
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
                    }
                }
            }
        }
        return processList;
    }

    LinuxProcesses getProcessTree()
    {
        LinuxProcesses procs { .processList = GetProcessList(), .processMap = {} };
        procs.processMap.insert( {procs.processList.begin()->pid, procs.processList.begin() });

        std::vector<LinuxProcess>& procList = procs.processList;
        std::unordered_map<uint32_t, std::vector<LinuxProcess>::iterator>& processTree = procs.processMap;
        for (auto procIter = procList.begin() + 1; procIter != procList.end(); ++procIter)
        {
            auto parent = processTree.find(procIter->ppid);
            if (processTree.end() == parent) {
                // TODO: handle
                continue;
            }

            procIter->parent = parent->second;
            parent->second->children.push_back(procIter);
            processTree.emplace(procIter->pid, procIter);
        }
        return procs;
    }

    void printProcTree(const LinuxProcess& process,
                       const std::string& padding = ""s)
    {
        std::cout << padding << process.pid << " [" << process.exePath << "] (" << process.name << ")\n";
        for (const auto child: process.children) {
            printProcTree(*child, padding + "       ");
        }
    }

    void readProcessListTest()
    {
        const auto processTree = getProcessTree();
        auto process = processTree.processMap.find(1);
        if (processTree.processMap.end() != process)
        {
            printProcTree(*process->second);
        }
    }
}


namespace ProcessManager::ProcessTree
{
    struct LinuxProcess
    {
        uint32_t pid { 0 };
        uint32_t ppid { 0 };
        std::string name;
        std::string cmdline;
        std::filesystem::path procPath{};
        std::filesystem::path exePath{};
        // environ : Values of environment variables
        // cwd     : working directory
        std::shared_ptr<LinuxProcess> parent{};

        // std::vector<std::shared_ptr<LinuxProcess>> children{};
        std::list<std::shared_ptr<LinuxProcess>> children{};

        explicit operator std::string() const noexcept {
            return  std::string {"Process (id: "}.append(std::to_string(pid))
                .append(", parent: ").append(std::to_string(ppid))
                .append(", name: ").append(name)
                .append(", cmdline: ").append(cmdline)
                .append(", exePath: ").append(exePath.filename().string()).append(")");
        }

        /*
        explicit operator std::string() const noexcept {
            return std::format("Process (id: {}, parent: {}, name: {}, cmdline: {}, exePath: {})",
                               pid, ppid, name, cmdline, exePath.filename().string());
        }
        */
    };

    std::map<uint32_t, std::shared_ptr<LinuxProcess>> getProcesses()
    {
        constexpr std::string_view dirPath{R"(/proc/)"};
        std::map<uint32_t, std::shared_ptr<LinuxProcess>> processTree {};

        for (uint32_t pid { 0 }; const auto &entry : std::filesystem::directory_iterator(dirPath))
        {
            const std::string_view name { entry.path().filename().string() };
            if (entry.is_directory())
            {
                if (std::from_chars(name.data(), name.data() + name.length(), pid).ec != std::errc())
                    continue;

                const auto [procIter, ok] = processTree.emplace(pid,
                           std::make_shared<LinuxProcess>(pid,0,""s,""s,entry.path(),"")
                );
                const std::shared_ptr<LinuxProcess>& process { procIter->second } ;

                const std::filesystem::path exePath { process->procPath / "exe" };
                if (std::filesystem::exists(exePath) && is_symlink(exePath)) {
                    process->exePath = read_symlink(exePath);
                }

                const std::filesystem::path cmdLinePath { process->procPath / "cmdline" };
                if (is_regular_file(cmdLinePath)) {
                    if (std::ifstream file(cmdLinePath.string().data()); file.is_open() && file.good()) {
                        std::getline(file, process->cmdline);
                    }
                }

                const std::filesystem::path statusPath { process->procPath / "status" };
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
                                process->name.assign(line.cbegin() + start, line.cend());
                            }
                            else if (line.contains("PPid:"))
                            {
                                start = line.find_first_not_of(' ', 6);
                                for (end = start; end < line.size(); ++end)
                                    if (!std::isdigit(line[end]))
                                        break;

                                const auto [ptr, err] =
                                        std::from_chars(line.data() + start, line.data() + end, process->ppid);
                                if (err != std::errc()) {
                                    // TODO: Handle error
                                }
                            }
                        }
                    }
                }

                if (auto parentIter = processTree.find(process->ppid); processTree.end() != parentIter) {
                    const std::shared_ptr<LinuxProcess>& parent = parentIter->second;

                    process->parent = parent;
                    parent->children.push_back(process);
                    processTree.emplace(process->pid, process);
                }
                else {
                    // TODO: Handle
                }
            }
        }

        return processTree;
    }

    void printProcTree(const LinuxProcess& process,
                       const std::string& padding = ""s)
    {
        std::cout << padding << process.pid << " [" << process.exePath << "] (" << process.name << ")\n";
        for (const auto& child: process.children) {
            printProcTree(*child, padding + "       ");
        }
    }

    void printProcTree(uint32_t processId = 1)
    {
        const auto processTree = getProcesses();
        auto process = processTree.find(processId);
        if (processTree.end() != process) {
            printProcTree(*process->second);
        }
    }

    void printProcTree_Chromium()
    {
        const auto processTree = getProcesses();
        for (const auto& [pid, process]: processTree)
        {
            if (process->name.contains("chrome")) {
                printProcTree(process->pid);
                break;
            }
        }
    }

    void killChromium()
    {
        const auto processTree = getProcesses();
        for (const auto& [pid, process]: processTree)
        {
            if (process->name.contains("chrome") && !process->parent->name.contains("chrome")) {
                // ExecuteShellCommand(std::format("kill -9 {}", process->pid));
                ExecuteShellCommand(std::string {"kill -9 "}.append(std::to_string(process->pid)));
                break;
            }
        }
    }
}

namespace ProcessManager::ProcessFilesystem
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

void ProcessManager::TestAll()
{
    // VectorAndMapDemo::readProcessListTest();

    // ProcessTree::getProcesses();
    // ProcessTree::printProcTree();
    // ProcessTree::printProcTree_Chromium();
    ProcessTree::killChromium();

    // ProcessFilesystem::Read_CmdLine();
    // ProcessFilesystem::Read_Status();
}