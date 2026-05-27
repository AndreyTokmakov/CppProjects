/**============================================================================
Name        : Semaphore.cpp
Created on  : 20.05.2021
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Semaphore
============================================================================**/


#include <iostream>
#include <format>
#include <thread>
#include <future>
#include <chrono>
#include <semaphore>
#include <syncstream>
#include <queue>
#include <latch>

#include "Semaphore.h"
#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << " "

using namespace std::literals;


namespace Semaphore::BinarySemaphore
{
    void Release_Acquire_SimpleExample()
    {
        int sharedData { 0 };
        std::binary_semaphore sem { 0 };

        const std::jthread consumer([&] {
            LOG << "Consumer: started" << std::endl;
            sem.acquire(); /**  Will block until producer call sem.release() **/
            LOG << sharedData << std::endl;
        });

        const std::jthread producer([&] {
            LOG << "Producer: started" << std::endl;
            sharedData = 123;
            std::this_thread::sleep_for(std::chrono::milliseconds (250U));

            LOG << "Producer: Update state and release" << std::endl;
            sem.release();
        });

        // Will print 123
    }
}

namespace Semaphore::BinarySemaphore
{
    void releaseAfterTimeout(std::binary_semaphore& semaphore,
                             const std::chrono::milliseconds& timeout)
    {
        semaphore.acquire();
        LOG << "Thread: Acquire semaphore. Sleeping for " << std::format("{} . . .\n", timeout);

        std::this_thread::sleep_for(timeout);

        LOG << "Thread: Release semaphore\n";
        semaphore.release();
    }

    void Release_Acquire_BasicTest()
    {
        std::binary_semaphore semaphore { 1 };
        const std::jthread worker(releaseAfterTimeout, std::ref(semaphore), 1s);

        std::this_thread::sleep_for(1ms);

        LOG << "Main  : Before acquire()\n";
        semaphore.acquire();
        LOG << "Main  : After  acquire()\n";

        /**
        2026-05-27 08:47:50.734291 Thread: Acquire semaphore. Sleeping for 1000ms . . .
        2026-05-27 08:47:50.735306 Main  : Before acquire()
        2026-05-27 08:47:51.734541 Thread: Release semaphore
        2026-05-27 08:47:51.734647 Main  : After  acquire()
        **/
    }

    void Release_TRY_Acquire__BasicTest()
    {
        std::binary_semaphore semaphore { 1 };
        const std::jthread worker(releaseAfterTimeout, std::ref(semaphore), 400ms);

        std::this_thread::sleep_for(1ms);

        bool acquired = semaphore.try_acquire();
        LOG << "Main  : Is semaphore acquired = " << std::boolalpha << acquired << std::endl;

        std::this_thread::sleep_for(500ms);

        acquired = semaphore.try_acquire();
        LOG << "Main  : Is semaphore acquired = " << std::boolalpha << acquired << std::endl;

        /**
        2026-05-27 08:43:16.081700 Thread: Acquire semaphore. Sleeping . . .
        2026-05-27 08:43:16.082721 Main  : Is semaphore acquired = false
        2026-05-27 08:43:16.481889 Thread: Release semaphore
        2026-05-27 08:43:16.582825 Main  : Is semaphore acquired = true
        **/
    }

    void Release_TRY_Acquire_FOR__BasicTest()
    {
        std::binary_semaphore semaphore { 1 };
        const std::jthread worker(releaseAfterTimeout, std::ref(semaphore), 2s);

        std::this_thread::sleep_for(1ms);
        while (!semaphore.try_acquire_for(250ms)) {
            LOG << "Main: Failed to acquire the semaphore\n";
        }
        LOG << "Main: Done\n";

        /**
        2026-05-27 07:34:03.929329 Thread: Acquire semaphore. Sleeping . . .
        2026-05-27 07:34:04.279477 Main: Failed to acquire the semaphore
        2026-05-27 07:34:04.529643 Main: Failed to acquire the semaphore
        2026-05-27 07:34:04.780104 Main: Failed to acquire the semaphore
        2026-05-27 07:34:05.030270 Main: Failed to acquire the semaphore
        2026-05-27 07:34:05.280728 Main: Failed to acquire the semaphore
        2026-05-27 07:34:05.530893 Main: Failed to acquire the semaphore
        2026-05-27 07:34:05.781074 Main: Failed to acquire the semaphore
        2026-05-27 07:34:05.929553 Thread: Release semaphore
        2026-05-27 07:34:05.929650 Main: Done
        **/
    }
};

