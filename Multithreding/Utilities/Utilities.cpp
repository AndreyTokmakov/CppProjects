/**============================================================================
Name        : Utilities.cpp
Created on  : 31.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities.cpp
============================================================================**/

#include <iostream>
#include <mutex>
#include <thread>
#include <syncstream>
#include <iomanip>
#include "Utilities.h"

namespace Utilities
{
    bool setThreadCore(const uint32_t coreId) noexcept
    {
        cpu_set_t cpuSet {};
        CPU_ZERO(&cpuSet);
        CPU_SET(coreId, &cpuSet);
        return 0 == pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuSet);
    }
}

namespace Utilities::Tests
{
    void thread_affinity_worker(int coreId)
    {
        const auto threadId { std::this_thread::get_id() };
        if (!setThreadCore(coreId))
        {
            std::cerr << "Failed to set core " << coreId << " for " << threadId << std::endl;
            return;
        }

        for(int i = 0; i < 4; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds (250u));
            std::osyncstream(std::cout) << threadId << " | " << i << ": on CPU " << sched_getcpu() << "\n";
        }
    }

    void threadAffinity()
    {
        std::jthread thread1(thread_affinity_worker, 1);
        std::jthread thread2(thread_affinity_worker, 2);
    }
}


namespace Utilities
{
    ScopedTimer::~ScopedTimer()
    {
        const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

        std::cout << std::left << std::setw(14) << benchmarkName << ":  ";
        std::cout << time_span.count() << " seconds.\n";
    }
}



void Utilities::TestAll()
{
    Tests::threadAffinity();
}