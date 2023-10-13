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
        uint64_t currentTime {0};

        uint32_t tokensMax {0};
        uint32_t refillInterval {0};

        uint64_t frameStart {0};
        uint64_t tokens {0};

        RateLimiter(uint32_t tokens, uint32_t refillIntervalSec):
            clockThread {std::jthread(&RateLimiter::clock, this)},
            tokensMax {tokens},
            refillInterval {refillIntervalSec * 1'000'000'000} {
        }

        void clock()
        {
            while (true)
            {
                currentTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                // std::cout << currentTime << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds (50));
            }
        }

        bool processRequest()
        {
            // TODO: remove
            if (0 == frameStart) {
                frameStart = currentTime;
                tokens = tokensMax - 1;
                return true;
            }

            if (currentTime - frameStart > refillInterval)
            {
                std::cout << "* * * * * REFILL * * * * * * \n";
                frameStart = currentTime;
                tokens = tokensMax;
                // TODO: FIX
            } else {
                --tokens;
            }


            std::cout << "currentTime = " << currentTime
                      << ", frameStart  = " << frameStart
                      << ", tokens = " << tokens
                      << std::endl;


            // std::cout << currentTime - frameStart << "\n\n";
            return true;
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
    RateLimiter limiter(100, 5);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds (100));
        limiter.processRequest();
    }
}