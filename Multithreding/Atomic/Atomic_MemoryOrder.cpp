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

namespace
{
    std::string timeString()
    {
        std::string buffer;
        buffer.reserve(32);
        std::format_to(std::back_inserter(buffer), "{:%Y-%m-%d %H:%M:%OS}", std::chrono::system_clock::now());
        buffer.shrink_to_fit();
        return buffer;
    }
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
            std::osyncstream{std::cout} << shared_data << std::endl;
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
            std::osyncstream{std::cout} << timeString() << " Consumer: started\n";
            while (flag.test_and_set(std::memory_order_acquire)) { /** **/ }
            std::osyncstream{std::cout} << timeString() << " Consumer: Data = " << data << std::endl;
        });

        std::future<void> producer = std::async( std::launch::async, [&]
        {
            std::osyncstream{std::cout} << timeString() << " Producer: Sleeping\n";
            std::this_thread::sleep_for(std::chrono::seconds (1));

            data = "Hello world!";

            flag.clear(std::memory_order_release);
        });

        producer.wait();
        consumer.wait();
    }
}


void Atomic_MemoryOrder::TestAll()
{
    // AtomicThreadFence::test();

    // Consumer_Producer::WaitFor_AtomicInt();
    Consumer_Producer::WaitFor_AtomicFlag();

}