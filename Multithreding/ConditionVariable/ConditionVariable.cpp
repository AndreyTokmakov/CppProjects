//============================================================================
// Name        : ConditionVariable.h
// Created on  : 22.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : condition_variable src
//============================================================================

#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <chrono>
#include <future>
#include <list>
#include <vector>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <syncstream>
#include <format>

#include "ConditionVariable.h"

#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << utilities::datetime::getCurrentTime() << " "


using namespace std::chrono_literals;

namespace
{
    struct CurrentTime
    {
        const std::chrono::time_point<std::chrono::high_resolution_clock> now { std::chrono::system_clock::now() };
    };

    std::ostream& operator<<(std::ostream& stream, const CurrentTime& time)
    {
        stream << std::format("{:%d-%m-%Y %H:%M:%OS} | ", time.now);
        return stream;
    }
}

namespace ConditionVariable::Classic_Test {

    void Test() {
        std::queue<int> produced_nums;
        std::mutex m;
        std::condition_variable cond_var;
        bool done = false;
        bool notified = false;

        std::thread producer([&]() {
            for (int i = 0; i < 5; ++i) {
                std::this_thread::sleep_for(std::chrono::seconds(3));
                std::unique_lock<std::mutex> lock(m);
                std::cout << "producing " << i << '\n';
                produced_nums.push(i);
                notified = true;
                cond_var.notify_one();
            }

            done = true;
            cond_var.notify_one();
        });

        std::thread consumer([&]() {
            std::unique_lock<std::mutex> lock(m);
            while (!done) {
                while (!notified) {  // loop to avoid spurious wakeups
                    cond_var.wait(lock);
                    std::cout << "Condition var notified." << std::endl;
                }
                while (!produced_nums.empty()) {
                    std::cout << "consuming " << produced_nums.front() << std::endl;
                    produced_nums.pop();
                }
                notified = false;
            }
        });

        producer.join();
        consumer.join();
    }

    void Test_Predicate_0() {
        std::queue<int> produced_nums;
        std::mutex m;
        std::condition_variable cond_var;
        bool done = false;
        bool notified = false;

        std::thread producer([&]() {
            for (int i = 0; i < 5; ++i) {
                std::this_thread::sleep_for(std::chrono::seconds(3));
                std::unique_lock<std::mutex> lock(m);
                std::cout << "producing " << i << std::endl;
                produced_nums.push(i);
                notified = true;
                cond_var.notify_one();
            }

            done = true;
            cond_var.notify_one();
        });

        std::thread consumer([&]() {
            std::unique_lock<std::mutex> lock(m);
            while (!done) {
                cond_var.wait(lock, [&] {return notified; });
                std::cout << "Condition var notified." << std::endl;

                while (!produced_nums.empty()) {
                    std::cout << "consuming " << produced_nums.front() << std::endl;
                    produced_nums.pop();
                }
                notified = false;
            }
        });

        producer.join();
        consumer.join();
    }



    void Test_Predicate() {
        std::condition_variable conditionVariable;
        std::mutex mtx;
        int i = 0;

        auto consumer = [&] {
            std::unique_lock<std::mutex> lock(mtx);
            std::cerr << "Waiting...." << std::endl;

            conditionVariable.wait(lock, [&] {return i == 1; });
            std::cout << "...finished waiting. i == 1." << std::endl;
        };

        auto producer = [&] {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            {
                std::lock_guard<std::mutex> lk(mtx);
                std::cerr << "Notifying..." << std::endl;
            }

            conditionVariable.notify_all();
            std::this_thread::sleep_for(std::chrono::seconds(1));

            {
                std::lock_guard<std::mutex> lk(mtx);
                i = 1;
                std::cerr << "Notifying again..." << std::endl;
            }

            conditionVariable.notify_all();
        };


        std::thread t1(consumer), t2(consumer), t3(consumer), t4(producer);
        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }

