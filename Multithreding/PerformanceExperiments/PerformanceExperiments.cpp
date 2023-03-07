/**============================================================================
Name        : PerformanceExperiments.cpp
Created on  : 26.02.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Multithreading performance experiments
============================================================================**/

#include "PerformanceExperiments.h"

#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <syncstream>

namespace PerformanceExperiments::CV_vs_Atomic
{
    struct RunnerBase
    {
        static constexpr int MaxCountTimes {1'000'000};

        virtual void ping() noexcept = 0;
        virtual void pong() noexcept = 0;
        virtual bool start() noexcept = 0;

        virtual ~RunnerBase() = default;
    };

    struct CVEngine: RunnerBase
    {
        std::condition_variable cv;
        std::mutex mtx;
        bool pingDone { false };
        bool pongDone { false };

        void ping() noexcept override
        {
            int counter = 0;
            while (counter <= MaxCountTimes)
            {
                {
                    std::unique_lock lck { mtx };
                    cv.wait(lck, [this]() {
                        return pingDone;
                    });
                    pingDone = false;
                    pongDone = true;
                    ++counter;
                }
                cv.notify_one();
            }
        }

        void pong() noexcept override
        {
            int counter = 0;
            while (counter<MaxCountTimes)
            {
                {
                    std::unique_lock lck { mtx };
                    cv.wait(lck, [this](){
                        return pongDone;
                    });
                    pingDone = true;
                    pongDone = false;
                    ++counter;
                }
                cv.notify_one();
            }
        }

        bool start() noexcept override
        {
            {
                std::unique_lock lck { mtx };
                pingDone = true;
            }
            cv.notify_one();
            return true;
        }
    };

    struct AtomicEngine: RunnerBase
    {
        std::atomic_flag pass;
        std::atomic_int counter;

        void ping() noexcept override
        {
            int counterLocal = 0;
            while (counterLocal <= MaxCountTimes) {
                pass.wait(false);
                pass.clear();
                ++counterLocal;
                ++counter;
                pass.notify_one();
                // std::osyncstream {std::cout}  << "ping: " << counterLocal << std::endl;
            }
            // std::osyncstream {std::cout} << "ping: done!!!" << std::endl;
        }

        void pong() noexcept override
        {
            int counterLocal = 0;
            while (counterLocal < MaxCountTimes) {
                pass.wait(true);
                pass.test_and_set();
                ++counterLocal;
                pass.notify_one();
                // std::osyncstream {std::cout}  << "pong: " << counterLocal << std::endl;
            }
            // std::osyncstream {std::cout} << "pong: done!!!" << std::endl;
        }

        bool start() noexcept override
        {
            pass.test_and_set();
            pass.notify_one();
            return true;
        }
    };

    void RunBenchmark()
    {
        {
            auto start = std::chrono::system_clock::now();

            if (auto engine = new AtomicEngine(); engine->start()) {
                std::jthread ping = std::jthread(&AtomicEngine::ping, engine);
                std::jthread pong = std::jthread(&AtomicEngine::pong, engine);
            }

            std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
            std::cout << "Duration: " << dur.count() << " seconds" << std::endl;
        }

        {
            auto start = std::chrono::system_clock::now();

            if (auto engine = new CVEngine(); engine->start()) {
                std::jthread ping = std::jthread(&CVEngine::ping, engine);
                std::jthread pong = std::jthread(&CVEngine::pong, engine);
            }

            std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
            std::cout << "Duration: " << dur.count() << " seconds" << std::endl;
        }


        // Duration: 0.171744 seconds
        // Duration: 2.54261 seconds
    }
};

void PerformanceExperiments::TestAll()
{
    CV_vs_Atomic::RunBenchmark();
};
