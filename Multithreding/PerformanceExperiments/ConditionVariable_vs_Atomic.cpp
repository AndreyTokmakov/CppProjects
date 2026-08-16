/**============================================================================
Name        : ConditionVariable_vs_Atomic.cpp
Created on  : 06.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

#include "ConditionVariable_vs_Atomic.h"
#include "DateTimeUtilities.hpp"
#include "PerfUtilities.hpp"


#define LOG  std::osyncstream { std::cout } << utilities::datetime::getCurrentTime() << " "


namespace ConditionVariable_vs_Atomic::TriggerLatency
{
    constexpr int32_t ITER_COUNT = 1'000'000;

    struct CVTester
    {
        std::condition_variable cv;
        std::mutex mtx;
        bool sharedState { false };

        alignas(std::hardware_destructive_interference_size)
        const int32_t testIterCount { 0 };

        alignas(std::hardware_destructive_interference_size)
        int32_t countProduced { 0 };

        alignas(std::hardware_destructive_interference_size)
        int32_t countConsumed { 0 };

        explicit CVTester(const int32_t count = ITER_COUNT): testIterCount { count } {
        }

        void produce() noexcept
        {
            while (testIterCount > countProduced)
            {
                {
                    std::unique_lock<std::mutex> lock { mtx };
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
            const utilities::perf::ScopedTimer timer {"ConditionVariable "};
            std::jthread producer { &CVTester::produce, this };
            std::jthread consumer { &CVTester::consume, this };
            producer.join();
            std::cout << countProduced << " " << countConsumed << std::endl;
        }
    };

    struct AtomicTester
    {
        std::atomic_flag flag;
        bool sharedState { true };

        alignas(std::hardware_destructive_interference_size)
        const int32_t testIterCount { 0 };

        alignas(std::hardware_destructive_interference_size)
        int32_t countProduced { 0 };

        alignas(std::hardware_destructive_interference_size)
        int32_t countConsumed { 0 };

        std::memory_order memoryOrder { std::memory_order_relaxed };

        explicit AtomicTester(const int32_t count = ITER_COUNT): testIterCount { count } {
        }

        void produce() noexcept
        {
            while (testIterCount > countProduced)
            {
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
            const utilities::perf::ScopedTimer timer {"AtomicVariableWait"};
            std::jthread producer { &AtomicTester::produce, this };
            std::jthread consumer { &AtomicTester::consume, this };
            producer.join();
            std::cout << countProduced << " " << countConsumed << std::endl;
        }
    };

    struct AtomicTesterBool
    {
        std::atomic_bool flag { false };
        bool sharedState { true };

        alignas(std::hardware_destructive_interference_size)
        const int32_t testIterCount { 0 };

        alignas(std::hardware_destructive_interference_size)
        int32_t countProduced { 0 };

        alignas(std::hardware_destructive_interference_size)
        int32_t countConsumed { 0 };

        std::memory_order memoryOrder { std::memory_order_relaxed };

        explicit AtomicTesterBool(const int32_t count = ITER_COUNT): testIterCount { count } {
        }

        void produce() noexcept
        {
            while (testIterCount > countProduced)
            {
                flag.store(true, memoryOrder);
                ++countProduced;
                flag.notify_one();
            }
        }

        void consume() noexcept
        {
            while (testIterCount > countConsumed)
            {
                flag.wait(false, memoryOrder);
                flag.store(false, memoryOrder);
                ++countConsumed;
                flag.notify_one();
            }
        }

        void runSimulation() noexcept
        {
            const utilities::perf::ScopedTimer timer {"AtomicVariableWait"};
            std::jthread producer { &AtomicTesterBool::produce, this };
            std::jthread consumer { &AtomicTesterBool::consume, this };
            producer.join();
            std::cout << countProduced << " " << countConsumed << std::endl;
        }
    };


    void benchmark()
    {
        if (constexpr int type = 2; 0 == type)
        {
            CVTester tester;
            tester.runSimulation(); // 1'000'000 101'502
        }
        else if (1 == type)
        {
            AtomicTester tester;
            tester.runSimulation(); // 1'000'000 979'731
        }
        else if (2 == type)
        {
            AtomicTesterBool tester;
            tester.runSimulation(); // 1'000'000 995'054
        }
    }
}


namespace ConditionVariable_vs_Atomic::PingPong
{
    constexpr int32_t ITER_COUNT = 1'000'000;

    struct CVTester
    {
        std::condition_variable cv;
        std::mutex mtx;

        bool sharedState { false };
        const int32_t testIterCount { 0 };

        alignas(std::hardware_destructive_interference_size) int32_t countProduced { 0 };
        alignas(std::hardware_destructive_interference_size) int32_t countConsumed { 0 };

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
            const utilities::perf::ScopedTimer timer {"ConditionVariable "};
            std::jthread producer { &CVTester::produce, this };
            std::jthread consumer { &CVTester::consume, this };
        }
    };

    struct AtomicTester
    {
        std::atomic_flag flag;

        bool sharedState { true };
        const int32_t testIterCount { 0 };

        alignas(std::hardware_destructive_interference_size) int32_t countProduced { 0 };
        alignas(std::hardware_destructive_interference_size) int32_t countConsumed { 0 };

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
            const utilities::perf::ScopedTimer timer {"AtomicVariableWait"};
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


namespace ConditionVariable_vs_Atomic::PingPong2
{
    struct RunnerBase
    {
        static constexpr int MaxCountTimes { 1'000'000 };

        virtual void ping() noexcept = 0;
        virtual void pong() noexcept = 0;
        virtual bool start() noexcept = 0;

        virtual ~RunnerBase() = default;
    };

    struct CVEngine final : RunnerBase
    {
        std::condition_variable cv;
        std::mutex mtx;
        bool pingDone { false };
        bool pongDone { false };

        void ping() noexcept override
        {
            int counter = 0;
            while (counter <= MaxCountTimes)
            {
                {
                    std::unique_lock lck { mtx };
                    cv.wait(lck, [this]() {
                        return pingDone;
                    });
                    pingDone = false;
                    pongDone = true;
                    ++counter;
                }
                cv.notify_one();
            }
        }

        void pong() noexcept override
        {
            int counter = 0;
            while (counter<MaxCountTimes)
            {
                {
                    std::unique_lock lck { mtx };
                    cv.wait(lck, [this](){
                        return pongDone;
                    });
                    pingDone = true;
                    pongDone = false;
                    ++counter;
                }
                cv.notify_one();
            }
        }

        bool start() noexcept override
        {
            {
                std::unique_lock lck { mtx };
                pingDone = true;
            }
            cv.notify_one();
            return true;
        }
    };

    struct AtomicEngine final : RunnerBase
    {
        std::atomic_flag pass;

        void ping() noexcept override
        {
            int counterLocal = 0;
            while (counterLocal <= MaxCountTimes)
            {
                pass.wait(false);
                pass.clear();
                ++counterLocal;
                pass.notify_one();
            }
        }

        void pong() noexcept override
        {
            int counterLocal = 0;
            while (counterLocal < MaxCountTimes)
            {
                pass.wait(true);
                pass.test_and_set();
                ++counterLocal;
                pass.notify_one();
            }
        }

        bool start() noexcept override
        {
            pass.test_and_set();
            pass.notify_one();
            return true;
        }
    };

    void benchmark()
    {
        {
            const utilities::perf::ScopedTimer timer {"Atomic variable     "};
            if (auto engine = new AtomicEngine(); engine->start()) {
                std::jthread ping = std::jthread(&AtomicEngine::ping, engine);
                std::jthread pong = std::jthread(&AtomicEngine::pong, engine);
            }
        }

        {
            const utilities::perf::ScopedTimer timer {"Conditional variable"};
            if (auto engine = new CVEngine(); engine->start()) {
                std::jthread ping = std::jthread(&CVEngine::ping, engine);
                std::jthread pong = std::jthread(&CVEngine::pong, engine);
            }
        }
        // Atomic variable     :  0.250215 seconds.
        // Conditional variable:  3.7974 seconds.
    }
};



void ConditionVariable_vs_Atomic::benchmarks()
{
    // PingPong::benchmark();
    PingPong2::benchmark();

    // TriggerLatency::benchmark();
}
