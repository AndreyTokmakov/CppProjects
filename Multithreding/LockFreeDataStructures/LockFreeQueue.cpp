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

namespace LockFreeQueue {
    template<class T, size_t N>
    class LockFreeQueue {
    public:
        LockFreeQueue() : read_pos_{0}, write_pos_{0}, size_{0} {
            //assert(size_.is_lock_free());
            if (!size_.is_lock_free()) {
                std::cout << "ERROR!\n";
            }
        }

        auto size() const {
            return size_.load();
        }

        auto push(const T &t) {
            if (size_.load() >= N) {
                throw std::overflow_error("Queue is full");
            }
            buffer_[write_pos_] = t;
            write_pos_ = (write_pos_ + 1) % N;
            size_.fetch_add(1);
        }

        auto &front() const {
            const auto s = size_.load();
            if (s == 0) {
                throw std::underflow_error("Queue is empty");
            }
            return buffer_[read_pos_];
        }

        auto pop() {
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

}

void LockFreeQueue::TEST_ALL() {

    LockFreeQueue<int, 5> queue{};
    queue.push(1);
    queue.push(2);

    std::cout << queue.size() << std::endl;
}