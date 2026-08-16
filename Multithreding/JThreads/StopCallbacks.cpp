/**============================================================================
Name        : StoppingThreads.cpp
Created on  : 20.11.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StoppingThreads.cpp
============================================================================**/

#include "JThreads.hpp"

#include <iostream>
#include <thread>
#include <syncstream>
#include <future>
#include <vector>
#include <optional>

#include "DateTimeUtilities.hpp"

namespace
{
    using utilities::datetime::getCurrentTime;
#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "
}

namespace jthreads::stop_callbacks
{
    using namespace std::chrono_literals;

    void multiple_callbacks()
    {
        std::jthread job = std::jthread([](const std::stop_token &token)
        {
            const std::stop_callback cbOne(token, [] {
                LOG << "Callback (one)   called: ID =  " << std::this_thread::get_id() << std::endl;
            });
            const std::stop_callback cbTwo(token, [] {
                LOG << "Callback (Two)   called: ID =  " << std::this_thread::get_id() << std::endl;
            });
            const std::stop_callback cbThree(token, [] {
                LOG << "Callback (Three) called: ID =  " << std::this_thread::get_id() << std::endl;
            });

            while (!token.stop_requested()) {
                std::this_thread::sleep_for(250ms);
            }
            LOG << "Thread id: " << std::this_thread::get_id() << " done\n";
        });

        std::this_thread::sleep_for(1s);
        LOG << "Main  : Stopping thread.\n";
        job.request_stop(); // request stop

        job.join();
        LOG << "Main  : Done.\n";
    }

    /*
    2026-04-12 07:18:29.691261 Main  : Stopping thread.
    2026-04-12 07:18:29.691528 Callback (Three) called: ID =  139786268497920
    2026-04-12 07:18:29.691564 Callback (Two)   called: ID =  139786268497920
    2026-04-12 07:18:29.691580 Callback (one)   called: ID =  139786268497920
    2026-04-12 07:18:29.941430 Thread id: 139786268493568 done
    2026-04-12 07:18:29.941518 Main  : Done.
    */
}


namespace jthreads::stop_callbacks::callback_with_cv
{
    std::mutex mtx;
    std::condition_variable cv;
    bool dataReady = false;

    void worker(const std::stop_token &stoken)
    {
        std::unique_lock lock(mtx);
        const std::stop_callback callback(stoken, [&] {
            LOG << "[callback] Stop requested! Waking thread.\n";
            cv.notify_all();
        });

        cv.wait(lock, [&] {
            return dataReady || stoken.stop_requested();
        });

        if (stoken.stop_requested()) {
            LOG << "Cancelled while waiting. Exiting gracefully.\n";
            return;
        }
        LOG << "Got data! Processing...\n";
    }

    void run_thread_and_exit()
    {
        const std::jthread jt(worker);
        std::this_thread::sleep_for(std::chrono::seconds(1U));

        // We never set dataReady = true.
        // jthread destructor will request_stop() -> callback fires
        //   -> thread wakes -> exits.

        LOG << "main() ending. jthread destructor handles everything.\n";
    }

    /*
    2026-04-12 07:13:08.072946 main() ending. jthread destructor handles everything.
    2026-04-12 07:13:08.073240 [callback] Stop requested! Waking thread.
    2026-04-12 07:13:08.073276 Cancelled while waiting. Exiting gracefully.
    */
}

void jthreads::stop_callbacks::TestAll()
{
    multiple_callbacks();
    // callback_with_cv::run_thread_and_exit();
};


