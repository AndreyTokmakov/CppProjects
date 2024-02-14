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

namespace LockFreeQueue
{
    template<class T, size_t N>
    struct LockFreeQueue
    {
        LockFreeQueue() : size {0}, read_pos{0}, write_pos{0} {
            //assert(size_.is_lock_free());
            if (!size.is_lock_free()) {
                std::cout << "ERROR!\n";
            }
        }

        [[nodiscard]]
        auto Size() const {
            return size.load();
        }

        auto push(const T &t) {
            if (size.load() >= N) {
                throw std::overflow_error("Queue is full");
            }
            data[write_pos] = t;
            write_pos = (write_pos + 1) % N;
            size.fetch_add(1);
        }

        [[nodiscard]]
        auto& front() const {
            const auto s = size.load();
            if (s == 0) {
                throw std::underflow_error("Queue is empty");
            }
            return data[read_pos];
        }

        auto pop() {
            if (size.load() == 0) {
                throw std::underflow_error("Queue is empty");
            }
            read_pos = (read_pos + 1) % N;
            size.fetch_sub(1);
        }

    private:
        std::array<T, N> data {};
        std::atomic<size_t> size {};
        size_t read_pos = 0;
        size_t write_pos = 0;
    };

}

void LockFreeQueue::TEST_ALL() {

    LockFreeQueue<int, 5> queue{};
    queue.push(1);
    queue.push(2);

    std::cout << queue.Size() << std::endl;
}