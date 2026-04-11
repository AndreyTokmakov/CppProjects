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
#include <mutex>
#include <ctime>

namespace RateLimiter
{
    struct RateLimiter
    {
        constexpr static size_t refillInterval { 1UL *  1'000'000'000 };

        uint64_t currentTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();

        uint32_t tokensMax {0};
        std::jthread clockThread {};

        struct Client
        {
            uint64_t frameStart {0};
            uint64_t tokens {0};
            std::mutex mtx;
        };

        Client client;

        explicit RateLimiter(uint32_t maxTokens):
                tokensMax { maxTokens },
                clockThread { std::jthread(&RateLimiter::clock, this) }
        {
            client.frameStart = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            client.tokens = tokensMax;
        }

        [[noreturn]]
        void clock()
        {
            while (true)
            {
                currentTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                std::this_thread::sleep_for(std::chrono::milliseconds (1));
            }
        }

        bool processRequest()
        {
            if (currentTime - client.frameStart >= refillInterval)
            {
                const uint64_t delta = currentTime - client.frameStart - refillInterval;
                client.frameStart = currentTime + delta;
                client.tokens = tokensMax;
            }

            if (client.tokens > 0) {
                --client.tokens;
                return true;
            }

            return false;
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
    constexpr int rpsTarget = 100;
    RateLimiter limiter(rpsTarget);

    size_t requestSend = 0;
    size_t requestDeclined = 0;
    constexpr size_t requestToSend = 200'000;


    const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    for (size_t n = 0; n < requestToSend; ++n)
    {
        std::this_thread::sleep_for(std::chrono::microseconds (100));

        if (limiter.processRequest()) {
            // std::cout << "Request allowed\n";
            ++requestSend;
        }
        else {
            // std::cout << "Request declined\n";
            ++requestDeclined;
        }

        // if (0 == n % 100)
        //     std::cout << n << std::endl;
    }

    const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
    const double seconds = time_span.count();

    std::cout << "seconds: " << seconds << std::endl;
    std::cout << "requestSend: " << requestSend << ", requestDeclined = " << requestDeclined << std::endl;

    std::cout << "Result RPS: " << static_cast<double>(requestSend) / seconds << std::endl;

    std::cout << "Total (rps): " << static_cast<double>(requestToSend) / seconds << ".  "
              << "Passed: " << (static_cast<double>(requestSend) / static_cast<double>(requestToSend)) * 100 << ".  "
              << "Dropped: " << (static_cast<double>(requestDeclined) / static_cast<double>(requestToSend)) * 100 << "%\n";
}