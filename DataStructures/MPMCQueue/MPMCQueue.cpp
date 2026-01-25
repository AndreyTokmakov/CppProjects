/**============================================================================
Name        : MPMCQueue.cpp
Created on  : 25.01.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MPMCQueue.cpp
============================================================================**/

#include "MPMCQueue.hpp"

#include <atomic>
#include <iostream>
#include <cstdint>
#include <new>
#include <type_traits>
#include <cstdlib>
#include <thread>
#include <vector>
#include <unordered_set>
#include <mutex>

namespace
{

    template<typename T, size_t Capacity>
    class MPMC_Queue
    {
        static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of two");

        struct alignas(64) Cell {
            std::atomic<size_t> seq;
            std::aligned_storage_t<sizeof(T), alignof(T)> storage;
        };

    public:

        MPMC_Queue()
        {
            for (size_t i = 0; i < Capacity; ++i)
                buffer_[i].seq.store(i, std::memory_order_relaxed);
        }

        ~MPMC_Queue() {
            T tmp;
            while (try_pop(tmp)) {}
        }

        bool try_push(const T& value) {
            return emplace(value);
        }

        bool try_push(T&& value) {
            return emplace(std::move(value));
        }

        bool try_pop(T& out)
        {
            Cell* cell;
            size_t pos = dequeue_pos_.load(std::memory_order_relaxed);

            for (;;) {
                cell = &buffer_[pos & mask_];
                size_t seq = cell->seq.load(std::memory_order_acquire);
                intptr_t diff = (intptr_t)seq - (intptr_t)(pos + 1);

                if (diff == 0) {
                    if (dequeue_pos_.compare_exchange_weak(
                            pos, pos + 1,
                            std::memory_order_relaxed))
                        break;
                } else if (diff < 0) {
                    return false; // empty
                } else {
                    pos = dequeue_pos_.load(std::memory_order_relaxed);
                }
            }

            T* data = reinterpret_cast<T*>(&cell->storage);
            out = std::move(*data);
            data->~T();

            cell->seq.store(pos + Capacity, std::memory_order_release);
            return true;
        }

    private:

        template<typename U>
        bool emplace(U&& value)
        {
            Cell* cell;
            size_t pos = enqueue_pos_.load(std::memory_order_relaxed);

            for (;;) {
                cell = &buffer_[pos & mask_];
                size_t seq = cell->seq.load(std::memory_order_acquire);
                intptr_t diff = (intptr_t)seq - (intptr_t)pos;

                if (diff == 0) {
                    if (enqueue_pos_.compare_exchange_weak(
                            pos, pos + 1,
                            std::memory_order_relaxed))
                        break;
                } else if (diff < 0) {
                    return false; // full
                } else {
                    pos = enqueue_pos_.load(std::memory_order_relaxed);
                }
            }

            new (&cell->storage) T(std::forward<U>(value));
            cell->seq.store(pos + 1, std::memory_order_release);
            return true;
        }

    private:
        static constexpr size_t mask_ = Capacity - 1;

        alignas(64) Cell buffer_[Capacity];
        alignas(64) std::atomic<size_t> enqueue_pos_{0};
        alignas(64) std::atomic<size_t> dequeue_pos_{0};
    };
}

namespace tests
{

    inline void Assert(const bool condition,
                       const std::string& msg,
                       const char* file = nullptr,
                       const int line = 0)
    {
        if (!condition)
        {
            std::cerr << "[ASSERT FAILED]";
            if (file)
                std::cerr << " File: " << file;
            if (line)
                std::cerr << " Line: " << line;
            std::cerr << " Msg: " << msg << "\n";
            std::exit(1); // для юнит-теста можно сразу завершить
        }
    }

    // Удобный wrapper, чтобы не писать файл/строку каждый раз
#define ASSERT(cond, msg) Assert((cond), (msg), __FILE__, __LINE__)

    void test_single_thread_basic()
    {
        MPMC_Queue<int, 1024> q;

        ASSERT(q.try_push(1), "push 1 failed");
        ASSERT(q.try_push(2), "push 2 failed");
        ASSERT(q.try_push(3), "push 3 failed");

        int v;
        ASSERT(q.try_pop(v) && v == 1, "pop 1 failed");
        ASSERT(q.try_pop(v) && v == 2, "pop 2 failed");
        ASSERT(q.try_pop(v) && v == 3, "pop 3 failed");
        ASSERT(!q.try_pop(v), "queue should be empty");

        std::cout << "test_single_thread_basic OK\n";
    }

    void test_queue_full_and_empty()
    {
        MPMC_Queue<int, 4> q;

        ASSERT(q.try_push(1), "push 1 failed");
        ASSERT(q.try_push(2), "push 2 failed");
        ASSERT(q.try_push(3), "push 3 failed");
        ASSERT(q.try_push(4), "push 4 failed");
        ASSERT(!q.try_push(5), "queue should be full");

        int v;
        ASSERT(q.try_pop(v) && v == 1, "pop 1 failed");
        ASSERT(q.try_pop(v) && v == 2, "pop 2 failed");
        ASSERT(q.try_pop(v) && v == 3, "pop 3 failed");
        ASSERT(q.try_pop(v) && v == 4, "pop 4 failed");
        ASSERT(!q.try_pop(v), "queue should be empty now");

        std::cout << "test_queue_full_and_empty OK\n";
    }


    void test_mpmc_stress()
    {
        constexpr size_t PRODUCERS = 4;
        constexpr size_t CONSUMERS = 4;
        constexpr size_t MSGS_PER_PRODUCER = 200'000;
        constexpr size_t TOTAL = PRODUCERS * MSGS_PER_PRODUCER;

        MPMC_Queue<uint64_t, 1 << 16> q;

        std::atomic<size_t> produced{0};
        std::atomic<size_t> consumed{0};

        std::vector<std::thread> threads;

        // Producers
        for (size_t p = 0; p < PRODUCERS; ++p) {
            threads.emplace_back([&, p] {
                for (size_t i = 0; i < MSGS_PER_PRODUCER; ++i) {
                    uint64_t value = (p << 48) | i;
                    while (!q.try_push(value)) {} // spin
                    produced.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }

        std::unordered_set<uint64_t> seen;
        seen.reserve(TOTAL);
        std::mutex seen_mutex;

        // Consumers
        for (size_t c = 0; c < CONSUMERS; ++c) {
            threads.emplace_back([&] {
                uint64_t v;
                while (consumed.load(std::memory_order_relaxed) < TOTAL) {
                    if (q.try_pop(v)) {
                        std::lock_guard lock(seen_mutex);
                        bool inserted = seen.insert(v).second;
                        ASSERT(inserted, "duplicate value detected");
                        consumed.fetch_add(1, std::memory_order_relaxed);
                    }
                }
            });
        }

        for (auto& t : threads)
            t.join();

        ASSERT(produced == TOTAL, "produced count mismatch");
        ASSERT(consumed == TOTAL, "consumed count mismatch");
        ASSERT(seen.size() == TOTAL, "seen count mismatch");

        std::cout << "test_mpmc_stress OK\n";
    }

}

void MPMCQueue::TestAll()
{
    tests::test_single_thread_basic();
    tests::test_queue_full_and_empty();
    tests::test_mpmc_stress();
}
