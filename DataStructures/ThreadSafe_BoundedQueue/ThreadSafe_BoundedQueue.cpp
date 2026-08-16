/**============================================================================
Name        : ThreadSafe_BoundedQueue.cpp
Created on  : 25.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ThreadSafe_BoundedQueue.cpp
============================================================================**/

#include "ThreadSafe_BoundedQueue.hpp"

#include <iostream>
#include <queue>

#include <mutex>
#include <condition_variable>
#include <syncstream>
#include <thread>

#include "DateTimeUtilities.hpp"

namespace
{
    using utilities::datetime::getCurrentTime;

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "
}


namespace thread_safe_bounded_queue::basic
{
    template<class T>
    class BoundedQueue
    {
        using size_type = size_t;

        std::mutex mtx;
        std::condition_variable notFull;
        std::condition_variable notEmpty;
        std::queue<T> queue;
        const size_type capacity { 0UL };
        bool stopped { false };

    public:

        explicit BoundedQueue(const size_t capacity) : capacity { capacity }
        {
            if (0 == capacity) {
                throw std::invalid_argument("capacity must be > 0");
            }
        }

        bool push(T v)
        {
            std::unique_lock lock { mtx };
            notFull.wait(lock, [&]{ return queue.size() < capacity || stopped; });
            if (stopped) {
                return false;
            }
            queue.push(std::move(v));
            lock.unlock();

            notEmpty.notify_one();
            return true;
        }

        template<class... Args>
        bool emplace(Args&&... args)
        {
            {
                std::unique_lock lock(mtx);
                notFull.wait(lock, [&] { return stopped || queue.size() < capacity; });
                if (stopped) {
                    return false;
                }
                queue.emplace(std::forward<Args>(args)...);
            }

            notEmpty.notify_one();
            return true;
        }

        bool pop(T& out)
        {
            {
                std::unique_lock lock { mtx };
                notEmpty.wait(lock, [&]{ return !queue.empty() || stopped; });
                if (queue.empty()) {
                    return false;
                }
                out = std::move(queue.front());
                queue.pop();
            }

            notFull.notify_one();
            return true;
        }

        void stop()
        {
            {
                std::lock_guard lk(mtx);
                stopped = true;
            }
            notFull.notify_all();
            notEmpty.notify_all();
        }
    };
}

namespace thread_safe_bounded_queue::split_lock_queue
{
    template<class T>
    class SplitLockQueue
    {
        using size_type = std::size_t;

        const size_type capacity { 0 };
        const size_type mask { 0 };

        std::vector<T> buffer;

        alignas(64) std::atomic<size_type> size{0};
        alignas(64) size_type head{0};
        alignas(64) size_type tail{0};

        std::mutex head_mtx;
        std::mutex tail_mtx;

        std::condition_variable notEmpty;
        std::condition_variable notFull;

        std::atomic<bool> stopped { false };

    public:

        explicit SplitLockQueue(const size_type initCapacity) :
            capacity { initCapacity },
            mask { initCapacity - 1 },
            buffer(initCapacity)
        {
            if (0 == capacity) {
                throw std::invalid_argument("capacity must be > 0");
            }
            if ((initCapacity & (initCapacity - 1)) != 0) {
                throw std::runtime_error("Capacity must be power of two");
            }
        }

        template<class U>
        bool push(U&& value)
        {
            std::unique_lock lock(tail_mtx);
            notFull.wait(lock, [&]{
                return stopped.load(std::memory_order_acquire)
                       || size.load(std::memory_order_acquire) < capacity;
            });

            if (stopped.load(std::memory_order_acquire))
                return false;

            buffer[tail] = std::forward<U>(value);
            // tail = (tail + 1) % capacity;
            tail = (tail + 1) & mask;

            size.fetch_add(1, std::memory_order_release);

            lock.unlock();
            notEmpty.notify_one();
            return true;
        }

        bool pop(T& out)
        {
            std::unique_lock lock(head_mtx);
            notEmpty.wait(lock, [&]{
                return stopped.load(std::memory_order_acquire)
                       || size.load(std::memory_order_acquire) > 0;
            });

            if (size.load(std::memory_order_acquire) == 0)
                return false;

            out = std::move(buffer[head]);
            // head = (head + 1) % capacity;
            head = (head + 1) & mask;

            size.fetch_sub(1, std::memory_order_release);

            lock.unlock();
            notFull.notify_one();
            return true;
        }

        void stop()
        {
            stopped.store(true, std::memory_order_release);
            notEmpty.notify_all();
            notFull.notify_all();
        }
    };

}

namespace thread_safe_bounded_queue::unit_tests
{
    using namespace std::chrono_literals;

    template <typename Q, typename Ty>
    concept Queue = std::constructible_from<Q, size_t> && requires(Q queue, Ty value)
    {
        { queue.push(value) } -> std::same_as<bool>;
        { queue.pop(value) } -> std::same_as<bool>;
    };

    template <typename Q, typename T> requires Queue<Q, T>
    void test_single_thread_basic()
    {
        Q q(2);

        if (!q.push(1))
            std::cerr << "push failed\n";
        if (!q.push(2))
            std::cerr << "push failed\n";

        int v = 0;
        if (!q.pop(v) || v != 1)
            std::cerr << "FIFO broken (1)\n";

        if (!q.pop(v) || v != 2)
            std::cerr << "FIFO broken (2)\n";
    }

