//============================================================================
// Name        : Barrier.h
// Created on  : 02.12.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Barrier src class
//============================================================================

#include <barrier>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <array>
#include <syncstream>
#include <random>
#include <future>
#include <iomanip>
#include <format>
#include <chrono>

#include "Barrier.h"

namespace
{
    int getRandomInteger(int from = 0, int to = 100)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distribution(from, to);
        return static_cast<int>(distribution(gen));
    }

    std::string getCurrentTime() noexcept {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        const auto nowMs = std::chrono::duration_cast<std::chrono::microseconds>(
                now.time_since_epoch()) % 1000000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%a %b %d %Y %T")
           << '.' << std::setfill('0') << std::setw(6) << nowMs.count();
        return ss.str();
    }

    struct SyncTimeStream
    {
        const std::chrono::time_point<std::chrono::high_resolution_clock> now { std::chrono::system_clock::now() };

        template<class T>
        std::osyncstream operator<<(T&& s)
        {
            const std::string time {std::format("{:%d-%m-%Y %H:%M:%OS} ", now)};
            std::osyncstream stream {std::cout} ;
            stream << time << std::forward<T>(s);
            return stream;
        }
    };

#define synch_cout SyncTimeStream {}
}


namespace Barrier
{
    void SimpleTest()
    {
        constexpr size_t threadsCount {3};
        std::barrier barrier(threadsCount);

        auto task = [&](std::string_view name, const uint32_t timeout) {
            synch_cout << name  <<  " started\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            synch_cout << name  <<  " completed. waiting for others\n";
            barrier.arrive_and_wait();
            synch_cout << name  <<  " done\n";
        };

        std::jthread t1 (task, "T1", getRandomInteger(0, 5));
        std::jthread t2 (task, "T2", getRandomInteger(0, 5));
        std::jthread t3 (task, "T3", getRandomInteger(0, 5));
    }

    void Test_WithCallback()
    {
        const auto workers = { "One", "Two", "Three" };

        auto completionCallback = []() noexcept {
            // locking not needed here
            static auto phase = "---> Done!! Cleaning up...\n";
            synch_cout << phase;
            phase = "... done\n";
        };

        std::barrier sync_point(std::ssize(workers), completionCallback);

        auto work = [&](const std::string& name) {
            synch_cout << name + "worked\n";
            sync_point.arrive_and_wait();
            synch_cout << name + "cleaned\n";;
            sync_point.arrive_and_wait();
        };

        synch_cout << "Starting...\n";
        for (std::vector<std::jthread> threads; auto const& worker : workers)
            threads.emplace_back(work, worker);
    }

    void Wait_To_All_Thread_Completed()
    {
        constexpr size_t threadsCount {5};
        std::barrier barrier(threadsCount);

        auto task = [&]() {
            const int secondsToSleep = getRandomInteger(1, 10);
            std::osyncstream {std::cout} << std::this_thread::get_id()<< " thread started. Sleep time = "
                    << secondsToSleep << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(secondsToSleep));


            [[maybe_unused]]
            const auto token = barrier.arrive();
        };

        std::cout << getCurrentTime() << " starting...\n";
        std::vector<std::jthread> threads;
        for (size_t idx = 0; idx < threadsCount; ++idx)
            threads.emplace_back(task);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::cout << getCurrentTime() << " waiting for all threads to finish...\n";
        barrier.arrive_and_wait();
        std::cout << getCurrentTime() << " done...\n";
    }


    void Run_CallBack_WhenAllDone()
    {

        auto on_completion = []() noexcept {
            std::osyncstream {std::cout} << " **** ALL DONE at " << getCurrentTime()  << " ***\n";
        };

        constexpr size_t threadsCount {5};
        std::barrier barrier(threadsCount, on_completion);

        auto task = [&]() {
            const int secondsToSleep = getRandomInteger(1, 4);
            std::osyncstream {std::cout} << std::this_thread::get_id()<< " thread started. Sleep time = "
                                         << secondsToSleep << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(secondsToSleep));
            barrier.arrive_and_wait();
        };

        std::cout << getCurrentTime() << " starting...\n";
        std::vector<std::jthread> threads;
        for (size_t idx = 0; idx < threadsCount; ++idx)
            threads.emplace_back(task);

        for (std::jthread& T: threads)
            T.join();
        std::cout << getCurrentTime() << " done\n";

    }


    void Barrier_With_Completion()
    {
        std::vector<std::string> names { "One", "Two", "Three" };
        std::barrier phase(std::ssize(names),[] {
            std::osyncstream(std::cout) << "Callback()\n";
        });

        auto work = [&](std::string&& name) {
            std::cout << name << " job started\n";
            phase.arrive_and_wait();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << name << " job completed\n";
            phase.arrive_and_wait();
        };

        std::cout << "Starting...\n";
        for (std::vector<std::jthread> jobs; std::string& name : names)
            jobs.emplace_back(work, name);

    }

    void Check_Block_By_Barrier()
    {
        std::array params { std::make_pair("One", 4), std::make_pair("Two", 2), std::make_pair("Three",3) };
        std::barrier phase(std::ssize(params),[] {
            std::osyncstream(std::cout) << "* * * * * Barrier limit reached!!!* * * * * \n";
        });

        auto work = [&](std::string&& name, uint32_t timeout) {
            std::osyncstream {std::cout} << name << " job started\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            phase.arrive_and_wait();
            std::osyncstream {std::cout} << name << " continuing\n";
        };

        std::cout << "Starting...\n";
        for (std::vector<std::jthread> jobs; auto && param : params)
            jobs.emplace_back(work, param.first, param.second);

    }
};


void Barrier::TEST_ALL()
{
    SimpleTest();
    // Test();

    // Wait_To_All_Thread_Completed();

    // Run_CallBack_WhenAllDone();

    // Barrier_With_Completion();
    // Check_Block_By_Barrier();
};
