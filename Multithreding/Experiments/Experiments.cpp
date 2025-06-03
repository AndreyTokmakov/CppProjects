/**============================================================================
Name        : Experiments.cpp
Created on  : 03.06.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Experiments.cpp
============================================================================**/

#include "Experiments.h"
#include "../Utilities/Utilities.h"

#include <iostream>
#include <string_view>
#include <vector>

#include <thread>
#include <barrier>
#include <syncstream>

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
                Utilities::setThreadCore(n);
                barrier.arrive_and_wait();
                for (int i = 0; i < 10'000; ++i)
                    counter++;
                //std::osyncstream { std::cout } << Utilities::getCpu() << std::endl;
            });
        }

        workers.clear();
        std::osyncstream { std::cout } << counter << std::endl;
    }
}

void Experiments::TestAll()
{
    RaceCondition();
}