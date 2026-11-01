/**============================================================================
Name        : ThreadSafePriorityQueue.cpp
Created on  : 01.11.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ThreadSafePriorityQueue.cpp
============================================================================**/

#include "ThreadSafePriorityQueue.hpp"

#include <array>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <limits>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

namespace
{
    enum class Priority : std::uint8_t
    {
        High = 0,
        Medium = 1,
        Low = 2
    };

    template<typename T>
    class ThreadSafePriorityQueue
    {
    private:
        static constexpr std::size_t PriorityCount = 3;
        static constexpr std::size_t Unlimited = std::numeric_limits<std::size_t>::max();

        static_assert(std::is_nothrow_move_constructible_v<T>,
            "T must be nothrow move constructible");

        struct PriorityQueue
        {
            explicit PriorityQueue(const std::size_t capacity):
                maxCapacity(capacity)
            {
            }

            std::deque<T> items;
            const std::size_t maxCapacity;
            std::condition_variable notFull;

            [[nodiscard]]
            bool isUnlimited() const noexcept
            {
                return maxCapacity == Unlimited;
            }

            [[nodiscard]]
            bool isFull() const noexcept
            {
                return !isUnlimited() && items.size() >= maxCapacity;
            }
        };

        std::array<PriorityQueue, PriorityCount> queues {
            PriorityQueue {Unlimited},
            PriorityQueue {Unlimited},
            PriorityQueue {Unlimited}
        };

        mutable std::mutex mutex;

        std::condition_variable notEmpty;

        bool closed = false;

        [[nodiscard]]
        static std::size_t toIndex(const Priority priority)
        {
            const auto index = static_cast<std::size_t>(priority);

            if (index >= PriorityCount) {
                throw std::invalid_argument("Invalid priority");
            }

            return index;
        }

        [[nodiscard]]
        bool emptyUnsafe() const noexcept
        {
            for (const auto& queue: queues) {
                if (!queue.items.empty()) {
                    return false;
                }
            }

            return true;
        }

        [[nodiscard]]
        std::optional<std::size_t> highestNonEmptyQueueUnsafe() const noexcept
        {
            for (std::size_t i = 0; i < PriorityCount; ++i) {
                if (!queues[i].items.empty()) {
                    return i;
                }
            }

            return std::nullopt;
        }

    public:
        ThreadSafePriorityQueue(
            const std::size_t highCapacity,
            const std::size_t mediumCapacity):
            queues {
                PriorityQueue {highCapacity},
                PriorityQueue {mediumCapacity},
                PriorityQueue {Unlimited}
            }
        {
            if (highCapacity == 0 || mediumCapacity == 0) {
                throw std::invalid_argument(
                    "High and Medium capacities must be greater than zero");
            }
        }

        ThreadSafePriorityQueue(const ThreadSafePriorityQueue&) = delete;
        ThreadSafePriorityQueue& operator=(const ThreadSafePriorityQueue&) = delete;

        // Returns false if the queue is closed.
        //
        // High and Medium producers block if their queue is full.
        // Low producers do not wait for capacity.
        bool push(T value, const Priority priority)
        {
            const std::size_t index = toIndex(priority);

            std::unique_lock<std::mutex> lock(mutex);

            PriorityQueue& target = queues[index];

            if (!target.isUnlimited()) {
                target.notFull.wait(lock, [this, &target] {
                    return closed || !target.isFull();
                });
            }

            if (closed) {
                return false;
            }

            target.items.push_back(std::move(value));

            lock.unlock();

            notEmpty.notify_one();

            return true;
        }

        // Blocks until:
        // 1. At least one task is available; or
        // 2. The queue is closed and fully drained.
        //
        // Returns nullopt only when the queue is closed and empty.
        std::optional<T> waitAndPop()
        {
            std::unique_lock<std::mutex> lock(mutex);

            notEmpty.wait(lock, [this] {
                return closed || !emptyUnsafe();
            });

            const std::optional<std::size_t> index =
                highestNonEmptyQueueUnsafe();

            if (!index.has_value()) {
                return std::nullopt;
            }

            PriorityQueue& source = queues[*index];

            T result = std::move(source.items.front());

            source.items.pop_front();

            const bool notifyProducer = !source.isUnlimited();

            lock.unlock();

            if (notifyProducer) {
                source.notFull.notify_one();
            }

            return result;
        }

        // Graceful shutdown:
        // - rejects new tasks;
        // - wakes blocked producers and consumers;
        // - allows consumers to drain remaining tasks.
        void close()
        {
            {
                std::lock_guard<std::mutex> lock(mutex);

                if (closed) {
                    return;
                }

                closed = true;
            }

            notEmpty.notify_all();

            for (auto& queue: queues) {
                queue.notFull.notify_all();
            }
        }

        [[nodiscard]]
        bool empty() const
        {
            std::lock_guard<std::mutex> lock(mutex);

            return emptyUnsafe();
        }

        [[nodiscard]]
        std::size_t size(const Priority priority) const
        {
            const std::size_t index = toIndex(priority);

            std::lock_guard<std::mutex> lock(mutex);

            return queues[index].items.size();
        }

        [[nodiscard]]
        std::size_t totalSize() const
        {
            std::lock_guard<std::mutex> lock(mutex);

            std::size_t result = 0;

            for (const auto& queue: queues) {
                result += queue.items.size();
            }

            return result;
        }
    };
}


#define Assert(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "Test failed!\n" \
                      << "Expression: " << #condition << '\n' \
                      << "File: " << __FILE__ << '\n' \
                      << "Line: " << __LINE__ << '\n'; \
            std::abort(); \
        } \
    } while (false)

