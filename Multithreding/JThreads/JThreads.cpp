//============================================================================
// Name        : Threads.cpp
// Created on  : 07.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Threads src class
//============================================================================


#include <iostream>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <sstream>
#include <thread>
#include <string>
#include <iomanip>
#include <chrono>
#include <future>
#include <exception>
#include <semaphore>
#include <future>         // std::async, std::future
#include <chrono>
#include <print>
#include <format>
#include <syncstream>

#include "JThreads.h"

#include <functional>

#include "DateTimeUtilities.hpp"

namespace
{
    using DateTimeUtilities::getCurrentTime;

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

        std::lock_guard<std::mutex> lock {mtxPrint};
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

namespace JThreads
{

    void cancelable_functions(std::stop_token stop_token, int value)
    {
        while (!stop_token.stop_requested()) {
            Debug(value++);
            std::this_thread::sleep_for(std::chrono::seconds(1u));
        }
    }


    void Start_and_Stop_Thread() {
        std::jthread thread(cancelable_functions, 0);
        std::this_thread::sleep_for(std::chrono::seconds(3u));
    }

    //--------------------------------------------------------------------------

    void Stop_Thread_2()
    {
        auto func = [](std::stop_token stoken) {
            int counter {0};
            const auto id = std::this_thread::get_id();

            std::stop_callback callBack(stoken, [&counter, id] {
                LOG << "Stop callback: id: " << id<< "; counter: " << counter << '\n';
            });

            while (counter < 10) {
                std::this_thread::sleep_for(std::chrono::milliseconds(175u));
                ++counter;
            }

            LOG << "Thread " << id << " done\n";
        };


        std::vector<std::jthread> vecThreads(3);
        for(auto& T: vecThreads)
            T = std::jthread(func);

        std::this_thread::sleep_for(std::chrono::seconds(1u));

        for(auto& T: vecThreads)
            T.request_stop();
    }

    //---------------------------------------------------------------------------

    using namespace std::chrono_literals;

    void Request_Stop()
    {
        // A sleepy worker thread
        std::jthread sleepy_worker([](std::stop_token stoken) {
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

        // A waiting worker thread
        // The condition variable will be awoken by the stop request.
        std::jthread waiting_worker([](std::stop_token stoken) {
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

        std::this_thread::sleep_for(std::chrono::seconds(1u));

        Debug("Requesting stop of sleepy worker");
        sleepy_worker.request_stop();
        sleepy_worker.join();
        Debug("Sleepy worker joined");
    }


    void Request_Stop_2()
    {
        bool done = false;

        std::jthread job([&done] (std::stop_token token) {
            while (!token.stop_requested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100u));
            }
            done = true;
        });

        std::osyncstream {std::cout} << "Tread stopped: " << std::boolalpha << done << std::endl;

        job.request_stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(250u));

        std::osyncstream {std::cout} << "Tread stopped: " << std::boolalpha << done << std::endl;
    }
}

namespace JThreads::Joinable
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

namespace JThreads::Run_JThread_as_ClassMethod
{
    struct Worker
    {
        void run() {
            worker = std::jthread(std::bind_front(&Worker::handler, this));
        }

        void stop() {
            worker.request_stop();
        }

    private:

        std::jthread worker;

        void handler(const std::stop_token &token)
        {
            LOG << "Starting thread.\n";
            while (!token.stop_requested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(250u));
                LOG << std::format("Wrorking. [Stop requested: {}]", token.stop_requested()) << std::endl;
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
    2026-03-03 18:17:50.040390 Starting thread.
    2026-03-03 18:17:50.290592 Wrorking. [Stop requested: false]
    2026-03-03 18:17:50.540722 Wrorking. [Stop requested: false]
    2026-03-03 18:17:50.790871 Wrorking. [Stop requested: false]
    2026-03-03 18:17:51.041038 Wrorking. [Stop requested: false]
    2026-03-03 18:17:51.291167 Wrorking. [Stop requested: false]
    2026-03-03 18:17:51.541303 Wrorking. [Stop requested: false]
    2026-03-03 18:17:51.791403 Wrorking. [Stop requested: false]
    2026-03-03 18:17:52.040420 Stopping thread.
    2026-03-03 18:17:52.041512 Wrorking. [Stop requested: true]
    2026-03-03 18:17:52.041576 Test completed
    */
}

void JThreads::TestAll()
{
    // Joinable::Test();

    Run_JThread_as_ClassMethod::demo();

    // Start_and_Stop_Thread();
    // Stop_Thread_2();
    // Request_Stop();
    // Request_Stop_2();
}
