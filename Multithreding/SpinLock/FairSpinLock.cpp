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
#include <syncstream>
#include <queue>

#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << utilities::datetime::getCurrentTime() << " "


namespace FairSpinLock
{

    class FairSpinLock
    {
        std::atomic<uint32_t> ticket {0 };  // Номер текущего билета
        std::atomic<uint32_t> serving {0 }; // Номер обслуживаемого потока

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

    struct SpinLock_Fast_NotFair
    {
        alignas(std::hardware_destructive_interference_size) std::atomic<uint32_t> flag {0 };
        alignas(std::hardware_destructive_interference_size) timespec ns {0, 1 };

        void lock()
        {
            for (uint8_t n = 0;flag.load(std::memory_order_relaxed) || flag.exchange(1, std::memory_order_acquire); ++n)
            {
                ns.tv_nsec = static_cast<int>(n);
                nanosleep(&ns, nullptr);
            }
        }

        void unlock() {
            flag.store(0, std::memory_order_release);
        }

        ~SpinLock_Fast_NotFair() {
            unlock();
        }
    };


    struct FairSpinLock2
    {
        static inline std::atomic<uint32_t> counter { 0 };
        static inline const thread_local uint32_t threadId = ++counter;

        std::atomic_flag flag {false};
        std::atomic_flag locked {false};
        std::queue<uint32_t> clients;
        std::atomic<uint32_t> nextOwner { counter  + 1};

        void lock()
        {
            while (flag.test_and_set(std::memory_order_acquire)) {
                std::this_thread::yield();
            }

            clients.push(threadId);
            flag.clear(std::memory_order_release);

            while (nextOwner.load(std::memory_order_relaxed) != threadId){
            }
            while (locked.test_and_set(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
        }

        void unlock()
        {
            while (flag.test_and_set(std::memory_order_acquire)) {
                std::this_thread::yield();
            }

            clients.pop();
            nextOwner.store(clients.front(), std::memory_order_relaxed);

            flag.clear(std::memory_order_release);
            locked.clear(std::memory_order_release);

        }
    };
}

namespace FairSpinLock::Tests
{
    constexpr int32_t numThreads = 8;
    constexpr int32_t iterCount = 1'000;

    void test1()
    {
        FairSpinLock spinLock;
        // SpinLock_Fast_NotFair spinLock;
        // FairSpinLock2 spinLock;

        uint32_t counterTotal = 0;

        auto increment = [&](const int times) {
            uint32_t counter = 0;
            for (int i = 0; i < times; ++i) {
                spinLock.lock();
                ++counter;
                ++counterTotal;
                if ((counter >= counterTotal + 8) || (counter <= counterTotal - 8))
                {
                    std::cout << "Err: total = " << counterTotal << ", counter = " << counter << std::endl;
                }
                spinLock.unlock();
            }
            std::osyncstream {std::cout} << std::this_thread::get_id() << " | counter = " << counter << std::endl;
        };

        std::vector<std::jthread> threads;
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(increment, iterCount);
        }
    }

    void test2()
    {
        FairSpinLock spinLock;
        // SpinLock_Fast_NotFair spinLock;
        // FairSpinLock2 spinLock;

        auto worker = [&](uint32_t timeoutNanos) {
            for (int i = 0; i < 10; ++i) {
                spinLock.lock();
                std::this_thread::sleep_for(std::chrono::nanoseconds (timeoutNanos));
                spinLock.unlock();
            }
            LOG  << std::endl;
        };

        std::vector<std::jthread> threads;
        threads.emplace_back(worker, 1'00);
        threads.emplace_back(worker, 1'000);
        threads.emplace_back(worker, 1'000'000);
        threads.emplace_back(worker, 250'000'000);
    }
}


void FairSpinLock::TestAll()
{
    // Tests::test1();
    Tests::test2();
}