    void Wait_PusthBack_2Vector() {
        std::vector<int> data;
        std::condition_variable trigger;
        std::mutex mtx;

        std::future<void> producer = std::async(std::launch::async, [&]()-> void {
            LOG << "Producer: started." << std::endl;
            for (int i = 0; i < 10; i++) {
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    data.push_back(i);
                    LOG << "Producer: data size = " << data.size() << std::endl;
                }
                trigger.notify_one();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });

        std::future<void> consumer = std::async(std::launch::async, [&]()-> void {
            LOG << "Consumer: started" << std::endl;
            std::unique_lock<std::mutex> lock(mtx);
            trigger.wait(lock, [&] {
                LOG << "Consumer: Check condition " << std::endl;
                return data.size() > 3;
            });
            LOG << "Consumer: done" << std::endl;
        });

        producer.wait();
        consumer.wait();
    }

    void Wait_Pust_2Queue() {
        std::queue<int> queue;
        std::mutex mtx;
        std::condition_variable trigger;
        bool done = false, notified = false;

        std::thread producer([&]() {
            for (int i = 0; i < 50; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::unique_lock<std::mutex> lock(mtx);
                LOG << "Producer: producing " << i << std::endl;
                queue.push(i);
                notified = true;
                trigger.notify_one();
            }
            done = true;
            trigger.notify_one();
        });

        std::thread consumer([&]() {
            std::unique_lock<std::mutex> lock(mtx);
            while (!done) {
                while (!notified) {  // loop to avoid spurious wakeups
                    trigger.wait(lock);
                }
                while (!queue.empty()) {
                    LOG << "Consumer: consuming " << queue.front() << std::endl;
                    queue.pop();
                }
                notified = false;
            }
        });

        producer.join();
        consumer.join();
    }

    void Condition_Variable_Test() {
        std::vector<int> data;
        std::condition_variable data_cond;
        std::mutex m;

        auto thread_func1 = [&]() {

            std::cout << "thread_func1 entered()" << std::endl;
            for (int i = 0; i < 10; i++) {
                {
                    std::unique_lock<std::mutex> lock(m);
                    data.push_back(i);
                    std::cout << "data size = " << data.size() << std::endl;
                }
                data_cond.notify_one();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        };

        auto thread_func2 = [&]() {
            std::cout << "thread_func2 entered()" << std::endl;
            std::unique_lock<std::mutex> lock(m);
            data_cond.wait(lock, [&] {
                std::cout << "Check condition " << std::endl;
                return data.size() > 3;
            });
            std::cout << "thread_func2 released" << std::endl;
        };

        std::thread th1(thread_func1);
        std::thread th2(thread_func2);
        th1.join();
        th2.join();
    }
}

namespace ConditionVariable::SimpleTest {

    void Test() {
        std::mutex mtx;
        std::condition_variable trigger;

        LOG << "Started..." << std::endl;

        const unsigned int timeout = 7;
        std::future<void> producer = std::async(std::launch::async, [&trigger](unsigned int timeout)-> void {
            LOG << "Sleeping for " << timeout << " seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            LOG << "Done. Notify condition variable." << std::endl;
            trigger.notify_one();
        }, timeout);

        std::unique_lock<std::mutex> lock(mtx);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        LOG << "Waiting for trigger..." << std::endl;
        trigger.wait(lock);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        LOG << "Done" << std::endl;
    }

    void WaitFor() {
        std::mutex mtx;
        std::condition_variable trigger;

        LOG << "Started..." << std::endl;

        const unsigned int timeout = 7;
        std::future<void> future = std::async(std::launch::async, [&trigger](unsigned int timeout)-> void {
            LOG << "Sleeping for " << timeout << " seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            std::cout << std::endl;
            LOG << "Done. Notify condition variable." << std::endl;
            trigger.notify_one();
        }, timeout);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));


        std::unique_lock<std::mutex> lock(mtx);
        while (trigger.wait_for(lock, std::chrono::milliseconds(100)) == std::cv_status::timeout) {
            std::cout << '.';
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        LOG << "Done" << std::endl;
    }

