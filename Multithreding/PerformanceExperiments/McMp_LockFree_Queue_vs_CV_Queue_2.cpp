/**============================================================================
Name        : McMp_LockFree_Queue_vs_CV_Queue_2.cpp
Created on  : 26.01.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : McMp_LockFree_Queue_vs_CV_Queue_2.cpp
============================================================================**/

#include "PerformanceExperiments.h"
#include "PerfUtilities.hpp"

#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <deque>


namespace cv_queue::base
{
    template<typename T>
    class ThreadSafeQueue
    {
    public:
        void push(T value)
        {
            {
                std::lock_guard lock { mutex };
                values.push_back(std::move(value));
            }
            condition.notify_one();
        }

        bool pop(T& value)
        {
            std::lock_guard lock { mutex };
            if (values.empty())
                return false;

            value = std::move(values.front());
            values.pop_front();

            return true;
        }

        bool waitPop(T& value)
        {
            std::unique_lock lock { mutex };
            condition.wait(lock, [this] { return  !values.empty(); });
            if (values.empty())
                return false;

            value = std::move(values.front());
            values.pop_front();

            return true;
        }

        [[nodiscard]]
        bool empty()const noexcept
        {
            std::lock_guard lock { mutex };
            return values.empty();
        }

    private:
        mutable std::mutex mutex;
        std::condition_variable condition;
        std::deque<T> values{};
    };
}

namespace cv_queue::ring_buffer
{

    template<typename T>
    class ThreadSafeQueue
    {
    public:
        explicit ThreadSafeQueue(const std::size_t capacity) : buffer(capacity)
        {
        }

        bool push(T value)
        {
            std::lock_guard lock { mutex };

            if (size == buffer.size())
                return false;

            buffer[tail] = std::move(value);
            tail = next(tail);
            ++size;

            condition.notify_one();
            return true;
        }

        bool pop(T& value)
        {
            std::lock_guard lock { mutex };

            if (size == 0)
                return false;

            value = std::move(buffer[head]);
            head = next(head);
            --size;

            return true;
        }

        void waitPop(T& value)
        {
            std::unique_lock lock { mutex };

            condition.wait(lock, [this] {
                return size != 0;
            });

            value = std::move(buffer[head]);
            head = next(head);
            --size;
        }

    private:
        [[nodiscard]]
        std::size_t next(const std::size_t index) const noexcept
        {
            const std::size_t nextIndex = index + 1;
            return nextIndex == buffer.size() ? 0 : nextIndex;
        }

        std::mutex mutex;
        std::condition_variable condition;

        std::vector<T> buffer;

        std::size_t head { 0 };
        std::size_t tail { 0 };
        std::size_t size { 0 };
    };
}

namespace cv_queue::ring_buffer_2
{
    template<typename T, std::size_t Capacity>
    class ThreadSafeQueue
    {
        static_assert(Capacity > 0);
        static_assert((Capacity & (Capacity - 1)) == 0);

        static constexpr std::size_t kMask { Capacity - 1 };

    public:

        ThreadSafeQueue(): buffer(Capacity) {
        }

        bool push(T value)
        {
            std::lock_guard lock { mutex };

            if (writeIdx - readIdx == Capacity)
                return false;

            const std::size_t pos = writeIdx & kMask;
            buffer[pos] = std::move(value);
            ++writeIdx;

            condition.notify_one();
            return true;
        }

        bool pop(T& value)
        {
            std::lock_guard lock { mutex };

            if (writeIdx == readIdx)
                return false;

            const std::size_t pos = readIdx & kMask;
            value = std::move(buffer[pos]);
            ++readIdx;

            return true;
        }

        void waitPop(T& value)
        {
            std::unique_lock lock { mutex };

            condition.wait(lock, [this] {
                return writeIdx != readIdx;
            });

            const std::size_t pos = readIdx & kMask;
            value = std::move(buffer[pos]);
            ++readIdx;
        }

    private:
        std::mutex mutex;
        std::condition_variable condition;

        std::vector<T> buffer{};

        std::size_t writeIdx { 0 };
        std::size_t readIdx { 0 };
    };
}

namespace cv_queue::tests
{
    struct Message
    {
        std::array<char, 512> data {};
    };

