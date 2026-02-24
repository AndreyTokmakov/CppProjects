/**============================================================================
Name        : Semaphore.cpp
Created on  : 23.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Semaphore.cpp
============================================================================**/

#include "Semaphore.hpp"

#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace semaphore
{
    class Semaphore
    {
        std::mutex mtx;
        std::condition_variable cv;
        const int32_t threshold { 0 };
        int32_t count { 0 };

    public:

        explicit Semaphore(const int32_t c): threshold {c}, count { c } {
        }

        void acquire()
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]{ return count > 0; });
            --count;
        }

        void release()
        {
            {
                std::unique_lock<std::mutex> lock(mtx);
                if (count >= threshold)
                    return;
                ++count;
            }
            cv.notify_one();
        }
    };
}

namespace semaphore_fast
{
    class Semaphore
    {
        using size_type = uint32_t;

        std::atomic<size_type> count { 0 };
        const size_type maxCount { 0 };

        std::mutex mtx;
        std::condition_variable cv;

    public:

        explicit Semaphore(const size_type initial):
            count { initial }, maxCount { initial }
        {
            if (0 == initial) {
                throw std::invalid_argument("negative semaphore");
            }
        }

        void acquire()
        {
            size_type old = count.load(std::memory_order_relaxed);
            while (old > 0)
            {
                if (count.compare_exchange_weak(old, old - 1,
                    std::memory_order_acquire,
                    std::memory_order_relaxed)) {
                    return; // успех без блокировки
                }
            }

            // Slow path
            std::unique_lock lock(mtx);

            cv.wait(lock, [&] {
                const size_type value = count.load(std::memory_order_relaxed);
                return value > 0;
            });

            // Guaranteed > 0
            count.fetch_sub(1, std::memory_order_acquire);
        }

        void release()
        {
            // Fast path
            size_type old = count.load(std::memory_order_relaxed);

            while (true)
            {
                if (old >= maxCount) {
                    return; // bounded
                }
                if (count.compare_exchange_weak(old, old + 1,
                        std::memory_order_release,
                        std::memory_order_relaxed)) {
                    break;
                }
            }

            // Slow path wake
            std::lock_guard lock(mtx);
            cv.notify_one();
        }
    };

}


namespace unit_tests
{
    template<typename T>
    concept ISemaphore = std::constructible_from<T, int> &&
        requires(T sem, int value) {
            { sem.acquire() } -> std::same_as<void>;
            { sem.release() } -> std::same_as<void>;
    };

    template<ISemaphore SemT>
    void test_semaphore_basic()
    {
        SemT sem(1);
        std::atomic<int> counter{0};

        auto worker = [&] {
            sem.acquire();
            counter.fetch_add(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            counter.fetch_sub(1);
            sem.release();
        };

        std::jthread t1(worker), t2(worker);

        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (1 != counter.load() ) {
                std::cerr << "Semaphore failed. Test - 1\n";
            }
        }
        {
            t1.join(); t2.join();
            if (0 != counter.load() ) {
                std::cerr << "Semaphore failed. Test - 2\n";
            }
        }
    }

}

void semaphore::TestAll()
{
    unit_tests::test_semaphore_basic<Semaphore>();
    unit_tests::test_semaphore_basic<semaphore_fast::Semaphore>();


}
