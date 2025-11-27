/**============================================================================
Name        : StoppingThreads.cpp
Created on  : 20.11.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StoppingThreads.cpp
============================================================================**/

#include "StoppingThreads.h"

#include <iostream>
#include <thread>
#include <syncstream>
#include <future>
#include <vector>
#include <optional>

#include "DateTimeUtilities.hpp"

namespace
{
    using DateTimeUtilities::getCurrentTime;

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "
}


using namespace std::literals; // for duration literals


namespace StoppingThreads::BasicExamples
{
    void worker(const std::stop_token &token)
    {
        int counter{0};
        while (!token.stop_requested()) {
            std::osyncstream(std::cout) << ++counter << ' ' << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        std::osyncstream(std::cout) << "Done\n";
    }

    void SimpleExample()
    {
        std::jthread job = std::jthread([](const std::stop_token &token) {
            while (!token.stop_requested()) {
                std::osyncstream(std::cout) << "Thread: Doing some work . . . . . \n";
                std::this_thread::sleep_for(250ms);
            }
            std::osyncstream(std::cout) << "Thread: Stopped!!!\n";
        });

        std::this_thread::sleep_for(std::chrono::seconds(1u));
        std::osyncstream(std::cout) << "Main  : Stopping thread.\n";
        job.request_stop(); // request stop

        job.join();
        std::osyncstream(std::cout) << "Main  : Done.\n";
    }

    void SimpleExample_StopCallback()
    {
        std::jthread job = std::jthread([](const std::stop_token &token) {
            std::atomic_flag isStopRequested{false};
            std::stop_callback callback(token, [&isStopRequested] {
                std::osyncstream(std::cout) << "Thread: Observed stop request\n";
                isStopRequested.test_and_set();
            });

            while (!isStopRequested.test(std::memory_order_relaxed)) {
                std::osyncstream(std::cout) << "Thread: Doing some work . . . . . \n";
                std::this_thread::sleep_for(250ms);
            }
            std::osyncstream(std::cout) << "Thread: Stopped!!!\n";
        });

        std::this_thread::sleep_for(1s);
        std::osyncstream(std::cout) << "Main  : Stopping thread.\n";
        job.request_stop(); // request stop

        job.join();
        std::osyncstream(std::cout) << "Main  : Done.\n";
    }

    void SimpleExample_ConditionalVariable()
    {
        struct Resource {
            std::mutex mux;
            std::condition_variable_any cv;
            bool ready = false;
        };

        Resource resource;

        std::jthread job = std::jthread([&resource](const std::stop_token &token) {
            std::osyncstream(std::cout) << "Thread: Wait until resource is ready, or stop was requested\n";

            std::unique_lock lock(resource.mux);
            resource.cv.wait(lock, token, [&resource] { return resource.ready; });

            if (resource.ready) { // resource is ready
                std::osyncstream(std::cout) << "Thread: Resource ready (with std::condition_variable_any)\n";
            } else { // stop was requested
                std::osyncstream(std::cout) << "Thread: Stop requested (with std::stop_token)\n";
            }

            std::osyncstream(std::cout) << "Thread: Stopped!!!\n";
        });

        std::this_thread::sleep_for(std::chrono::seconds(1));


        bool stopWithRequest = false;
        if (stopWithRequest) { // change to see the other option
            std::osyncstream(std::cout) << "Main  : Stopping thread.\n";
            job.request_stop(); // request stop
        } else {
            std::osyncstream(std::cout) << "Main  : Stopping thread.\n";
            std::unique_lock lock(resource.mux);
            resource.ready = true;
            resource.cv.notify_one();
        }

        job.join();
        std::osyncstream(std::cout) << "Main  : Done.\n";
    }

    void StopMultipleThreads()
    {
        auto func = [](const std::stop_token &stop_token) {
            int counter{0};
            const auto thread_id = std::this_thread::get_id();

            std::stop_callback callBack(stop_token, [&counter, thread_id] {
                std::osyncstream(std::cout) << "(Stop callback) id: " << thread_id << "; counter: " << counter << '\n';
            });

            while (!stop_token.stop_requested()) {
                std::this_thread::sleep_for(250ms);
                ++counter;
            }

            std::osyncstream(std::cout) << "Thread id: " << thread_id << " done\n";
        };

        std::vector<std::jthread> tasks(10);
        for (std::jthread &T: tasks)
            T = std::jthread(func);

        std::this_thread::sleep_for(1s);
        for (std::jthread &T: tasks)
            T.request_stop();
    }

    void Test()
    {
        std::stop_source source{};
        std::stop_token token = source.get_token();

        // std::thread job(func);
        std::jthread job(worker, token);

        std::this_thread::sleep_for(std::chrono::seconds(2U));
        // std::cout << "stop_possible() = " << std::boolalpha << token.stop_possible() << std::endl;

        job.request_stop();
        std::osyncstream(std::cout) << "Stop_possible() = " << std::boolalpha << token.stop_possible() << std::endl;


        job.join();

    }
}




namespace StoppingThreads::Callbacks
{
    void MultipleCallbacks()
    {
        std::jthread job = std::jthread([](const std::stop_token &token)
        {
            std::stop_callback cbOne(token, [] {
                std::osyncstream(std::cout) << "\tCallback (one)   called: ID =  " << std::this_thread::get_id() << std::endl;
            });

            std::stop_callback cbTwo(token, [] {
                std::osyncstream(std::cout) << "\tCallback (Two)   called: ID =  " << std::this_thread::get_id() << std::endl;
            });

            std::stop_callback cbThree(token, [] {
                std::osyncstream(std::cout) << "\tCallback (Three) called: ID =  " << std::this_thread::get_id() << std::endl;
            });

            while (!token.stop_requested()) {
                std::this_thread::sleep_for(250ms);
            }

            std::osyncstream(std::cout) << "Thread id: " << std::this_thread::get_id() << " done\n";
        });

        std::this_thread::sleep_for(1s);
        std::osyncstream(std::cout) << "Main  : Stopping thread.\n";
        job.request_stop(); // request stop

        job.join();
        std::osyncstream(std::cout) << "Main  : Done.\n";
    }
}

namespace StoppingThreads::StopSource
{
    void task(const int id, const std::stop_token& token)
    {
        const std::thread::id threadId = std::this_thread::get_id();
        for (int i = 10; i; --i)
        {
            if (token.stop_requested()) {
                std::osyncstream{std::cout} << "[" << threadId << " - " << id << "] stop request\n";
                return;
            }

            std::this_thread::sleep_for(250ms);
            std::osyncstream{std::cout} << "[" << threadId << " - " << id << "] sleeping\n";
        }
    }

