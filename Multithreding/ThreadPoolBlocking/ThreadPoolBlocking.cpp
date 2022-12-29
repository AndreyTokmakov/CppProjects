//============================================================================
// Name        : ThreadPools.cpp
// Created on  : 28.10.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ThreadPools src class
//============================================================================

#include <iostream>
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <thread>
#include <deque>
#include <list>
#include <chrono>
#include <iomanip>

#include "ThreadPoolBlocking.h"

namespace ThreadPoolBlocking
{

    // TODO: Use packaged task
    // TODO: add constrains of Task??
    //       Concepts???
    // template<typename Task = std::function<void()>>
    template<typename Task = std::string>
    class ThreadPool {
    private:
        // TODO: use this mutex for block submit()
        //       with condition variable
        //       wait until taskQueue.size() > MAX_CAPACITY
        mutable std::mutex mtx;

        using TaskListType = std::deque<Task>;

        TaskListType tasks {};

        std::condition_variable tasksListUpdated;

        /** Run switch: **/
        std::atomic_bool run {true};

        /** **/
        std::vector<std::thread> threads;

        /** Maximum number of request handler workers: **/
        // static inline const size_t THREAD_COUNT_MAX { std::thread::hardware_concurrency() };
        static inline const size_t THREAD_COUNT_MAX { 1 };

        /** Maximum number of request handler workers: **/
        // static inline const size_t MAX_CAPACITY { std::thread::hardware_concurrency() * 2 };
        static inline const size_t MAX_CAPACITY { 10 };

        //static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> TIMEOUT =
        //        std::chrono::milliseconds(2000);

        static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> POLL_TIMEOUT =
                std::chrono::milliseconds(1000);

    protected:

        /*
        template <typename... Args>
        void emplace(Args&& ... args) {
            // TODO: Add blocking!!!!
            if (std::lock_guard<std::mutex> lock(mtx); true) {
                tasks.emplace_back(std::forward<Args>(args)...);
            }
            tasksListUpdated.notify_one();
        }
        */

        // INFO: Consumer
        void worker_thread() {
            while (true) { // TODO: Check is there is any more right way to access the 'run' value?
                if (std::unique_lock<std::mutex> lock(mtx); true) {
                    while (!tasksListUpdated.wait_for(lock, std::chrono::milliseconds(POLL_TIMEOUT), [&] {
                        return not tasks.empty();
                    })) {
                        if (!run) /** Timeout **/
                            return;
                    }

                    const auto task = std::move(tasks.front());
                    std::cout << "Consumer: " << task << std::endl;
                    tasks.pop_front();

                    {   // INFO: debug !!!!!!!!!!
                        lock.unlock();
                        std::this_thread::sleep_for(std::chrono::seconds(10));
                        // if (0 == ++counter % 1000) std::cout << counter << std::endl;
                    }
                }
                tasksListUpdated.notify_all();
            }
        }

    public:
        ThreadPool() {
            try {
                // TODO: Refactor
                for (size_t i = 0; i < THREAD_COUNT_MAX; ++i) {
                    threads.emplace_back(&ThreadPool::worker_thread, this);
                }
            } catch (...) {
                // TODO: Handle this correctly
                run = false;
                throw;
            }
        }

        ~ThreadPool() {
            /** Join threads: **/
            // TODO: Check if queue is empty!
            run = false;
            for (auto& T : threads)
                T.join();
        }

        template<typename FunctionType>
        void submit(FunctionType f)
        {
            if (std::unique_lock<std::mutex> lock {mtx}; tasks.size() < MAX_CAPACITY) {
                tasks.emplace_back(f);
            }
            else {
                while (!tasksListUpdated.wait_for(lock, std::chrono::milliseconds(POLL_TIMEOUT), [&] {
                    return tasks.size() < MAX_CAPACITY;
                })) { /** Timeout **/ }
                tasks.emplace_back(f);
            }

            std::cout << "Producer: Task pushed \n";
            tasksListUpdated.notify_all();
        }

        bool empty() const noexcept {
            std::lock_guard<std::mutex> lock(mtx);
            return tasks.empty();
        }

        size_t size() const noexcept {
            std::lock_guard<std::mutex> lock(mtx);
            return tasks.size();
        }

        // TODO: Available workers
        // TODO: Capacity
    };


    struct synch_stream final : public std::stringstream {
    private:
        static inline std::mutex mtx;
        const static inline std::thread::id mainThreadId { std::this_thread::get_id() };

    private:
        std::string getCurrentTime() const noexcept {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
            return ss.str();
        }

    public:
        ~synch_stream() override {
            const auto currId = std::this_thread::get_id();

            // TODO: Prepare string before lock
            std::string info("[");
            info.append(getCurrentTime()).append("] Thread [");

            std::lock_guard<std::mutex> lock{ mtx };
            std::cout << info;
            if (mainThreadId == currId)
                std::cout << std::setiosflags(std::ios::left) << std::setw(9) << "Main";
            else
                std::cout << "Id: " << std::setiosflags(std::ios::left) << std::setw(5) << currId;

            std::cout << "] " << rdbuf();
            std::cout.flush();
        }
    };


};

namespace ThreadPoolBlocking
{
    std::mutex printMutex;

    template<typename T>
    void print_with_space(const T& arg) {
        std::cout << arg << ' ';
    }

    template<typename ... Types>
    void print(Types&&... params) {
        std::lock_guard<std::mutex> lock {printMutex};
        (print_with_space(std::forward<Types>(params)), ...);
        std::cout << std::endl;
    }

    // std::atomic<int> counter = 0;


    void RunTest()
    {

        /*
        auto job1 = [&]() {
            //print("Starting", "job");
            // std::this_thread::sleep_for(std::chrono::milliseconds(1));
            //print("Job done" );
            counter++;
        };*/

        ThreadPool pool;
        for (int i = 0; i < 1000; i++)
            pool.submit("Task_" + std::to_string(i));

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void ThreadPoolBlocking::TEST_ALL() {

    RunTest();
};