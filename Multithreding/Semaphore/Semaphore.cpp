//============================================================================
// Name        : Semaphore.cpp
// Created on  : 20.05.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Threads src class
//============================================================================

#include "../ThreadHelperUtilities/ThreadHelperUtilities.h"
#include "Semaphore.h"

#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <semaphore>
#include <syncstream>


using namespace std::literals;


namespace Semaphore::BinarySemaphore {

    std::binary_semaphore semaphoreOne {0};
    std::binary_semaphore semaphoreTwo {0};

    void thread_proc(int timeout = 3) {
        // wait for a signal from the main proc  by attempting to decrement the semaphore
        semaphoreOne.acquire();
        THREAD_INFO << "Got signal from 'semaphoreOne'\n";

        // this call blocks until the semaphore's count is increased from the main proc
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        THREAD_INFO << "Send the signal\n"; // message

        // signal the main proc back
        semaphoreTwo.release();
    }

    void Release_Acquire_BasicTest() {
        THREAD_INFO << "Starting main thread\n";
        std::thread thrWorker(thread_proc, 3);

        std::this_thread::sleep_for(1s);
        THREAD_INFO << "Send the signal\n";

        // signal the worker thread to start working by increasing the semaphore's count
        semaphoreOne.release();

        // wait until the worker thread is done doing the work by attempting to decrement the semaphore's count
        THREAD_INFO << "Before acquire()...\n";
        semaphoreTwo.acquire();
        THREAD_INFO << "After acquire()...\n";

        thrWorker.join();
        THREAD_INFO << "Done\n";
    }

    //=====================================================================//

    void Release_TRY_Acquire__BasicTest() {
        THREAD_INFO << "Starting main thread\n";
        std::thread thrWorker(thread_proc, 3);

        std::this_thread::sleep_for(1s);
        THREAD_INFO << "Send the signal\n";

        semaphoreOne.release(); // signal the worker thread to start working by increasing the semaphore's count
        THREAD_INFO << "Before try_acquire()...\n";

        [[maybe_unused]]
        auto x = semaphoreTwo.try_acquire();
        THREAD_INFO << "After try_acquire()...\n";

        thrWorker.join();
        THREAD_INFO << "Done\n";
    }


    void Release_TRY_Acquire_FOR__BasicTest() {
        THREAD_INFO << "Starting main thread\n";
        std::thread thrWorker(thread_proc, 5);

        std::this_thread::sleep_for(1s);
        THREAD_INFO << "Send the signal\n";

        semaphoreOne.release(); // signal the worker thread to start working by increasing the semaphore's count
        THREAD_INFO << "Before try_acquire()...\n";

        while (!semaphoreTwo.try_acquire_for(500ms)) {
            THREAD_INFO << "   Failed to acquire the semaphoreTwo\n";
        }
        THREAD_INFO << "After try_acquire()...\n";


        thrWorker.join();
        THREAD_INFO << "Done\n";
    }
};

namespace Semaphore::BinarySemaphore
{
    void Simple_Acquire_Release()
    {
        std::binary_semaphore semaphore {1};

        auto task = [&semaphore] {
            THREAD_INFO << "Started\n";
            semaphore.acquire();
            THREAD_INFO << "Semaphore captured...\n";

            std::this_thread::sleep_for(std::chrono::seconds(2));

            THREAD_INFO << "Semaphore released...\n";
            semaphore.release();
        };

        std::future<void> job1 = std::async(task);
        std::future<void> job2 = std::async(task);

        job1.wait();
        job2.wait();
    }

    void Semaphore_VS_ConditionalVariable()
    {
        const std::chrono::duration timeout = 1000ms;

        {
            std::binary_semaphore signal(0);
            auto t = std::jthread([&signal]() {
                std::osyncstream(std::cout) << std::this_thread::get_id() << " Waiting\n";

                // Wait until this thread is signaled
                signal.acquire();

                std::osyncstream(std::cout) << std::this_thread::get_id() << " Running\n";
            });

            // Injected wait to demonstrate correct ordering
            std::this_thread::sleep_for(timeout);

            std::osyncstream(std::cout) << std::this_thread::get_id() << " Before unblocking the thread.\n";

            // Signal the thread to run
            signal.release();
        }

        std::cout << "\n\n";

        {
            // Example of how this would look with a condition variable:
            std::mutex mux;
            std::condition_variable cond;
            bool received = false;

            auto t = std::jthread([&mux, &cond, &received]() {
                std::osyncstream(std::cout) << std::this_thread::get_id() << " Waiting\n";

                // Wait until this thread is signaled
                std::unique_lock lock(mux);
                cond.wait(lock, [&received] { return received; });

                std::osyncstream(std::cout) << std::this_thread::get_id() << " Running\n";
            });

            // Injected wait to demonstrate correct ordering
            std::this_thread::sleep_for(timeout);
            std::osyncstream(std::cout) << std::this_thread::get_id() << " Before unblocking the thread.\n";
            { // Signal the thread to run
                std::unique_lock lock(mux);
                received = true;
            }
            cond.notify_one();
        }
    }
}


void Semaphore::TEST_ALL()
{
    // BinarySemaphore::Release_Acquire_BasicTest();
    // BinarySemaphore::Release_TRY_Acquire__BasicTest();
    // BinarySemaphore::Release_TRY_Acquire_FOR__BasicTest();

    // BinarySemaphore::Simple_Acquire_Release();
    BinarySemaphore::Semaphore_VS_ConditionalVariable();
};

