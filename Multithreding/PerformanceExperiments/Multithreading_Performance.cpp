//============================================================================
// Name        : Multithread_Performance.h
// Created on  : 02.10.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Multithread performance tests class
//============================================================================

#include "../Utilities/Utilities.h"
#include "Multithreading_Performance.h"

#include <iostream>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <chrono>
#include <future>
#include <cassert>


namespace Multithreading_Performance::SycnronizationTests {

    constexpr int countlimit = 1'000'000;

    void ConditionVariables() {
        bool dataReady{ false };
        std::mutex mutex_;
        std::condition_variable condVar1;
        std::condition_variable condVar2;
        std::atomic<int> counter{};

        auto ping = [&]() {
            while (counter <= countlimit) {
                {
                    std::unique_lock<std::mutex> lck(mutex_);
                    condVar1.wait(lck, [&] {return dataReady == false; });
                    dataReady = true;
                }
                ++counter;
                condVar2.notify_one();
            }
        };

        auto pong = [&]() {
            while (counter < countlimit) {
                {
                    std::unique_lock<std::mutex> lck(mutex_);
                    condVar2.wait(lck, [&] {return dataReady == true; });
                    dataReady = false;
                }
                condVar1.notify_one();
            }
        };

        auto start = std::chrono::system_clock::now();

        std::thread t1(ping);
        std::thread t2(pong);

        t1.join();
        t2.join();

        std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
        std::cout << "Condition Variables Sycnronization : Duration: " << dur.count() << " seconds" << std::endl;
        assert(counter == countlimit + 1);
    }

    void AtomicFlag() {
        std::atomic_flag condAtomicFlag1{};
        std::atomic_flag condAtomicFlag2{};
        std::atomic<int> counter{};

        auto ping = [&]() {
            while (counter <= countlimit) {
                condAtomicFlag1.wait(false);
                condAtomicFlag1.clear();

                ++counter;

                condAtomicFlag2.test_and_set();
                condAtomicFlag2.notify_one();
            }
        };

        auto pong = [&]() {
            while (counter < countlimit) {
                condAtomicFlag2.wait(false);
                condAtomicFlag2.clear();

                condAtomicFlag1.test_and_set();
                condAtomicFlag1.notify_one();
            }
        };

        auto start = std::chrono::system_clock::now();

        condAtomicFlag1.test_and_set();
        std::thread t1(ping);
        std::thread t2(pong);

        t1.join();
        t2.join();

        std::cout << "Done!\n";

        std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
        std::cout << "Atomic Flag Sycnronization : Duration: " << dur.count() << " seconds" << std::endl;
        assert(counter == countlimit + 1);
    }

    void OneAtomicFlag() {
        std::atomic_flag condAtomicFlag{};
        std::atomic<int> counter{};

        auto ping = [&]() {
            while (counter <= countlimit) {
                condAtomicFlag.wait(true);
                condAtomicFlag.test_and_set();
                ++counter;
                condAtomicFlag.notify_one();
            }
        };

        auto pong = [&]() {
            while (counter < countlimit) {
                condAtomicFlag.wait(false);
                condAtomicFlag.clear();
                condAtomicFlag.notify_one();
            }
        };

        auto start = std::chrono::system_clock::now();
        condAtomicFlag.test_and_set();
        std::thread t1(ping);
        std::thread t2(pong);

        t1.join();
        t2.join();

        std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
        std::cout << "OneAtomicFlag Sycnronization : Duration: " << dur.count() << " seconds" << std::endl;
        assert(counter == countlimit + 1);
    }

    void AtomicBool() {
        std::atomic<bool> atomicBool{};
        std::atomic<int> counter{};

        auto ping = [&]() {
            while (counter <= countlimit) {
                atomicBool.wait(true);
                atomicBool.store(true);
                ++counter;
                atomicBool.notify_one();
            }
        };

        auto pong = [&]() {
            while (counter < countlimit) {
                atomicBool.wait(false);
                atomicBool.store(false);
                atomicBool.notify_one();
            }
        };

        auto start = std::chrono::system_clock::now();

        atomicBool.store(true);
        std::thread t1(ping);
        std::thread t2(pong);

        t1.join();
        t2.join();

        std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
        std::cout << "AtomicBool Sycnronization : Duration: " << dur.count() << " seconds" << std::endl;
        assert(counter == countlimit + 1);
    }

    void Mutex() {
        std::mutex mtx;
        int counter = 0;

        auto ping = [&]() {
            bool run{true};
            while (run) {
                std::lock_guard<std::mutex> lock(mtx);
                ++counter;
                run = counter <= countlimit;
            }
        };

        auto pong = [&]() {
            bool run{ true };
            while (run) {
                std::lock_guard<std::mutex> lock(mtx);
                ++counter;
                counter--;
                run = counter <= countlimit;
            }
        };

        auto start = std::chrono::system_clock::now();

        std::thread t1(ping);
        std::thread t2(pong);

        t1.join();
        t2.join();

        std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
        std::cout << "Mutex Sycnronization : Duration: " << dur.count() << " seconds" << std::endl;
        assert(counter == countlimit + 1);
    }
}


