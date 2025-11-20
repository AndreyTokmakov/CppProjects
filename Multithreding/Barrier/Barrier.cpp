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
#include <syncstream>

#include "Barrier.h"
#include "DateTimeUtilities.hpp"

namespace
{
    int getRandomInteger(int from = 0, int to = 100)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distribution(from, to);
        return static_cast<int>(distribution(gen));
    }

    using DateTimeUtilities::getCurrentTime;

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "

}


namespace Barrier
{
    void SimpleTest()
    {
        constexpr size_t threadsCount {3};
        std::barrier barrier(threadsCount);

        auto task = [&](std::string_view name, const uint32_t timeout) {
            LOG << name  <<  " started\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            LOG << name  <<  " completed in " << timeout << " seconds. Waiting for others\n";
            barrier.arrive_and_wait();
            LOG << name  <<  " done\n";
        };

        std::jthread t1 (task, "T1", getRandomInteger(0, 5));
        std::jthread t2 (task, "T2", getRandomInteger(0, 5));
        std::jthread t3 (task, "T3", getRandomInteger(0, 5));

        /**
        2025-11-20 18:33:39.095909 T3 started
        2025-11-20 18:33:39.095865 T1 started
        2025-11-20 18:33:39.095871 T2 started
        2025-11-20 18:33:40.096161 T3 completed in 1 seconds. Waiting for others
        2025-11-20 18:33:41.096249 T2 completed in 2 seconds. Waiting for others
        2025-11-20 18:33:42.096156 T1 completed in 3 seconds. Waiting for others
        2025-11-20 18:33:42.096222 T1 done
        2025-11-20 18:33:42.096247 T3 done
        2025-11-20 18:33:42.096275 T2 done
        **/
    }

    void Test_WithCallback()
    {
        const auto workers = { "One", "Two", "Three" };

        auto completionCallback = []() noexcept {
            // locking not needed here
            static auto phase = "---> Done!! Cleaning up...\n";
            LOG << phase;
            phase = "... done\n";
        };

        std::barrier sync_point(std::ssize(workers), completionCallback);

        auto work = [&](const std::string& name) {
            LOG << name + "worked\n";
            sync_point.arrive_and_wait();
            LOG << name + "cleaned\n";;
            sync_point.arrive_and_wait();
        };

        LOG << "Starting...\n";
        for (std::vector<std::jthread> threads; auto const& worker : workers)
            threads.emplace_back(work, worker);
    }

    void Wait_To_All_Thread_Completed()
    {
        constexpr size_t threadsCount {3};
        std::barrier barrier(threadsCount + 1); /**  +1 for the Main Thread **/

        auto task = [&]() {
            const uint32_t secondsToSleep = getRandomInteger(1, 10);
            LOG << '[' <<std::this_thread::get_id() << "] thread started. Will be sleeping for " << secondsToSleep << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(secondsToSleep));

            [[maybe_unused]]
            const auto token = barrier.arrive();
            LOG << '[' <<std::this_thread::get_id() << "] task completed\n";
        };

        LOG << "starting...\n";
        std::vector<std::jthread> threads;
        for (size_t idx = 0; idx < threadsCount; ++idx)
            threads.emplace_back(task);

        std::this_thread::sleep_for(std::chrono::milliseconds(1U));

        LOG << "waiting for all threads to finish...\n";
        barrier.arrive_and_wait();
        LOG << "done...\n";

