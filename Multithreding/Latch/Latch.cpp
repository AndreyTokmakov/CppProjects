/**============================================================================
Name        : Latch.cpp
Created on  : 02.12.2020
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Latch src class
============================================================================**/

#include "Latch.h"

#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <latch>
#include <future>
#include <thread>
#include <chrono>
#include <iomanip>
#include <syncstream>

namespace
{
    using namespace std::chrono;

    constexpr char FORMAT[] { "[%d-%02d-%02d %02d:%02d:%02d.%06ld] " };

    std::string getCurrentTime(const time_point<system_clock>& timestamp = system_clock::now()) noexcept
    {
        const time_t time { system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(buffer.data(), FORMAT,
              tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
              duration_cast<microseconds>(timestamp - time_point_cast<seconds>(timestamp)).count());
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
    }

    template<typename ...Args>
    void debug1(Args&&... args)
    {
        std::osyncstream {std::cout } << std::format("{:%d-%m-%Y %H:%M:%OS}", system_clock::now()) << " ";
        (std::osyncstream {std::cout }  << ... << std::forward<Args>(args)) << std::endl;
    }

    std::mutex mtx;

    template<typename ...Args>
    void debug(Args&&... args)
    {
        std::lock_guard<std::mutex> lock {mtx};
        // std::cout << std::format("{:%d-%m-%Y %H:%M:%OS}", system_clock::now()) << " ";
        std::cout << getCurrentTime();
        (std::cout  << ... << std::forward<Args>(args)) << std::endl;
    }

}

namespace Latch
{

    void Wait_Test()
    {
        int max_workers = 4;
        std::latch completion_latch(max_workers);

        std::vector<std::jthread> workers;
        auto task = [&](unsigned int timeout)-> void {
            debug("Waiting for ", timeout, " seconds");
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            completion_latch.count_down();
        };

        debug("Starting threads....");
        while (max_workers--)
            workers.emplace_back(task, rand() % 8);

        debug("Block with latch.wait() until work is done.");
        completion_latch.wait();
        debug( "\n ****** Latch.wait() done. ***** \n");
    }

    void TryWait_Test()
    {
        int max_workers = 4;
        std::latch completion_latch(max_workers);

        std::vector<std::jthread> workers;
        auto task = [&](unsigned int timeout)-> void {
            debug("Waiting for ", timeout, " seconds");
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            debug("Thread done");
            completion_latch.count_down();

        };

        debug("Starting threads....");
        while (max_workers--)
            workers.emplace_back(task, rand() % 8);


        debug("Block with latch.wait() until work is done.");
        while (!completion_latch.try_wait()) {
            std::this_thread::sleep_for(std::chrono::microseconds(10U));
        }
        debug("Done");
    }
};


namespace Latch::SharedResourceTest
{
    struct SharedResource
    {
        static constexpr int numOfThreads { 4 };
        std::latch allReady{ numOfThreads };
    };

    void startWorkAtTheSameTime(SharedResource& resource, int workerId)
    {
        debug("Worker ", workerId,  " started");
        std::this_thread::sleep_for(std::chrono::seconds (1UL));

        // Synchronize threads to ensure they all start processing at the same time.
        // Atomically decrements the counter and blocks until the counter reaches zero.
        resource.allReady.arrive_and_wait();


        debug("Worker ", workerId,  " can continue.");
        // Simulated work for each thread
        for (int i{ 0 }; i < workerId; ++i)
        {
            debug("Worker ID: ", workerId,  " processing..." );
            std::this_thread::sleep_for(std::chrono::milliseconds(2U * workerId));
        }
    }

    void Test()
    {
        SharedResource resource;
        std::vector<std::jthread> workers;

        for (int i { 0 }; i < resource.numOfThreads; ++i) {
            workers.emplace_back(startWorkAtTheSameTime, std::ref(resource), i);
        }
    }
}

void Latch::TEST_ALL()
{
    // Wait_Test();
    // TryWait_Test();

    SharedResourceTest::Test();
}
