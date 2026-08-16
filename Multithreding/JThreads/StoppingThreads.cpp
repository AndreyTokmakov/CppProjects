/**============================================================================
Name        : StoppingThreads.cpp
Created on  : 20.11.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StoppingThreads.cpp
============================================================================**/

#include "JThreads.hpp"

#include <iostream>
#include <thread>
#include <syncstream>
#include <future>
#include <vector>
#include <optional>

#include "DateTimeUtilities.hpp"

namespace
{
    using utilities::datetime::getCurrentTime;

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "
}


using namespace std::literals; // for duration literals

namespace jthreads::stopping_threads
{
    void worker(const std::stop_token &token,
                const std::chrono::duration<double> timeout = std::chrono::milliseconds (250))
    {
        int counter{0};
        while (!token.stop_requested()) {
            std::osyncstream(std::cout) << ++counter << std::endl;
            std::this_thread::sleep_for(timeout);
        }
        std::osyncstream(std::cout) << "Done\n";
    }
}

namespace jthreads::stopping_threads::basic_examples
{
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
            const std::stop_callback callBack(stop_token, [&counter, thread_id] {
                LOG << "(Stop callback) id: " << thread_id << "; counter: " << counter << '\n';
            });

            while (!stop_token.stop_requested()) {
                std::this_thread::sleep_for(250ms);
                ++counter;
            }
            LOG << "Thread id: " << thread_id << " done\n";
        };

        std::vector<std::jthread> tasks(3);
        for (std::jthread &T: tasks)
            T = std::jthread(func);

        std::this_thread::sleep_for(1s);
        for (std::jthread &T: tasks)
            T.request_stop();
        /*
        2026-04-12 07:29:02.058494 (Stop callback) id: 140388556433152; counter: 3
        2026-04-12 07:29:02.058773 (Stop callback) id: 140388548040448; counter: 3
        2026-04-12 07:29:02.058788 (Stop callback) id: 140388539647744; counter: 3
        2026-04-12 07:29:02.059533 Thread id: 140388548040448 done
        2026-04-12 07:29:02.059537 Thread id: 140388556433152 done
        2026-04-12 07:29:02.059848 Thread id: 140388539647744 done
        */
    }
}

namespace jthreads::stopping_threads::stop_source
{
    void check_is_stop_possible()
    {
        const std::stop_source source{};
        std::jthread job(worker, source.get_token(), 250ms);

        std::this_thread::sleep_for(std::chrono::seconds(1U));

        LOG << "Calling request_stop()\n";
        source.request_stop();
        LOG << "Stop_possible() = " << std::boolalpha << source.stop_possible() << std::endl;

        job.join();

        /**
        1
        2
        3
        4
        2026-04-12 07:48:03.755064 Calling request_stop()
        2026-04-12 07:48:03.755258 Stop_possible() = true
        Done
        **/
    }

    void task(const int id, const std::stop_token& token)
    {
        const std::thread::id threadId = std::this_thread::get_id();
        for (int i = 10; i; --i)
        {
            if (token.stop_requested()) {
                LOG << "[" << threadId << " - " << id << "] stop request\n";
                return;
            }

            std::this_thread::sleep_for(250ms);
            LOG << "[" << threadId << " - " << id << "] sleeping\n";
        }
    }

    void printStopSourceInfo(const std::stop_source& source)
    {
        LOG << "Stop Source info:\n"
            << "\tstop_possible : " << std::boolalpha  <<  source.stop_possible() << "\n"
            << "\tstop_requested: " << std::boolalpha  <<  source.stop_requested() << std::endl;
    };

    // A stop request made for one stop_source object is visible to all
    // stop_sources and std::stop_tokens of the same associated

