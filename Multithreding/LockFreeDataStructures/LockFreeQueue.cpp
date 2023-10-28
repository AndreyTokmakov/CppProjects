//============================================================================
// Name        : LockFreeQueue.cpp
// Created on  : 03.06.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : LockFreeQueue
//============================================================================

#include <iostream>
#include <optional>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>
#include <experimental/propagate_const>

#include <thread>
#include <future>

#include <mutex>
#include <atomic>

#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <array>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <span>
#include <concepts>


#include "LockFreeQueue.h"

namespace LockFreeQueue::One
{
    template<class T, size_t N>
    class LockFreeQueue {
    public:
        LockFreeQueue() :size_{0},  read_pos_{0}, write_pos_{0}
        {
            // assert(size_.is_lock_free());
            if (!size_.is_lock_free()) {
                std::cout << "ERROR!\n";
            }
        }

        [[nodiscard]]
        size_t size() const
        {
            return size_.load();
        }

        auto push(const T &t)
        {
            if (size_.load() >= N) {
                throw std::overflow_error("Queue is full");
            }
            buffer_[write_pos_] = t;
            write_pos_ = (write_pos_ + 1) % N;
            size_.fetch_add(1);
        }

        [[nodiscard]]
        size_t& front() const
        {
            const auto s = size_.load();
            if (s == 0) {
                throw std::underflow_error("Queue is empty");
            }
            return buffer_[read_pos_];
        }

        auto pop()
        {
            if (size_.load() == 0) {
                throw std::underflow_error("Queue is empty");
            }
            read_pos_ = (read_pos_ + 1) % N;
            size_.fetch_sub(1);
        }

    private:
        std::array <T, N> buffer_{}; // Used by both threads
        std::atomic <size_t> size_{}; // Used by both threads
        size_t read_pos_ = 0;
        size_t write_pos_ = 0;
    };

    void test()
    {
        LockFreeQueue<int, 5> queue{};
        queue.push(1);
        queue.push(2);

        std::cout << queue.size() << std::endl;
    }
}

namespace LockFreeQueue::Two
{

    template<typename T>
    struct lock_free_queue
    {
        struct node
        {
            std::shared_ptr<T> data;
            std::atomic<node*> next;
            explicit node(T const& data): data(std::make_shared<T>(data)) {}
        };

        std::atomic<node*> head;
        std::atomic<node*> tail;

    public:
        void push(T const& data)
        {
            node* new_node = new node(data);
            node* old_tail = tail.load();
            while (!old_tail->next.compare_exchange_weak(reinterpret_cast<node*>(nullptr), new_node)){
                old_tail = tail.load();
            }
            tail.compare_exchange_weak(old_tail, new_node);
        }

        std::shared_ptr<T> pop()
        {
            node* old_head=head.load();
            while (old_head && !head.compare_exchange_weak(old_head, old_head->next)) {
                old_head=head.load();
            }
            return old_head ? old_head->data : std::shared_ptr<T>();
        }
    };

    void test()
    {
        // lock_free_queue<int> queue;
        // queue.push(1);
    }
}

void LockFreeQueue::TEST_ALL()
{
    // One::test();
    Two::test();
}