    template <typename Q, typename T> requires Queue<Q, T>
    void test_fifo_order()
    {
        Q q(16);

        for (int i = 0; i < 8; ++i)
            q.push(i);

        int v {};
        for (int i = 0; i < 8; ++i)
        {
            q.pop(v);
            if (v != i)
                std::cerr << "FIFO order violated\n";
        }
    }

    template <typename Q, typename T> requires Queue<Q, T>
    void test_block_on_full()
    {
        Q q(1);
        q.push(42);

        std::atomic<bool> pushed{false};

        std::jthread t([&] {
            q.push(43);
            pushed = true;
        });

        std::this_thread::sleep_for(100ms);
        if (pushed.load())
            std::cerr << "push did not block on full queue\n";

        int v;
        q.pop(v);
    }

    template <typename Q, typename T> requires Queue<Q, T>
    void test_block_on_empty()
    {
        Q q(1);
        std::atomic<bool> popped{false};

        std::jthread t([&] {
            int v;
            q.pop(v);
            popped = true;
        });

        std::this_thread::sleep_for(100ms);
        if (popped.load())
            std::cerr << "pop did not block on empty queue\n";

        q.push(10);
    }

    template <typename Q, typename T> requires Queue<Q, T>
    void test_stop_unblocks_waiters()
    {
        Q q(1);

        std::atomic<bool> finished{false};
        std::jthread t([&] {
            int v;
            q.pop(v); // будет ждать
            finished = true;
        });

        std::this_thread::sleep_for(100ms);
        q.stop();

        for (int i = 0; i < 20; ++i) {
            if (finished.load())
                break;
            std::this_thread::sleep_for(10ms);
        }

        if (!finished.load())
            std::cerr << "stop() did not unblock pop()\n";
    }

    template <typename Q, typename T>
    requires Queue<Q, T>
    void test_stop_prevents_push()
    {
        Q q(2);
        q.stop();

        if (q.push(1))
            std::cerr << "push allowed after stop\n";
    }


    template <typename Q, typename T> requires Queue<Q, T>
    void test_multi_producer_consumer()
    {
        Q q(16);

        constexpr int N = 10000;
        std::atomic<int> produced{0}, consumed{0};

        std::jthread producer([&] {
            for (int i = 0; i < N; ++i) {
                if (q.push(i))
                    ++produced;
            }
        });

        std::jthread consumer([&] {
            while (consumed < N) {
                if (int v; q.pop(v))
                    ++consumed;
            }
        });

        producer.join();
        consumer.join();

        if (produced != N) {
            std::cerr << "producer count mismatch\n";
        }
        if (consumed != N) {
            std::cerr << "consumer count mismatch\n";
        }
    }

    template <typename Q, typename T> requires Queue<Q, T>
    void test_stress()
    {
        Q q(50);

        constexpr int threads = 4,  perThread = 5000;

        std::atomic<int> totalProduced{0};
        std::atomic<int> totalConsumed{0};

        std::vector<std::jthread> producers;
        std::vector<std::jthread> consumers;

        for (int i = 0; i < threads; ++i)
        {
            producers.emplace_back([&] {
                for (int j = 0; j < perThread; ++j){
                    if (q.push(j)) {
                        ++totalProduced;
                    }
                }
            });
            consumers.emplace_back([&] {
                while (totalConsumed < threads * perThread) {
                    if (int v; q.pop(v)) {
                        ++totalConsumed;
                    }
                }
            });
        }

        producers.clear();
        q.stop();
        consumers.clear();

        if (totalProduced != threads * perThread) {
            std::cerr << "stress produced mismatch\n";
        }
        if (totalConsumed != threads * perThread) {
            std::cerr << "stress consumed mismatch\n";
        }
    }
}

void thread_safe_bounded_queue::TestAll()
{
    unit_tests::test_single_thread_basic<basic::BoundedQueue<int>, int>();
    unit_tests::test_fifo_order<basic::BoundedQueue<int>, int>();
    unit_tests::test_block_on_full<basic::BoundedQueue<int>, int>();
    unit_tests::test_block_on_empty<basic::BoundedQueue<int>, int>();
    unit_tests::test_stop_unblocks_waiters<basic::BoundedQueue<int>, int>();
    unit_tests::test_stop_prevents_push<basic::BoundedQueue<int>, int>();
    unit_tests::test_multi_producer_consumer<basic::BoundedQueue<int>, int>();
    // unit_tests::test_stress<basic::BoundedQueue<int>, int>();

    unit_tests::test_single_thread_basic<split_lock_queue::SplitLockQueue<int>, int>();
    unit_tests::test_fifo_order<split_lock_queue::SplitLockQueue<int>, int>();
    unit_tests::test_block_on_full<split_lock_queue::SplitLockQueue<int>, int>();
    unit_tests::test_block_on_empty<split_lock_queue::SplitLockQueue<int>, int>();
    unit_tests::test_stop_unblocks_waiters<split_lock_queue::SplitLockQueue<int>, int>();
    unit_tests::test_stop_prevents_push<split_lock_queue::SplitLockQueue<int>, int>();
    unit_tests::test_multi_producer_consumer<split_lock_queue::SplitLockQueue<int>, int>();

}