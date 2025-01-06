/**============================================================================
Name        : ConditionVariable_vs_Atomic.cpp
Created on  : 06.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

#include "ConditionVariable_vs_Atomic.h"
#include "../Utilities/Utilities.h"

namespace ConditionVariable_vs_Atomic::PingPong
{
    constexpr int32_t ITER_COUNT = 1'000'000;

    struct CVTester
    {
        std::condition_variable cv;
        std::mutex mtx;

        bool sharedState { false };
        const int32_t testIterCount { 0 };

        int32_t countProduced { 0 };
        int32_t countConsumed { 0 };

        explicit CVTester(const int32_t count = ITER_COUNT): testIterCount { count } {
        }

        void produce() noexcept
        {
            while (testIterCount > countProduced)
            {
                {
                    std::unique_lock<std::mutex> lock { mtx };
                    cv.wait(lock, [this](){ return false == sharedState; });
                    sharedState = true;
                }
                ++countProduced;
                cv.notify_one();
            }
        }

        void consume() noexcept
        {
            while (testIterCount > countConsumed)
            {
                {
                    std::unique_lock lock { mtx };
                    cv.wait(lock, [this](){ return true == sharedState; });
                    sharedState = false;
                }
                ++countConsumed;
                cv.notify_one();
            }
        }

        void runSimulation() noexcept
        {
            Utilities::ScopedTimer timer {"ConditionVariable "};
            std::jthread producer { &CVTester::produce, this };
            std::jthread consumer { &CVTester::consume, this };
        }
    };

    struct AtomicTester
    {
        std::atomic_flag flag;

        bool sharedState { true };
        const int32_t testIterCount { 0 };

        int32_t countProduced { 0 };
        int32_t countConsumed { 0 };

        std::memory_order memoryOrder { std::memory_order_relaxed };

        explicit AtomicTester(const int32_t count = ITER_COUNT): testIterCount { count } {
        }

        void produce() noexcept
        {
            while (testIterCount > countProduced)
            {
                flag.wait(false, memoryOrder);
                flag.clear(memoryOrder);
                ++countProduced;
                flag.notify_one();
            }
        }

        void consume() noexcept
        {
            while (testIterCount > countConsumed)
            {
                flag.wait(true, memoryOrder);
                flag.test_and_set(memoryOrder);
                ++countConsumed;
                flag.notify_one();
            }
        }

        void runSimulation() noexcept
        {
            Utilities::ScopedTimer timer {"AtomicVariableWait"};
            std::jthread producer { &AtomicTester::produce, this };
            std::jthread consumer { &AtomicTester::consume, this };
        }
    };


    void benchmark()
    {
        {
            CVTester tester;
            tester.runSimulation();
        }
        {
            AtomicTester tester;
            tester.runSimulation();
        }
    }
}

void ConditionVariable_vs_Atomic::TestAll()
{
    PingPong::benchmark();
}