    template<typename Queue>
    std::chrono::nanoseconds benchmarkOriginal(Queue& queue,
                                               const std::size_t count)
    {
        std::atomic<bool> consumerReady { false };
        std::thread consumer([&]
        {
            Message message;
            consumerReady.store(true, std::memory_order_release);
            for (std::size_t i = 0; i < count; ++i) {
                queue.waitPop(message);
            }
        });

        while (!consumerReady.load(std::memory_order_acquire))
            std::this_thread::yield();

        const auto start = std::chrono::steady_clock::now();

        std::thread producer([&] {
            for (std::size_t i = 0; i < count; ++i)
                queue.push(Message {  });
        });

        producer.join();
        consumer.join();

        return std::chrono::steady_clock::now() - start;
    }


    template<typename Queue>
    std::chrono::nanoseconds benchmarkRing(Queue& queue,
                                           const std::size_t count)
    {
        std::atomic<bool> consumerReady { false };
        std::thread consumer([&]
        {
            Message message;
            consumerReady.store(true, std::memory_order_release);
            for (std::size_t i = 0; i < count; ++i)
                queue.waitPop(message);
        });

        while (!consumerReady.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        const auto start = std::chrono::steady_clock::now();
        std::thread producer([&]
        {
            for (std::size_t i = 0; i < count; ++i){
                while (!queue.push(Message {  })) {
                    std::this_thread::yield();
                }
            }
        });

        producer.join();
        consumer.join();

        return std::chrono::steady_clock::now() - start;
    }


    void runBenchmark()
    {
        constexpr std::size_t count { 10'000'000 };
        constexpr std::size_t capacity { 16'384 };
        constexpr std::size_t iterations { 5 };

        std::cout << "Messages     : " << count << '\n';
        std::cout << "Object size: : " << sizeof(Message) << '\n';
        std::cout << "Iterations   : " << iterations << '\n';
        std::cout << "Capacity     : " << capacity << "\n\n";

        for (std::size_t i = 0; i < iterations; ++i)
        {
            base::ThreadSafeQueue<Message> original;
            ring_buffer::ThreadSafeQueue<Message> ringBufferQueue1 { capacity };
            ring_buffer_2::ThreadSafeQueue<Message, capacity> ringBufferQueue2{};

            const auto originalTime = benchmarkOriginal(original, count);
            const auto ringTime1 = benchmarkRing(ringBufferQueue1, count);
            const auto ringTime2 = benchmarkRing(ringBufferQueue2, count);

            const double originalSeconds = std::chrono::duration<double>(originalTime).count();
            const double ringSeconds1 = std::chrono::duration<double>(ringTime1).count();
            const double ringSeconds2 = std::chrono::duration<double>(ringTime2).count();

            const double originalMps = count / originalSeconds / 1'000'000.0;
            const double ringMps1 = count / ringSeconds1 / 1'000'000.0;
            const double ringMps2 = count / ringSeconds2 / 1'000'000.0;

            std::cout
                << "Iteration " << i + 1 << '\n'
                << "  deque : " << originalSeconds << " s, " << originalMps << " M msg/s\n"
                << "  ring  : " << ringSeconds1 << " s, " << ringMps1 << " M msg/s\n"
                << "  ring  : " << ringSeconds2 << " s, " << ringMps2 << " M msg/s\n"
                << '\n';
        }
    }
}

namespace mcmp_queue
{

    template<typename T>
    class MPMCQueue
    {
        using size_type = std::size_t;

        struct Cell
        {
            std::atomic<size_type> sequence { 0 };
            T data {};
        };

    public:
        explicit MPMCQueue(const size_type capacity):
            buffer { validateCapacity(capacity) }, mask { buffer.size() - 1 }
        {
            for (size_type i = 0; i < buffer.size(); ++i)
                buffer[i].sequence.store(i, std::memory_order_relaxed);
        }

        MPMCQueue(const MPMCQueue&) = delete;
        MPMCQueue& operator=(const MPMCQueue&) = delete;

        [[nodiscard]]
        bool push(T value)
        {
            Cell* cell = nullptr;
            size_type pos = enqueuePos.load(std::memory_order_relaxed);

            for (;;)
            {
                cell = &buffer[pos & mask];
                const size_type seq = cell->sequence.load(std::memory_order_acquire);
                const auto dif = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos);

                if (dif == 0)
                {
                    if (enqueuePos.compare_exchange_weak(pos, pos + 1,
                            std::memory_order_relaxed, std::memory_order_relaxed))
                    {
                        break;
                    }
                }
                else if (dif < 0) {
                    return false;
                } else {
                    pos = enqueuePos.load(std::memory_order_relaxed);
                }
            }

            cell->data = std::move(value);
            cell->sequence.store(pos + 1, std::memory_order_release);

            return true;
        }

        [[nodiscard]]
        bool pop(T& value)
        {
            Cell* cell = nullptr;
            size_type pos = dequeuePos.load(std::memory_order_relaxed);
            for (;;)
            {
                cell = &buffer[pos & mask];
                const size_type seq = cell->sequence.load(std::memory_order_acquire);
                const auto dif = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos + 1);

                if (dif == 0)
                {
                    if (dequeuePos.compare_exchange_weak(pos, pos + 1,
                        std::memory_order_relaxed,std::memory_order_relaxed)) {
                        break;
                    }
                }
                else if (dif < 0) {
                    return false;
                } else {
                    pos = dequeuePos.load(std::memory_order_relaxed);
                }
            }

            value = std::move(cell->data);
            cell->sequence.store(pos + mask + 1, std::memory_order_release);

            return true;
        }

        [[nodiscard]]
        size_type capacity() const noexcept
        {
            return buffer.size();
        }

    private:

        [[nodiscard]]
        static size_type validateCapacity(const size_type capacity)
        {
            if (capacity == 0 || (capacity & (capacity - 1)) != 0)
                throw std::invalid_argument("Capacity must be a power of two");

            return capacity;
        }

        std::vector<Cell> buffer;
        const size_type mask;

        alignas(std::hardware_destructive_interference_size)
        std::atomic<size_type> enqueuePos { 0 };

        alignas(std::hardware_destructive_interference_size)
        std::atomic<size_type> dequeuePos { 0 };
    };