    void WaitFor_1() {
        std::mutex mtx;
        std::condition_variable trigger;
        int i = 0;

        LOG << "Started..." << std::endl;

        auto consumer = [&](int idx) {
            std::unique_lock<std::mutex> lock(mtx);
            if (trigger.wait_for(lock, idx * 1000ms, [&] {return i == 1; }))
                LOG << "Thread " << idx << " finished waiting. i == " << i << std::endl;
            else
                LOG << "Thread " << idx << " timed out. i == " << i << std::endl;
        };

        auto producer = [&]() {
            std::this_thread::sleep_for(1200ms);
            LOG << "Producer: Notifying..." << std::endl;
            trigger.notify_all();
            std::this_thread::sleep_for(1000ms);
            {
                std::lock_guard<std::mutex> lock(mtx);
                i = 1;
            }
            LOG << "Producer: Notifying..." << std::endl;
            trigger.notify_all();
        };

        std::thread t1(consumer, 1), t2(consumer, 2), t3(consumer, 3), t4(producer);
        t1.join();
        t2.join();
        t3.join();
        t4.join();


        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        LOG << "Done" << std::endl;
    }

    void WaitFor_FullfilledConditionAtStart() {
        std::mutex mtx;
        std::condition_variable trigger;
        bool state {false};

        LOG << "Started..." << std::endl;

        auto consumer = [&]() {
            std::unique_lock<std::mutex> lock(mtx);
            if (trigger.wait_for(lock, std::chrono::seconds(3), [&] {return state; }))
                LOG << "Consumer thread " << std::this_thread::get_id() << " finished waiting"<< std::endl;
            else
                LOG << "Consumer thread " << std::this_thread::get_id() << " timed out" << std::endl;
        };

        auto producer = [&]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            {
                std::lock_guard<std::mutex> lock(mtx);
                // state = true;
                // We will have TIMEOUT for CONDTION_WARIABLE if we'll not set state == TRUE
            }
            LOG << "Producer: Notifying..." << std::endl;
            trigger.notify_all();
        };

