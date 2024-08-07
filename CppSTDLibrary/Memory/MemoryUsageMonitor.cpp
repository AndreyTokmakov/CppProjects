/**============================================================================
Name        : MemoryUsageMonitor.cpp
Created on  : 15.03.2024
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : MemoryUsageTest
============================================================================**/

#include "MemoryUsageMonitor.h"

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <cstdint>
#include <vector>
#include <thread>



namespace ProcessStatusInfo
{
    struct ProcessStatus
    {
        uint32_t processId { 0 };
        uint32_t parentId { 0 };
        double memoryUsage { 0.0 };
        uint32_t threadsCount { 0 };
        uint32_t peakVirtualMemSize { 0 };
        uint32_t contextSwitches { 0 };
        double cpuUsage { 0.0 };
        std::string name;
    };

    void split_to(const std::string &str,
                  std::vector<std::string_view> &parts,
                  const std::string &delimiter)
    {
        parts.clear();
        size_t pos = 0, prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos) {
            parts.emplace_back(str.data() + prev, pos - prev);
            prev = pos + delimiter.length();
        }
        parts.emplace_back(str.data() + prev, str.length() - prev);
    }

    // https://manpages.ubuntu.com/manpages/trusty/man5/proc.5.html
    ProcessStatus getProcessStatus()
    {
        ProcessStatus status {};
        std::string line;
        if (std::ifstream statusFile("/proc/self/status"); statusFile.is_open() && statusFile.good())
        {
            while (std::getline(statusFile, line))
            {

                if (std::string::npos != line.find("VmRSS")) {
                    status.memoryUsage =  atoi(line.data() + 6) / 1024.0;
                }
                else if (0 == line.find("Pid:")) {
                    status.processId = atoi(line.data() + line.find_first_not_of(' ', 5));
                }
                else if (0 == line.find("PPid:")) {
                    status.parentId = atoi(line.data() + line.find_first_not_of(' ', 6));
                }
                else if (0 == line.find("Threads:")) {
                    status.threadsCount = atoi(line.data() + line.find_first_not_of(' ', 9));
                }
                else if (0 == line.find("VmPeak:")) {
                    status.peakVirtualMemSize = atoi(line.data() + line.find_first_not_of(' ', 8));
                }
                else if (0 == line.find("voluntary_ctxt_switches:")) {
                    status.contextSwitches = atoi(line.data() + line.find_first_not_of(' ', 25));
                }
                else if (0 == line.find("Name:")) {
                    status.name.assign(line.data() + line.find_first_not_of(' ', 6));
                }
            };
        }

        static std::vector<std::string_view> parts;
        static uint32_t cpuTimePrev {0};
        static std::chrono::steady_clock::time_point prev = std::chrono::steady_clock::now();

        if (std::ifstream statusFile("/proc/self/stat"); statusFile.is_open() && statusFile.good())
        {
            const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            if (std::getline(statusFile, line)) {
                split_to(line, parts, " ");
            }

            const uint32_t cpuTimeNow = atoi(parts[13].data()) + atoi(parts[14].data());
            const double timePassed = std::chrono::duration_cast<std::chrono::duration<double>>(now - prev).count();

            status.cpuUsage = (cpuTimeNow - cpuTimePrev) / timePassed;

            prev = now;
            cpuTimePrev = cpuTimeNow;
        }
        return status;
    };
}

namespace MemoryUsageMonitor::Tests
{
    using namespace ProcessStatusInfo;

    void PrintProcessStatus()
    {
        const ProcessStatus status = getProcessStatus();

        std::cout << "Process: " << status.name << std::endl;
        std::cout << "\tProcess ID     : " << status.processId << std::endl;
        std::cout << "\tParent ID      : " << status.parentId << std::endl;
        std::cout << "\tMem Usage      : " << status.memoryUsage << std::endl;
        std::cout << "\tThreads        : " << status.threadsCount << std::endl;
        std::cout << "\tVirt Mem Usage : " << status.peakVirtualMemSize << std::endl;
        std::cout << "\tCts Switches   : " << status.contextSwitches << std::endl;
        std::cout << "\tCPU Usage      : " << status.cpuUsage << std::endl;
    }

    void debug()
    {
        uint32_t counter = 0;

        PrintProcessStatus();

        for (int i = 0; i < 200'000; i++)
        {
            counter++;
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            if (counter % 3 == 0 || counter % 123 == 1)
                --counter;
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        PrintProcessStatus();
    }

    void AllocateMemory()
    {
        PrintProcessStatus();
        std::vector<std::string> buffer(10'000'000);
        PrintProcessStatus();
    }
}



void MemoryUsageMonitor::TestAll()
{
    Tests::debug();
    // Tests::AllocateMemory();
}