namespace tests
{
    using Queue = ::ThreadSafePriorityQueue<int>;
    using Priority = ::Priority;

    void testPriorityOrdering()
    {
        Queue queue(10, 10);

        queue.push(1, Priority::Low);
        queue.push(2, Priority::Medium);
        queue.push(3, Priority::High);

        Assert(queue.waitAndPop() == 3);
        Assert(queue.waitAndPop() == 2);
        Assert(queue.waitAndPop() == 1);

        Assert(queue.empty());

        std::cout << "testPriorityOrdering: OK\n";
    }

    void testFifoWithinPriority()
    {
        Queue queue(10, 10);

        queue.push(1, Priority::High);
        queue.push(2, Priority::High);
        queue.push(3, Priority::High);

        Assert(queue.waitAndPop() == 1);
        Assert(queue.waitAndPop() == 2);
        Assert(queue.waitAndPop() == 3);

        std::cout << "testFifoWithinPriority: OK\n";
    }

    void testCapacity()
    {
        Queue queue(2, 2);

        queue.push(1, Priority::High);
        queue.push(2, Priority::High);

        Assert(queue.size(Priority::High) == 2);
        Assert(queue.totalSize() == 2);

        std::cout << "testCapacity: OK\n";
    }

    void testProducerBlocking()
    {
        Queue queue(1, 1);

        queue.push(1, Priority::High);

        std::promise<void> started;
        const auto startedFuture = started.get_future();

        auto producer = std::async(std::launch::async, [&] {
            started.set_value();

            return queue.push(2, Priority::High);
        });

        startedFuture.wait();

        // Producer should be blocked because High is full.
        Assert(producer.wait_for(100ms) == std::future_status::timeout);

        // Remove one item, allowing producer to continue.
        Assert(queue.waitAndPop() == 1);

        Assert(producer.wait_for(1s) == std::future_status::ready);
        Assert(producer.get());

        Assert(queue.waitAndPop() == 2);

        std::cout << "testProducerBlocking: OK\n";
    }

    void testConsumerBlocking()
    {
        Queue queue(10, 10);
        std::future<std::optional<int>> consumer = std::async(std::launch::async, [&] {
            return queue.waitAndPop();
        });

        // Consumer should be blocked because queue is empty.
        Assert(consumer.wait_for(100ms) == std::future_status::timeout);

        queue.push(42, Priority::Medium);
        Assert(consumer.wait_for(1ms) == std::future_status::ready);

        const std::optional<int> result = consumer.get();

        Assert(result.has_value());
        Assert(*result == 42);

        std::cout << "testConsumerBlocking: OK\n";
    }

    void testCloseAndDrain()
    {
        Queue queue(10, 10);

        queue.push(1, Priority::High);
        queue.push(2, Priority::Medium);
        queue.push(3, Priority::Low);

        queue.close();

        Assert(queue.waitAndPop() == 1);
        Assert(queue.waitAndPop() == 2);
        Assert(queue.waitAndPop() == 3);
        Assert(!queue.waitAndPop().has_value());

        std::cout << "testCloseAndDrain: OK\n";
    }

    void testPushAfterClose()
    {
        Queue queue(10, 10);
        queue.close();

        Assert(!queue.push(1, Priority::High));
        Assert(!queue.push(2, Priority::Medium));
        Assert(!queue.push(3, Priority::Low));
        Assert(queue.empty());

        std::cout << "testPushAfterClose: OK\n";
    }

    void testCloseUnblocksProducer()
    {
        Queue queue(1, 1);
        queue.push(1, Priority::High);

        std::future<bool> producer = std::async(std::launch::async, [&] {
            return queue.push(2, Priority::High);
        });

        // Producer should block because High is full.
        Assert(producer.wait_for(100ms) == std::future_status::timeout);

        queue.close();
        
        Assert(producer.wait_for(1s) == std::future_status::ready); // close() should unblock producer.
        Assert(!producer.get());                                    //
        Assert(queue.waitAndPop() == 1);                            // The existing task is still available.
        Assert(!queue.waitAndPop().has_value());                    // Closed and drained.

        std::cout << "testCloseUnblocksProducer: OK\n";
    }

    void testCloseUnblocksConsumer()
    {
        Queue queue(10, 10);
        std::future<std::optional<int>>  consumer = std::async(std::launch::async, [&] {
            return queue.waitAndPop();
        });

        // Consumer should block because queue is empty.
        Assert(consumer.wait_for(100ms) == std::future_status::timeout);

        queue.close();

        // close() should unblock consumer.
        Assert(consumer.wait_for(1s) == std::future_status::ready);
        Assert(!consumer.get().has_value());

        std::cout << "testCloseUnblocksConsumer: OK\n";
    }

    void testInvalidPriority()
    {
        Queue queue(10, 10);

        constexpr auto invalidPriority = static_cast<Priority>(100);
        bool exceptionThrown = false;
        try {
            queue.push(1, invalidPriority);
        }
        catch (const std::invalid_argument&) {
            exceptionThrown = true;
        }

        Assert(exceptionThrown);
        std::cout << "testInvalidPriority: OK\n";
    }
}

void thread_safe_priority_queue::TestAll()
{
    using namespace tests;

    testPriorityOrdering();
    testFifoWithinPriority();
    testCapacity();
    testProducerBlocking();
    testConsumerBlocking();
    testCloseAndDrain();
    testPushAfterClose();
    testCloseUnblocksProducer();
    testCloseUnblocksConsumer();
    testInvalidPriority();
}