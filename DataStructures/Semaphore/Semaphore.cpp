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
#include <vector>
#include <condition_variable>

namespace semaphore
{
    class Semaphore
    {
        std::mutex mtx;
        std::condition_variable cv;
        int32_t count { 0 };

    public:

        explicit Semaphore(const int32_t c): count { c } {
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
        using size_type = int32_t;

        std::atomic<size_type> count { 0 };
        const size_type threshold { 0 };

        std::mutex mtx;
        std::condition_variable cv;

    public:

        explicit Semaphore(const size_type initial):
            count { initial }, threshold { initial }
        {
            if (0 == initial) {
                throw std::invalid_argument("negative semaphore");
            }
        }

        void acquire()
        {
            for (size_type old = count.load(std::memory_order_relaxed); 0 < old; /**/)
            {
                if (count.compare_exchange_weak(old, old - 1,
                    std::memory_order_acquire,
                    std::memory_order_relaxed)) {
                    return; // ---> Was able to decrement 'count' ---> OK ---> exit
                }
            }

            // Slow path
            std::unique_lock lock(mtx);
            cv.wait(lock, [&] {
                return count.load(std::memory_order_relaxed) > 0;
            });

            // Guaranteed > 0
            count.fetch_sub(1, std::memory_order_acquire);
        }

        void release()
        {
            // Fast path
            for (size_type old = count.load(std::memory_order_relaxed); /**/; /**/)
            {
                if (old >= threshold) {
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

    template<ISemaphore SemT>
    void test_initial_zero_blocks()
    {
        SemT sem(0);
        std::atomic<bool> acquired{false};

        std::thread t([&] {
            sem.acquire();
            acquired = true;
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (acquired.load()) {
            std::cerr << "Semaphore did not block when initialized with 0\n";
        }
        sem.release();
        t.join();
    }

    template<ISemaphore SemT>
    void test_release_unblocks_exactly_one()
    {
        SemT sem(0);
        std::atomic<int> counter{0};

        auto worker = [&] {
            sem.acquire();
            counter.fetch_add(1);
        };

        std::jthread t1(worker), t2(worker);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        sem.release();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (const int value = counter.load(); value != 1) {
            std::cerr << "notify_one woke more than one thread\n";
        }

        sem.release();
    }

    template<ISemaphore SemT>
    void test_no_overflow()
    {
        SemT sem(2);

        sem.release();
        sem.release();

        std::atomic<int> counter{0};
        auto worker = [&] {
            sem.acquire();
            counter.fetch_add(1);
        };

        std::vector<std::jthread> tasks;
        for (int i = 0; i < 3; ++i) {
            tasks.emplace_back(worker);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (counter.load() != 2)
            std::cerr << "Semaphore overflow detected\n";

        sem.release();
        tasks.clear();
    }


}

void semaphore::TestAll()
{
    // unit_tests::test_semaphore_basic<Semaphore>();
    // unit_tests::test_initial_zero_blocks<Semaphore>();
    // unit_tests::test_release_unblocks_exactly_one<Semaphore>();
    unit_tests::test_no_overflow<Semaphore>();

    // unit_tests::test_semaphore_basic<semaphore_fast::Semaphore>();

}
