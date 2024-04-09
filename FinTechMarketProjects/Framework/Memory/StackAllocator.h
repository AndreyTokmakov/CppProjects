/**============================================================================
Name        : MemoryPool.h
Created on  : 09.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MemoryPool.h
============================================================================**/

#ifndef CPPPROJECTS_STACKALLOCATOR_H
#define CPPPROJECTS_STACKALLOCATOR_H

#include <iostream>
#include <array>
#include <cstdint>
#include <numeric>

namespace Memory
{
    template<class T, size_t N>
    struct StackAllocator
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

        StackAllocator()
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

        void DestroyAndDeallocate(pointer ptr)
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

#endif //CPPPROJECTS_STACKALLOCATOR_H
