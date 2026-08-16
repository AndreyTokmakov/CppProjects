/**============================================================================
Name        : Threads.cpp
Created on  : 07.06.2020
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Threads src class
============================================================================**/

#include <iostream>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <sstream>
#include <thread>
#include <string>
#include <iomanip>
#include <chrono>
#include <format>
#include <syncstream>
#include <functional>

#include "JThreads.hpp"
#include "DateTimeUtilities.hpp"

namespace
{
    using utilities::datetime::getCurrentTime;

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "
}


namespace
{
    const std::thread::id mainThreadId = std::this_thread::get_id();
    std::mutex mtxPrint;

    template<typename ... _Types>
    void Debug(_Types&& ... params) {
        auto add_space = []<typename T>(const T& arg) {
            std::cout << ' ';
            return arg;
        };

        const auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");

        const std::lock_guard<std::mutex> lock {mtxPrint};
        const auto threadID = std::this_thread::get_id();

        LOG  << "[" << ss.str() << "] Thread [";
        if (mainThreadId == threadID)
            LOG  << std::setiosflags(std::ios::left) << std::setw(19) << "Main";
        else
            LOG << "Id: " << std::setiosflags(std::ios::left) << std::setw(11) << threadID;
        LOG << "] ";
        (std::cout << ... << add_space(std::forward<_Types>(params))) << std::endl;
    }
}

namespace jthreads
{

    void cancelable_functions(const std::stop_token& stop_token, int value)
    {
        while (!stop_token.stop_requested()) {
            Debug(value++);
            std::this_thread::sleep_for(std::chrono::seconds(1U));
        }
    }


    void Start_and_Stop_Thread()
    {
        const std::jthread thread(cancelable_functions, 0);
        std::this_thread::sleep_for(std::chrono::seconds(3U));
    }

    //--------------------------------------------------------------------------

    void Stop_Thread_2()
    {
        auto func = [](const std::stop_token& stoken) {
            int counter {0};
            const auto id = std::this_thread::get_id();

            std::stop_callback callBack(stoken, [&counter, id] {
                LOG << "Stop callback: id: " << id<< "; counter: " << counter << '\n';
            });

            while (counter < 10) {
                std::this_thread::sleep_for(std::chrono::milliseconds(175U));
                ++counter;
            }

            LOG << "Thread " << id << " done\n";
        };


        std::vector<std::jthread> vecThreads(3);
        for(auto& T: vecThreads)
            T = std::jthread(func);

        std::this_thread::sleep_for(std::chrono::seconds(1U));

        for(auto& T: vecThreads)
            T.request_stop();
    }

    //---------------------------------------------------------------------------

    using namespace std::chrono_literals;

    void Request_Stop()
    {
        // A sleepy worker thread
        std::jthread sleepy_worker([](const std::stop_token& stoken) {
            Debug("sleepy_worker started");
            for (int i = 10; i; --i) {
                std::this_thread::sleep_for(300ms);
                if (stoken.stop_requested()) {
                    Debug("Sleepy worker is requested to stop");
                    return;
                }
                Debug("Sleepy worker goes back to sleep");
            }
        });

        // A waiting worker thread - The condition variable will be awoken by the stop request.
        std::jthread const waiting_worker([](std::stop_token stoken) {
            Debug("waiting_worker started");
            std::mutex mutex;
            std::unique_lock lock(mutex);
            std::condition_variable_any().wait(lock, stoken,
                                               [&stoken] { return false; });
            if (stoken.stop_requested()) {
                Debug("Waiting worker is requested to stop");
                return;
            }
        });

        std::this_thread::sleep_for(std::chrono::seconds(1U));

        Debug("Requesting stop of sleepy worker");
        sleepy_worker.request_stop();
        sleepy_worker.join();
        Debug("Sleepy worker joined");
    }


    void Request_Stop_2()
    {
        bool done = false;

        std::jthread job([&done] (const std::stop_token& token) {
            while (!token.stop_requested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100U));
            }
            done = true;
        });

        std::osyncstream {std::cout} << "Tread stopped: " << std::boolalpha << done << std::endl;

        job.request_stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(250U));

        std::osyncstream {std::cout} << "Tread stopped: " << std::boolalpha << done << std::endl;
    }
}

namespace jthreads::Joinable
{
    void Test()
    {
        std::jthread job = std::jthread([](const std::stop_token &token) {
            while (!token.stop_requested()) {
                LOG << "Thread: Doing some work . . . . . \n";
                std::this_thread::sleep_for(250ms);
            }
            LOG << "Thread: Stopped!!!\n";
        });

        for (int i = 0; i < 10; ++i) {
            std::this_thread::sleep_for(250ms);
            LOG << "Main: Is Job Joinable = " << std::boolalpha << job.joinable() << '\n';
            if (i == 6 && job.joinable()) {
                job.request_stop();
                LOG << "Main: Stopping thread.\n";
            }
        }

        job.join();
        std::osyncstream(std::cout) << "Main  : Done.\n";
    }
}

namespace jthreads::Run_JThread_as_ClassMethod
{
    struct Worker
    {
        void run()
        {
#if 0
            worker = std::jthread(std::bind_front(&Worker::handler, this));
#else
            worker = std::jthread([&] (const std::stop_token& token) {
                handler(token);
            });
#endif
        }

        void stop() {
            worker.request_stop();
        }

    private:

        std::jthread worker;

        static void handler(const std::stop_token &token)
        {
            LOG << "Starting thread.\n";
            while (!token.stop_requested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(250U));
                LOG << std::format("Working. [Stop requested: {}]", token.stop_requested()) << std::endl;
            }
        }
    };

    void demo()
    {
        {
            Worker worker;
            worker.run();

            std::this_thread::sleep_for(std::chrono::seconds(2));
            LOG << "Stopping thread.\n";
            worker.stop();
        }
        LOG << "Test completed\n";
    }

    /*
    2026-04-12 07:01:06.325397 Starting thread.
    2026-04-12 07:01:06.575613 Working. [Stop requested: false]
    2026-04-12 07:01:06.825744 Working. [Stop requested: false]
    2026-04-12 07:01:07.075868 Working. [Stop requested: false]
    2026-04-12 07:01:07.325978 Working. [Stop requested: false]
    2026-04-12 07:01:07.576118 Working. [Stop requested: false]
    2026-04-12 07:01:07.826254 Working. [Stop requested: false]
    2026-04-12 07:01:08.076361 Working. [Stop requested: false]
    2026-04-12 07:01:08.325446 Stopping thread.
    2026-04-12 07:01:08.326470 Working. [Stop requested: true]
    2026-04-12 07:01:08.326575 Test completed
    */
}

void jthreads::TestAll()
{
    stopping_threads::TestAll();
    // stop_callbacks::TestAll();

    // Joinable::Test();
    // Run_JThread_as_ClassMethod::demo();
    // Start_and_Stop_Thread();
    // Stop_Thread_2();
    // Request_Stop();
    // Request_Stop_2();
}