        std::thread t1(consumer), t2(producer);
        t1.join();
        t2.join();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        LOG << "Done" << std::endl;
    }

    void WaitFor_DiffThreads() {
        std::mutex mtx;
        std::condition_variable trigger;

        const unsigned int timeout = 7;
        std::future<void> producer = std::async(std::launch::async, [&trigger](unsigned int timeout)-> void {
            LOG << "Producer: Started. timeout =  " << timeout << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            std::cout << std::endl;
            LOG << "Producer: Done. Notify condition variable." << std::endl;
            trigger.notify_one();
        }, timeout);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::future<void> consumer = std::async(std::launch::async, [&trigger, &mtx]()-> void {
            LOG << "Consumer: Started. " << std::endl;
            std::unique_lock<std::mutex> lock(mtx);
            while (trigger.wait_for(lock, std::chrono::milliseconds(100)) == std::cv_status::timeout) {
                std::cout << '.';
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            LOG << "Consumer: Done" << std::endl;
        });
    }

    void WaitFor_DiffThreads_Long() {
        std::mutex mtx;
        std::condition_variable trigger;

        const unsigned int timeout = 16;
        std::future<void> producer = std::async(std::launch::async, [&trigger](unsigned int timeout)-> void {
            LOG << "Producer: Started. timeout =  " << timeout << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            std::cout << std::endl;
            LOG << "Producer: Done. Notify condition variable." << std::endl;
            trigger.notify_one();
        }, timeout);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::future<void> consumer = std::async(std::launch::async, [&trigger, &mtx](int timeout)-> void {
            LOG << "Consumer: Started. " << std::endl;
            std::unique_lock<std::mutex> lock(mtx);
            while (trigger.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout) {
                LOG << "Consumer: Timeout " << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            LOG << "Consumer: Done" << std::endl;
        }, 5);
    }

    void WaitFor_Consumer_Producer() {
        std::mutex mtx;
        std::condition_variable trigger;

        [[maybe_unused]]
        int i = 0;

        auto consumer_func = [&](int timeout) {
            while (true) {
                LOG << "Consumer: Entered..." << std::endl;
                std::unique_lock<std::mutex> lock(mtx);
                while (trigger.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout) {
                    LOG << "Consumer: Timeout " << std::endl;
                }
                std::cout << "Consumer: Do some work..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(3));
                std::cout << "Consumer: Done." << std::endl;
            }
        };

        auto producer_func = [&](int timeout) {
            while (true) {
                LOG << "Producer: Entered..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(timeout));
                LOG << "Producer: Send message." << std::endl;
                std::unique_lock<std::mutex> lock(mtx);
                std::cout << std::endl;
                LOG << "Producer: Done. Notify condition variable." << std::endl;
                trigger.notify_one();
            }
        };

        std::thread consumer(consumer_func, 1);
        std::thread producer(producer_func, 5);

        consumer.join();
        producer.join();
    }

    //-----------------------------------------------------------------------------------------------------//

    void WaitUntil_OK() {
        std::mutex mtx;
        std::condition_variable trigger;
        std::atomic<int> index = ATOMIC_VAR_INIT(0);

        LOG << "Started..." << std::endl;

        const unsigned int timeout = 7;
        std::future<void> future = std::async(std::launch::async, [&trigger, &index](unsigned int timeout)-> void {
            LOG << "Sleeping for " << timeout << " seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            std::cout << std::endl;
            LOG << "Done. Notify condition variable." << std::endl;
            index = 1;
            trigger.notify_one();
        }, timeout);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::unique_lock<std::mutex> lock(mtx);
        auto now = std::chrono::system_clock::now();

        unsigned short wait_timeout = 8;
        LOG << "Waiting for " << wait_timeout << " seconds." << std::endl;
        if (trigger.wait_until(lock, now + std::chrono::milliseconds(wait_timeout * 1000), [&]() {return index == 1; })) {
            LOG << "Finished waiting. i == " << index << std::endl;
            return;
        }
        else {
            LOG << "Timed out. i == " << index << std::endl;
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        LOG << "Done" << std::endl;
    }

    void WaitUntil_Timeout() {
        std::mutex mtx;
        std::condition_variable trigger;
        std::atomic<int> index = ATOMIC_VAR_INIT(0);

        LOG << "Started..." << std::endl;

        const unsigned int timeout = 7;
        std::future<void> future = std::async(std::launch::async, [&trigger, &index](unsigned int timeout)-> void {
            LOG << "Sleeping for " << timeout << " seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            std::cout << std::endl;
            LOG << "Done. Notify condition variable." << std::endl;
            index = 1;
            trigger.notify_one();
        }, timeout);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::unique_lock<std::mutex> lock(mtx);
        auto now = std::chrono::system_clock::now();

        unsigned short wait_timeout = 5;
        LOG << "Waiting for " << wait_timeout << " seconds." << std::endl;
        if (trigger.wait_until(lock, now + std::chrono::milliseconds(wait_timeout * 1000), [&]() {return index == 1; })) {
            LOG << "Finished waiting. i == " << index << std::endl;
            return;
        } else {
            LOG << "Timed out. i == " << index << std::endl;
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        LOG << "Done" << std::endl;
    }

    void WaitUntil_OK_PredicateFalse() {
        std::mutex mtx;
        std::condition_variable trigger;
        std::atomic<int> index = ATOMIC_VAR_INIT(0);


        std::future<void> producer = std::async(std::launch::async, [&trigger](unsigned int timeout)-> void {
            LOG << "Producer: Sleeping for " << timeout << " seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            std::cout << std::endl;
            LOG << "Producer: Notify ... [Index = 0 !!!!!]" << std::endl;
            // index = 1;
            trigger.notify_one();
        }, 3 /* Timeout = 3 sec*/);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::future<void> consumer = std::async(std::launch::async, [&](unsigned int timeout)-> void {
            LOG << "Consumer: Waiting for " << timeout << " seconds." << std::endl;

            std::unique_lock<std::mutex> lock(mtx);
            while (true) {
                auto now = std::chrono::system_clock::now();
                if (trigger.wait_until(lock, now + std::chrono::milliseconds(timeout * 1000), [&]() {return index == 1; })) {
                    LOG << "Consumer:Finished waiting. i == " << index << std::endl;
                    return;
                } else {
                    LOG << "Consumer: Timed out. i == " << index << std::endl;
                }
            }
        }, 1 /* Timeout = 3 sec*/);
    }
}


namespace ConditionVariable::NotifyAtExit {

    void SimpleTest()
    {
        std::mutex m;
        std::condition_variable cv;
        bool ready = false;
        std::string result;

        auto func = [&](unsigned int timeout) {
            LOG << "Starting thread." << std::endl;
            thread_local std::string thread_local_data = "42";

            std::unique_lock<std::mutex> lk(m);

            std::this_thread::sleep_for(std::chrono::seconds(timeout));

            // assign a value to result using thread_local data
            result = thread_local_data;
            ready = true;

            LOG << "Thread done" << std::endl;
            std::notify_all_at_thread_exit(cv, std::move(lk));
        };


        std::thread task(func, 5);
        task.detach();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        LOG << "Waiting for thread to exit...." << std::endl;

        // wait for the detached thread
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&](){
            return ready;
        });

        LOG << "Main done" << std::endl;
        // result is ready and thread_local destructors have finished, no UB
        assert(result == "42");
    }


    void SetValue_AtExit()
    {
        std::promise<int> promise;
        std::future<int> future = promise.get_future();
        std::thread([&promise] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            promise.set_value_at_thread_exit(9);
        }).detach();

        LOG << "Waiting for thread to exit...." << std::endl;
        future.wait();

        LOG << "Done!!!. Result = " << future.get() << std::endl;
    }
}

