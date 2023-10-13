/**============================================================================
Name        : RateLimiter.cpp
Created on  : 13.10.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RateLimiter.cpp
============================================================================**/

#include "RateLimiter.h"



#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>

namespace RateLimiter
{
    struct RateLimiter
    {
        std::jthread clockThread {};

        RateLimiter()
        {
            clockThread = std::jthread(&RateLimiter::clock, this);
        }

        void clock()
        {
            for (int i = 0; i < 10; ++i)
            {
                std::cout << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    };
}

uint32_t getCount()
{
    // return std::chrono::system_clock::now().time_since_epoch().count();
    // return std::chrono::steady_clock::now().time_since_epoch().count();
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

void RateLimiter::TestAll()
{
    /*
    uint32_t initial = getCount();
    while (true)
    {
        uint32_t currentTime = getCount();
        std::cout << currentTime - initial << std::endl;

        initial = currentTime;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
     */


    RateLimiter limiter;
}