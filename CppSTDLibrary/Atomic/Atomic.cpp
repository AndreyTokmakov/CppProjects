
#include <iostream>       // std::cout
#include <atomic>         // std::atomic, std::atomic_flag, ATOMIC_FLAG_INIT
#include <thread>         // std::thread, std::this_thread::yield
#include <vector>         // std::vector
#include <string>
#include <string_view>


#include "Atomic.h"
#include "../Helpers/Helpers.h"


namespace Atomic
{
    namespace Bool_Atomic_Tests {

        std::atomic_flag winner = ATOMIC_FLAG_INIT;
        std::atomic<bool> ready(false);
        //bool ready;

        void count1m(int id) {
            while (false == ready) {
                std::this_thread::yield();
            }      // wait for the ready signal
            for (int i = 0; i < 1000000; ++i) {
                // go!, count to 1 million
            }
            if (false == winner.test_and_set()) {
                std::cout << "thread #" << id << " won!" << std::endl;
            }
        };

        void Test() {
            //ready = false;

            std::vector<std::thread> threads;
            std::cout << "spawning 10 threads that count to 1 million..." << std::endl;
            for (int i = 1; i <= 10; ++i)
                threads.push_back(std::thread(count1m, i));

            ready = true;
            for (auto& th : threads) th.join();
        }

        //////////////////////////////////////////////////////

        //std::atomic_flag winner = ATOMIC_FLAG_INIT;
        std::atomic<bool> switcher(false);

        void task(int i) {
            std::cout << "Thread " << i << " waiting" << std::endl;
            if (false == switcher) {
                std::cout << "Thread " << i << " started" << std::endl;
                switcher = true;
                int i = 0;
                for (; i < 100; ++i) {
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                }
                std::cout << i << std::endl;
                switcher = false;
            }
            std::cout << "Thread " << i << " done" << std::endl;
        };

        void Test2() {
            std::vector<std::thread> threads;
            std::cout << "spawning 10 threads that count to 1 million..." << std::endl;
            for (int i = 1; i <= 10; ++i)
                threads.push_back(std::thread(task, i));

            ready = true;
            for (auto& th : threads)
                th.join();
        }
    }
};

namespace Atomic
{
    void TestAll() {
        // Bool_Atomic_Tests::Test();
        Bool_Atomic_Tests::Test2();
    }
};