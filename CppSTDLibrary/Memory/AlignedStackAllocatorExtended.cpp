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
#include <vector>
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
            Allocator* allocator {};

            void operator()(const pointer ptr) const noexcept {
                allocator->deallocate(ptr);
            }
        };

        using ObjectPtr = std::unique_ptr<object_type, Deleter>;

        static_assert(!std::is_same_v<object_type, void>, "Type of the Objects in the pool can not be void");

        struct Slot
        {
            alignas(object_type) std::byte storage[sizeof(object_type)];
        };

        std::array<Slot, Capacity> pool {};
        std::array<uint16_t, Capacity> available {};
        int32_t freeIndex {0};

    public:

        Allocator()
        {
            freeIndex = Capacity - 1;
            std::iota(available.begin(), available.end(), 0);
        }

        template<typename... Args>
        ObjectPtr allocate(Args&&... params)
        {
            if (freeIndex < 0) {
                return nullptr;
            }

            const std::size_t offset = available[freeIndex--];

            try {
                pointer ptr = std::construct_at(
                    reinterpret_cast<pointer>(pool[offset].storage),
                    std::forward<Args>(params)...);

                return ObjectPtr { ptr, Deleter { this } };
            } catch (...) {
                ++freeIndex;
                throw;
            }
        }


        void deallocate(pointer ptr) noexcept
        {
            const size_t offset = getOffset(ptr);
            std::destroy_at(ptr);
            available[++freeIndex] = offset;
        }

        [[nodiscard]]
        std::size_t getOffset(const pointer ptr) const noexcept
        {
            const auto* first = reinterpret_cast<const std::byte*>(pool.data());
            const auto* current = reinterpret_cast<const std::byte*>(ptr);

            return (current - first) / sizeof(Slot);
        }
    };
}


namespace
{
    void simpleTest()
    {
        constexpr size_t capacity {100};
        Allocator<Integer, capacity> allocator;
        const auto v = allocator.allocate(123);
    }

    void complexTest()
    {
        constexpr size_t capacity {100};
        Allocator<Integer, capacity> allocator;

        std::vector<decltype(allocator)::ObjectPtr> objects;
        objects.reserve(capacity);

        for (int i = 0; i < capacity; ++i) {
            objects.push_back(allocator.allocate(i));
        }
    }
}


void memory::aligned_stack_allocator_extended::TestAll()
{
    // simpleTest();
    complexTest();
}