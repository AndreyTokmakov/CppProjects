/**============================================================================
Name        : Experiments.cpp
Created on  : 03.06.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Experiments.cpp
============================================================================**/

#include <iostream>
#include <vector>
#include <thread>
#include <barrier>
#include <syncstream>

#include "Experiments.h"
#include "ThreadUtilities.hpp"

namespace Experiments
{
    void RaceCondition()
    {
        int32_t counter = 0;
        int32_t threadsCount = 8;
        std::barrier barrier(threadsCount);

        std::vector<std::jthread> workers;
        for (int n = 0; n < threadsCount; ++n) {
            workers.emplace_back([&] {
                utilities::threading::setThreadCore(n);
                barrier.arrive_and_wait();
                for (int i = 0; i < 10'000; ++i)
                    counter++;
                //std::osyncstream { std::cout } << Utilities::getCpu() << std::endl;
            });
        }

        workers.clear();
        std::osyncstream { std::cout } << counter << std::endl;
    }


    void CreateThread_LambdaWithState()
    {
        auto func = [counter = 0] mutable -> void {
            ++counter;
            std::osyncstream { std::cout } << counter << std::endl;
        };

        func();

        std::jthread t1(func);
        std::jthread t2(func);
    }

    struct Worker
    {
        std::atomic<int32_t> counter { 0 };

        void doSomething() {
            const int32_t prev = counter.fetch_add(1, std::memory_order::relaxed);
            std::osyncstream { std::cout } << (1 + prev) << std::endl;
        }
    };

    void CreateThread_ClassMethod_Callback()
    {
        Worker worker;

        std::jthread t1(&Worker::doSomething, &worker);
        std::jthread t2(&Worker::doSomething, &worker);
    }
}

void Experiments::TestAll()
{
    // RaceCondition();
    // CreateThread_LambdaWithState();
    CreateThread_ClassMethod_Callback();
}