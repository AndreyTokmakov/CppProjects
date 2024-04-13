/**============================================================================
Name        : SingleConsumerProducerQueue.cpp
Created on  : 10.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SingleConsumerProducerQueue.cpp
============================================================================**/

#include "../Utilities/Wrapper.h"
#include "../Utilities/Utilities.h"

#include "SingleConsumerProducerQueue.h"

#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include <atomic>
#include <thread>
#include <chrono>
#include <syncstream>

namespace
{
    using Integer = Helpers::Wrapper<int, false>;
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

        std::unique_ptr<Placeholder[]> buffer { std::make_unique<Placeholder[]>(Capacity) };

        std::atomic<size_type> head {0};
        std::atomic<size_type> tail {0};

        template<typename ... Args>
        void emplace(Args&& ... params)
        {
            size_type idx = head.load(std::memory_order_relaxed);
            new (&buffer[idx]) value_type { std::forward<Args>(params)... };
            idx = idx + 1 >= Capacity ? 0 : idx + 1;
            head.store(idx, std::memory_order_relaxed);
        }

        [[nodiscard]]
        bool get(value_type& entry) noexcept
        {
            size_type idxTail = tail.load(std::memory_order_relaxed);
            if (idxTail >= Capacity)
                idxTail = 0;

            const size_type idxHead = head.load(std::memory_order_relaxed);
            if (idxTail == idxHead)
                return false;

            tail.store(idxTail + 1, std::memory_order_release);
            entry =  std::move(reinterpret_cast<value_type&>(buffer[idxTail]));
            return true;
        }
    };


    template<class T,
            size_t Capacity = 100>
    struct RingBuffer2
    {
        using value_type = T;
        using size_type = size_t;

        struct alignas(sizeof(value_type)) Placeholder {};

        static_assert(!std::is_same_v<T, void>, "Type of the RingBuffer can not be void");
        static_assert(0 != Capacity, "Please try a little bigger buffer");

        std::unique_ptr<Placeholder[]> buffer { std::make_unique<Placeholder[]>(Capacity) };

        std::atomic<size_type> headAtomic {0};
        size_type head {0};

        std::atomic<size_type> tailAtomic {0};
        size_type tail {0};

        template<typename ... Args>
        void emplace(Args&& ... params)
        {
            new (&buffer[head]) value_type { std::forward<Args>(params)... };
            head = head + 1 >= Capacity ? 0 : head + 1;
            headAtomic.store(head, std::memory_order_relaxed);
        }

        [[nodiscard]]
        bool get(value_type& entry) noexcept
        {
            if (tail >= Capacity)
                tail = 0;
            if (tail == head)
                return false;

            tailAtomic.store(tail + 1, std::memory_order_release);
            entry =  std::move(reinterpret_cast<value_type&>(buffer[tail]));
            return true;
        }
    };
};


namespace SingleConsumerProducerQueue::DemoTwo
{
    template<class T,
            size_t Capacity = 100>
    struct RingBuffer
    {
        using value_type = T;
        using size_type = size_t;

        struct alignas(sizeof(value_type)) Placeholder {};

        static_assert(!std::is_same_v<T, void>, "Type of the RingBuffer can not be void");
        static_assert(0 != Capacity, "Please try a little bigger buffer");

        std::unique_ptr<Placeholder[]> buffer { std::make_unique<Placeholder[]>(Capacity) };

        std::atomic<size_type> head {0};
        size_type idx {0};
        // TODO: Try with NON ATOMIC headIndex to remove 'size_type idx = head.load(std::memory_order_relaxed);'



        template<typename ... Args>
        void emplace(Args&& ... params)
        {
            // TODO: Run test with fetch_and_add
            // size_t idx = head.load(std::memory_order_relaxed);

            idx = (idx == Capacity) ? 0 : idx + 1;
            new (&buffer[idx]) value_type { std::forward<Args>(params)... };
            head.store(idx, std::memory_order_relaxed);
            head.notify_one();
            // std::osyncstream {std::cout} << "idx -> " << idx << std::endl;
        }