namespace ConditionVariable::VariableAny {

    void NotifyAll_Test1() {
        std::condition_variable_any cv;
        std::mutex cv_m;
        bool finished {false};

        auto waits = [&]()-> void {
            std::unique_lock<std::mutex> lk(cv_m);
            LOG << "Waiting... ." << std::endl;
            cv.wait(lk, [&] { return finished; });
            LOG << "...finished waiting. finished!!!" << std::endl;
        };

        auto signals = [&]()-> void {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            {
                std::lock_guard<std::mutex> lk(cv_m);
                LOG << "Notifying...." << std::endl;
            }

            cv.notify_all();
            std::this_thread::sleep_for(std::chrono::seconds(1));

            {
                std::lock_guard<std::mutex> lk(cv_m);
                finished = true;
                LOG << "Notifying again...." << std::endl;
            }

            cv.notify_all();
        };

        auto task1 = std::async(std::launch::async, waits);
        auto task2 = std::async(std::launch::async, waits);
        auto task3 = std::async(std::launch::async, waits);
        auto task4 = std::async(std::launch::async, signals);

        task1.wait();
        task2.wait();
        task3.wait();
        task4.wait();
    }

    void NotifyAll_CV_ANY() {
        std::mutex mtx;
        std::condition_variable_any trigger;
        std::atomic<bool> ready{ false };

        LOG << "Started..." << std::endl;

        auto print_id = [&](int id)-> void {
            LOG << "Print thread " << id << " started." << std::endl;
            std::lock_guard<std::mutex> lock(mtx);
            while (!ready) {
                trigger.wait(mtx);
            }
            LOG << "Print thread " << id << " done." << std::endl;
        };

        const auto go = [&]()-> void {
            std::lock_guard<std::mutex> lock(mtx);
            std::this_thread::sleep_for(std::chrono::seconds(5));
            ready = true;
            LOG << "'notify_all'." << std::endl;
            trigger.notify_all();
        };

        std::thread threads[10];
        for (int i = 0; i < 10; ++i)
            threads[i] = std::thread(print_id, i);

        LOG << "10 threads ready to race..." << std::endl;
        /** GO!!! **/
        go();
        for (auto& th : threads)
            th.join();
        LOG << "Done" << std::endl;
    }

