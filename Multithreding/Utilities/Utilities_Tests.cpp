/**============================================================================
Name        : Utilities_Tests.cpp
Created on  : 31.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <mutex>
#include <thread>
#include <syncstream>
#include "Utilities.h"


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

void Utilities::TestAll()
{
    Tests::threadAffinity();
}