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

namespace semaphore::unit_tests
{
    void test_semaphore_basic()
    {
        Semaphore sem(1);
        std::atomic<int> counter{0};

        auto worker = [&] {
            sem.acquire();
            counter.fetch_add(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            counter.fetch_sub(1);
            sem.release();
        };

        std::jthread t1(worker), t2(worker);
        if (counter.load() != 0) {
            std::cerr << "Semaphore failed, counter != 0\n";
        }
    }

}

void semaphore::TestAll()
{

}
