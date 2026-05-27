/**============================================================================
Name        : Latch.cpp
Created on  : 02.12.2020
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Latch src class
============================================================================**/

#include "Latch.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <latch>
#include <thread>
#include <chrono>
#include <syncstream>

#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << " "


namespace Latch
{

    void Wait_Test()
    {
        int max_workers = 4;
        std::latch completion_latch(max_workers);

        std::vector<std::jthread> workers;
        auto task = [&](const uint32_t timeout)-> void {
            LOG << "Sleeping for " << timeout << " seconds" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            completion_latch.count_down();
        };

        LOG << "Starting threads...." << std::endl;
        while (max_workers--)
            workers.emplace_back(task, rand() % 8);

        LOG << "Block with latch.wait() until work is done." << std::endl;
        completion_latch.wait();
        LOG << '\n' << std::string(120,'-') << "\nLatch.wait() done\n" << std::string(120,'-') << '\n';
    }

    void TryWait_Test()
    {
        int max_workers = 4;
        std::latch completion_latch(max_workers);

        std::vector<std::jthread> workers;
        auto task = [&](const unsigned int timeout)-> void {
            LOG << "Waiting for " << timeout << " seconds" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            LOG << "Thread done" << std::endl;
            completion_latch.count_down();

        };

        LOG << "Starting threads...." << std::endl;
        while ((max_workers--) != 0)
            workers.emplace_back(task, rand() % 8);


        LOG << "Block with latch.wait() until work is done." << std::endl;
        while (!completion_latch.try_wait()) {
            std::this_thread::sleep_for(std::chrono::microseconds(10U));
        }
        LOG << "Done" << std::endl;
    }
};


namespace Latch::SharedResourceTest
{
    struct SharedResource
    {
        static constexpr int numOfThreads { 4 };
        std::latch allReady{ numOfThreads };
    };

    void startWorkAtTheSameTime(SharedResource& resource, const int workerId)
    {
        LOG << "Worker " << workerId <<  " started" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds (1UL));

        // Synchronize threads to ensure they all start processing at the same time.
        // Atomically decrements the counter and blocks until the counter reaches zero.
        resource.allReady.arrive_and_wait();

        LOG << "Worker " << workerId <<  " can continue." << std::endl;
        // Simulated work for each thread
        for (int32_t i = 0; i < workerId; ++i)
        {
            LOG << "Worker ID: " << workerId <<  " processing..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2U * workerId));
        }
    }

    void Test()
    {
        SharedResource resource;
        std::vector<std::jthread> workers;

        for (int i { 0 }; i < SharedResource::numOfThreads; ++i) {
            workers.emplace_back(startWorkAtTheSameTime, std::ref(resource), i);
        }
    }
}

void Latch::TestAll()
{
    Wait_Test();
    // TryWait_Test();
    // SharedResourceTest::Test();
}
