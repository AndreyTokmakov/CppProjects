//============================================================================
// Name        : Latch.cpp
// Created on  : 02.12.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Latch src class
//============================================================================

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
    template<typename ...Args>
    void debug1(Args&&... args) {
        std::osyncstream {std::cout} << std::format("{:%d-%m-%Y %H:%M:%OS}", std::chrono::system_clock::now()) << " ";
        (std::osyncstream {std::cout}  << ... << std::forward<Args>(args)) << std::endl;
    }

    std::mutex mtx;

    template<typename ...Args>
    void debug(Args&&... args) {
        std::lock_guard<std::mutex> lock {mtx};
        std::cout << std::format("{:%d-%m-%Y %H:%M:%OS}", std::chrono::system_clock::now()) << " ";
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
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        debug("Done");
    }
};

void Latch::TEST_ALL()
{
    // Wait_Test();
    TryWait_Test();
}
