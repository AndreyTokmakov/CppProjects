/**============================================================================
Name        : Promise.cpp
Created on  : 14.09.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Promise src class
============================================================================**/

#include <iostream>
#include <syncstream>
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <chrono>
#include <future>
#include <algorithm>
#include <execution>
#include <format>

#include "Promise.h"
#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << utilities::datetime::getCurrentTime() << " "

namespace
{
    std::string currentTime()
    {
        return std::format("{:%d-%m-%Y %H:%M:%OS}",  std::chrono::system_clock::now());
    }
}

namespace Promise {

    void accumulate(const std::vector<int>& vector, std::promise<int> accumulate_promise)
    {
        LOG << "accumulate() started" << std::endl;
        int sum = std::accumulate(vector.begin(), vector.end(), 0);
        // Notify future
        accumulate_promise.set_value(sum);
        LOG << "accumulate() done" << std::endl;
    }

    void do_work(std::promise<void> barrier) {
        LOG << "do_work() started" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10U));
        barrier.set_value();
        LOG << "do_work() done" << std::endl;
    }

    //----------------------------------------------------------------------------------------------

    void Simple_Test_0()
    {
        std::promise<int> promise;

        std::jthread T2([&promise]{
            LOG << "T2 started" << std::endl;
            std::future<int> fut = promise.get_future();
            LOG << "T2 waiting" << std::endl;
            const int result = fut.get();
            LOG << "T2 done. Result = " << result << std::endl;
        });

        std::jthread T1([&promise]{
            LOG << "T1 started" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1U));
            promise.set_value(123);
            LOG << "T1 done. Value -> 123" << std::endl;
        });
    }

    void Simple_Test_1()
    {
        std::promise<std::string> promise;
        std::future<std::string> future = promise.get_future();

        std::jthread t1 = std::jthread([promise = std::move(promise)] mutable {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000U));
            promise.set_value("Message from " + currentTime());
        });

        LOG << "Before" << std::endl;

        // Will block until value awailable, then returns the stored value:
        std::string result = future.get();

        std::cout << "future.get() = " << result << std::endl;
    }

    void Simple_Test_2()
    {
        const auto print_int = [](std::future<int>& fut) {
            LOG << "func started" << std::endl;
            int x = fut.get();
            std::this_thread::sleep_for(std::chrono::seconds(3U));
            LOG << "value: " << x << std::endl;
        };

        LOG << "main thread" << std::endl;

        std::promise<int> promise;
        std::future<int> future = promise.get_future();
        std::jthread job(print_int, std::ref(future));

        promise.set_value(123);
    }

    void Simple_Test_3()
    {
        std::promise<void> promise;
        std::future<void> ready = promise.get_future();

        std::thread thread_b([&]() {
            LOG << "Worer: Do some work 1 . . . ." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3U));
            promise.set_value();
            LOG << "Worer: Do some work 2 . . . ." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3U));
            LOG << "Worer: Done" << std::endl;
        });

        LOG << "Before wait()" << std::endl;

        ready.wait();

        LOG << "After wait()" << std::endl;
    }

    void Simple_Test_4()
    {
        std::promise<void> promise;
        std::future<void> future = promise.get_future();

        std::jthread task([promise = std::move(promise)]()mutable {
            LOG << "Worker: Do some work 1 . . . ." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3U));
            promise.set_value();
            LOG << "Worker: Do some work 2 . . . ." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3U));
            LOG << "Worker: Done" << std::endl;
        });

        std::this_thread::sleep_for(std::chrono::milliseconds (10U));
        LOG << "Before wait()" << std::endl;
        future.wait();
        LOG << "After wait()" << std::endl;
    }

    void Simple_Test_5()
    {
        const auto initiazer = [](std::promise<int>* promObj) {
            LOG << " Started." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3U));
            promObj->set_value(35);
        };

        LOG << " Starting test." << std::endl;

        std::promise<int> promiseObj;
        std::future<int> futureObj = promiseObj.get_future();
        std::thread th(initiazer, &promiseObj);

        LOG << ". Result: " << futureObj.get() << std::endl;
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
        LOG << "result=" << accumulate_future.get() << '\n';
        work_thread.join();  // wait for thread completion
    }

    void Promise_Threads_2() {
        // Demonstrate using promise<void> to signal state between threads.
        std::promise<void> barrier;
        std::future<void> barrier_future = barrier.get_future();
        std::thread new_work_thread(do_work, std::move(barrier));

        LOG << "barrier_future.wait()..." << std::endl;
        barrier_future.wait();
        LOG << "new_work_thread.join()..." << std::endl;
        new_work_thread.join();
        LOG << "new_work_thread.join() done" << std::endl;
    }

    void SetValueAtThreadExit()
    {
        std::promise<int> promise;
        std::future<int> future = promise.get_future();
        std::thread([&promise] {
            std::this_thread::sleep_for(std::chrono::seconds(1U));
            promise.set_value_at_thread_exit(9);
        }).detach();

        LOG << "Waiting for thread to exit...." << std::endl;
        future.wait();

        LOG << "Done!!!. Result = " << future.get() << std::endl;
    }

    void ComplexTest()
    {
        auto spPromise = std::make_shared<std::promise<void>>();
        std::future<void> waiter = spPromise->get_future();

        auto call = [spPromise](size_t value) {
            //size_t i = std::numeric_limits<size_t>::max();
            size_t i = 100;
            while (i--) {
                LOG << i << "    " << value << std::endl;
                if (i == value) {
                    LOG << i << " == " << value << std::endl;
                    spPromise->set_value();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100U));
            }
        };

        LOG << "Starting thread" << std::endl;
        //std::thread thread(call, std::numeric_limits<size_t>::max() - 500);
        std::thread thread(call, 50);

        LOG << "Detach thread" << std::endl;
        thread.detach();

        LOG << "Waiting.." << std::endl;
        waiter.get();

        LOG << "Done" << std::endl;
    }

    void SetException()
    {
        std::promise<int> promise;
        std::future<int> future = promise.get_future();

        LOG << "Thread [id: " << std::this_thread::get_id() << "] " << "Starting test." << std::endl;

        std::thread thread([&promise] {
            try {
                // code that may throw
                LOG << "Thread [id: " << std::this_thread::get_id() << "]" << "Starting test." << std::endl;
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
            LOG << future.get();
        }
        catch (const std::exception& e) {
            LOG << "Exception from the thread: " << e.what() << std::endl;
        }
        thread.join();
    }

    void Propagate_Exception()
    {
        std::promise<int> other;
        std::future<int> will_fail = other.get_future();
        auto t2 = std::jthread([promise = std::move(other)] mutable {
            try {
                throw std::runtime_error("Some error happened.");
                promise.set_value(10); /**  unreachable **/
            } catch (...) {
                promise.set_exception(std::current_exception());
                // same as before we can also: promise.set_exception_at_thread_exit(std::current_exception());
            }
        });

        // Block until value awailable, in this case, the exception will be propagated instead.
        try {
            const int v = will_fail.get();
            std::cout  << std::format("Unreachable, will not print. v == {}", v) << std::endl;
        } catch (const std::exception& e) {
            std::cout  << std::format("Caught a propagated exception: e.what() == {}", e.what()) << std::endl;
        }
    }

    void Consumer_Producer() {
        LOG << " Starting test." << std::endl;
        auto promise = std::promise<std::string>();
        auto future = promise.get_future();

        auto producer = std::thread([&promise] {
            LOG << " Producer started." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2U));
            promise.set_value("Hello World");
        });

        auto consumer = std::thread([&future] {
            LOG << " Consumer started." << std::endl;
            LOG << future.get();
        });

        LOG << " producer.join()" << std::endl;
        producer.detach();

        LOG << " consumer.join()" << std::endl;
        consumer.join();
    }
}

