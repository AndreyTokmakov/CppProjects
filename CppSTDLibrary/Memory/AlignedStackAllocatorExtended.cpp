/**============================================================================
Name        : AlignedStackAllocatorExtended.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AlignedStackAllocatorExtended
============================================================================**/

#include "Memory.hpp"

#include <iostream>
#include <array>
#include <memory>
#include <numeric>
#include "../Helpers/Helpers.h"

using namespace Helpers;

namespace
{
    template<class T, size_t Capacity>
    struct Allocator
    {
        using object_type = T;
        using pointer = object_type*;

        struct Deleter final
        {
            static inline Allocator*  allocator { nullptr };

            void operator()(const pointer ptr) const noexcept {
                allocator->deallocate(ptr);
            }
        };

        using ObjectPtr = std::unique_ptr<object_type, Deleter>;

        static_assert(!std::is_same_v<object_type, void>, "Type of the Objects in the pool can not be void");

        struct alignas(sizeof(object_type)) Placeholder {};

        std::array<Placeholder, Capacity> pool {};
        std::array<uint16_t, Capacity> available {};
        int32_t tail {0};

    public:

        Allocator()
        {
            tail = Capacity - 1;
            std::iota(available.begin(), available.end(), 0);
            Deleter::allocator = this;
        }

        template<typename ... Args>
        ObjectPtr AllocateAndConstruct(Args ... params)
        {
            ObjectPtr result { nullptr };
            if (tail < 0) {
                return nullptr;
            }

            const size_t offset = available[tail--];
            try {
                pointer ptr = new (&pool[offset]) object_type { std::forward<Args>(params)... };
                result = ObjectPtr { ptr, Deleter{} };
            } catch (...) {
                ++tail;
                throw;
            }

            return result;
        }

        void deallocate(pointer ptr)
        {
            const size_t offset = (reinterpret_cast<size_t>(ptr) - reinterpret_cast<size_t>(&pool)) / sizeof (object_type);
            std::destroy_at(ptr);
            available[++tail] = offset;
        }
    };
}


namespace
{
    void simpleTest()
    {
        constexpr size_t capacity {100};
        Allocator<Integer, capacity> allocator;
        const auto v = allocator.AllocateAndConstruct(123);
    }

    void complexTest()
    {
        constexpr size_t capacity {100};
        Allocator<Integer, capacity> allocator;
        const auto v = allocator.AllocateAndConstruct(123);
    }
}


void memory::aligned_stack_allocator_extended::TestAll()
{
    // simpleTest();
    complexTest();
}