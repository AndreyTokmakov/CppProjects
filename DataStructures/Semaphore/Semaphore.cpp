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

#include "DateTimeUtilities.hpp"
#include "Testing.hpp"

#define LOG  std::osyncstream { std::cout } << utilities::datetime::getCurrentTime() << " "


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


namespace semaphore_atomic
{
    struct Semaphore
    {
        using counter_t = uint32_t;

        explicit Semaphore(const counter_t n) : count { n }{
        }

        void acquire()
        {
            counter_t old = count.load(std::memory_order_relaxed);
            while (true)
            {
                while (old == 0)
                {
                    count.wait(0, std::memory_order_relaxed);
                    old = count.load(std::memory_order_relaxed);
                }
                if (count.compare_exchange_weak(old, old - 1,
                                                std::memory_order_acquire,
                                                std::memory_order_relaxed)) {
                    return;
                }
            }
        }

        void release()
        {
            count.fetch_add(1, std::memory_order_release);
            count.notify_one();
        }

    private:

        std::atomic<counter_t> count;
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
            utilities::testing::AssertEqual( 1, counter.load() , "Semaphore failed.");
        }

        t1.join(); t2.join();
        utilities::testing::AssertEqual( 0, counter.load() , "Semaphore failed.");
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
        utilities::testing::AssertTrue( 0 == acquired.load() , "Semaphore did not block when initialized with ");
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

        utilities::testing::AssertEqual(1, counter.load(), "notify_one woke more than one thread");
        sem.release();
    }

    template<ISemaphore SemT>
    void max_simultaneous_task(const uint32_t maxConcurrent = 3,
                               const uint32_t totalThreads = 10)
    {
        SemT sem(maxConcurrent);
        std::atomic<uint32_t> current{0}, max_seen{0};
        std::vector<std::jthread> threads;
        for (uint32_t i = 0; i < totalThreads; ++i) {
            threads.emplace_back([&]() {
                sem.acquire();
                const uint32_t now = ++current;

                max_seen.store(std::max(max_seen.load(), now));
                utilities::testing::AssertFalse(now > maxConcurrent, std::format("Too many threads: {}", now));
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                --current;
                sem.release();
            });
        }

        for (std::jthread& task : threads) {
            task.join();
        }
        utilities::testing::AssertTrue(maxConcurrent >= max_seen.load(),
            std::format("Max concurrent more than expected {}", maxConcurrent ));
    }


    template<ISemaphore SemT>
    void testSemaphore()
    {
        unit_tests::test_semaphore_basic<SemT>();
        unit_tests::test_initial_zero_blocks<SemT>();
        unit_tests::test_release_unblocks_exactly_one<SemT>();

        unit_tests::max_simultaneous_task<SemT>();
        unit_tests::max_simultaneous_task<SemT>(5, 8);
        unit_tests::max_simultaneous_task<SemT>(4, 12);
    }

    void test_all()
    {
        testSemaphore<semaphore::Semaphore>();
        testSemaphore<semaphore_atomic::Semaphore>();
        testSemaphore<std::counting_semaphore<>>();
    }

}

void semaphore::TestAll()
{
    unit_tests::test_all();
}