namespace Multithreading_Performance::IncrementCounterTests {

    constexpr int countlimit = 10'000'000;
    constexpr int threadCount = 10;

    void AtomicIncrement() {
        std::atomic<int> counter{0};
        // int counter{ 0 };

        auto task = [&]() {
            for (int i = 0; i < countlimit; ++i) {
                counter++;
            }
        };

        std::vector<std::thread> jobs;

        auto start = std::chrono::system_clock::now();
        for (int i = 0; i < threadCount; ++i)
            jobs.emplace_back(task);
        for (std::thread& T : jobs)
            T.join();
        auto end = std::chrono::system_clock::now();


        std::chrono::duration<double> dur = end - start;
        std::cout << "Atomic counter: duration: " << dur.count() << " seconds" << std::endl;
        assert(counter == countlimit * 10);
    }

    void MutexIncrement() {
        int counter{ 0 };
        std::mutex mtx;

        auto task = [&]() {
            for (int i = 0; i < countlimit; ++i) {
                std::lock_guard<std::mutex> lock(mtx);
                counter++;
            }
        };

        std::vector<std::thread> jobs;

        auto start = std::chrono::system_clock::now();
        for (int i = 0; i < threadCount; ++i)
            jobs.emplace_back(task);
        for (std::thread& T : jobs)
            T.join();
        auto end = std::chrono::system_clock::now();


        std::chrono::duration<double> dur = end - start;
        std::cout << "Mutex counter: duration: " << dur.count() << " seconds" << std::endl;
        assert(counter == countlimit * 10);
    }
}


namespace Multithreading_Performance::AtomicRef {

    class SomeHeavyObjectToBeCopied {
    public:
        int counter{ 0 };
    };


    class ComplexHeavyObjectToBeCopied {
    public:
        int counter_one{ 0 };
        int counter_two{ 0 };
        int counter_three{ 0 };
    };


    constexpr int countlimit = 10'000'000;
    constexpr int threadCount = 10;


    void NoAtomicIncrement() {
        SomeHeavyObjectToBeCopied resource;
        std::vector<std::thread> jobs;

        auto start = std::chrono::system_clock::now();
        for (int i = 0; i < threadCount; ++i)
            jobs.emplace_back([&resource]() {
                                  for (int i = 0; i < countlimit; ++i) {
                                      resource.counter++;
                                  }
                              }
            );
        for (std::thread& T : jobs)
            T.join();
        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> dur = end - start;
        std::cout << "NonSynch increment duration: " << dur.count() << " seconds" << std::endl;
    }

    void AtomicIncrement() {
        SomeHeavyObjectToBeCopied resource;
        std::atomic_ref<int> atomicResFef{ resource.counter };
        std::vector<std::thread> jobs;

        auto start = std::chrono::system_clock::now();
        for (int i = 0; i < threadCount; ++i)
            jobs.emplace_back([&atomicResFef]() {
                                  for (int i = 0; i < countlimit; ++i) {
                                      atomicResFef++;
                                  }
                              }
            );

        for (std::thread& T : jobs)
            T.join();

        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> dur = end - start;
        std::cout << "AromicRef increment duration: " << dur.count() << " seconds" << std::endl;
        assert(resource.counter == countlimit * threadCount);
    }

    void MutexSynch_Increment() {
        std::mutex mtx;
        SomeHeavyObjectToBeCopied resource;
        std::vector<std::thread> jobs;

        auto start = std::chrono::system_clock::now();
        for (int i = 0; i < threadCount; ++i)
            jobs.emplace_back([&resource, &mtx]() {
                                  for (int i = 0; i < countlimit; ++i) {
                                      std::lock_guard<std::mutex> lock(mtx);
                                      resource.counter++;
                                  }
                              }
            );

        for (std::thread& T : jobs)
            T.join();

        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> dur = end - start;
        std::cout << "Mutex Synch increment duration: " << dur.count() << " seconds" << std::endl;
        assert(resource.counter == countlimit * threadCount);
    }

    //---------------------------------------------------------------------------------------//

    void AtomicIncrement_ThreeVariable() {
        ComplexHeavyObjectToBeCopied resource;

        std::atomic_ref<int> atomicResFefOne{ resource.counter_one };
        std::atomic_ref<int> atomicResFefTwo{ resource.counter_two };
        std::atomic_ref<int> atomicResFefThree{ resource.counter_three };

        std::vector<std::thread> jobs;

        auto start = std::chrono::system_clock::now();
        for (int i = 0; i < threadCount; ++i)
            jobs.emplace_back([&atomicResFefOne, &atomicResFefTwo, &atomicResFefThree]() {
                                  for (int i = 0; i < countlimit; ++i) {
                                      atomicResFefOne++;
                                      atomicResFefTwo++;
                                      atomicResFefThree++;
                                  }
                              }
            );

        for (std::thread& T : jobs)
            T.join();

        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> dur = end - start;
        std::cout << "AromicRef (Complex) increment duration: " << dur.count() << " seconds" << std::endl;

        assert(resource.counter_one == countlimit * threadCount);
        assert(resource.counter_two == countlimit * threadCount);
        assert(resource.counter_three == countlimit * threadCount);
    }

