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
#include <vector>
#include <list>

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

    void compareExchangeWeak()
    {
        std::atomic<uint32_t> idx {100};
        uint32_t expected = 100;

        std:: cout << std::boolalpha << idx.compare_exchange_weak(expected, 2, std::memory_order_relaxed) << std::endl;
        std:: cout << expected << std::endl;
        std:: cout << idx.load(std::memory_order_relaxed) << std::endl;

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
        constexpr uint32_t maxCapacity {1000};
        constexpr size_t iterCount { 10'000 };
        std::atomic<uint32_t> idx {0};

        auto task = [&]()
        {
            for (size_t i = 0; i < iterCount; ++i)
            {
                uint32_t index = idx.fetch_add(1, std::memory_order_relaxed);
                if (maxCapacity - 1 > index)
                {
                    //index = idx.fetch_add(1, std::memory_order_relaxed);
                    //std::osyncstream {std::cout} << std::this_thread::get_id() << " index = " << index << std::endl;
                }
                else if (index == maxCapacity - 1)
                {
                    //std::osyncstream {std::cout} << std::this_thread::get_id() << "**** SWAP **** index = " << index << "\n";
                    //std::this_thread::sleep_for(std::chrono::seconds(1));
                    idx.store(0, std::memory_order_relaxed);
                }
                else {
                    //std::osyncstream{std::cout} << std::this_thread::get_id() << " WAIT Start. index = " << index << "\n";

                    // idx.wait(index, std::memory_order_relaxed);
                    while (idx.load(std::memory_order_relaxed) >= maxCapacity) {
                        std::this_thread::sleep_for(std::chrono::microseconds (1));
                    }

                    index = idx.fetch_add(1, std::memory_order_relaxed);
                    //std::osyncstream{std::cout} << std::this_thread::get_id() << " WAIT End. index = " << index << "\n";
                }

                //idx.compare_exchange_weak(index, 0, )

                /*
                std::osyncstream {std::cout} << std::this_thread::get_id() << " index = " << index << std::endl;

                if (index == maxCapacity - 1) {
                    idx.fetch_add(1, std::memory_order_relaxed);

                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    std::osyncstream {std::cout} << std::this_thread::get_id() << "**** SWAP ****\n";

                    idx.store(0, std::memory_order_relaxed);
                    std::osyncstream {std::cout} << std::this_thread::get_id() << "**** RESET ****\n";
                }

                if (index >= maxCapacity) {
                    std::osyncstream {std::cout} << std::this_thread::get_id() << " HAS to wait" << std::endl;
                    idx.wait(maxCapacity, std::memory_order_relaxed);
                    index = idx.fetch_add(1, std::memory_order_relaxed);
                }
                */

            }
        };

        {
            std::vector<std::jthread> workers;
            for (int i = 0; i < 2; ++i)
                workers.emplace_back(task);
        }

        std::cout << idx.load(std::memory_order_relaxed) << std::endl;
    }

    void multipleWriters_MTX()
    {
        constexpr uint32_t maxCapacity {1000};
        constexpr size_t iterCount { 1'000 };

        std::mutex mtx;
        std::atomic<uint32_t> idx {0};
        std::atomic<uint32_t> total1 {0};
        std::atomic<uint32_t> total2 {0};

        auto task = [&]()
        {
            for (size_t i = 0; i < iterCount; ++i)
            {
                uint32_t index = idx.fetch_add(1, std::memory_order_relaxed);
                if (maxCapacity - 1 > index)
                {
                    // Put logs
                    // std::osyncstream {std::cout} << std::this_thread::get_id() << " index = " << index << std::endl;
                    total1.fetch_add(1, std::memory_order_relaxed);
                    continue;
                }

                std::lock_guard<std::mutex> lock {mtx};

                if (index == maxCapacity - 1)
                {
                    //std::osyncstream {std::cout} << std::this_thread::get_id() << "**** SWAP ****\n";
                    idx.store(0, std::memory_order_relaxed);
                    total2.fetch_add(1, std::memory_order_relaxed);
                }
                else
                {
                    index = idx.fetch_add(1, std::memory_order_relaxed);
                    //std::osyncstream{std::cout} << std::this_thread::get_id() << "Synchronized. index = " << index << "\n";
                    total2.fetch_add(1, std::memory_order_relaxed);
                }
            }
        };

        std::vector<std::jthread> workers;
        for (int i = 0; i < 32; ++i) {
            workers.emplace_back(task);
        }

        for (auto& job: workers)
            job.join();

        //std::cout << idx.load(std::memory_order_relaxed) << std::endl;
        std::cout << total1.load(std::memory_order_relaxed) << std::endl;
        std::cout << total2.load(std::memory_order_relaxed) << std::endl;
    }

    void atomicPerformanceTest()
    {
        constexpr uint32_t maxCapacity {1'000}, threadsCount {32};
        constexpr size_t iterCount { 1'000'000 };
        std::mutex mtx;
        std::atomic<uint32_t> idx {0};

        auto task = [&]()
        {
            for (size_t i = 0; i < iterCount; ++i)
            {
                uint32_t index = idx.fetch_add(1, std::memory_order_relaxed);
                if (maxCapacity - 1 > index)
                {
                    // Put logs
                    continue;
                }

                std::lock_guard<std::mutex> lock {mtx};

                if (index == maxCapacity - 1)
                {
                    // SWAP
                    idx.store(0, std::memory_order_relaxed);
                    // Put logs
                }
                else
                {
                    index = idx.fetch_add(1, std::memory_order_relaxed);
                    // Put logs
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
        constexpr uint32_t maxCapacity {1000}, threadsCount {32};
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


namespace LowLatencyLogger_AtomicDemo
{
    struct Logger
    {
        using Block = std::vector<std::string>;
        using BlockList = std::list<Block>;
        using BlockIter = BlockList::iterator;

        // constexpr static inline uint32_t blockSize { 1'000 };
        constexpr static inline uint32_t blockSize { 10 };

        BlockList available;
        BlockList toBeHandled;
        BlockIter logsBlock { available.end() };

        // FIXME: False sharing ?
        std::atomic<uint32_t> idx {0};

        Logger()
        {   /*
            available.emplace_back(Block{"11", "22", "33"});
            available.emplace_back(Block{"44", "55", "66"});
            available.emplace_back(Block{"77", "88", "99"});

            logsBlock = available.begin();
            */

            /*
            available.emplace_back(Block{"11", "22"});
            available.emplace_back(Block{"33", "44"});
            available.emplace_back(Block{"55", "66"});
            available.emplace_back(Block{"77", "88"});

            logsBlock = available.begin();
            */

            addEmptyBlock();
        }

        void addEmptyBlock()
        {
            available.emplace_back(blockSize, "--");
        }
    };

    void printBlock(const Logger::Block& block)
    {
        std::cout << " [ ";
        for (const auto& entry: block)
            std::cout << "'" << entry << "' ";
        std::cout << "] ";
    }

    void printBlockList(const Logger::BlockList& blockList, std::string&& name)
    {
        std::cout << name << ". size: " << blockList.size() << std::endl;
        for (const auto& block: blockList)
            printBlock(block);
        std::cout << std::endl;
    }

    void Test()
    {
        Logger logger;

        /*
        printBlockList(logger.available, "available");
        printBlockList(logger.toBeHandled, "toBeHandled");

        logger.toBeHandled.splice(logger.toBeHandled.begin(), logger.available, logger.available.begin());

        printBlockList(logger.available, "available");
        printBlockList(logger.toBeHandled, "toBeHandled");
        */


        /*
        logger.available.emplace_back(Logger::Block{"99", "1010"});


        printBlockList(logger.available, "available");
        printBlockList(logger.toBeHandled, "toBeHandled");
        // printBlockList(Logger::BlockList{*logger.logsBlock}, "logsBlock");


        logger.toBeHandled.splice(logger.toBeHandled.begin(), logger.available, logger.logsBlock);

        printBlockList(logger.available, "available");
        printBlockList(logger.toBeHandled, "toBeHandled");
         */


        printBlockList(logger.available, "available");
    }
}

void LowLatencyLogger::TestAll()
{
    // MultithreadingExperiments::wait();
    // MultithreadingExperiments::compareExchangeWeak();
    // MultithreadingExperiments::fetchAndAdd();

    // MultithreadingExperiments::multipleWriters();
    // MultithreadingExperiments::multipleWriters_MTX();

    // MultithreadingExperiments::atomicPerformanceTest();
    // MultithreadingExperiments::mutexPerformanceTest();


    LowLatencyLogger_AtomicDemo::Test();
}