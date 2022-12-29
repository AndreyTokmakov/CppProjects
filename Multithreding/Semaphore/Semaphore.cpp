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
#include <chrono>
#include <semaphore>

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


void Semaphore::TEST_ALL()
{
    // BinarySemaphore::Release_Acquire_BasicTest();
    // BinarySemaphore::Release_TRY_Acquire__BasicTest();
    BinarySemaphore::Release_TRY_Acquire_FOR__BasicTest();
};

