/**============================================================================
Name        : SingleConsumerProducerQueue.cpp
Created on  : 10.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SingleConsumerProducerQueue.cpp
============================================================================**/

#include "../Utilities/Wrapper.h"

#include "SingleConsumerProducerQueue.h"

#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include <atomic>

namespace
{
    using Integer = Helpers::Integer;
}

namespace SingleConsumerProducerQueue
{
    // TODO: Concepts ?
    //       trivial_destructible
    // TODO: Использовать Placeholder чтобы алоцировать память для коллекции
    // TODO: emplace() вместо add() | использовать Placement new()
    //       идея в том что бы конструировать объекты на месте уже существующих
    // TODO: do not call a destructor ?

#if 0
    template<class T,
            size_t Capacity = 100>
    class RingBuffer
    {
        using value_type = T;
        using size_type = size_t;

        struct alignas(sizeof(value_type)) Placeholder {};

        static_assert(!std::is_same_v<T, void>, "Type of the RingBuffer can not be void");
        static_assert(0 != Capacity, "Please try a little bigger buffer");

    private:
        std::array<Placeholder, Capacity> buffer {};
        // std::unique_ptr<Placeholder[]> buffer { std::make_unique<Placeholder[]>(Capacity) };

        size_type head {0};
        size_type tail {0};
        bool full {false};


        template<typename ... Args>
        void emplace(Args ... params)
        {

            new (&buffer[head]) value_type { std::forward<Args>(params)... };
            ++head;

            if (head >= Capacity) {
                full = true;
            }
        }
    };
#endif


    template<class T,
            size_t Capacity = 100>
    struct RingBuffer
    {
        using value_type = T;
        using size_type = size_t;

        struct alignas(sizeof(value_type)) Placeholder {};

        static_assert(!std::is_same_v<T, void>, "Type of the RingBuffer can not be void");
        static_assert(0 != Capacity, "Please try a little bigger buffer");

    // private:

        // std::array<Placeholder, Capacity> buffer {};
        std::unique_ptr<Placeholder[]> buffer { std::make_unique<Placeholder[]>(Capacity) };

        std::atomic<size_type> head {0};
        std::atomic<size_type> tail {0};

        template<typename ... Args>
        void emplace(Args&& ... params)
        {
            size_type idx = head.fetch_add(1, std::memory_order_relaxed);
            if (idx >= Capacity) {
                head.store(0); // FIXME. memory order
                idx = 0;
            }

            new (&buffer[idx]) value_type { std::forward<Args>(params)... };
        }

        [[nodiscard]]
        inline value_type& front() const noexcept
        {
            return reinterpret_cast<value_type&>(buffer[tail.load(std::memory_order_relaxed)]);
        }

        [[nodiscard]]
        bool moveFront() noexcept
        {
            const size_type idx = tail.load(std::memory_order_relaxed);
            if (idx >= Capacity) {
                tail.store(0); // FIXME. memory order
            }
        }
    };
};

void SingleConsumerProducerQueue::TestAll()
{
    RingBuffer<Integer, 5> buffer;


    buffer.emplace(2);

    std::cout << buffer.front().value << std::endl;

};