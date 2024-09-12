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

            back = back < capacity ? back : 0;
            new (buffer + back) value_type(v);
            ++back;
            size.fetch_add(1, std::memory_order_release);
            return true;
        }

        std::optional<value_type> pop()
        {
            if (size.load(std::memory_order_acquire) == 0)
                return std::optional<value_type>(std::nullopt);

            front = front < capacity ? front : 0;
            std::optional<value_type> res(std::move(buffer[front]));
            buffer[front].~value_type();
            ++front;
            size.fetch_sub(1, std::memory_order_relaxed);

            return res;
        }

    public:

        const size_type capacity { 0 };
        pointer buffer { nullptr };
        size_type front { 0 };
        size_type back { 0 };
        std::atomic<size_type> size { 0 };
    };



    template<typename T>
    void printQueue(const Queue<T>& queue)
    {
        for (typename Queue<T>::size_type idx = 0; idx < queue.capacity; ++idx ) {
            std::cout << queue.buffer[idx] << ' ';
        }
        std::cout << std::endl;
    }

    template<typename T>
    void push(Queue<T>& queue, const T& val)
    {
        const bool result = queue.push(val);
        std::cout << "push : " << std::boolalpha << result << " -> " << val << ". size = "
            << queue.size.load(std::memory_order_relaxed) << std::endl;
    }

    template<typename T>
    void pop(Queue<T>& queue)
    {
        const std::optional<typename Queue<T>::value_type> val = queue.pop();
        if (val) {
            std::cout << "pop  : " << val.value() << ". size = "
                  << queue.size.load(std::memory_order_relaxed) << std::endl;
        } else {
            std::cout << "pop  : Failed, size = "
                      << queue.size.load(std::memory_order_relaxed) << std::endl;
        }
    }
};

void Single_ConsumerProducerQueue_Two::TestAll()
{
    Queue<int> queue(3);

    printQueue(queue);

    push(queue , 1);
    push(queue , 2);
    push(queue , 3);

    printQueue(queue);

    pop(queue);
    pop(queue);
    pop(queue);
    pop(queue);

    printQueue(queue);

    push(queue , 4);
    push(queue , 5);
    push(queue , 6);

    printQueue(queue);

    pop(queue);
    pop(queue);
    pop(queue);
    pop(queue);
};