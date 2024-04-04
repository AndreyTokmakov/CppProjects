/**============================================================================
Name        : LowLatencyLogger.cpp
Created on  : 03.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : LowLatencyLogger
============================================================================**/

#include "LowLatencyLogger.h"

#include <iostream>
#include <string_view>
#include <chrono>
#include <atomic>
#include <thread>
#include <syncstream>

namespace LowLatencyLogger
{
    struct LongEntry
    {
        std::time_t timestamp { std::time(nullptr) };
        std::string text ;

        explicit LongEntry(std::string txt): text { std::move(txt)} {
        }
    };

    struct LogHandler
    {
        virtual void handleEntry(const LogHandler&) const noexcept = 0;
        virtual ~LogHandler() = default;
    };

    struct Logger
    {
        std::vector<LongEntry> logs;
        std::vector<LongEntry> logsToWrite;

        // TODO: Check it required
        void log(std::string message)
        {
            logs.emplace_back(std::move(message));
        }

        // TODO: Check it required
        void log(std::string&& message)
        {
            logs.emplace_back(std::move(message));
        }
    };
}


namespace MultithreadingExperiments
{
    void fetchAndAdd()
    {
        std::atomic<size_t> pos {1};

        size_t val = pos.fetch_add(1, std::memory_order_relaxed);
        std::cout << val << std::endl;

        val = pos.fetch_add(1, std::memory_order_relaxed);
        std::cout << val << std::endl;
    }

    void wait()
    {
        std::atomic<int> counter {1};

        auto update = [&]() {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::osyncstream {std::cout} << "update" << std::endl;
            counter = 1;
            counter.notify_one();
        };

        auto waiter = [&] {
            std::osyncstream {std::cout} << "Waiter: started." << std::endl;
            counter.wait(100);
            std::osyncstream {std::cout} <<  "Waiter: done" << std::endl;
            std::cout << counter << std::endl;
        };

        std::jthread t2(waiter);
        std::jthread t1(update);
    }

    void multipleWriters()
    {
        constexpr uint32_t maxCapacity {100};
        std::atomic<uint32_t> idx {0};

        // auto getNextIndex = [](std::atomic<uint32_t>& idx){};

        auto task = [&]() {
            for (int i = 0; i < 40; ++i)
            {
                std::osyncstream {std::cout} << std::this_thread::get_id() << " before wait()" << std::endl;
                idx.wait(maxCapacity /**, std::memory_order_relaxed **/);

                const uint32_t index = idx.fetch_add(1, std::memory_order_relaxed);
                std::osyncstream {std::cout} << std::this_thread::get_id() << " index = " << index << std::endl;

                if (index == maxCapacity - 1)
                {
                    std::osyncstream {std::cout} << std::this_thread::get_id() << "**** RESET ****\n";
                    idx.store(0, std::memory_order_relaxed);
                }
            }
        };

        std::vector<std::jthread> workers;
        for (int i = 0; i < 5; ++i) {
            workers.emplace_back(task);
        }

        for (auto& job: workers)
            job.join();

        std::cout << idx.load(std::memory_order_relaxed) << std::endl;
    }

    void atomicPerformanceTest()
    {
        constexpr uint32_t maxCapacity {1000}, threadsCount {12};
        constexpr size_t iterCount { 1'000'000 };
        std::atomic<uint64_t> idx {0};
        //std::atomic<uint64_t> dumpsCount {0};

        auto task = [&]() {
            for (size_t i = 0; i < iterCount; ++i)
            {
                idx.wait(maxCapacity , std::memory_order_relaxed);
                const uint64_t index = idx.fetch_add(1, std::memory_order_relaxed);
                if (index == maxCapacity - 1) {
                    idx.store(0, std::memory_order_relaxed);
                    //dumpsCount.fetch_add(1,std::memory_order_relaxed);
                }
            }
        };

        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::jthread> workers;
        for (uint32_t i = 0; i < threadsCount; ++i) {
            workers.emplace_back(task);
        }

        for (auto& job: workers)
            job.join();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "Result: " << duration << " microseconds" << std::endl;
    }

    void mutexPerformanceTest()
    {
        constexpr uint32_t maxCapacity {1000}, threadsCount {12};
        constexpr size_t iterCount { 1'000'000 };

        std::mutex mtx {};
        uint64_t idx {0};

        auto task = [&]() {
            for (size_t i = 0; i < iterCount; ++i)
            {
                std::lock_guard<std::mutex> lock {mtx};
                const uint64_t index = idx++;
                if (index == maxCapacity - 1) {
                    idx = 0;
                }
            }
        };

        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::jthread> workers;
        for (uint32_t i = 0; i < threadsCount; ++i) {
            workers.emplace_back(task);
        }

        for (auto& job: workers)
            job.join();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "Result: " << duration << " microseconds" << std::endl;
    }
}

void LowLatencyLogger::TestAll()
{
    // MultithreadingExperiments::wait();
    // MultithreadingExperiments::fetchAndAdd();
    // MultithreadingExperiments::multipleWriters();

    MultithreadingExperiments::atomicPerformanceTest();
    MultithreadingExperiments::mutexPerformanceTest();
}