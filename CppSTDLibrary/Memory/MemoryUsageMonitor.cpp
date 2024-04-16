/**============================================================================
Name        : MemoryUsageMonitor.cpp
Created on  : 15.03.2024
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : MemoryUsageTest
============================================================================**/

#include "MemoryUsageMonitor.h"

#include <sys/resource.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>



namespace MemoryUsageMonitor
{

    long get_mem_usage()
    {
        rusage usage{};
        int ret = getrusage(RUSAGE_SELF, &usage);
        return usage.ru_maxrss; // in KB
    }

    double get_cpu_time() {
        rusage usage{};
        getrusage(RUSAGE_SELF, &usage);
        return usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0;
    }

    double getMemoryUsage()
    {
        int resultKb = 0;
        if (std::ifstream statusFile("/proc/self/status"); statusFile.is_open() && statusFile.good())
        {
            std::string line;
            while (std::getline(statusFile, line)) {
                if (line.contains("VmRSS")) {
                    resultKb = atoi(line.data() + 6);
                    break;
                }
            };
        }
        return resultKb / 1024.0;
    }

    // get ID
    // cat /proc/<PROCESS_ID>/status

    void Test()
    {
        std::cout << "Memory: " << getMemoryUsage() << std::endl;

        {
            std::vector<std::string> buffer(10'000'000);

            // std::cout << buffer.capacity() * sizeof (std::string) << std::endl;
            std::cout << "Memory: " << getMemoryUsage() << std::endl;

            int32_t id { getpid() };
            std::cout << "\tPid = " << id << std::endl;

            //std::this_thread::sleep_for(std::chrono::seconds (15));
        }

        //std::cout << (intmax_t) getpid () << std::endl;
        std::cout << "Memory: " << getMemoryUsage() << std::endl;

        //std::this_thread::sleep_for(std::chrono::seconds (30));
    }
};




void MemoryUsageMonitor::TestAll()
{
    Test();
}