/**============================================================================
Name        : Single_ConsumerProducerQueue_Two.cpp
Created on  : 12.09.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Single_ConsumerProducerQueue_Two.cpp
============================================================================**/

#include "Single_ConsumerProducerQueue_Two.h"

#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include <atomic>
#include <thread>
#include <chrono>
#include <syncstream>

namespace Single_ConsumerProducerQueue_Two
{
    template <typename T>
    struct Queue
    {
        using value_type = T;
        using size_type = size_t;
        using pointer = value_type*;

        static_assert(!std::is_same_v<T, void>, "Type of the Queue can not be void");
        // static_assert(0 != Capacity, "Please try a little bigger buffer");

        explicit Queue(size_type capacity) :
                capacity { capacity },
                buffer { new value_type[capacity] } {
        }

        ~Queue() {
            delete[] buffer;
        }

        bool push(const value_type& v)
        {
            if (size.load(std::memory_order_relaxed) >= capacity)
                return false;

            new (buffer + (back % capacity)) value_type(v);
            ++back;
            size.fetch_add(1, std::memory_order_release);
            return true;
        }

        std::optional<value_type> pop()
        {
            if (size.load(std::memory_order_acquire) == 0)
                return std::optional<value_type>(std::nullopt);

            const size_type idx = front % capacity;
            std::optional<value_type> res(std::move(buffer[idx]));
            buffer[idx].~value_type();
            ++front;
            size.fetch_sub(1, std::memory_order_relaxed);

            return res;
        }

    private:

        const size_type capacity { 0 };
        pointer buffer { nullptr };
        size_type front { 0 };
        size_type back { 0 };
        std::atomic<size_type> size { 0 };
    };
};

void Single_ConsumerProducerQueue_Two::TestAll()
{

};