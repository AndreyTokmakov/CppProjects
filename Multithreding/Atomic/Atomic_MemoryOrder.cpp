/**============================================================================
Name        : Atomic_MemoryOrder.cpp
Created on  : 23.10.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Atomic_MemoryOrder.cpp
============================================================================**/

#include "Atomic_MemoryOrder.h"

#include <iostream>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <string_view>
#include <chrono>
#include <future>
#include <cassert>
#include <format>
#include <syncstream>

#include <print>
#include <format>
#include "DateTimeUtilities.hpp"

namespace
{
    using utilities::datetime::getCurrentTime;

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "
}


namespace Atomic_MemoryOrder::AtomicThreadFence
{
    std::string computation(int a) {
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return std::to_string(a);
    }

    std::atomic<int> vars[3] = {-1, -1, -1};
    std::string data[1000]; //non-atomic data

    // Thread A, compute 3 values.
    void ThreadA(int v0, int v1, int v2)
    {
        //  assert(0 <= v0, v1, v2 < 1000);
        data[v0] = computation(v0);
        data[v1] = computation(v1);
        data[v2] = computation(v2);

        std::atomic_thread_fence(std::memory_order_release);

        std::atomic_store_explicit(&vars[0], v0, std::memory_order_relaxed);
        std::atomic_store_explicit(&vars[1], v1, std::memory_order_relaxed);
        std::atomic_store_explicit(&vars[2], v2, std::memory_order_relaxed);
    }

    // Thread B, prints between 0 and 3 values already computed.
    void ThreadB()
    {
        int v0 = std::atomic_load_explicit(&vars[0], std::memory_order_relaxed);
        int v1 = std::atomic_load_explicit(&vars[1], std::memory_order_relaxed);
        int v2 = std::atomic_load_explicit(&vars[2], std::memory_order_relaxed);

        std::atomic_thread_fence(std::memory_order_acquire);

        //  v0, v1, v2 might turn out to be -1, some or all of them.
        //  Otherwise it is safe to read the non-atomic data because of the fences:
        if (v0 != -1)
            std::cout << data[v0] << std::endl;
        if (v1 != -1)
            std::cout << data[v1] << std::endl;
        if (v2 != -1)
            std::cout << data[v2] << std::endl;
    }

    void test()
    {
        std::jthread jobA {ThreadA, 1, 2, 3};
        std::jthread jobB {ThreadB};
    }
}

namespace Atomic_MemoryOrder::Consumer_Producer
{

    /**
     The producer sets shared_data then signals that the data is ready (or “publishes” it).
     The write to shared_data “happens before” the write to data_ready because the use of memory_order_release
     guarantees that the write to shared_data can not be reordered after data_ready.

     Meanwhile, the consumer busy waits for the signal on data_ready, then safely reads shared_data.
     The read from data_ready “happens before” the read from shared_data because the use of
     memory_order_acquire guarantees that the read from shared_data can not be reordered before data_ready.

     Things to note:

     1. shared_data itself need not be atomic. (This is particularly useful for data that can not be made atomic
     either because it’s too big or is not trivially copyable.)
     2. You can set any amount of data, then “publish” all of it simultaneously.
    */

    void WaitFor_AtomicInt()
    {
        int shared_data;
        std::atomic<int> data_ready { 0 };

        std::future<void> consumer = std::async( std::launch::async, [&]{
            while (data_ready.load( std::memory_order_acquire ) == 0 ) { /** **/ }
            LOG << shared_data << std::endl;
        });

        std::future<void> producer = std::async( std::launch::async, [&]{
            shared_data = 42;
            data_ready.store( 1, std::memory_order_release );
        });

        producer.wait();
        consumer.wait();
    }

