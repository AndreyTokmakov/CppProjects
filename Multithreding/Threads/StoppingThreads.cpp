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


namespace StoppingThreads
{
    void worker(std::stop_token stop_token) {
        int counter {0};
        while (!stop_token.stop_requested()) {
            std::cout << ++counter << ' ' << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds (250));
        }
        std::cout << "Done\n";
    }

    void func() {
        std::osyncstream(std::cout) << " Entered. Sleeping 2 seconds." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::osyncstream(std::cout) << " Done" << std::endl;
    };

    void Test() {
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

    using namespace std::literals; // for duration literals

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
    // StopThread2();
    // Test();

    StopThread3();
};