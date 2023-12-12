//============================================================================
// Name        : Multithreading_Collections.cpp
// Created on  : 20.10.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Multithreading collections testing
//============================================================================

#include <iostream>
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include <deque>

#include "Multithreading_Collections.h"
#include "../Integer/Integer.h"
#include "../ThreadHelperUtilities/ThreadHelperUtilities.h"

namespace Multithreading_Collections::ThreadSafeContainer {

    template <typename T>
    class ThreadSafeContainer {
    protected:
        std::mutex mtx;
        std::vector<T> elements;

    public:
        template <typename... Args>
        void Add(Args&& ... args) {
            std::lock_guard<std::mutex> lock(mtx);
            elements.emplace_back(std::forward<Args>(args)...);
        }

        template <typename... Args>
        void AddRange(Args&&... args) {
            std::lock_guard<std::mutex> lock(mtx);
            (elements.push_back(args), ...);
        }

        void Dump() {
            std::lock_guard<std::mutex> lock(mtx);
            for (const T& entry : elements)
                std::cout << entry << std::endl;
        }
    };

    //---------------------------------------------- src: ----------------------------------------------/

    std::mutex mtx_global;

    void threadFunction() {
        std::lock_guard lock(mtx_global);
        THREAD_INFO  << "Entered thread " << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(rand() % 10));
        THREAD_INFO << "Leaving thread " << std::endl;
    }

    void threadFunction_SafeTests(ThreadSafeContainer<int>& containter) {
        containter.AddRange(rand(), rand(), rand());
    }

    void SynchTest_1() {
        srand((unsigned int)time(0));
        std::thread t1(threadFunction);
        std::thread t2(threadFunction);
        std::thread t3(threadFunction);
        t1.join();
        t2.join();
        t3.join();
    }

    void SynchTest_2() {
        srand((unsigned int)time(0));
        ThreadSafeContainer<int> cntr;
        std::thread t1(threadFunction_SafeTests, std::ref(cntr));
        std::thread t2(threadFunction_SafeTests, std::ref(cntr));
        std::thread t3(threadFunction_SafeTests, std::ref(cntr));
        t1.join();
        t2.join();
        t3.join();
        cntr.Dump();
    }

    void Simple_OneThread_Test() {
        ThreadSafeContainer<int> containter;
        containter.AddRange(1, 2, 3, 4, 5);
        containter.Dump();
    }
};

namespace Multithreading_Collections::Threadsafe_Queues {



    //////////////////////////////////////////////////////////////////////////////////////

    void RunTest1() {
        ThreadsafeQueue<Integer> queue;

        std::future<void> producer = std::async(std::launch::async, [&]()-> void {
            THREAD_INFO << "Producer: started." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            queue.emplace(1);
            THREAD_INFO << "Producer: done" << std::endl;
        });

        std::future<void> consumer = std::async(std::launch::async, [&]()-> void {
            THREAD_INFO << "Consumer: started" << std::endl;
            auto&& entry = queue.wait_and_pop();
            THREAD_INFO << "Consumer: We've got some" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            entry.printInfo();
            THREAD_INFO << "Consumer: done" << std::endl;
        });

        producer.wait();
        consumer.wait();

        THREAD_INFO << "Done!!" << std::endl;
    }

    void RunTest_WairFor() {
        ThreadsafeQueue<Integer> queue;

        std::future<void> producer = std::async(std::launch::async, [&]()-> void {
            THREAD_INFO << "Producer: started." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            queue.emplace(1);
            THREAD_INFO << "Producer: done" << std::endl;
        });

        std::future<void> consumer = std::async(std::launch::async, [&]()-> void {
            THREAD_INFO << "Consumer: started" << std::endl;
            Integer entry;
            while (true)
            {
                auto ok = queue.wait_for_and_pop(entry, std::chrono::milliseconds(250));
                if (ok) {
                    THREAD_INFO << "Consumer: We've got some" << std::endl;
                    break;
                }
                else {
                    THREAD_INFO << "Timeout" << std::endl;
                }

            }			std::this_thread::sleep_for(std::chrono::seconds(2));
            entry.printInfo();
            THREAD_INFO << "Consumer: done" << std::endl;
        });

        producer.wait();
        consumer.wait();

        THREAD_INFO << "Done!!" << std::endl;
    }
}


void Multithreading_Collections::TEST_ALL() {

    // ThreadSafeContainer::SynchTest_1();
    // ThreadSafeContainer::SynchTest_2();
    // ThreadSafeContainer::Simple_OneThread_Test();

    // Threadsafe_Queues::RunTest1();
    Threadsafe_Queues::RunTest_WairFor();

};