/**============================================================================
Name        : Futex.cpp
Created on  : 25.01.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Futex.cpp
============================================================================**/

#include "Futex.hpp"

#include <iostream>
#include <atomic>
#include <thread>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <climits>

namespace
{
    int64_t futex_wait(std::atomic<int>* addr, const int expected) {
        return syscall(
            SYS_futex,
            reinterpret_cast<int*>(addr),
            FUTEX_WAIT,
            expected,
            nullptr,   // timeout
            nullptr,
            0
        );
    }

    int64_t futex_wake(std::atomic<int>* addr, const int count) {
        return syscall(
            SYS_futex,
            reinterpret_cast<int*>(addr),
            FUTEX_WAKE,
            count,
            nullptr,
            nullptr,
            0
        );
    }
}

namespace Futex
{
    class FutexMutex
    {
        std::atomic<int> state { 0 };

    public:

        void lock()
        {
            int expected = 0;
            if (state.compare_exchange_strong(expected, 1,std::memory_order_acquire)) {
                return; // fast path
            }

            // slow path
            while (true)
            {
                expected = 1;
                if (state.compare_exchange_strong(expected, 2,std::memory_order_acquire)) {
                    futex_wait(&state, 2);
                }

                expected = 0;
                if (state.compare_exchange_strong(expected, 2,std::memory_order_acquire)) {
                    return;
                }
            }
        }

        void unlock()
        {
            if (state.fetch_sub(1, std::memory_order_release) != 1) {
                state.store(0, std::memory_order_release);
                futex_wake(&state, 1);
            }
        }
    };

}


void Futex::TestAll()
{
    FutexMutex m;
    int counter = 0;

    auto worker = [&]() {
        for (int i = 0; i < 1'000'000; ++i) {
            m.lock();
            ++counter;
            m.unlock();
        }
    };

    {
        std::jthread t1(worker);
        std::jthread t2(worker);
    }

    std::cout << counter << "\n";
}
