/**============================================================================
Name        : Semaphore.cpp
Created on  : 23.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Semaphore.cpp
============================================================================**/

#include "Semaphore.hpp"

#include <mutex>
#include <condition_variable>

namespace semaphore
{
    class Semaphore
    {
        std::mutex m;
        std::condition_variable cv;
        int32_t count { 0 };

    public:

        explicit Semaphore(const int32_t c): count { c } {
        }

        void acquire()
        {
            std::unique_lock lk(m);
            cv.wait(lk, [&]{ return count > 0; });
            --count;
        }

        void release()
        {
            std::lock_guard lk(m);
            ++count;
            cv.notify_one();
        }
    };
}



void semaphore::TestAll()
{

}
