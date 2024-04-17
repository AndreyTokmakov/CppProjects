/**============================================================================
Name        : UsingCustomAllocator_List.cpp
Created on  : 17.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : UsingCustomAllocator_List.cpp
============================================================================**/

#include "UsingCustomAllocator_List.h"
#include "../Helpers/Helpers.h"

#include <iostream>
#include <cstdint>
#include <numeric>

#include <array>
#include <list>
#include <vector>

namespace UsingCustomAllocator_List::AllocatorOne
{
    template<class _Tp>
    struct Allocator: public std::__allocator_base<_Tp>
    {
        using value_type = _Tp;
        using pointer = value_type*;
        using size_type = size_t;

        static inline constexpr size_type Capacity { 10 };

        static_assert(!std::is_same_v<value_type, void>,
                      "Type of the Objects in the pool can not be void");

        struct alignas(sizeof(value_type)) Placeholder {};

        // Located together to increase Cache Hit chances
        std::array<Placeholder, Capacity> pool {};
        std::array<uint16_t, Capacity> available {};
        int32_t tail {0};

    public:

        Allocator()
        {
            tail = Capacity - 1;
            std::iota(available.begin(), available.end(), 0);
        }

        pointer allocate(size_type n)
        {
            if (n > tail + 1)
                return nullptr;

            // FIXME: если N более нужно найти щелый последовательный блок свободной памяти

            tail = tail - n;
            const size_t offset = available[tail + 1];

            std::cout << "Allocate(n: " << n << ", tail: " << tail + 1 << ", offset: " << available[tail + 1] << ")\n";
            return reinterpret_cast<pointer>(&pool[offset]);
        }

        void deallocate(pointer ptr,
                        [[maybe_unused]] size_type n)
        {
            const size_t offset = (reinterpret_cast<size_type>(ptr) - reinterpret_cast<size_type>(&pool)) / sizeof (value_type);
            std::cout << "deallocate(n: " << n << ", tail: " << tail + 1 << ", offset: " << offset << ")\n";

            // FIXME: если N более 1 помещать всех слоты из блока как свободные
            available[++tail] = offset;
        }
    };
};


namespace UsingCustomAllocator_List::AllocatorSimple
{
    template <typename T>
    struct Allocator
    {
        using value_type = T;
        using pointer = value_type*;
        using size_type = size_t;

        pointer allocate(size_type n)
        {
            return static_cast<pointer>(::operator new(n * sizeof(T)));
        }

        void deallocate(pointer prt, size_type n) noexcept
        {
            ::operator delete(prt);
        }
    };
}

namespace UsingCustomAllocator_List::AllocatorTwo
{
    // Backend storage for our allocator
    struct StackBuffer
    {
        // 512kB buffer
        alignas(alignof(std::max_align_t)) std::array<std::byte,512*1024> buffer;
        size_t used = 0;

        // Calculate the required offset for allocating type T
        // so that T is properly aligned.
        template <typename T>
        [[nodiscard]]
        size_t get_offset() const
        {
            if (used % alignof(T) == 0)
                return 0;
            return alignof(T) - (used % alignof(T));
        }

        // Allocate sizeof(T)*cnt bytes in the buffer, properly aligned.
        template <typename T>
        T* allocate(std::size_t cnt)
        {
            size_t off = get_offset<T>();
            if (used + off + cnt*sizeof(T) >= buffer.size())
                throw std::bad_alloc(); // The buffer is full.

            // Pointer to the start of the allocated block.
            T* result = reinterpret_cast<T*>(buffer.data()+used+off);
            used += off + cnt*sizeof(T);
            return result;
        }

        // Deallocation is a no-op.
        void deallocate(void*, std::size_t) {}
    };

    template <typename T>
    struct StackAllocator
    {
        using value_type = T; // Required

        // We need to initialize the first allocator with our buffer.
        explicit StackAllocator(StackBuffer* storage) : storage_(storage) {}
        StackAllocator(const StackAllocator&) = default;

        // Conversion constructor that passes the buffer along.
        template <typename U>
        explicit StackAllocator(const StackAllocator<U>& other) : storage_(other.storage_) {
        }

        // Required
        T* allocate(std::size_t n) {
            return storage_->allocate<T>(n);
        }

        // Required
        void deallocate(T* p, std::size_t n) {
            storage_->deallocate(p,n);
        }
    private:
        // Required for the conversion constructor.
        template <typename U> friend struct StackAllocator;

        // Pointer to the buffer, note that this will increase
        // the size of each container that uses this allocator
        // by sizeof(StackBuffer*), e.g. 8 bytes on x86-64.
        StackBuffer* storage_;
    };
}

namespace UsingCustomAllocator_List
{
    void Test()
    {
        using namespace Helpers;

        // std::list<int, AllocatorOne::Allocator<int>> testList1 {};
        // std::list<int, AllocatorSimple::Allocator<int>> testList2 {};


        std::vector<Integer, AllocatorOne::Allocator<Integer>> values { };

        values.reserve(2);


        values.emplace_back(100);
        values.emplace_back(200);

        for (const auto & entry: values)
            std::cout << entry.value << std::endl;


    }
};

void UsingCustomAllocator_List::TestAll()
{
    Test();
};