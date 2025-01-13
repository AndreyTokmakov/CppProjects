//============================================================================
// Name        : Multithreading_Communication.cpp
// Created on  : 21.11.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ MultiThrading communication src
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

#include "../Integer/Integer.h"
#include "../Utilities/Utilities.h"
#include "Multithreading_Communication.h"

namespace Multithreading_Communication {

    template<typename T>
    class Queue {
    private:
        mutable std::mutex mutex;
        std::deque<T> qeque;
        std::condition_variable updated;

    public:
        Queue(size_t capacity = 0): qeque(capacity){
        }

        void push(T&& new_value) {
            std::lock_guard<std::mutex> lock(mutex);
            qeque.push_back(std::move(new_value));
            updated.notify_one();
        }

        template <typename... Args>
        void emplace(Args&& ... args) {
            std::lock_guard<std::mutex> lock(mutex);
            qeque.emplace_back(std::forward<Args>(args)...);
            updated.notify_one();
        }

        void wait_and_pop(T& value) {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == qeque.empty();
            });
            value = std::move(qeque.front());
            qeque.pop_front();
        }

        template<class _Rep, class _Period>
        bool wait_for_and_pop(T& value, const std::chrono::duration<_Rep, _Period>& _Rel_time) {
            std::unique_lock<std::mutex> lock(mutex);
            bool ok = updated.wait_for(lock, _Rel_time, [this] {
                return false == qeque.empty();
            });
            if (false == ok)
                return false;
            value = std::move(qeque.front());
            qeque.pop_front();
            return true;
        }

        T&& wait_and_pop() {
            std::unique_lock<std::mutex> lock(mutex);
            updated.wait(lock, [this] {
                return false == qeque.empty();
            });
            auto&& entry = qeque.front();
            qeque.pop_front();
            return std::move(entry);
        }

        bool try_pop(T& value) {
            std::lock_guard<std::mutex> lock(mutex);
            if (qeque.empty())
                return false;
            value = std::move(qeque.front());
            qeque.pop_front();
            return true;
        }

        std::shared_ptr<T> try_pop() {
            std::lock_guard<std::mutex> lock(mutex);
            if (qeque.empty())
                return std::shared_ptr<T>();
            std::shared_ptr<T> result =
                    std::make_shared<T>(std::move(qeque.front()));
            qeque.pop_front();
            return result;
        }

        bool empty() const noexcept {
            std::lock_guard<std::mutex> lock(mutex);
            return qeque.empty();
        }

        bool size() const noexcept {
            std::lock_guard<std::mutex> lock(mutex);
            return qeque.size();
        }
    };
};

namespace Multithreading_Communication::Tests {

    void Test()
    {
        Queue<std::string> queue;
        std::unordered_map<std::thread::id, size_t> stats;

        const auto producer = [&queue]()->void {
            THREAD_INFO << "Producer started" << std::endl;
            for (int i = 0; i < 10000000; i++) {
                queue.emplace(std::string("Message_").append(std::to_string(i)));
                // std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
            THREAD_INFO << "Producer done" << std::endl;
        };

        const auto consumer = [&queue, &stats]()->void {
            THREAD_INFO << "Consumer started" << std::endl;
            std::string message;
            std::thread::id id = std::this_thread::get_id();
            while (true) {
                auto result = queue.wait_for_and_pop(message, std::chrono::milliseconds(2000));
                if (true == result) {
                    stats[id]++;
                }
            }
        };

        std::vector<std::future<void>> consumers, producers;
        for (int i = 0; i < 20; i++)
            consumers.emplace_back(std::async(consumer));
        for (int i = 0; i < 10; i++)
            producers.emplace_back(std::async(producer));


        for (auto& P : producers)
            P.wait();
        while (false == queue.empty())
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Test done" << std::endl;


        size_t count = 0;
        for (auto& [k, v] : stats) {
            std::cout << k << " = " << v << std::endl;
            count += v;
        }
        std::cout << "Total count: " << count << std::endl;
    }
}

void Multithreading_Communication::TEST_ALL() {
    Tests::Test();


};
