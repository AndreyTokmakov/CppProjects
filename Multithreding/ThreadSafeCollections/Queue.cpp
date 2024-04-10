/**============================================================================
Name        : Queue.cpp
Created on  : 12.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Queue.cpp
============================================================================**/

#include "Queue.h"

#include <iostream>
#include <string_view>
#include <thread>
#include <condition_variable>
#include <vector>
#include <deque>
#include <future>
#include <syncstream>
#include <format>
#include <chrono>
#include "../ThreadHelperUtilities/ThreadHelperUtilities.h"
#include "../Integer/Integer.h"



namespace
{
    template<typename T>
    T add_space(const T& arg) {
        std::osyncstream {std::cout} << ' ';
        return arg;
    };

    template<typename ...Args>
    void print(Args&&... args)
    {
        const std::chrono::time_point now = std::chrono::system_clock::now();
        std::cout << std::format("{:%d-%m-%Y %H:%M:%OS}", now) << ":";
        (std::osyncstream {std::cout}  << ... << add_space(std::forward<Args>(args))) << std::endl;
    }

    template<typename ...Args>
    void FoldPrintEx(Args&&... args) {
        auto add_space = []<typename T>(const T& arg) {
            std::osyncstream {std::cout} << ' ';
            return arg;
        };
        (std::osyncstream {std::cout} << ... << add_space(std::forward<Args>(args))) << std::endl;
    }


}

namespace QueueCV
{
    template<typename T>
    class Queue
    {
        using value_type = T;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        mutable std::mutex mutex;
        std::deque<value_type> data_queue;
        std::condition_variable updated;

    public:
        Queue() = default;

        void push(value_type new_value)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                data_queue.push_back(std::move(new_value));
            }
            updated.notify_one();
        }

        template <typename... Args>
        void emplace(Args&& ... args)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                data_queue.emplace_back(std::forward<Args>(args)...);
            }
            updated.notify_one();
        }

        /*
        template <typename... Args>
        void AddRange(Args&&... args) {
            std::lock_guard<std::mtx> lock(mtx);
            (elements.push_back(args), ...);
        }
        */

        void wait_and_pop(value_type& value)
        {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == data_queue.empty();
            });
            value = std::move(data_queue.front());
            data_queue.pop_front();
        }


        template<class _Rep, class _Period>
        bool wait_for_and_pop(value_type& value,
                              const std::chrono::duration<_Rep, _Period>& _Rel_time)
        {
            std::unique_lock<std::mutex> lock(mutex);
            bool ok = updated.wait_for(lock, _Rel_time, [this] {
                return false == data_queue.empty();
            });
            if (!ok)
                return false;
            value = std::move(data_queue.front());
            data_queue.pop_front();
            return true;
        }

        value_type&& wait_and_pop()
        {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == data_queue.empty();
            });
            auto&& entry = data_queue.front();
            data_queue.pop_front();
            return std::move(entry);
        }

        bool try_pop(value_type& value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (data_queue.empty())
                return false;
            value = std::move(data_queue.front());
            data_queue.pop_front();
            return true;
        }

        std::shared_ptr<value_type> try_pop()
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (data_queue.empty())
                return std::shared_ptr<value_type>();
            std::shared_ptr<value_type> result =
                    std::make_shared<value_type>(std::move(data_queue.front()));
            data_queue.pop_front();
            return result;
        }

        bool empty() const noexcept
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.empty();
        }

        size_t size() const noexcept
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.size();
        }
    };
}

namespace QueueAtomic
{
    template<typename T>
    class Queue
    {
        using value_type = T;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        mutable std::mutex mutex;
        std::deque<value_type> data_queue;
        std::atomic_flag updated {false};

    public:
        Queue() = default;