    void MutexSynch_Increment_ThreeVariable() {
        std::mutex mtx;
        ComplexHeavyObjectToBeCopied resource;
        std::vector<std::thread> jobs;

        auto start = std::chrono::system_clock::now();
        for (int i = 0; i < threadCount; ++i)
            jobs.emplace_back([&resource, &mtx]() {
                                  for (int i = 0; i < countlimit; ++i) {
                                      std::lock_guard<std::mutex> lock(mtx);
                                      resource.counter_one++;
                                      resource.counter_two++;
                                      resource.counter_three++;
                                  }
                              }
            );

        for (std::thread& T : jobs)
            T.join();

        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> dur = end - start;
        std::cout << "Mutex Synch (Complex) increment duration: " << dur.count() << " seconds" << std::endl;

        assert(resource.counter_one == countlimit * threadCount);
        assert(resource.counter_two == countlimit * threadCount);
        assert(resource.counter_three == countlimit * threadCount);
    }
}

namespace Multithreading_Performance::Atomics {

    void AtomicFlag_Test_MemoryMode() {
        constexpr size_t TESTS_COUNT { 10'000'000'000 };
        std::atomic_flag flag = ATOMIC_FLAG_INIT;

        {
            const auto start = std::chrono::system_clock::now();
            for (size_t i = 0; i < TESTS_COUNT; ++i) {
                flag.test();
            }
            const auto end = std::chrono::system_clock::now();
            const std::chrono::duration<double> dur = end - start;
            std::cout << "Duration: " << dur.count() << " seconds" << std::endl;
        }

        {
            const auto start = std::chrono::system_clock::now();
            for (size_t i = 0; i < TESTS_COUNT; ++i) {
                flag.test(std::memory_order::relaxed);
            }
            const auto end = std::chrono::system_clock::now();
            const std::chrono::duration<double> dur = end - start;
            std::cout << "Duration: " << dur.count() << " seconds" << std::endl;
        }

        {
            const auto start = std::chrono::system_clock::now();
            for (size_t i = 0; i < TESTS_COUNT; ++i) {
                flag.test(std::memory_order::acquire);
            }
            const auto end = std::chrono::system_clock::now();
            const std::chrono::duration<double> dur = end - start;
            std::cout << "Duration: " << dur.count() << " seconds" << std::endl;
        }

        {
            const auto start = std::chrono::system_clock::now();
            for (size_t i = 0; i < TESTS_COUNT; ++i) {
                flag.test(std::memory_order::consume);
            }
            const auto end = std::chrono::system_clock::now();
            const std::chrono::duration<double> dur = end - start;
            std::cout << "Duration: " << dur.count() << " seconds" << std::endl;
        }
    }


    void AtomicBool_Test_MemoryMode() {
        constexpr size_t TESTS_COUNT { 10'000'000'000 };
        std::atomic_bool run { true };

        {
            const auto start = std::chrono::system_clock::now();
            for (size_t i = 0; i < TESTS_COUNT; ++i) {
                if (run) { /** **/ }
            }
            const auto end = std::chrono::system_clock::now();
            const std::chrono::duration<double> dur = end - start;
            std::cout << "Duration: " << dur.count() << " seconds" << std::endl;
        }

        {
            const auto start = std::chrono::system_clock::now();
            for (size_t i = 0; i < TESTS_COUNT; ++i) {
                if (run.load()) { /** **/ }
            }
            const auto end = std::chrono::system_clock::now();
            const std::chrono::duration<double> dur = end - start;
            std::cout << "Duration: " << dur.count() << " seconds" << std::endl;
        }

        {
            const auto start = std::chrono::system_clock::now();
            for (size_t i = 0; i < TESTS_COUNT; ++i) {
                if (run.load(std::memory_order::relaxed)) { /** **/ }
            }
            const auto end = std::chrono::system_clock::now();
            const std::chrono::duration<double> dur = end - start;
            std::cout << "Duration: " << dur.count() << " seconds" << std::endl;
        }
    }
}

void Multithreading_Performance::TEST_ALL()
{
    // SycnronizationTests::ConditionVariables();
    // SycnronizationTests::AtomicFlag();
    // SycnronizationTests::OneAtomicFlag();
    // SycnronizationTests::AtomicBool();
    // SycnronizationTests::Mutex();


    // Atomics::AtomicFlag_Test_MemoryMode();
    // Atomics::AtomicBool_Test_MemoryMode();


    // IncrementCounterTests::AtomicIncrement();
    // IncrementCounterTests::MutexIncrement();

    // AtomicRef::NoAtomicIncrement();
    // AtomicRef::AtomicIncrement();
    // AtomicRef::MutexSynch_Increment();
    // AtomicRef::AtomicIncrement_ThreeVariable();
    // AtomicRef::MutexSynch_Increment_ThreeVariable();
};