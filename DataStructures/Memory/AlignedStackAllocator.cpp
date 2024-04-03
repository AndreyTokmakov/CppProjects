/**============================================================================
Name        : AlignedStackAllocator.cpp
Created on  : 02.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AlignedStackAllocator
============================================================================**/

#include "AlignedStackAllocator.h"

#include <iostream>
#include <array>
#include <cstdint>
#include <numeric>
#include "../Wrapper/Wrapper.h"

namespace Memory::AlignedStackAllocator
{

    using namespace Helpers;

    template<class T, size_t N>
    struct Allocator
    {
        using object_type = T;
        using pointer = object_type*;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        struct alignas(sizeof(object_type)) Placeholder {};

        // Located together to increase Cache Hit chances
        std::array<Placeholder, N> pool {};
        std::array<uint16_t, N> available {};
        int32_t tail {0};

    public:

        Allocator()
        {
            tail = N - 1;
            std::iota(available.begin(), available.end(), 0);
        }

        template<typename ... Args>
        pointer AllocateAndConstruct(Args ... params)
        {
            if (tail < 0) {
                return nullptr;
            }

            const size_t offset = available[tail--];
            try {
                return new (&pool[offset]) object_type { std::forward<Args>(params)... };
            } catch (...) {
                ++tail;
                throw;
            }
        }

        void DestroyAndDeallocate(pointer ptr) // + validate
        {
            if (nullptr == ptr)
                throw std::runtime_error("nullptr ptr");
            if (0 != (reinterpret_cast<size_t>(ptr) - reinterpret_cast<size_t>(&pool[0]))  % sizeof(object_type))
                throw std::runtime_error("wrong alignment ptr");
            if (reinterpret_cast<size_t>(&pool[0]) > reinterpret_cast<size_t>(ptr))
                throw std::runtime_error("Not in the allocated block 1");
            if (reinterpret_cast<size_t>(ptr) > reinterpret_cast<size_t>(&pool[N - 1]))
                throw std::runtime_error("Not in the allocated block 2");

            const size_t offset = (reinterpret_cast<size_t>(ptr) - reinterpret_cast<size_t>(&pool)) / sizeof (object_type);
            std::destroy_at(ptr);
            available[++tail] = offset;
        }
    };
};

namespace Memory::AlignedStackAllocator::Tests
{
    void SimpleTest()
    {
        constexpr size_t capacity {100};
        Allocator<Integer, capacity> allocator;


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


void Memory::AlignedStackAllocator::TestAll()
{
    Tests::SimpleTest();
}