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
#include <ctime>
#include <chrono>
#include <thread>

#include <list>
#include <deque>
#include <vector>

namespace RateLimiterEx
{
    struct Limiter
    {
        using time_type   = uint64_t;
        using num_type    = uint32_t;
        using Collection  = std::deque<time_type>;

        num_type  maxRequests { 0 };
        time_type nsTimeout { 0 };
        Collection calls;

        Limiter(num_type reqMax, time_type timeNanos):
            maxRequests { reqMax }, nsTimeout { timeNanos } {
        }

        [[nodiscard]]
        bool check(/*time_type now*/)
        {
            const time_type now = std::chrono::system_clock::now().time_since_epoch().count();
            auto iter = calls.begin();
            for (; iter != calls.end() && (now - *iter) > nsTimeout; ++iter) { /** **/ }
            calls.erase(calls.begin(), iter);

            if (calls.size() == maxRequests)
                return false;

            calls.push_back(now);
            return true;
        }
    };
}

namespace RateLimiterEx::Tests
{
    void runTest(uint32_t maxNumOfCalls,
                 uint64_t interval,
                 uint32_t testRps,
                 uint32_t testReqCount = 10'000)
    {
        Limiter limiter(maxNumOfCalls, interval);
        uint32_t okCalls { 0 };
        const int64_t start = std::chrono::system_clock::now().time_since_epoch().count();
        for (uint32_t timeout = 1'000'000'000 / testRps, i = 0; i < testReqCount; ++i)
        {
            okCalls += static_cast<uint32_t>(limiter.check());
            std::this_thread::sleep_for(std::chrono::nanoseconds (timeout));
        }
        const int64_t end = std::chrono::system_clock::now().time_since_epoch().count();

        std::cout << "OK calls : " << okCalls << " from " << testReqCount << " total number of calls\n";
        std::cout << "RPS Limit: " << static_cast<double>(okCalls)  /
                ((static_cast<double>(end) - static_cast<double>(start)) / static_cast<double>(1'000'000'000)) << std::endl;
    }

    void tests()
    {
        runTest(250,1'000'000'000,500, 10000);
    }
}

void RateLimiterEx::TestAll()
{
    /*
    const int64_t start = std::chrono::system_clock::now().time_since_epoch().count();
    std::cout << start << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds (999));

    const int64_t end = std::chrono::system_clock::now().time_since_epoch().count();
    std::cout << end - start << std::endl;
    */


    Tests::tests();
;}