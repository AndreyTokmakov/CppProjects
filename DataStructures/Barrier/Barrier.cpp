/**============================================================================
Name        : Barrier.cpp
Created on  : 23.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Barrier.cpp
============================================================================**/

#include "Barrier.hpp"

#include <iostream>
#include <string_view>
#include <vector>
#include <thread>
#include <functional>

#include <syncstream>
#include <mutex>
#include <condition_variable>

#include "DateTimeUtilities.hpp"
#include "Random.hpp"

#define LOG  std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << " "

namespace barrier
{
    struct Barrier
    {
        using size_type = std::size_t;

        explicit Barrier(const size_type num) : threshold { num }, count { num } {
        }

        Barrier(const size_type num, std::function<void()>&& callback) :
            threshold { num }, count { num }, callback { std::move(callback) } {
        }

        void arrive_and_wait()
        {
            std::unique_lock<std::mutex> lock(mtx);
            const size_t gen = generation;

            if (--count == 0)
            {
                generation++;
                count = threshold;
                lock.unlock();

                if (const auto cbCopy = callback; cbCopy) {
                    cbCopy(); // TODO: Try-catch --> terminate
                }
                reached.notify_all();
            }
            else
            {
                reached.wait(lock, [this, gen] {
                    return gen != generation;
                });
            }
        }

        void wait()
        {
            std::unique_lock lock { mtx };
            const size_t gen = generation;
            reached.wait(lock, [&, gen] {
                return generation != gen;
            });
        }

    private:
        std::mutex mtx;
        std::condition_variable reached;


        const size_type threshold { 0 };
        alignas(64) size_type count { 0 };
        size_type generation { 0 };
        std::function<void()> callback {};
    };
}


namespace barrier::tests
{
    void SimpleTest()
    {
        constexpr size_t threadsCount {3};
        Barrier barrier(threadsCount);

        auto task = [&](std::string_view name, const uint32_t timeout) {
            LOG << name  <<  " started\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            LOG << name  <<  " completed in " << timeout << " seconds. Waiting for others\n";
            barrier.arrive_and_wait();
            LOG << name  <<  " done\n";
        };

        std::jthread t1 (task, "T1", utilities::random::getRandomInRange(0, 5));
        std::jthread t2 (task, "T2", utilities::random::getRandomInRange(0, 5));
        std::jthread t3 (task, "T3", utilities::random::getRandomInRange(0, 5));

        /**
        2026-02-23 19:29:01.112433 T2 started
        2026-02-23 19:29:01.112463 T3 started
        2026-02-23 19:29:01.112429 T1 started
        2026-02-23 19:29:01.112581 T3 completed in 0 seconds. Waiting for others
        2026-02-23 19:29:02.112671 T1 completed in 1 seconds. Waiting for others
        2026-02-23 19:29:03.112644 T2 completed in 2 seconds. Waiting for others
        2026-02-23 19:29:03.112689 T2 done
        2026-02-23 19:29:03.112725 T1 done
        2026-02-23 19:29:03.112728 T3 done
        **/
    }
}

namespace barrier::unit_tests
{
    void error(const std::string_view msg) {
        std::cerr << "TEST FAILED: " << msg << "\n";
    }

    void test_threshold_one()
    {
        std::atomic<int> passed{0};
        Barrier barrier(1);
        std::jthread t([&] {
            barrier.arrive_and_wait();
            passed.fetch_add(1);
        });

        t.join();
        if (passed.load() != 1)
            std::cerr << "Threshold=1 failed\n";
    }


    void test_basic()
    {
        constexpr int N = 8;
        std::atomic<int> entered{0}, passed{0};
        std::vector<std::jthread> threads;

        Barrier barrier(N);
        for (int i = 0; i < N; ++i){
            threads.emplace_back([&] {
                entered.fetch_add(1, std::memory_order_relaxed);
                barrier.arrive_and_wait();
                passed.fetch_add(1, std::memory_order_relaxed);
            });
        }

        threads.clear();
        if (entered.load() != N)
            error("Not all threads entered barrier");
        if (passed.load() != N)
            error("Not all threads passed barrier");
    }

    void test_no_early_release()
    {
        constexpr int N = 4;
        Barrier barrier(N);
        std::atomic<int> passed{0};

        std::thread t1([&] {
            barrier.arrive_and_wait();
            passed.fetch_add(1);
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (passed.load() != 0)
            error("Barrier released too early");

        std::vector<std::thread> rest;
        for (int i = 1; i < N; ++i) {
            rest.emplace_back([&] {
                barrier.arrive_and_wait();
                passed.fetch_add(1);
            });
        }

        t1.join();
        for (auto& t : rest) t.join();

        if (passed.load() != N)
            error("Barrier did not release correctly");
    }

    void test_reuse()
    {
        constexpr int N = 6, ROUNDS = 50;

        Barrier barrier(N);
        std::atomic<int> counter{0};
        std::vector<std::jthread> threads;

        for (int i = 0; i < N; ++i){
            threads.emplace_back([&] {
                for (int r = 0; r < ROUNDS; ++r) {
                    barrier.arrive_and_wait();
                    counter.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }

        threads.clear();
        if (counter.load() != N * ROUNDS)
            error("Barrier reuse failed");
    }

    void test_stress()
    {
        constexpr int N = 16, ROUNDS = 200;

        Barrier barrier(N);
        std::vector<std::jthread> threads;

        for (int i = 0; i < N; ++i){
            threads.emplace_back([&] {
                for (int r = 0; r < ROUNDS; ++r){
                    barrier.arrive_and_wait();
                }
            });
        }
    }

    void test_callback_once()
    {
        constexpr int N = 5, ROUNDS = 20;

        std::atomic<int> cb_count{0};
        Barrier barrier(N, [&] {
            cb_count.fetch_add(1, std::memory_order_relaxed);
        });

        std::vector<std::jthread> threads;
        for (int i = 0; i < N; ++i) {
            threads.emplace_back([&] {
                for (int r = 0; r < ROUNDS; ++r)
                    barrier.arrive_and_wait();
            });
        }

        threads.clear();
        if (cb_count.load() != ROUNDS)
            std::cerr << "Callback count mismatch\n";
    }

    void test_wait_blocks()
    {
        Barrier barrier(2);
        std::atomic<bool> finished{false};

        std::jthread t1([&] {
            barrier.wait();
            finished = true;
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (finished.load())
            std::cerr << "wait() returned without generation change\n";

        std::jthread t2([&] {
            barrier.arrive_and_wait();
        });
    }
}

void barrier::TestAll()
{
    unit_tests::test_basic();
    unit_tests::test_no_early_release();
    unit_tests::test_reuse();
    unit_tests::test_stress();
    unit_tests::test_callback_once();
    // unit_tests::test_wait_blocks();

    tests::SimpleTest();
}