    void StopMultipleThreads()
    {
        const std::stop_source stopSource;
        printStopSourceInfo(stopSource);

        std::vector<std::jthread> pool;
        for (int i = 0; i < 5; ++i)
            pool.emplace_back(task, i, stopSource.get_token());

        std::this_thread::sleep_for(500ms);
        LOG << "Request stop\n";

        stopSource.request_stop();
        printStopSourceInfo(stopSource);
    }
}


namespace jthreads::stopping_threads::stop_source
{
    void sensor_loop(const std::stop_token& token,
                     const std::string_view name,
                     const std::chrono::duration<double> timeout = std::chrono::milliseconds (250))
    {
        while (!token.stop_requested()) {
            LOG << name << " reading...\n";
            std::this_thread::sleep_for(timeout);
        }
        LOG << name << " stopped.\n";
    }

    void cancellation_multiple_threads()
    {
        const std::stop_source stopSource;

        const std::jthread t1(sensor_loop, stopSource.get_token(), "Sensor-A", 250ms);
        const std::jthread t2(sensor_loop, stopSource.get_token(), "Sensor-B", 250ms);
        const std::jthread t3(sensor_loop, stopSource.get_token(), "Sensor-C", 250ms);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        stopSource.request_stop();
        std::cout << "All sensors told to stop.\n";

        /*
        2026-04-12 08:03:40.617054 Sensor-A reading...
        2026-04-12 08:03:40.617053 Sensor-B reading...
        2026-04-12 08:03:40.617109 Sensor-C reading...
        2026-04-12 08:03:40.867271 Sensor-B reading...
        2026-04-12 08:03:40.867268 Sensor-A reading...
        2026-04-12 08:03:40.867269 Sensor-C reading...
        2026-04-12 08:03:41.117394 Sensor-B reading...
        2026-04-12 08:03:41.117409 Sensor-A reading...
        2026-04-12 08:03:41.117446 Sensor-C reading...
        2026-04-12 08:03:41.367500 Sensor-B reading...
        2026-04-12 08:03:41.367526 Sensor-A reading...
        2026-04-12 08:03:41.367537 Sensor-C reading...
        All sensors told to stop.
        2026-04-12 08:03:41.617631 Sensor-B stopped.
        2026-04-12 08:03:41.617659 Sensor-C stopped.
        2026-04-12 08:03:41.617631 Sensor-A stopped.
        */
    }
}

namespace jthreads::stopping_threads
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
        const std::stop_token token = job1.get_stop_token();
        const std::jthread job2 = std::jthread(func, 2);

        std::this_thread::sleep_for(1s);

        LOG << "Main  : Stopping threads.\n";
        job1.request_stop();

        /*
        2026-04-12 07:02:40.120243 Thread: 1 Doing some work . . . . .
        2026-04-12 07:02:40.120243 Thread: 2 Doing some work . . . . .
        2026-04-12 07:02:40.370448 Thread: 1 Doing some work . . . . .
        2026-04-12 07:02:40.370488 Thread: 2 Doing some work . . . . .
        2026-04-12 07:02:40.620547 Thread: 1 Doing some work . . . . .
        2026-04-12 07:02:40.620576 Thread: 2 Doing some work . . . . .
        2026-04-12 07:02:40.870686 Thread: 1 Doing some work . . . . .
        2026-04-12 07:02:40.870724 Thread: 2 Doing some work . . . . .
        2026-04-12 07:02:41.120312 Main  : Stopping threads.
        2026-04-12 07:02:41.120795 Thread: 1 Stopped!!!
        2026-04-12 07:02:41.120821 Thread: 2 Stopped!!!
        */
    }
}

void jthreads::stopping_threads::TestAll()
{
    // basic_examples::SimpleExample();
    // basic_examples::SimpleExample_StopCallback();
    // basic_examples::SimpleExample_ConditionalVariable();
    // basic_examples::StopMultipleThreads();

    // stop_source::StopMultipleThreads();
    // stop_source::check_is_stop_possible();
    stop_source::cancellation_multiple_threads();


    // StopMultipleThreads_UsingOneStopToken();
};