namespace Semaphore::BinarySemaphore
{
    void Simple_Acquire_Release()
    {
        std::binary_semaphore semaphore {1};

        auto task = [&semaphore] {
            LOG << "Started\n";
            semaphore.acquire();
            LOG << "Semaphore captured...\n";

            std::this_thread::sleep_for(std::chrono::seconds(2U));

            LOG << "Semaphore released...\n";
            semaphore.release();
        };

        const std::jthread t1(task), t2(task);

        // [2025-10-23 22:18:08.698040] Thread [139956563613440] Started
        // [2025-10-23 22:18:08.698207] Thread [139956563613440] Semaphore captured...
        // [2025-10-23 22:18:08.698220] Thread [139956555220736] Started
        // [2025-10-23 22:18:10.698347] Thread [139956563613440] Semaphore released...
        // [2025-10-23 22:18:10.698476] Thread [139956555220736] Semaphore captured...
        // [2025-10-23 22:18:12.698618] Thread [139956555220736] Semaphore released...
    }

    void Semaphore_VS_ConditionalVariable()
    {
        const std::chrono::duration timeout = 1000ms;

        {
            std::binary_semaphore signal(0);
            auto t = std::jthread([&signal]() {
                LOG << std::this_thread::get_id() << " Waiting\n";

                // Wait until this thread is signaled
                signal.acquire();

                LOG << std::this_thread::get_id() << " Running\n";
            });

            // Injected wait to demonstrate correct ordering
            std::this_thread::sleep_for(timeout);

            LOG << std::this_thread::get_id() << " Before unblocking the thread.\n";

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
                LOG << std::this_thread::get_id() << " Waiting\n";

                // Wait until this thread is signaled
                std::unique_lock lock(mux);
                cond.wait(lock, [&received] { return received; });

                LOG << std::this_thread::get_id() << " Running\n";
            });

            // Injected wait to demonstrate correct ordering
            std::this_thread::sleep_for(timeout);
            LOG << std::this_thread::get_id() << " Before unblocking the thread.\n";
            { // Signal the thread to run
                const std::unique_lock lock(mux);
                received = true;
            }
            cond.notify_one();
        }
    }
}

namespace Semaphore::CountingSemaphore
{
    void BasicTest()
    {
        std::counting_semaphore<> semaphore {0};

        auto producer = [&]() {
            LOG << "Producer: Started" << '\n';
            std::this_thread::sleep_for(std::chrono::seconds(1U));
            LOG << "Producer: Releasing the semaphore" << '\n';
            semaphore.release();
            LOG << "Producer: Done" << '\n';
        };

        auto consumer = [&] {
            LOG << "Consumer: Started. Trying to acquire semaphore" << '\n';
            semaphore.acquire();
            LOG << "Consumer: Done" << '\n';
        };

        const std::jthread t1(producer), t2(consumer);

        // 2026-05-26 07:52:31.479132 Consumer: Started. Trying to acquire semaphore
        // 2026-05-26 07:52:31.479130 Producer: Started
        // 2026-05-26 07:52:32.479373 Producer: Releasing the semaphore
        // 2026-05-26 07:52:32.479418 Producer: Done
        // 2026-05-26 07:52:32.479437 Consumer: Done
    }
}

namespace Semaphore::CountingSemaphore
{
    void Basic_LimitNumberOfRunningTasks()
    {
        constexpr uint32_t tasksToRun { 16 }, maxParallel { 4 };

        std::atomic<uint32_t> runningTasks { 0 };
        std::counting_semaphore<maxParallel> semaphore {maxParallel};
        std::vector<std::jthread> workers;

        for (uint32_t n = 0; n < tasksToRun; ++n) {
            workers.emplace_back([&semaphore, &runningTasks] {
                LOG << "Worker: Started and waiting....." << std::endl;
                semaphore.acquire();
                const uint32_t num = runningTasks.fetch_add(1, std::memory_order_release) + 1;

                LOG << "Worker: Executing (Active tasks: " << num << ")\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(250U));

                runningTasks.fetch_sub(1, std::memory_order_release);
                semaphore.release();
                LOG << "Worker: Done" << std::endl;
            });
        }

        workers.clear();
        LOG << "Done: Active tasks: " << runningTasks << "\n";
    }
}

namespace Semaphore::CountingSemaphore
{
    void NegativeInitialValue()
    {
        std::counting_semaphore<> semaphore { -1 };
        std::vector<std::jthread> workers;

        workers.emplace_back([&semaphore] {
            LOG << "Producer: Started" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1U));
            LOG << "Producer: Release semaphore" << std::endl;
            semaphore.release();
            LOG << "Producer: Done" << std::endl;
        });

        workers.emplace_back([&semaphore] {
            LOG << "Consumer: Started and waiting....." << std::endl;
            semaphore.acquire();
            LOG << "Consumer: Done" << std::endl;
        });
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
            std::this_thread::sleep_for(std::chrono::seconds(2U));

            LOG << "Producer: Data prepared." << '\n';
            prepareSignal.release();
        };

        auto consumer = [&] {
            LOG << "Consumer: Waiting for data." << '\n';
            prepareSignal.acquire();
            myVec[2] = 2;
            LOG << "Consumer: Complete the work." << '\n';

            for (auto i: myVec)
                LOG << i << " ";
            LOG << '\n';
        };

        const std::jthread t1(producer), t2(consumer);
    }
};


