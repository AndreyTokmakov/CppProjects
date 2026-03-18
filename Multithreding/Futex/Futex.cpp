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

namespace Futex_1
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

namespace Futex_2
{
    class Futex
    {
        using Ty = uint32_t;

        constexpr static Ty Unlocked { 0U };
        constexpr static Ty Locked { 1U };

        Ty isLocked { Unlocked } ;
        std::atomic_ref<Ty> isLockedARef { isLocked };

    public:

        void lock()
        {
            ::syscall(SYS_futex, &isLocked, FUTEX_WAIT, Locked, nullptr, nullptr, 0);
            isLockedARef.store(Locked, std::memory_order_release);
        }

        void unlock()
        {
            isLockedARef.store(Unlocked, std::memory_order_release);
            ::syscall(SYS_futex, &isLocked, FUTEX_WAKE, Locked, nullptr, nullptr, 0);
        }
    };
}

namespace Futex_3
{
    class Futex
    {
        using Ty = uint32_t;

        constexpr static Ty Unlocked { 0U };
        constexpr static Ty Locked { 0U };

        Ty isLocked { Unlocked } ;
        std::atomic_ref<Ty> isLockedARef { isLocked };

    public:

        void lock()
        {
            while (Unlocked != isLockedARef.exchange(Locked, std::memory_order_acquire)) {
                ::syscall(SYS_futex, &isLocked, FUTEX_WAIT_PRIVATE, Locked, nullptr);
            }
        }

        void unlock()
        {
            isLockedARef.store(Unlocked, std::memory_order_release);
            ::syscall(SYS_futex, &isLocked, FUTEX_WAIT_PRIVATE, 1);
        }
    };
}

void Futex::TestAll()
{
    auto test = [&]<typename FtxType> () {
        FtxType ftxLock;
        uint64_t counter = 0;

        auto task = [&ftxLock, &counter]() {
            for (int i = 0; i < 1'000'000; ++i) {
                ftxLock.lock();
                ++counter;
                ftxLock.unlock();
            }
        };

        {
            std::jthread t1(task), t2(task);
        }
        std::cout << counter << "\n";
    };


    test.operator()<Futex_1::FutexMutex>();
    test.operator()<Futex_2::Futex>();
    test.operator()<Futex_3::Futex>();

    // 2000000
    // 1999667
    // 1999764
}
