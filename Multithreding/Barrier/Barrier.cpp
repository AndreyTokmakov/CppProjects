//============================================================================
// Name        : Barrier.h
// Created on  : 02.12.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Barrier src class
//============================================================================

#include <barrier>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <array>
#include <syncstream>

#include "Barrier.h"

namespace Barrier
{
    void Test()
    {
        const auto workers = { "One", "Two", "Three" };

        auto on_completion = []() noexcept {
            // locking not needed here
            static auto phase = "---> Done!! Cleaning up...\n";
            std::cout << phase;
            phase = "... done\n";
        };

        std::barrier sync_point(std::ssize(workers), on_completion);

        auto work = [&](std::string name) {
            std::string product = "  " + name + " worked\n";
            std::cout << product;  // ok, op<< call is atomic

            sync_point.arrive_and_wait();

            product = "  " + name + " cleaned\n";
            std::cout << product;
            sync_point.arrive_and_wait();
        };

        std::cout << "Starting...\n";
        for (std::vector<std::jthread> threads; auto const& worker : workers)
            threads.emplace_back(work, worker);
    }


    void Barrier_With_Completion()
    {
        auto names = { "One", "Two", "Three" };
        std::barrier phase(std::ssize(names),[] {
            std::osyncstream(std::cout) << "Callback()\n";
        });

        auto work = [&](std::string&& name) {
            std::cout << name << " job started\n";
            phase.arrive_and_wait();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << name << " job completed\n";
            phase.arrive_and_wait();
        };

        std::cout << "Starting...\n";
        for (std::vector<std::jthread> jobs; std::string&& name : names)
            jobs.emplace_back(work, name);

    }

    void Check_Block_By_Barrier()
    {
        std::array params { std::make_pair("One", 4), std::make_pair("Two", 2), std::make_pair("Three",3) };
        std::barrier phase(std::ssize(params),[] {
            std::osyncstream(std::cout) << "* * * * * Barrier limit reached!!!* * * * * \n";
        });

        auto work = [&](std::string&& name, uint32_t timeout) {
            std::osyncstream {std::cout} << name << " job started\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            phase.arrive_and_wait();
            std::osyncstream {std::cout} << name << " continuing\n";
        };

        std::cout << "Starting...\n";
        for (std::vector<std::jthread> jobs; auto && param : params)
            jobs.emplace_back(work, param.first, param.second);

    }
};


void Barrier::TEST_ALL()
{
    // Test();
    // Barrier_With_Completion();
    Check_Block_By_Barrier();
};
