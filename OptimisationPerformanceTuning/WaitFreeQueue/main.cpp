/**============================================================================
Name        : main.cpp
Created on  : 23.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>

#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <chrono>

#include <atomic>
#include <thread>

namespace
{
    constexpr bool is_pow_of_2(int value) noexcept {
        return (value && !(value & value - 1));
    }


    template<typename T>
    std::ostream& operator<<(std::ostream& stream,
                             const std::vector<T>& vector)
    {
        for (const auto& v: vector)
            stream << v << ' ';
        return stream;
    }

    struct ScopedTimer
    {
        const std::string_view benchmarkName;
        const std::chrono::high_resolution_clock::time_point start {
                std::chrono::high_resolution_clock::now()
        };

        explicit ScopedTimer(std::string_view info) :
                benchmarkName {info} {
        }

        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer(ScopedTimer&&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;
        ScopedTimer& operator=(ScopedTimer&&) = delete;

        ~ScopedTimer()
        {
            const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

            std::cout << std::left << std::setw(14) << benchmarkName << ":  ";
            std::cout << time_span.count() << " seconds.\n";
        }
    };
}




namespace RingBuffer_WithAtomics
{
    template<typename T, size_t Capacity>
    struct QueueOne
    {
        using value_type = T;
        using size_type = size_t;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> tail {0};
        alignas(std::hardware_destructive_interference_size) std::atomic<size_type> head {0};

        std::vector<value_type> buffer;
        bool overlapped {false};

        explicit QueueOne()
        {
            buffer.resize(Capacity);
        }

        // TODO: Prefetch ??
        bool read(value_type& output) noexcept
        {
            size_type readIdx = tail.load(std::memory_order_relaxed);
            if (!overlapped && readIdx == head.load(std::memory_order_relaxed)) {
                return false;
            }

            output = buffer[readIdx];
            if (++readIdx == Capacity) {
                overlapped = false;
                readIdx = 0;
            }

            tail.store(readIdx, std::memory_order_release);
            return true;
        }

        // TODO: Prefetch ??
        bool add(value_type value) noexcept
        {
            size_type idx = head.load(std::memory_order_relaxed);
            buffer[idx] = value;

            if (++idx == Capacity) {
                overlapped = true;
                idx = 0;
            }

            head.store(idx, std::memory_order_release);
            return true;
        }
    };


    template<typename T, size_t Capacity>
    struct QueueTwo
    {
        using value_type = T;
        using size_type = size_t;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        struct IndexData
        {
            std::atomic<size_t> atomicIdx {0};
            size_t idx {0} ;

            void store()
            {
                atomicIdx.store(idx, std::memory_order_release);
            }
        };

        alignas(std::hardware_destructive_interference_size) IndexData tail {};
        alignas(std::hardware_destructive_interference_size) IndexData head {};

        std::vector<value_type> buffer;
        bool overlapped {false};

        explicit QueueTwo()
        {
            buffer.resize(Capacity);
        }

        // TODO: Prefetch ??
        bool read(value_type& output) noexcept
        {
            // TODO: Remove atomic
            if (!overlapped && tail.idx == head.atomicIdx.load(std::memory_order_relaxed)) {
                return false;
            }

            output = buffer[tail.idx];
            if (++tail.idx == Capacity) {
                overlapped = false;
                tail.idx = 0;
            }

            tail.store();
            return true;
        }

        // TODO: Prefetch ??
        bool add(value_type value) noexcept
        {
            // TODO: Remove atomic
            buffer[head.idx] = value;

            if (++head.idx == Capacity) {
                overlapped = true;
                head.idx = 0;
            }

            head.store();
            return true;
        }
    };

    template<typename T>
    void get(T& ring) {
        int value{0};
        bool res = ring.read(value);

        std::cout << std::boolalpha << res << " -> " << value << std::endl;
    }

    void Test()
    {
        QueueTwo<int, 3> ring{};

        ring.add(1);
        ring.add(2);
        ring.add(3);

        std::cout << ring.buffer << std::endl;

        get(ring);
        get(ring);

        //ring.add(4);
        //ring.add(5);

        //get(ring);
    }


    void Benchmark()
    {
        QueueOne<int, 100'000> queue {};
        // QueueTwo<int, 100'000> queue {};

        constexpr int eventsCount {100'000'000};

        ScopedTimer timer {"benchmark"};

        auto consume = [&]() {
            int eventsRead = 0, result;
            while (eventsCount > eventsRead)
            {
               queue.read(result);
               ++eventsRead;
            }
        };

        auto produce = [&]() {
            for (int idx = 0; idx <= eventsCount; ++idx)
            {
                queue.add(idx);
            }
        };

        std::jthread consumer {consume};
        std::jthread producer {produce};

        consumer.join();
        producer.join();
    }
}



int main(int argc, char **argv)
{
    RingBuffer_WithAtomics::Benchmark();

    return 0;
}