    void NotifyAll_CV_not_Any() {
        std::mutex mtx;
        std::condition_variable trigger;
        LOG << "Started..." << std::endl;

        auto print_id = [&](int id)-> void {
            LOG << "Print thread " << id << " started." << std::endl;
            std::unique_lock<std::mutex> lock(mtx);
            trigger.wait(lock);
            LOG << "Print thread " << id << " done." << std::endl;
        };

        const auto go = [&]()-> void {
            std::lock_guard<std::mutex> lock(mtx);
            std::this_thread::sleep_for(std::chrono::seconds(5));
            LOG << "'notify_all'." << std::endl;
            trigger.notify_all();
        };

        std::thread threads[10];
        for (int i = 0; i < 10; ++i)
            threads[i] = std::thread(print_id, i);

        LOG << "10 threads ready to race..." << std::endl;
        go();
        for (auto& th : threads)
            th.join();
        LOG << "Done" << std::endl;
    }
}


namespace ConditionVariable::Experiments
{

    struct synch_stream final : public std::stringstream {
    private:
        static inline std::mutex mtx;
        const static inline std::thread::id mainThreadId { std::this_thread::get_id() };

    private:
        static std::string getCurrentTime() noexcept {
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
            info.append(synch_stream::getCurrentTime()).append("] Thread [");

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

    void Consumer_BlockingProducer()
    {
        std::mutex mtx;
        std::condition_variable tasksListUpdated;
        std::deque<std::string> tasks;
        constexpr size_t MAX_CAPACITY {50};

        size_t counter {0};
        auto consumer = [&](int timeout) {
            while (true) {
                std::unique_lock<std::mutex> lock(mtx);
                while (!tasksListUpdated.wait_for(lock, std::chrono::milliseconds(timeout), [&] {
                    return not tasks.empty();
                })) { /** Timeout **/ }

                const auto task = std::move(tasks.front());
                tasks.pop_front();
                // synch_stream() << "Consumer: Processing task " << task << std::endl;

                ++counter;
                lock.unlock();

                if (0 == counter % 1000)
                    std::cout << counter << std::endl;

                tasksListUpdated.notify_all();
            }
        };

        auto producer = [&](int timeout) {
            while (true)
            {
                if (std::unique_lock<std::mutex> lock {mtx}; tasks.size() < MAX_CAPACITY) {
                    tasks.emplace_back("Task");
                }
                else {
                    while (!tasksListUpdated.wait_for(lock, std::chrono::milliseconds(timeout), [&] {
                        return tasks.size() < MAX_CAPACITY;
                    })) { /** Timeout **/ }
                    tasks.emplace_back("Task");
                }
                tasksListUpdated.notify_all();
            }
        };

        constexpr size_t PRODUCERS_COUNT = 3, CONSUMERS_COUNT {3};
        std::vector<std::thread> producers {}, consumers {};
        for (size_t i = 0; i < CONSUMERS_COUNT; ++i)
            consumers.emplace_back(consumer, 1000);
        for (size_t i = 0; i < PRODUCERS_COUNT; ++i)
            producers.emplace_back(producer, 2500);

        std::for_each(producers.begin(), producers.end(), [](auto& T) { T.join(); });
        std::for_each(consumers.begin(), consumers.end(), [](auto& T) { T.join(); });
    }
}

namespace ConditionVariable::ResourceClass
{
    struct Resource
    {
        bool full {false};
        std::mutex mux {};
        std::condition_variable cond {};

        void produce()
        {
            {
                std::unique_lock lock(mux);
                // wait until the condition is true
                // 1. the lock is released
                // 2. when the thread is woken up, the lock is reacquired and the condition checked
                // 3. if the condition is still not true, the lock is rereleased, and we go to step 2.
                // 4. if the condition is true, the wait() call finishes
                cond.wait(lock, [this] { return !full; });
                std::osyncstream(std::cout) << "Filling the resource and notifying the consumer.\n";
                full = true;
                std::this_thread::sleep_for(200ms);
            }

            // wake up one thread waiting on this condition variable
            // note that we already released our lock, otherwise
            // the notified thread would wake up and fail to acquire
            // the lock and suspend itself again
            cond.notify_one();
        }

