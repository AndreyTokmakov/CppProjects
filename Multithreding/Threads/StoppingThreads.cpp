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


using namespace std::literals; // for duration literals


namespace StoppingThreads
{
    void worker(std::stop_token stop_token)
    {
        int counter {0};
        while (!stop_token.stop_requested()) {
            std::osyncstream(std::cout) << ++counter << ' ' << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds (250));
        }
        std::osyncstream(std::cout) << "Done\n";
    }


    void SimpleExample()
    {
        std::jthread job = std::jthread([](const std::stop_token& token){
            while (!token.stop_requested()) {
                std::osyncstream(std::cout) << "Thread: Doing some work . . . . . \n";
                std::this_thread::sleep_for(250ms);
            }
            std::osyncstream(std::cout) << "Thread: Stopped!!!\n";
        });

        std::this_thread::sleep_for(std::chrono::seconds (1));
        std::osyncstream(std::cout) << "Main  : Stopping thread.\n";
        job.request_stop(); // request stop

        job.join();
        std::osyncstream(std::cout) << "Main  : Done.\n";
    }

    void SimpleExample_StopCallback()
    {
        std::jthread job = std::jthread([](const std::stop_token& token){
            std::atomic_flag isStopRequested {false};
            std::stop_callback callback(token, [&isStopRequested]{
                std::osyncstream(std::cout) << "Thread: Observed stop request\n";
                isStopRequested.test_and_set();
            });

            while (!isStopRequested.test(std::memory_order_relaxed)) {
                std::osyncstream(std::cout) << "Thread: Doing some work . . . . . \n";
                std::this_thread::sleep_for(250ms);
            }
            std::osyncstream(std::cout) << "Thread: Stopped!!!\n";
        });

        std::this_thread::sleep_for(std::chrono::seconds (1));
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

        std::jthread job = std::jthread([&resource](const std::stop_token& token){
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

        std::this_thread::sleep_for(std::chrono::seconds (1));


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

    void Test()
    {
        std::stop_source source {};
        std::stop_token token = source.get_token();

        // std::thread job(func);
        std::jthread job(worker, token);

        std::this_thread::sleep_for(std::chrono::seconds(2));
        // std::cout << "stop_possible() = " << std::boolalpha << token.stop_possible() << std::endl;

        job.request_stop();
        std::osyncstream(std::cout) << "Stop_possible() = " << std::boolalpha << token.stop_possible() << std::endl;


        job.join();

    }

    void func1(const std::stop_token& st, int num)
    {
        const auto id = std::this_thread::get_id();
        std::osyncstream(std::cout) << "call func1(" << num << ")\n";

        // register a first callback:
        std::stop_callback cb1{st, [num, id] {
            std::osyncstream(std::cout) << "- STOP1 requested in func(" << num
                      << (id == std::this_thread::get_id() ? ")\n" :") in main thread\n");
        }};

        std::this_thread::sleep_for(9ms);

        /*
        // register a second callback:
        std::stop_callback cb2{st, [num, id]{
            std::cout << "- STOP2 requested in func(" << num
                      << (id == std::this_thread::get_id() ? ")\n" : ") in main thread\n");
        }};
        std::this_thread::sleep_for(2ms);
        */
    }


    void StopThread2()
    {
        // create stop_source and stop_token:
        std::stop_source ssrc;
        std::stop_token stok { ssrc.get_token() };

        // register callback:
        std::stop_callback cb{stok, []{
            std::osyncstream(std::cout)<< "- STOP requested in main()\n" << std::flush;
        }};

        // in the background call func() a couple of times:
        auto fut = std::async([stok] {
            for (int num = 1; num < 13; ++num) {
                func1(stok, num);
            }
        });

        // after a while, request stop:
        std::this_thread::sleep_for(120ms);
        ssrc.request_stop();
    }


    void StopThread3()
    {
        auto func = [](std::stop_token token) {
            std::atomic<int> counter{0};
            auto thread_id = std::this_thread::get_id();
            std::stop_callback callBack(token, [&counter, thread_id] {
                std::osyncstream(std::cout) << "(Stop callback) id: " << thread_id << "; counter: " << counter << '\n';
            });
            while (counter < 10) {
                std::this_thread::sleep_for(std::chrono::milliseconds (250));
                ++counter;
            }

            std::osyncstream(std::cout) << "Thread id: " << thread_id << " stopped actually now\n";
        };

        std::vector<std::jthread> tasks(10);
        for (std::jthread & T: tasks)
            T = std::jthread(func);

        std::this_thread::sleep_for(1s);
        for (std::jthread & T: tasks)
            T.request_stop();
    }
};

void StoppingThreads::TestAll()
{
    // SimpleExample();
    // SimpleExample_StopCallback();
    SimpleExample_ConditionalVariable();

    // SimpleExample();
    // StopThread2();
    // Test();

    //StopThread3();
};