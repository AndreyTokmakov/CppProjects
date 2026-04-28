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
        std::atomic<int> atomicVar { 10 };
        constexpr int expected = 20, newValue = 30;

        const std::jthread  producer([&] {
            std::this_thread::sleep_for(std::chrono::seconds (2U));
            atomicVar.store(expected, std::memory_order::relaxed);
            std::cout << "atomicVar: 10 ==> 20\n";
        });

        int expectedTmp { expected };
        bool result { false };
        while (!result)
        {
            const int oldValueForDebug = atomicVar.load(std::memory_order::relaxed);
            expectedTmp = expected;

            result = atomicVar.compare_exchange_weak(expectedTmp, newValue);

            if (!result) {
                std::cout << "Result: False | expected: " << expected << " -> " << expectedTmp
                         << " | newValue: " << newValue << " | atomicVar = " << atomicVar << std::endl;
            }
            else {
                std::cout << "Result: True  | expected: " << expected << " == " << expectedTmp
                         << " | newValue: " << newValue << " | atomicVar : " << oldValueForDebug << " => " << atomicVar << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500U));
        }
    }

    void CompareExchangeStrong()
    {
        std::atomic<int> atomicVar { 10 };
        constexpr int expected = 20, newValue = 30;

        std::jthread producer([&] {
            std::this_thread::sleep_for(std::chrono::seconds (2u));
            atomicVar.store(expected, std::memory_order::relaxed);
        });

        int expectedTmp { expected };
        bool result { false };
        while (true)
        {
            const int oldValueForDebug = atomicVar.load(std::memory_order::relaxed);
            expectedTmp = expected;

            result = atomicVar.compare_exchange_strong(expectedTmp, newValue);

            if (!result) {
                std::cout << "Result: False | expected: " << expected << " -> " << expectedTmp
                          << " | newValue: " << newValue << " | atomicVar = " << atomicVar << std::endl;
            }
            else {
                std::cout << "Result: True  | expected: " << expected << " == " << expectedTmp
                          << " | newValue: " << newValue << " | atomicVar : " << oldValueForDebug << " => " << atomicVar << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500u));
        }
    }
}


void Atomic_CompareExchange::TestAll()
{
    CompareExchangeWeak();
    // CompareExchangeStrong();
}