        /**
        2025-11-20 18:31:06.579687 starting...
        2025-11-20 18:31:06.579831 [139943672960768] thread started. Will be sleeping for 8
        2025-11-20 18:31:06.579861 [139943664568064] thread started. Will be sleeping for 1
        2025-11-20 18:31:06.579897 [139943656175360] thread started. Will be sleeping for 4
        2025-11-20 18:31:06.580921 waiting for all threads to finish...
        2025-11-20 18:31:07.580028 [139943664568064] task completed
        2025-11-20 18:31:10.580391 [139943656175360] task completed
        2025-11-20 18:31:14.579991 [139943672960768] task completed
        2025-11-20 18:31:14.580035 done...
        **/
    }

    void Run_CallBack_WhenAllDone()
    {

        auto on_completion = []() noexcept {
            LOG << " **** ALL DONE at " << getCurrentTime()  << " ***\n";
        };

        constexpr size_t threadsCount {5};
        std::barrier barrier(threadsCount, on_completion);

        auto task = [&]() {
            const uint32_t secondsToSleep = getRandomInteger(1, 4);
            LOG << std::this_thread::get_id()<< " thread started. Sleep time = " << secondsToSleep << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(secondsToSleep));
            barrier.arrive_and_wait();
        };

        LOG << " starting...\n";
        std::vector<std::jthread> threads;
        for (size_t idx = 0; idx < threadsCount; ++idx)
            threads.emplace_back(task);

        for (std::jthread& T: threads)
            T.join();
        LOG << " done\n";

    }


    void Barrier_With_Completion()
    {
        std::vector<std::string> names { "One", "Two", "Three" };
        std::barrier phase(std::ssize(names),[] {
            LOG << "Callback()\n";
        });

        auto work = [&](std::string&& name) {
            LOG<< name << " job started\n";
            phase.arrive_and_wait();
            std::this_thread::sleep_for(std::chrono::seconds(1U));
            LOG << name << " job completed\n";
            phase.arrive_and_wait();
        };

        LOG << "Starting...\n";
        for (std::vector<std::jthread> jobs; std::string& name : names)
            jobs.emplace_back(work, name);
    }

    void Check_Block_By_Barrier()
    {
        std::array params { std::make_pair("One", 4), std::make_pair("Two", 2), std::make_pair("Three",3) };
        std::barrier phase(std::ssize(params),[] {
            LOG << "* * * * * Barrier limit reached!!!* * * * * \n";
        });

        auto work = [&](std::string&& name, uint32_t timeout) {
            LOG << name << " job started\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            phase.arrive_and_wait();
            LOG << name << " continuing\n";
        };

        LOG << "Starting...\n";
        for (std::vector<std::jthread> jobs; auto && param : params)
            jobs.emplace_back(work, param.first, param.second);

    }

    void Arrive_and_Wait_Separately()
    {
        std::atomic<uint32_t> counter {0};
        constexpr int num_threads = 3;
        std::barrier sync_point(num_threads, [] {
            LOG << ">>> All threads reached the barrier. Moving on... <<<\n";
        });

        auto task = [&]() {
            const uint32_t id = counter.fetch_add(1, std::memory_order_relaxed);
            const uint32_t msToSleep = getRandomInteger(100, 1000);
            LOG << "Thread[" <<id << "] thread started. Will be sleeping for " << msToSleep << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(msToSleep));

            LOG<< "Thread[" << id << "] signaled arrival, Signaling\n";
            auto token = sync_point.arrive(); // Does NOT wait

            LOG<< "Thread[" << id << "] doing extra work before waiting...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(250u));

            // Now explicitly wait for others
            sync_point.wait(std::move(token));

            LOG << "Thread[" << id << "] passed the barrier.\n";
        };

        {
            std::vector<std::jthread> threads;
            for (int i = 0; i < num_threads; ++i)
                threads.emplace_back(task);
        }

        LOG << ">>> All threads finished <<<\n";

        /**
        2025-11-10 19:50:16.555381 Thread[2] thread started. Will be sleeping for 872
        2025-11-10 19:50:16.555356 Thread[0] thread started. Will be sleeping for 637
        2025-11-10 19:50:16.555356 Thread[1] thread started. Will be sleeping for 150
        2025-11-10 19:50:16.705612 Thread[1] signaled arrival, Signaling
        2025-11-10 19:50:16.705653 Thread[1] doing extra work before waiting...
        2025-11-10 19:50:17.192601 Thread[0] signaled arrival, Signaling
        2025-11-10 19:50:17.192643 Thread[0] doing extra work before waiting...
        2025-11-10 19:50:17.427596 Thread[2] signaled arrival, Signaling
        2025-11-10 19:50:17.427651 >>> All threads reached the barrier. Moving on... <<<
        2025-11-10 19:50:17.427669 Thread[2] doing extra work before waiting...
        2025-11-10 19:50:17.427690 Thread[1] passed the barrier.
        2025-11-10 19:50:17.442734 Thread[0] passed the barrier.
        2025-11-10 19:50:17.677766 Thread[2] passed the barrier.
        2025-11-10 19:50:17.677909 >>> All threads finished <<<
        **/
    }
};

namespace Barrier::Reuse_Callback
{
    void Test()
    {
        constexpr uint16_t threadCount {4};

        std::barrier phase(threadCount,[id = 1] mutable {
            LOG << "Phase " << id << " complete\n";
            id++;
        });

        std::vector<std::jthread> runners;
        std::generate_n(std::back_inserter(runners), threadCount, [&phase]{
            return std::jthread([&phase]{
                LOG << "Running phase 1 for thread " << std::this_thread::get_id() << std::endl;

                std::this_thread::yield(); /** block until all threads arrive **/
                phase.arrive_and_wait();

                LOG << "Running phase 2 for thread " << std::this_thread::get_id() << std::endl;

                std::this_thread::yield(); /** block until all threads arrive **/
                phase.arrive_and_wait();
            });
        });
    }
}

void Barrier::TEST_ALL()
{
    // SimpleTest();
    // Test();
    // Wait_To_All_Thread_Completed();
    // Run_CallBack_WhenAllDone();
    Barrier_With_Completion();
    // Check_Block_By_Barrier();
    // Reuse_Callback::Test();
    // Arrive_and_Wait_Separately();
};
