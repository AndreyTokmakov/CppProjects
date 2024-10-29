/**============================================================================
Name        : Atomic_CompareExchange.cpp
Created on  : 28.10.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Atomic_CompareExchange.cpp
============================================================================**/

#include "Atomic_CompareExchange.h"

#include <iostream>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <string_view>
#include <chrono>
#include <future>
#include <cassert>
#include <format>
#include <syncstream>


namespace Atomic_CompareExchange
{
    void CompareExchangeWeak()
    {
        std::atomic<int> atomicVar { 35 };
        int expected = 30;
        constexpr int newValue = 40;

        auto exchange = [&]
        {
            std::cout << "Before: "
                      << "value = " << atomicVar.load(std::memory_order::relaxed)
                      << ", expected = " << expected << ", newValue = " << newValue << std::endl;

            auto exchanged = atomicVar.compare_exchange_weak(expected, newValue);

            std::cout << "After : "
                      << "value = " << atomicVar.load(std::memory_order::relaxed)
                      << ", expected = " << expected << ", newValue = " << newValue
                      << " | Exchange result --> " << std::boolalpha << exchanged
                      << std::endl;
        };

        exchange();
        expected = atomicVar.load(std::memory_order::relaxed);
        exchange();
    }

    void CompareExchangeStrong()
    {
        std::atomic<int> atomicVar { 35 };
        int expected = 30;
        constexpr int newValue = 40;

        auto exchange = [&]
        {
            std::cout << "Before: "
                      << "value = " << atomicVar.load(std::memory_order::relaxed)
                      << ", expected = " << expected << ", newValue = " << newValue << std::endl;

            auto exchanged = atomicVar.compare_exchange_strong(expected, newValue);

            std::cout << "After : "
                      << "value = " << atomicVar.load(std::memory_order::relaxed)
                      << ", expected = " << expected << ", newValue = " << newValue
                      << " | Exchange result --> " << std::boolalpha << exchanged
                      << std::endl;
        };

        exchange();
        expected = atomicVar.load(std::memory_order::relaxed);
        exchange();
    }
}


void Atomic_CompareExchange::TestAll()
{
    // CompareExchangeWeak();
    CompareExchangeStrong();
}