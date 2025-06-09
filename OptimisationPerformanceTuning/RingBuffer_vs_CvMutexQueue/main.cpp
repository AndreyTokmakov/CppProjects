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
#include <deque>
#include <memory>
#include <array>
#include <atomic>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <syncstream>

namespace
{
    struct ScopedTimer
    {
        const std::string_view benchmarkName;
        const bool warmUp { false };
        const std::chrono::high_resolution_clock::time_point start;

        explicit ScopedTimer(std::string_view info, bool warmUp = false) :
                benchmarkName { info },
                warmUp { warmUp },
                start { std::chrono::high_resolution_clock::now() } {
        }

        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer(ScopedTimer&&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;
        ScopedTimer& operator=(ScopedTimer&&) = delete;

        ~ScopedTimer()
        {
            const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

            if (false == warmUp)
            {
                std::cout << std::left << std::setw(14) << benchmarkName << ":  ";
                std::cout << time_span.count() << " seconds.\n";
            }
        }
    };
}


namespace RingBuffer_vs_CVMutexQueue
{
    template<typename T,
            int_fast16_t Capacity = std::numeric_limits<uint16_t>::max()>
    struct RingBuffer
    {
        using size_type = uint16_t;
        using value_type = T;
        using collection_type = std::vector<value_type>;

        std::atomic<size_type> idxWrite { 0 };
        size_type idxWriteCached { 0 };
        size_type idxRead { 0 };
        collection_type buffer {};

        explicit RingBuffer(): buffer(Capacity + 1)  {
        }

        void put(const value_type& value)
        {
            buffer[idxWriteCached] = value;
            idxWriteCached = idxWrite.fetch_add(1, std::memory_order::release) + 1;
        }

        template<typename ... Types>
        void emplace(Types&& ... params)
        {
            new (&buffer[idxWriteCached]) value_type { std::forward<Types>(params)... };
            idxWriteCached = idxWrite.fetch_add(1, std::memory_order::release) + 1;
        }

        [[nodiscard]]
        bool try_read_next(value_type& result)
        {
            if (idxRead == idxWrite.load(std::memory_order::acquire)) {
                return false;
            }

            result = std::move(buffer[idxRead++]);
            return true;
        }
    };
}

namespace RingBuffer_vs_CVMutexQueue
{
    template<typename T>
    class BlockingQueue
    {
        using value_type = T;
        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        static constexpr std::chrono::duration<int64_t, std::ratio<1, 1000>> waitTimeout { std::chrono::seconds(5U) };

        mutable std::mutex mutex;
        std::deque<value_type> data_queue;
        std::condition_variable updated;

    public:

        void push(value_type&& new_value)
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                data_queue.push_back(std::move(new_value));
            }
            updated.notify_one();
        }

        bool pop(value_type& value)
        {
            std::unique_lock<std::mutex> lock(mutex);
            bool ok = updated.wait_for(lock, waitTimeout, [this] {
                return false == data_queue.empty();
            });
            if (!ok)
                return false;
            value = std::move(data_queue.front());
            data_queue.pop_front();
            return true;
        }

        [[nodiscard]]
        bool empty() const noexcept
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.empty();
        }

        [[nodiscard]]
        size_t size() const noexcept
        {
            std::lock_guard<std::mutex> lock(mutex);
            return data_queue.size();
        }
    };
}

namespace RingBuffer_vs_CVMutexQueue::Tests
{
    constexpr int32_t evtCount = 10'000'000;

    template<typename T, size_t Size>
    struct Data
    {
        T value { 0 };
        std::array<char, Size> buffer {};
    };

    using Type = int;
    // using Type = Data<int, 32>;
    // using Type = Data<int, 128>;
    // using Type = Data<int, 1024>;
    // using Type = Data<int, 1024 * 4>;