        void consume()
        {
            {
                std::unique_lock lock(mux);
                // same as above, but with opposite semantics
                cond.wait(lock, [this]{ return full; });
                std::osyncstream(std::cout) << "Consuming the resource and notifying the producer.\n";
                full = false;
                std::this_thread::sleep_for(200ms);
            }
            cond.notify_one();
        }
    };

    void Consume_Produce()
    {
        Resource resource;

        auto t1 = std::jthread([&resource](const std::stop_token& token){
            while (!token.stop_requested())
                resource.produce();
        });

        auto t2 = std::jthread([&resource](const std::stop_token& token){
            while (!token.stop_requested())
                resource.consume();
        });

        std::this_thread::sleep_for(2s);
        t1.request_stop();
        t2.request_stop();
    }
}

namespace ConditionVariable::PingPongGame
{
    struct Engine
    {
        static constexpr int MaxCountTimes {1'000'000};

        std::condition_variable cv;
        std::mutex mtx;
        bool pingDone { false };
        bool pongDone { false };

        void ping()
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

        void pong()
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

        void start()
        {
            {
                std::unique_lock lck { mtx };
                pingDone = true;
            }
            cv.notify_one();
        }
    };


    void Test()
    {
        Engine engine;
        engine.start();

        std::thread ping = std::thread(&Engine::ping, &engine);
        std::thread pong = std::thread(&Engine::pong, &engine);

        if (ping.joinable()) {
            ping.join();
        }
        if (pong.joinable()) {
            pong.join();
        }

        std::cout << "Done\n";
    }
}


namespace ConditionVariable::PredicateTests
{
    void TriggerPredicateWithoutLock()
    {
        int value {0};
        bool updated {false};
        std::condition_variable cv;
        std::mutex mtx;

        auto consumer = [&](const int timeout) -> void
        {
            while (true) {
                std::unique_lock<std::mutex> lock(mtx);
                while (!cv.wait_for(lock, std::chrono::milliseconds(timeout), [&] {
                    return updated;
                })) { /** Timeout **/
                    std::osyncstream {std::cout} << CurrentTime{} << "Timeout\n";
                }
                break;
            }

            std::osyncstream {std::cout} << CurrentTime{} << "Done. value = " << value << std::endl;
        };

        std::jthread consume(consumer, 3000);

        std::this_thread::sleep_for(std::chrono::milliseconds (3500));
        std::osyncstream {std::cout} << CurrentTime{} << "Updating value\n";

        updated = true;

        /** if 'cv.notify_one()' is not called will be delay between updated = true and  "Done. "
         * from the 'consume' thread **/
        // cv.notify_one();
    }
}

void ConditionVariable::TEST_ALL()
{
    // Classic_Test::Test();
    // Classic_Test::Test_Predicate_0();
    // Classic_Test::Test_Predicate();
    // Classic_Test::Wait_PusthBack_2Vector();
    // Classic_Test::Wait_Pust_2Queue();

    // SimpleTest::Test();

    // SimpleTest::WaitFor();
    // SimpleTest::WaitFor_1();
    // SimpleTest::WaitFor_FullfilledConditionAtStart();
    // SimpleTest::WaitFor_DiffThreads_Long();
    // SimpleTest::WaitFor_DiffThreads();
    // SimpleTest::WaitFor_Consumer_Producer();

    // SimpleTest::WaitUntil_OK();
    // SimpleTest::WaitUntil_OK_PredicateFalse();
    // SimpleTest::WaitUntil_Timeout();

    // NotifyAtExit::SimpleTest();
    // NotifyAtExit::SetValue_AtExit();

    // VariableAny::NotifyAll_Test1();
    // VariableAny::NotifyAll_CV_ANY();
    // VariableAny::NotifyAll_CV_not_Any();

    // ResourceClass::Consume_Produce();

    // Experiments::Consumer_BlockingProducer();

    // PingPongGame::Test();

    PredicateTests::TriggerPredicateWithoutLock();
};