        void push(value_type new_value)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                data_queue.push_back(std::move(new_value));
            }
            updated.test_and_set(std::memory_order_release);
            updated.notify_one();
        }

        template <typename... Args>
        void emplace(Args&& ... args)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                data_queue.emplace_back(std::forward<Args>(args)...);
            }
            updated.test_and_set();
            updated.notify_one();
        }

        /*
        template <typename... Args>
        void AddRange(Args&&... args) {
            std::lock_guard<std::mtx> lock(mtx);
            (elements.push_back(args), ...);
        }
        */

        void wait_and_pop(value_type& value)
        {
            updated.wait(false, std::memory_order_acquire);
            updated.clear();

            {
                std::lock_guard<std::mutex> lock(mutex);
                value = std::move(data_queue.front());
                data_queue.pop_front();
            }
        }

        /*
        template<class _Rep, class _Period>
        bool wait_for_and_pop(value_type& value,
                              const std::chrono::duration<_Rep, _Period>& _Rel_time)
        {
            std::unique_lock<std::mutex> lock(mutex);
            bool ok = updated.wait_for(lock, _Rel_time, [this] {
                return false == data_queue.empty();
            });
            if (!ok)
                return false;
            value = std::move(data_queue.front());
            data_queue.pop_front();
            return true;
        }
        */

        value_type&& wait_and_pop()
        {
            updated.wait(false, std::memory_order_acquire);
            updated.clear();

            {
                std::lock_guard<std::mutex> lock(mutex);
                auto&& entry = data_queue.front();
                data_queue.pop_front();
                return std::move(entry);
            }
        }

        /*
        bool try_pop(value_type& value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (data_queue.empty())
                return false;
            value = std::move(data_queue.front());
            data_queue.pop_front();
            return true;
        }

        std::shared_ptr<value_type> try_pop()
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (data_queue.empty())
                return std::shared_ptr<value_type>();
            std::shared_ptr<value_type> result =
                    std::make_shared<value_type>(std::move(data_queue.front()));
            data_queue.pop_front();
            return result;
        }
        */

        bool empty() const noexcept
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.empty();
        }

        size_t size() const noexcept
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.size();
        }
    };
}



namespace Tests
{
    using namespace QueueCV;
    using namespace QueueAtomic;

    using Int = Integer<false>;
    using Queue = QueueAtomic::Queue<Int>;
    // using Queue = QueueCV::Queue<Int>;


    void RunTest1()
    {
        Queue queue;

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

    /*
    void RunTest_WaitFor()
    {
        Queue queue;

        std::future<void> producer = std::async(std::launch::async, [&]()-> void {
            THREAD_INFO << "Producer: started." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            queue.emplace(1);
            THREAD_INFO << "Producer: done" << std::endl;
        });


        std::future<void> consumer = std::async(std::launch::async, [&]()-> void {
            THREAD_INFO << "Consumer: started" << std::endl;
            Int entry;
            while (true)
            {
                const bool ok = queue.wait_for_and_pop(entry, std::chrono::milliseconds(250));
                if (ok)
                    break;
                THREAD_INFO << "Timeout" << std::endl;
            }

            THREAD_INFO << "Consumer: done. Value: ";
            entry.printInfo();
        });

        producer.wait();
        consumer.wait();

        THREAD_INFO << "Done!!" << std::endl;
    }
     */


    void RunTest_Wait_NoTimeout()
    {
        Queue queue;

        std::future<void> producer = std::async(std::launch::async, [&]()-> void {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            queue.emplace(1);
            THREAD_INFO << "Producer: done" << std::endl;
        });


        std::future<void> consumer = std::async(std::launch::async, [&]()-> void {
            Int entry;
            queue.wait_and_pop(entry);
            THREAD_INFO << "Consumer: done. Value: ";
            entry.printInfo();
        });

        producer.wait();
        consumer.wait();
    }
}


void Queue::TestAll()
{
    // print(1, 2, 3);
    // FoldPrintEx(1, 2, 3);

    // Tests::RunTest1();
    // Tests::RunTest_WaitFor();
    Tests::RunTest_Wait_NoTimeout();
};