    void WaitFor_AtomicFlag()
    {
        std::string data = "None";
        std::atomic_flag flag {true };

        std::future<void> consumer = std::async( std::launch::async, [&]
        {
            LOG << " Consumer: started\n";
            while (flag.test_and_set(std::memory_order_acquire)) { /** **/ }
            LOG << " Consumer: Data = " << data << std::endl;
        });

        std::future<void> producer = std::async( std::launch::async, [&]
        {
            LOG << " Producer: Sleeping\n";
            std::this_thread::sleep_for(std::chrono::seconds (1u));

            data = "Hello world!";

            flag.clear(std::memory_order_release);
        });

        producer.wait();
        consumer.wait();
    }
}

namespace Atomic_MemoryOrder::CompareExchange
{
    void Weak_Test_1()
    {
        std::atomic<int> variable {0};

        auto updater = [&variable]
        {
            std::this_thread::sleep_for(std::chrono::seconds (1u));
            LOG << " Updater : variable -> 1\n";
            variable.store(1, std::memory_order::release);
        };

        auto consumer = [&variable]()
        {
            constexpr int expected = 1, desired = 2;
            int actual = expected;

            // while (!order.compare_exchange_weak(expected, desired, std::memory_order_acquire, std::memory_order_relaxed))
            while (!variable.compare_exchange_weak(actual, desired, std::memory_order_acquire))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds (250u));
                LOG << " Consumer: False. actual: " << actual << std::endl;
                actual = expected;
            }

            LOG << " Consumer: True.  variable: " << variable << std::endl;
        };

        std::jthread a(updater), b(consumer);
    }

    void Strong_Test_1()
    {
        std::atomic<int> variable {0};

        auto updater = [&variable]
        {
            std::this_thread::sleep_for(std::chrono::seconds (1u));
            LOG << " Updater : variable -> 1\n";
            variable.store(1, std::memory_order::release);
        };

        auto consumer = [&variable]()
        {
            constexpr int expected = 1, desired = 2;
            int actual = expected;

            // while (!order.compare_exchange_weak(expected, desired, std::memory_order_acquire, std::memory_order_relaxed))
            while (!variable.compare_exchange_strong(actual, desired, std::memory_order_acquire))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds (250u));
                LOG
                                            << " Consumer: False. actual: " << actual << std::endl;
                actual = expected;
            }

            LOG << " Consumer: True.  variable: " << variable << std::endl;
        };

        std::jthread a(updater), b(consumer);
    }
}

namespace Atomic_MemoryOrder::SynchThreads
{
    std::atomic<int> order {0};

    void updater()
    {
        std::this_thread::sleep_for(std::chrono::seconds (5u));
        LOG << " Updater: order -> 1\n";
        order.store(1, std::memory_order::release);
    }

    void thread_1()
    {
        int expected = 1;
        // while (!order.compare_exchange_weak(expected, 2, std::memory_order_acquire, std::memory_order_relaxed))
        while (!order.compare_exchange_weak(expected, 2, std::memory_order_acquire))
        {
            std::this_thread::sleep_for(std::chrono::seconds (1u));
            LOG
                << " False. order: " << order << ", expected: " << expected << std::endl;
            expected = 1;
        }

        LOG << " Order: " << order << std::endl;
    }

    /*
    void thread_2()
    {
        int expected = 1;
        // memory_order_relaxed is okay because this is an RMW,
        // and RMWs (with any ordering) following a release form a release sequence
        while (!flag.compare_exchange_strong(expected, 2, std::memory_order_relaxed))
        {
            expected = 1;
        }
    }

    void thread_3()
    {
        while (flag.load(std::memory_order_acquire) < 2);
        // if we read the value 2 from the atomic flag, we see 42 in the vector
        assert(data.at(0) == 42); // will never fire
    }
    */

    void test()
    {
        std::jthread a(thread_1);
        std::jthread t2(updater);
    }
}


