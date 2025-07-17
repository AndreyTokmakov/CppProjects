/**============================================================================
Name        : RateLimiterEx.cpp
Created on  : 17.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RateLimiterEx.cpp
============================================================================**/

#include "RateLimiterEx.h"
#include <iostream>
#include <cstdint>

namespace RateLimiterEx
{
    struct Limiter
    {
        uint32_t maxRequests { 0 };
        uint64_t timeout { 0 };

        Limiter(uint32_t reqMax, uint64_t tm):
            maxRequests { reqMax }, timeout { tm } {
        }

        [[nodiscard]]
        bool check(uint64_t now)
        {
            return true;
        }
    };
}

void RateLimiterEx::TestAll()
{

    std::cout << "RateLimiterEx\n";
}