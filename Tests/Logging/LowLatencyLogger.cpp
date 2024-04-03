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


void LowLatencyLogger::TestAll()
{

    /*
    std::atomic<size_t> pos {1};

    size_t val = pos.fetch_add(1, std::memory_order_relaxed);
    std::cout << val << std::endl;

    val = pos.fetch_add(1, std::memory_order_relaxed);
    std::cout << val << std::endl;
    */

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