    void printStopSourceInfo(const std::stop_source& source)
    {
        std::cout << "Stop Source info:\n"
            << "\tstop_possible : " << std::boolalpha  <<  source.stop_possible() << "\n"
            << "\tstop_requested: " << std::boolalpha  <<  source.stop_requested() << std::endl;
    };

    // A stop request made for one stop_source object is visible to all
    // stop_sources and std::stop_tokens of the same associated

    void StopMultipleThreads()
    {
        std::stop_source stopSource;
        printStopSourceInfo(stopSource);

        std::vector<std::jthread> pool;
        for (int i = 0; i < 5; ++i)
            pool.emplace_back(task, i, stopSource.get_token());

        std::this_thread::sleep_for(500ms);
        std::osyncstream{std::cout} << "Request stop\n";

        stopSource.request_stop();
        printStopSourceInfo(stopSource);
    }
}

namespace StoppingThreads
{
    void StopMultipleThreads_UsingOneStopToken()
    {
        auto func = [](const std::stop_token &token, const int id) {
            while (!token.stop_requested()) {
                LOG << std::format("Thread: {} Doing some work . . . . . \n", id);
                std::this_thread::sleep_for(250ms);
            }
            LOG << std::format("Thread: {} Stopped!!!\n", id);
        };

        std::jthread job1 = std::jthread(func, 1);
        std::stop_token token = job1.get_stop_token();
        std::jthread job2 = std::jthread(func, 2);

        std::this_thread::sleep_for(1s);

        LOG << "Main  : Stopping threads.\n";
        job1.request_stop();
    }
}

void StoppingThreads::TestAll()
{
    // BasicExamples::SimpleExample();
    // BasicExamples::SimpleExample_StopCallback();
    // BasicExamples::SimpleExample_ConditionalVariable();
    // BasicExamples::StopMultipleThreads();

    // Callbacks::MultipleCallbacks();

    // StopSource::StopMultipleThreads();

    StopMultipleThreads_UsingOneStopToken();
};