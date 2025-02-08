/**============================================================================
Name        : MultiThreading.cpp
Created on  : 06.07.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MultiThreading algorithms
//============================================================================**/

#include "MultiThreading.h"

#include <iostream>
#include <stop_token>
#include <future>
#include <thread>
#include <vector>
#include <list>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <atomic>
#include <deque>
#include <concepts>
#include <functional>
#include <syncstream>
#include <random>


// https://leetcode.com/problems/print-in-order/submissions/
/**

public class Foo {
    public void first() { print("first"); }
    public void second() { print("second"); }
    public void third() { print("third"); }
}

The same instance of Foo will be passed to three different threads.
Thread A will call first(), thread B will call second(), and thread C will call third().
Design a mechanism and modify the program to ensure that second() is executed
after first(), and third() is executed after second().
 */
namespace MultiThreading::Print_In_Order
{
    struct Worker
    {
        std::atomic<uint32_t> turnSwitch;

        explicit Worker(uint32_t turn = 1): turnSwitch {turn} {
        }

        void spinLock(uint32_t turn) {
            while (turnSwitch.load(std::memory_order_acquire) != turn) {
            }
        }

        void first()
        {
            std::cout << "First" << std::endl;
            turnSwitch.store(2, std::memory_order_release); // turnSwitch.store(2) or turnSwitch = 2
        }

        void second()
        {
            spinLock(2);
            std::cout << "Second" << std::endl;
            turnSwitch.store(3, std::memory_order_release);
        }

        void third()
        {
            spinLock(3);
            std::cout << "Third" << std::endl;
        }
    };


    int32_t getRandomInt(int32_t from = 0, int32_t until = 100) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distribution(from, until);
        return distribution(gen);
    }

    void singleThreadTest()
    {
        Worker worker;

        worker.first();
        worker.second();
        worker.third();
    }

    void multiThreadTest()
    {
        Worker worker;

        std::vector<std::future<void>> jobs;
        for (size_t idx = 1; idx <= 3; ++idx) {
            jobs.emplace_back(std::async(std::launch::async, [&worker, idx]()
            {
                const int32_t sleepTime = getRandomInt(0, 5);
                std::this_thread::sleep_for(std::chrono::seconds(sleepTime));

                switch (idx) {
                    case 1: worker.first();  break;
                    case 2: worker.second(); break;
                    case 3: worker.third();  break;
                }
            }));
        }

        for (const auto& T: jobs)
            T.wait();
    }

    void TestAll()
    {

        // singleThreadTest();
        multiThreadTest();


        /*
        for (int i = 0; i < 10; ++i)
            std::cout << getRandomInt(0, 5) << std::endl;
        */
    }
}


void MultiThreading::TestAll()
{
    Print_In_Order::TestAll();
};
