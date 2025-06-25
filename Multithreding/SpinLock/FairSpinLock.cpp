/**============================================================================
Name        : FairSpinLock.cpp
Created on  : 25.06.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FairSpinLock.cpp
============================================================================**/

#include "FairSpinLock.h"

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>


namespace FairSpinLock
{

    class FairSpinLock
    {
        std::atomic<uint32_t> ticket {0};  // Номер текущего билета
        std::atomic<uint32_t> serving {0}; // Номер обслуживаемого потока

    public:

        void lock()
        {
            uint32_t my_ticket = ticket.fetch_add(1, std::memory_order_relaxed);
            while (serving.load(std::memory_order_acquire) != my_ticket) {
                std::this_thread::yield(); // Уступить другим потокам, если очередь
            }
        }

        void unlock() {
            serving.fetch_add(1, std::memory_order_release);
        }
    };
}

void FairSpinLock::TestAll()
{
    FairSpinLock spinLock;
    int counter = 0;

    constexpr int num_threads = 8;
    constexpr int increments_per_thread = 100000;

    auto increment = [&](int times) {
        for (int i = 0; i < times; ++i) {
            spinLock.lock();
            ++counter;
            spinLock.unlock();
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(increment, increments_per_thread);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Counter: " << counter << std::endl;
}
