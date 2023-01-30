//============================================================================
// Name        : Promise.cpp
// Created on  : 14.09.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Promise src class
//============================================================================

#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <chrono>
#include <future>
#include <algorithm>
#include <execution>

#include "../ThreadHelperUtilities/ThreadHelperUtilities.h"
#include "Promise.h"

namespace Promise {

    void accumulate(const std::vector<int>& vector, std::promise<int> accumulate_promise)
    {
        THREAD_INFO << "accumulate() started" << std::endl;
        int sum = std::accumulate(vector.begin(), vector.end(), 0);
        // Notify future
        accumulate_promise.set_value(sum);
        THREAD_INFO << "accumulate() done" << std::endl;
    }

    void do_work(std::promise<void> barrier) {
        THREAD_INFO << "do_work() started" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
        barrier.set_value();
        THREAD_INFO << "do_work() done" << std::endl;
    }

    ////////////////////////////////////////////////////////////////

    void SimpleTest() {
        const auto print_int = [](std::future<int>& fut) {
            THREAD_INFO << "func started" << std::endl;
            int x = fut.get();
            std::this_thread::sleep_for(std::chrono::seconds(3));
            THREAD_INFO << "value: " << x << std::endl;
        };

        THREAD_INFO << "main thread" << std::endl;

        std::promise<int> promise;
        std::future<int> future = promise.get_future();
        std::jthread job(print_int, std::ref(future));

        promise.set_value(123);
    }


    void SimpleTest1() {
        std::promise<void> promise;
        std::future<void> ready = promise.get_future();

        std::thread thread_b([&]() {
            THREAD_INFO << "Worer: Do some work 1 . . . ." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            promise.set_value();
            THREAD_INFO << "Worer: Do some work 2 . . . ." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            THREAD_INFO << "Worer: Done" << std::endl;
        });

        THREAD_INFO << "Before wait()" << std::endl;

        ready.wait();

        THREAD_INFO << "After wait()" << std::endl;
    }

    void SimpleTest1_1()
    {
        std::promise<void> promise;
        std::future<void> future = promise.get_future();

        std::jthread task([promise = std::move(promise)]()mutable {
            THREAD_INFO << "Worker: Do some work 1 . . . ." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            promise.set_value();
            THREAD_INFO << "Worker: Do some work 2 . . . ." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            THREAD_INFO << "Worker: Done" << std::endl;
        });

        std::this_thread::sleep_for(std::chrono::milliseconds (10));
        THREAD_INFO << "Before wait()" << std::endl;
        future.wait();
        THREAD_INFO << "After wait()" << std::endl;
    }


    void SimpleTest2() {
        const auto initiazer = [](std::promise<int>* promObj) {
            THREAD_INFO << " Started." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            promObj->set_value(35);
        };

        THREAD_INFO << " Starting test." << std::endl;

        std::promise<int> promiseObj;
        std::future<int> futureObj = promiseObj.get_future();
        std::thread th(initiazer, &promiseObj);

        THREAD_INFO << ". Result: " << futureObj.get() << std::endl;
        th.join();
    }

    void Promise_Threads_1() {
        // Demonstrate using promise<int> to transmit a result between threads.
        std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
        std::promise<int> accumulate_promise;
        std::future<int> accumulate_future = accumulate_promise.get_future();

        std::thread work_thread(accumulate, numbers, std::move(accumulate_promise));

        // future::get() will wait until the future has a valid result and retrieves it.
        // Calling wait() before get() is not needed
        //accumulate_future.wait();  // wait for result
        THREAD_INFO << "result=" << accumulate_future.get() << '\n';
        work_thread.join();  // wait for thread completion
    }

    void Promise_Threads_2() {
        // Demonstrate using promise<void> to signal state between threads.
        std::promise<void> barrier;
        std::future<void> barrier_future = barrier.get_future();
        std::thread new_work_thread(do_work, std::move(barrier));

        THREAD_INFO << "barrier_future.wait()..." << std::endl;
        barrier_future.wait();
        THREAD_INFO << "new_work_thread.join()..." << std::endl;
        new_work_thread.join();
        THREAD_INFO << "new_work_thread.join() done" << std::endl;
    }

    void SetValueAtThreadExit()
    {
        std::promise<int> promise;
        std::future<int> future = promise.get_future();
        std::thread([&promise] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            promise.set_value_at_thread_exit(9);
        }).detach();

        THREAD_INFO << "Waiting for thread to exit...." << std::endl;
        future.wait();

        THREAD_INFO << "Done!!!. Result = " << future.get() << std::endl;
    }

    void ComplexTest() {
        auto spPromise = std::make_shared<std::promise<void>>();
        std::future<void> waiter = spPromise->get_future();

        auto call = [spPromise](size_t value) {
            //size_t i = std::numeric_limits<size_t>::max();
            size_t i = 100;
            while (i--) {
                THREAD_INFO << i << "    " << value << std::endl;
                if (i == value) {
                    THREAD_INFO << i << " == " << value << std::endl;
                    spPromise->set_value();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        };

        THREAD_INFO << "Starting thread" << std::endl;
        //std::thread thread(call, std::numeric_limits<size_t>::max() - 500);
        std::thread thread(call, 50);

        THREAD_INFO << "Detach thread" << std::endl;
        thread.detach();

        THREAD_INFO << "Waiting.." << std::endl;
        waiter.get();

        THREAD_INFO << "Done" << std::endl;
    }

    void SetException() {
        std::promise<int> promise;
        std::future<int> future = promise.get_future();

        THREAD_INFO << "Thread [id: " << std::this_thread::get_id() << "] " << "Starting test." << std::endl;

        std::thread thread([&promise] {
            try {
                // code that may throw
                THREAD_INFO << "Thread [id: " << std::this_thread::get_id() << "]" << "Starting test." << std::endl;
                throw std::runtime_error("Example");
            }
            catch (...) {
                try {
                    // store anything thrown in the promise
                    promise.set_exception(std::current_exception());
                }
                catch (...) {
                    // set_exception() may throw too
                }
            }
        });

        try {
            THREAD_INFO << future.get();
        }
        catch (const std::exception& e) {
            THREAD_INFO << "Exception from the thread: " << e.what() << std::endl;
        }
        thread.join();
    }

    void Consumer_Producer() {
        THREAD_INFO << " Starting test." << std::endl;
        auto promise = std::promise<std::string>();
        auto future = promise.get_future();

        auto producer = std::thread([&promise] {
            THREAD_INFO << " Producer started." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            promise.set_value("Hello World");
        });

        auto consumer = std::thread([&future] {
            THREAD_INFO << " Consumer started." << std::endl;
            THREAD_INFO << future.get();
        });

        THREAD_INFO << " producer.join()" << std::endl;
        producer.detach();

        THREAD_INFO << " consumer.join()" << std::endl;
        consumer.join();
    }
}

void Promise::TEST_ALL()
{
    // SimpleTest();
    // SimpleTest1();
    SimpleTest1_1();
    // SimpleTest2();

    // ComplexTest();

    // Promise_Threads_1();

    // Promise_Threads_2();

    // SetValueAtThreadExit();

    // SetException();

    // Consumer_Producer();
};