namespace Promise
{
    void Wait_Timeout()
    {
        std::promise<void> slow;
        std::future<void> will_timeout = slow.get_future();
        std::jthread job = std::jthread([promise = std::move(slow)] mutable {
            // Sleep, causing a timeout for the consumer
            std::this_thread::sleep_for(std::chrono::seconds(2U));
            promise.set_value();
        });

        /** Wait for 500 ms (which will timeout) **/
        if (will_timeout.wait_for(std::chrono::milliseconds(500U)) == std::future_status::timeout) {
            std::cout << std::format("Future did not receive state within 500 ms, bailing out.") << std::endl;
        } else {
            // If we didn't timeout, calling get() will not block (in general could also be a deferred function)
            will_timeout.get();
            std::cout <<   std::format("Future fulfilled.")<< std::endl;
        }
    }
}

void Promise::TEST_ALL()
{
    // Simple_Test_0();
    Simple_Test_1();
    // Simple_Test_2();
    // Simple_Test_3();
    // Simple_Test_4();
    // Simple_Test_5();


    // Wait_Timeout();

    // ComplexTest();

    // Promise_Threads_1();

    // Promise_Threads_2();

    // SetValueAtThreadExit();

    // Consumer_Producer();


    // SetException();
    // Propagate_Exception();
};
