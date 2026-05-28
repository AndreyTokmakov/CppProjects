/**============================================================================
Name        : Tests.cpp
Created on  : 09.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Tests.cpp
============================================================================**/

#include <iostream>
#include <chrono>
#include <thread>

#include "Tests.h"
#include "Wrapper.h"
#include "../Memory/MemoryPool.h"
#include "../Memory/StackAllocator.h"

#include "PerfUtilities.hpp"


namespace MemoryPool
{
    template<size_t N>
    class Object  {
        char buffer[N]{ 0 };

    public:
        Object() {}

        /*
        Object(const Object& obj) {}
        Object& operator=(Object& right) {}

        Object(Object&& obj) noexcept {}
        Object& operator=(Object&& right) noexcept {}
        */
    };

    using TypeTiny   = Object<sizeof(int)>;
    using TypeSmall  = Object<128>;
    using TypeMedium = Object<1024>;
    using TypeLarge  = Object<1024 * 64>;

    void PerformanceTests_SingleThread()
    {
        using TestType = TypeLarge;
        constexpr size_t size = 256, MAX_COUNT = 32, threadsCount {16};

        ObjectPool<TestType> pool{};
        thread_local TestType* ints[size];

        auto task_allocator = [&]()
        {
            for (size_t i = 0; i < MAX_COUNT; i++) {
                for (size_t n = 0; n < MAX_COUNT; n++) {
                    for (size_t k = 0; k < size; k++)
                        ints[k] = new TestType;
                    for (size_t k = 0; k < size; k++)
                        delete ints[k];
                }
            }
        };

        auto task_memory_pool = [&]()
        {
            for (size_t i = 0; i < MAX_COUNT; i++) {
                for (size_t n = 0; n < MAX_COUNT; n++) {
                    for (size_t k = 0; k < size; k++)
                        auto object{ pool.acquireObject() };
                }
            }
        };

        std::cout << " ======================== Using pool: =========================\n";
        {
            const PerfUtilities::ScopedTimer timer { "Pool" };
            std::vector<std::jthread> workers;
            for (uint32_t i = 0; i < threadsCount; ++i)
                workers.emplace_back(task_memory_pool);
            for (auto& job: workers)
                job.join();
        }

        std::cout << " ======================== Using std::new(): =========================\n";
        {
            const PerfUtilities::ScopedTimer timer { "New" };
            std::vector<std::jthread> workers;
            for (uint32_t i = 0; i < threadsCount; ++i)
                workers.emplace_back(task_allocator);
            for (auto& job: workers)
                job.join();
        }
    }
}

namespace StackAllocator
{
    using namespace Helpers;

    void SimpleTest()
    {
        constexpr size_t capacity {100};
        Memory::StackAllocator<Integer, capacity> allocator;

        std::cout << "-----------------------------------------------------\n";
        std::array<Integer*, capacity> objs {};
        for (int testId  = 0; testId < 100; ++testId)
        {
            for (int i = 0; i < capacity; ++i)
                objs[i] = allocator.AllocateAndConstruct((i + 1) * 10);
            for (const auto ptr: objs)
                allocator.DestroyAndDeallocate(ptr);
        }
        std::cout << "-----------------------------------------------------\n";
    }
}


void Tests::TestAll()
{
    // MemoryPool::PerformanceTests_SingleThread();
    StackAllocator::SimpleTest ();
}