    void demo()
    {
        MPMCQueue<int> queue { 1024 };

        constexpr std::size_t producerCount { 4 };
        constexpr std::size_t consumerCount { 10 };
        constexpr std::size_t itemsPerProducer { 100'000 };

        std::atomic<std::size_t> consumed { 0 };

        std::vector<std::jthread> producers;
        std::vector<std::jthread> consumers;

        producers.reserve(producerCount);
        consumers.reserve(consumerCount);

        for (std::size_t i = 0; i < consumerCount; ++i)
        {
            consumers.emplace_back([&] {
                int value {};
                while (consumed.load(std::memory_order_relaxed) < producerCount * itemsPerProducer)
                {
                    if (queue.pop(value)){
                        consumed.fetch_add(1, std::memory_order_relaxed);
                        // Process value.
                    } else {
                        std::this_thread::yield();
                    }
                }
            });
        }

        for (std::size_t i = 0; i < producerCount; ++i)
        {
            producers.emplace_back([&, i] {
                for (std::size_t start = i * itemsPerProducer, j = 0; j < itemsPerProducer; ++j) {
                    const int value = static_cast<int>(start + j);
                    while (!queue.push(value)) {
                        std::this_thread::yield();
                    }
                }
            });
        }

        producers.clear();
        consumers.clear();

        std::cout << "Done. Consumed: " << consumed.load(std::memory_order_relaxed) << '\n';
    }
}



namespace perf_tests
{
    constexpr std::size_t kCapacity { 1024 };
    constexpr std::size_t kProducerCount { 4 };
    constexpr std::size_t kConsumerCount { 4 };
    constexpr std::size_t kItemsPerProducer { 250'000 };
    constexpr std::size_t kTotalItems { kProducerCount * kItemsPerProducer };
    constexpr std::size_t kIterations { 5 };

    struct TestResult
    {
        double elapsedMs {};
        double itemsPerSecond {};
        std::size_t consumed {};
        std::size_t errors {};
    };

    template<typename Queue>
    [[nodiscard]]
    TestResult runStressTest(const std::string_view name, Queue& queue)
    {
        std::vector<std::atomic<std::uint32_t>> seen(kTotalItems);

        for (auto& count : seen) {
            count.store(0, std::memory_order_relaxed);
        }

        std::atomic<std::size_t> producersRemaining { kProducerCount };
        std::atomic<std::size_t> consumed { 0 }, errors { 0 };
        std::vector<std::jthread> consumers, producers;

        consumers.reserve(kConsumerCount);
        producers.reserve(kProducerCount);

        const auto produce = [&](const std::size_t id)
        {
            const std::size_t first = id * kItemsPerProducer, last = first + kItemsPerProducer;
            for (std::size_t j = first; j < last; ++j)
            {
                const int value = static_cast<int>(j);
                while (!queue.push(value)) {
                    std::this_thread::yield();
                }
            }

            producersRemaining.fetch_sub(1, std::memory_order_acq_rel);
        };

        const auto consume = [&]
        {
            for (int value {}; ; )
            {
                if (queue.pop(value))
                {
                    if (value < 0 || static_cast<std::size_t>(value) >= kTotalItems){
                        errors.fetch_add(1, std::memory_order_relaxed);
                    } else{
                        seen[static_cast<std::size_t>(value)].fetch_add(1, std::memory_order_relaxed);
                    }
                    consumed.fetch_add(1, std::memory_order_relaxed);
                }
                else if (producersRemaining.load(std::memory_order_acquire) == 0) {
                    break;
                } else{
                    std::this_thread::yield();
                }
            }
        };

        const std::chrono::time_point start = std::chrono::steady_clock::now();
        {
            for (std::size_t i = 0; i < kConsumerCount; ++i)
                consumers.emplace_back(consume);
            for (std::size_t i = 0; i < kProducerCount; ++i)
                producers.emplace_back(produce, i);
            for (auto& thread : producers)
                thread.join();
            for (auto& thread : consumers)
                thread.join();
        }
        const std::chrono::time_point finish = std::chrono::steady_clock::now();

        const double elapsedMs = std::chrono::duration<double, std::milli>(finish - start).count();
        for (const auto& count : seen)
        {
            if (count.load(std::memory_order_relaxed) != 1)
                errors.fetch_add(1, std::memory_order_relaxed);
        }

        const std::size_t consumedCount = consumed.load(std::memory_order_relaxed);
        const std::size_t errorCount = errors.load(std::memory_order_relaxed);
        const double itemsPerSecond = static_cast<double>(consumedCount) / (elapsedMs / 1000.0);

        std::cout << name
                  << ": consumed = " << consumedCount
                  << ", errors = " << errorCount
                  << ", time = " << elapsedMs << " ms"
                  << ", throughput = " << static_cast<std::uint64_t>(itemsPerSecond)
                  << " items/s\n";

        return {
            .elapsedMs = elapsedMs,
            .itemsPerSecond = itemsPerSecond,
            .consumed = consumedCount,
            .errors = errorCount
        };
    }

    void runLockFreeTest(std::vector<TestResult>& results)
    {
        mcmp_queue::MPMCQueue<int> queue { kCapacity };
        results.push_back(runStressTest("Lock-free MPMC", queue));
    }

    void runMutexTest(std::vector<TestResult>& results)
    {
        cv_queue::ring_buffer_2::ThreadSafeQueue<int, kCapacity> queue;
        results.push_back(runStressTest("Mutex MPMC", queue));
    }

    void printAverage(const std::string_view name, const std::vector<TestResult>& results)
    {
        double totalMs {};
        double totalThroughput {};
        std::size_t totalErrors {};

        for (const auto& result : results)
        {
            totalMs += result.elapsedMs;
            totalThroughput += result.itemsPerSecond;
            totalErrors += result.errors;
        }

        const double count = static_cast<double>(results.size());
        std::cout << "\n" << name << " average:\n"
                  << "  time: " << totalMs / count << " ms\n"
                  << "  throughput: "
                  << static_cast<std::uint64_t>(totalThroughput / count)
                  << " items/s\n"
                  << "  errors: " << totalErrors << '\n';
    }

    void run()
    {
        std::vector<TestResult> lockFreeResults;
        std::vector<TestResult> mutexResults;

        lockFreeResults.reserve(kIterations);
        mutexResults.reserve(kIterations);

        for (std::size_t i = 0; i < kIterations; ++i)
        {
            std::cout << "\nIteration " << i + 1 << ":\n";

            if (i % 2 == 0)
            {
                runLockFreeTest(lockFreeResults);
                runMutexTest(mutexResults);
            }
            else
            {
                runMutexTest(mutexResults);
                runLockFreeTest(lockFreeResults);
            }
        }

        std::cout << "\n========== SUMMARY ==========\n";

        printAverage("Lock-free MPMC", lockFreeResults);
        printAverage("Mutex MPMC", mutexResults);
    }
}


void PerformanceExperiments::McMp_LockFree_Queue_vs_CV_Queue_2::TestAll()
{
    // cv_queue::tests::runBenchmark();
    // mcmp_queue::demo();

    perf_tests::run();
}