namespace Atomic_MemoryOrder::FailureCases
{
    void SetMultipleVariables()
    {
        constexpr int32_t iterCount { 10'000 };
        constexpr int32_t threadsMax { 32 };

        int32_t varOne {0};
        int32_t varTwo {0};
        int32_t varThree {0};
        std::atomic<int32_t> atomicVar {0};

        auto updater = [&] {
            for (size_t idx  = 0; idx < iterCount; ++idx)
            {
                ++varOne; ++varTwo; ++varThree;
                atomicVar.fetch_add(1, std::memory_order_release);
            }
        };

        auto validator = [&] {
            for (size_t idx  = 0; idx < iterCount; ++idx)
            {
                const int32_t val = atomicVar.fetch_add(1, std::memory_order_acquire);
                if (varOne != varTwo || varTwo != varThree || varThree != val)
                {
                    std::cerr << "ERROR: " << varOne << " - " << varTwo << " - " << varThree << " - " << val << std::endl;
                    return;
                }
            }
        };

        {
            std::vector<std::jthread> jobs;
            for (uint32_t t = 0; t < threadsMax / 2; ++t) {
                jobs.emplace_back(updater);
                jobs.emplace_back(validator);
            }
        }

        std::cout << "OK\n";
    }
}

namespace Atomic_MemoryOrder::Relaxed_Ordering_Missmatch_Tests
{
    void benchmark()
    {
        constexpr uint64_t maxCount { 10'000'000'000 };
        std::atomic<uint64_t> x = 0, y = 0;

        std::jthread t1{[&] {
            for (uint64_t i = 0; i < maxCount; ++i) {
                x.store(i, std::memory_order_relaxed);
                y.store(i, std::memory_order_relaxed);
            }
        }};

        std::jthread t2{[&] {
            int count_mismatch = 0;
            for (uint64_t i = 0; i < maxCount; ++i) {
                auto yy = y.load(std::memory_order_relaxed);
                auto xx = x.load(std::memory_order_relaxed);
                if (xx < yy) count_mismatch++;
            }
            std::print("Mismatch count: {}\n", count_mismatch);
        }};
    }
}


namespace Atomic_MemoryOrder::Fences
{
    void Demo_Release_Release()
    {
        std::atomic<int> x { 1 }, y { 2 };
        x.store( 2, std::memory_order_release );
        y.store( 1, std::memory_order_release );

        /**
         mov    DWORD PTR [rsp-0x8],0x1
         xor    eax,eax
         mov    DWORD PTR [rsp-0x4],0x2
         mov    DWORD PTR [rsp-0x8],0x2
         mov    DWORD PTR [rsp-0x4],0x1
         ret                               **/
    }

    void Demo_Relaxed_Release()
    {
        std::atomic<int> x { 1 }, y { 2 };
        x.store( 2, std::memory_order_relaxed );
        y.store( 1, std::memory_order_release );

        /** mov    DWORD PTR [rsp-0x4],0x2
            xor    eax,eax
            mov    DWORD PTR [rsp-0x8],0x2
            mov    DWORD PTR [rsp-0x4],0x1
            ret                               */
    }


    void Demo_Relaxed_Relaxed__Fence()
    {
        std::atomic<int> x { 1 }, y { 2 };
        x.store( 2, std::memory_order_relaxed );
        y.store( 1, std::memory_order_relaxed );
        std::atomic_thread_fence( std::memory_order_release );

        /** mov    DWORD PTR [rsp-0x8],0x2
            xor    eax,eax
            mov    DWORD PTR [rsp-0x4],0x1  */
    }
}

void Atomic_MemoryOrder::TestAll()
{
    // AtomicThreadFence::test();

    // Consumer_Producer::WaitFor_AtomicInt();
    // Consumer_Producer::WaitFor_AtomicFlag();

    // SynchThreads::test();

    // CompareExchange::Weak_Test_1();
    // CompareExchange::Strong_Test_1();

    // FailureCases::SetMultipleVariables();

    // Relaxed_Ordering_Missmatch_Tests::benchmark();

    Atomic_MemoryOrder::Fences::Demo_Release_Release();
    Atomic_MemoryOrder::Fences::Demo_Relaxed_Release();
    Atomic_MemoryOrder::Fences::Demo_Relaxed_Relaxed__Fence();

}