namespace Semaphore::CountingSemaphore
{
    struct Data {};

    // Simple unbounded thread-safe many<->many producer/consumer queue
    class  WorkQueue
    {
        std::deque<Data> queue;
        std::mutex mutex;
        std::counting_semaphore<> semaphore {0};

    public:

        void push(std::convertible_to<Data> auto&& data)
        {   // Push a new element into the queue
            {
                const std::lock_guard lock {mutex};
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
            const std::lock_guard lock {mutex};
            const Data result = std::move(queue.front());
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

            LOG << "Producer: publishing data" << std::endl;
            q.push(Data{});

            std::this_thread::sleep_for(200ms);

            for (int i = 0; i < 2; ++i) {
                LOG<< "Producer: publishing data" << std::endl;
                q.push(Data{});
            }
        }};

        auto consumer = [&q](const std::string& name)
        {
            for (int i = 0; i < 2; ++i) {
                LOG << name << ": attempting to read data" << std::endl;
                auto _ = q.pop();
                LOG << name << ": succeeded in reading data" << std::endl;
            }
        };

        const std::jthread consumer1 {consumer, "Consumer1"},
                           consumer2 {consumer, "Consumer2"};
    }
};


namespace Semaphore::CountingSemaphore
{
    void WaitFor_N_InitialEvents()
    {
        std::counting_semaphore<5> semaphore { 0 };
        std::vector<std::jthread> workers;

        workers.emplace_back([&semaphore] {
            for (int i = 1; i <= 5; ++i) {
                LOG << "Producer: Release semaphore (Task " << i << " executed)" << std::endl;
                //semaphore.release();
                std::this_thread::sleep_for(std::chrono::milliseconds(250U));
            }

            semaphore.release();
            LOG << "Producer: Done" << std::endl;
        });

        workers.emplace_back([&semaphore] {
            LOG << "Consumer: Started" << std::endl;
            semaphore.acquire();
            LOG << "Consumer: Done" << std::endl;
        });

    }
}

namespace Semaphore::Consumer_Producer
{
    struct SharedResource
    {
        static constexpr int BUFFER_SIZE { 10 };
        std::array<int, BUFFER_SIZE> data {};

        // Semaphores to control the execution flow between producer and consumer
        std::binary_semaphore producer { 1 };
        std::binary_semaphore consumer { 0 };

        // Latch to ensure main waits for both threads (producer and consumer) to complete before proceeding
        std::latch done{2};
    };

    void consumeData(SharedResource& resource)
    {
        for (int i = 0; i < SharedResource::BUFFER_SIZE; ++i)
        {
            // Wait for the producer to signal that data is ready
            resource.consumer.acquire();

            LOG << "Consumer Reads:  " << resource.data[i] << '\n';
            std::this_thread::sleep_for(std::chrono::milliseconds(5U));

            // Signal the producer to continue production
            resource.producer.release();
        }
        // Signal completion of the consumer thread to the latch
        resource.done.count_down();
    }

    void produceData(SharedResource& resource)
    {
        for (int i = 0; i < SharedResource::BUFFER_SIZE; ++i)
        {
            // Wait for the consumer to signal readiness
            resource.producer.acquire();
            resource.data[i] = i;

            LOG << "Producer writes: " << resource.data[i] << '\n';
            std::this_thread::sleep_for(std::chrono::milliseconds(10U));

            // Signal the consumer that data is ready
            resource.consumer.release();
        }
        // Signal completion of the producer thread to the latch
        resource.done.count_down();
    }

    void runDemo()
    {
        SharedResource resource;
        const std::jthread producer(produceData, std::ref(resource)),
                           consumer(consumeData, std::ref(resource));

        // Main thread waits here until both threads signal completion via the latch
        resource.done.wait();

        // Output the data processed by threads
        for (int const value : resource.data) {
            LOG << value << " ";
        }
    }
}



void Semaphore::TestAll()
{
    // BinarySemaphore::Release_Acquire_SimpleExample();

    BinarySemaphore::Release_Acquire_BasicTest();
    // BinarySemaphore::Release_TRY_Acquire__BasicTest();
    // BinarySemaphore::Release_TRY_Acquire_FOR__BasicTest();

    // BinarySemaphore::Simple_Acquire_Release();
    // BinarySemaphore::Semaphore_VS_ConditionalVariable();

    // CountingSemaphore::BasicTest();
    // CountingSemaphore::Basic_LimitNumberOfRunningTasks();
    // CountingSemaphore::NegativeInitialValue();
    // CountingSemaphore::Producer_Consumer();
    // CountingSemaphore::WorkQueue_Demo();
    // CountingSemaphore::WaitFor_N_InitialEvents();

    // Consumer_Producer::runDemo();
};

