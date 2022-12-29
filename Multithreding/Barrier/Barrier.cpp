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

#include "Barrier.h"

namespace Barrier
{
    void Test() {
        const auto workers = { "anil", "busara", "carl" };

        auto on_completion = []() noexcept {
            // locking not needed here
            static auto phase = "---> Done!! Cleaning up...\n";
            std::cout << phase;
            phase = "... done\n";
        };
        std::barrier sync_point(workers.size(), on_completion);

        auto work = [&](std::string name) {
            std::string product = "  " + name + " worked\n";
            std::cout << product;  // ok, op<< call is atomic
            sync_point.arrive_and_wait();

            product = "  " + name + " cleaned\n";
            std::cout << product;
            sync_point.arrive_and_wait();
        };

        std::cout << "Starting...\n";


        std::vector<std::thread> threads;
        for (auto const& worker : workers)
            threads.emplace_back(work, worker);
        for (auto& thread : threads)
            thread.join();
    }
};


void Barrier::TEST_ALL() {
    Test();
};