        // TODO: add timeout???
        [[nodiscard]]
        bool try_read_next(size_type& index,
                           value_type& entry) const  noexcept
        {
            // std::osyncstream {std::cout} << "waiting for index != " << index << std::endl;
            head.wait(index, std::memory_order_relaxed);
            // index = head.load(std::memory_order_relaxed);
            // std::cout << "index <- " << index << std::endl;

            index = (index == Capacity) ? 0 : index + 1;

            entry =  std::move(reinterpret_cast<value_type&>(buffer[index]));
            return true;
        }
    };
}

namespace SingleConsumerProducerQueue::Tests
{
    void debugTest()
    {
        RingBuffer<Integer, 5> buffer;

        auto consume = [&]() {
            Integer integer;

            while (true)
            {
                if (buffer.get(integer))
                {
                    std::cout << integer.value << std::endl;
                } else
                {
                    // std::cout << "Sleeping" << std::endl;
                    std::this_thread::sleep_for(std::chrono::nanoseconds (10));
                }
            }
        };

        auto produce = [&]() {
            int i = 0;
            while (true)
            {
                buffer.emplace(i++);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        };

        std::jthread consumer {consume};
        std::jthread producer {produce};
    }

    void benchmark()
    {
        Integer integer;
        // RingBuffer<Integer, 1000> buffer;
        RingBuffer2<Integer, 1000> buffer;
        constexpr int maxCount = 1'000'000'000;
        size_t total = 0;

        auto consume = [&]() {
            while (true)
            {
                if (buffer.get(integer)) {
                    if (integer.value == maxCount)
                        break;
                    ++total;
                } else {
                    std::this_thread::sleep_for(std::chrono::nanoseconds (1));
                }
            }
            std::cout << "Consumer done\n";

        };

        auto produce = [&]() {
            for (int idx = 0; idx <= maxCount; ++idx)
            {
                buffer.emplace(idx++);
                // std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            std::cout << "Producer done\n";
        };

        std::jthread consumer {consume};
        std::jthread producer {produce};

        consumer.join();
        producer.join();

        std::cout << integer << " " << total << std::endl;
    }


    void debugTest_DemoTwo()
    {
        DemoTwo::RingBuffer<Integer, 5> buffer;

        auto consume = [&]() {
            Integer integer;
            decltype(buffer)::size_type idx {};

            while (true)
            {
                buffer.try_read_next(idx, integer);
                std::osyncstream {std::cout} << integer.value << std::endl;
            }
        };

        auto produce = [&]() {
            for (int i = 100; i <= 110; ++i)
            {
                buffer.emplace(i);
                std::this_thread::sleep_for(std::chrono::milliseconds (1));
            }
        };

        std::jthread consumer {consume};
        std::jthread producer {produce};
    }

    void benchmark_DemoTwo()
    {
        using namespace DemoTwo;

        Integer integer;
        DemoTwo::RingBuffer<Integer, 1000> buffer;
        constexpr int maxCount = 1'000'000'000;
        size_t total = 0;

        auto consume = [&]() {
            while (true)
            {
                /*
                if (buffer.get(integer)) {
                    if (integer.value == maxCount)
                        break;
                    ++total;
                } else {
                    std::this_thread::sleep_for(std::chrono::nanoseconds (1));
                }
                 */
            }
            std::cout << "Consumer done\n";

        };

        auto produce = [&]() {
            for (int idx = 0; idx <= maxCount; ++idx)
            {
                buffer.emplace(idx++);
                // std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            std::cout << "Producer done\n";
        };

        Utilities::ScopedTimer timer {"benchmark_DemoTwo"};

        //std::jthread consumer {consume};
        std::jthread producer {produce};

        //consumer.join();
        producer.join();

        std::cout << integer << " " << total << std::endl;
    }
}

void SingleConsumerProducerQueue::TestAll()
{
    // Tests::debugTest();
    // Tests::benchmark();

    Tests::debugTest_DemoTwo();
    // Tests::benchmark_DemoTwo();
};