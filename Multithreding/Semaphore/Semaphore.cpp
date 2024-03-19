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
#include <queue>


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

namespace Semaphore::CountingSemaphore
{
    std::string time() {
        return std::format("{:%d-%m-%Y %H:%M:%OS} ", std::chrono::system_clock::now());
    }

    void BasicTest()
    {
        std::counting_semaphore<> semaphore {0};

        auto producer = [&]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::osyncstream(std::cout) << time() << "Producer: releasing the semaphore" << '\n';
            semaphore.release();
        };

        auto consumer = [&] {
            std::osyncstream(std::cout) << time() << "Consumer: trying to acquire semaphore" << '\n';
            // Try to atomically decrease the counter in the semaphore. If the counter is already 0, blocks.
            semaphore.acquire();
            std::osyncstream(std::cout) << time() << "Consumer: Done" << '\n';
        };

        std::jthread t1(producer), t2(consumer);
    }
}

namespace Semaphore::CountingSemaphore
{
    void Producer_Consumer()
    {
        std::vector<int> myVec{};
        std::counting_semaphore<1> prepareSignal(0);

        auto producer = [&]() {
            myVec.insert(myVec.end(), {0, 1, 0, 3});

            std::this_thread::sleep_for(std::chrono::seconds(2));

            std::osyncstream(std::cout) << "Producer: Data prepared." << '\n';
            prepareSignal.release();
        };

        auto consumer = [&] {
            std::osyncstream(std::cout) << "Consumer: Waiting for data." << '\n';
            prepareSignal.acquire();
            myVec[2] = 2;
            std::osyncstream(std::cout) << "Consumer: Complete the work." << '\n';

            for (auto i: myVec)
                std::osyncstream(std::cout) << i << " ";
            std::osyncstream(std::cout) << '\n';
        };

        std::jthread t1(producer), t2(consumer);
    }
};


namespace Semaphore::CountingSemaphore
{
    struct Data {};

    // Simple unbounded thread-safe many<->many producer/consumer queue
    struct WorkQueue
    {
        std::deque<Data> queue {};
        std::mutex mutex {};
        std::counting_semaphore<> semaphore {0};

        void push(std::convertible_to<Data> auto&& data)
        {   // Push a new element into the queue
            {
                std::lock_guard lock {mutex};
                queue.push_back(std::forward<decltype(data)>(data));
            }

            // Atomically increase the counter in the semaphore.
            // If any threads are blocked on acquire, they will be notified.
            semaphore.release();
        }

        Data pop()
        {   // Try to atomically decrease the counter in the semaphore. If the counter is already 0, blocks.
            semaphore.acquire();

            // At this point we are guaranteed available data, still need to synchronize against other consumers.
            std::lock_guard lock {mutex};
            Data result = std::move(queue.front());
            queue.pop_front();
            return result;
        }
    };

    void WorkQueue_Demo()
    {
        WorkQueue q;

        auto producer = std::jthread{[&q]
        {
            std::this_thread::sleep_for(200ms);

            std::cout << "Producer: publishing data" << std::endl;
            q.push(Data{});

            std::this_thread::sleep_for(200ms);

            for (int i = 0; i < 2; ++i)
            {
                std::cout << "Producer: publishing data" << std::endl;
                q.push(Data{});
            }
        }};

        auto consumer = [&q](const std::string& name)
        {
            for (int i = 0; i < 2; ++i)
            {
                std::cout << name << ": attempting to read data" << std::endl;
                auto _ = q.pop();
                std::cout << name << ": succeeded in reading data" << std::endl;
            }
        };

        std::jthread consumer1 {consumer, "Consumer1"},
                     consumer2 {consumer, "Consumer2"};
    }
}


void Semaphore::TEST_ALL()
{
    // BinarySemaphore::Release_Acquire_BasicTest();
    // BinarySemaphore::Release_TRY_Acquire__BasicTest();
    // BinarySemaphore::Release_TRY_Acquire_FOR__BasicTest();

    // BinarySemaphore::Simple_Acquire_Release();
    // BinarySemaphore::Semaphore_VS_ConditionalVariable();

    CountingSemaphore::BasicTest();
    // CountingSemaphore::Producer_Consumer();
    // CountingSemaphore::WorkQueue_Demo();
};