    void multithreaded_buffer_test(bool warmUp = false)
    {
        RingBuffer<Type> buffer;
        std::jthread producer ([&buffer] {
            for (int i = 0; i < evtCount; ++i) {
                // buffer.put(Type {i});
                buffer.emplace(i);
            }
        });

        std::jthread consumer ([&buffer] {
            int count { 0 };
            Type result;
            while (true) {
                count += buffer.try_read_next(result);
                if (count >= evtCount) {
                    break;
                }
            }
        });
    }

    void multithreaded_queue_test(bool warmUp = false)
    {
        BlockingQueue<Type> queue;
        std::jthread producer ([&queue] {
            for (int i = 0; i < evtCount; ++i) {
                queue.push(Type {i});
            }
        });

        std::jthread consumer ([&queue] {
            int count { 0 };
            Type result;
            while (true) {
                queue.pop(result);
                ++count;
                if (count >= evtCount) {
                    break;
                }
            }
        });
    }
}



int main([[maybe_unused]] const int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);
    const int type = args.empty() ? 0 : atoi(args.front().data());

    using namespace RingBuffer_vs_CVMutexQueue::Tests;

    ScopedTimer timer { "Result"};
    // multithreaded_buffer_test();
    multithreaded_queue_test();

    /*
    ScopedTimer timer { "Result"};
    if (0 == type) {
        multithreaded_buffer_test();
    }
    else if (1 == type) {
        multithreaded_queue_test();
    }*/

    return 0;
}


/**
 *
 context-switches  :   130 vs 10,194
 sys calls seconds :   0.000000000 vs 1.013109000

 Performance counter stats for './app':

            947.94 msec task-clock                #    1.990 CPUs utilized
               130      context-switches          #  137.139 /sec
                19      cpu-migrations            #   20.043 /sec
               214      page-faults               #  225.752 /sec
     3,912,391,128      cycles                    #    4.127 GHz                      (74.75%)
        61,443,791      stalled-cycles-frontend   #    1.57% frontend cycles idle     (74.75%)
         2,595,076      stalled-cycles-backend    #    0.07% backend cycles idle      (74.73%)
     1,013,155,610      instructions              #    0.26  insn per cycle
                                                  #    0.06  stalled cycles per insn  (74.74%)
       192,628,098      branches                  #  203.206 M/sec                    (74.77%)
         8,326,695      branch-misses             #    4.32% of all branches          (75.47%)
     1,044,698,335      L1-dcache-loads           #    1.102 G/sec                    (75.86%)
        24,127,802      L1-dcache-load-misses     #    2.31% of all L1-dcache accesses  (75.05%)
   <not supported>      LLC-loads
   <not supported>      LLC-load-misses

       0.476393553 seconds time elapsed

       0.948336000 seconds user
       0.000000000 seconds sys



Result        :  1.07288 seconds.

 Performance counter stats for './app':

          2,079.30 msec task-clock                #    1.935 CPUs utilized
            10,194      context-switches          #    4.903 K/sec
                17      cpu-migrations            #    8.176 /sec
               215      page-faults               #  103.400 /sec
     8,491,155,549      cycles                    #    4.084 GHz                      (75.02%)
        15,905,225      stalled-cycles-frontend   #    0.19% frontend cycles idle     (75.00%)
       893,884,439      stalled-cycles-backend    #   10.53% backend cycles idle      (74.83%)
     6,675,364,736      instructions              #    0.79  insn per cycle
                                                  #    0.13  stalled cycles per insn  (74.71%)
     1,484,501,096      branches                  #  713.943 M/sec                    (75.17%)
       111,892,252      branch-misses             #    7.54% of all branches          (75.26%)
     2,464,967,518      L1-dcache-loads           #    1.185 G/sec                    (75.24%)
        24,987,352      L1-dcache-load-misses     #    1.01% of all L1-dcache accesses  (75.13%)
   <not supported>      LLC-loads
   <not supported>      LLC-load-misses

       1.074702235 seconds time elapsed

       1.064556000 seconds user
       1.013109000 seconds sys

*/