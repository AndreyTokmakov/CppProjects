/**============================================================================
Name        : CustomStackAllocator.cpp
Created on  : 02.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CustomStackAllocator.cpp
============================================================================**/

#include "CustomStackAllocator.h"

#include <iostream>
#include <array>
#include "../Wrapper/Wrapper.h"


#if 0
void* operator new(size_t count) {
    std::cout << "allocating " << count << " bytes\n";
    return malloc(count);
}

void operator delete(void* ptr) noexcept {
    std::cout << "Deleting the " << reinterpret_cast<long>(ptr) << "\n";
    free(ptr);
}
#endif

namespace Memory::CustomStackAllocator
{
    using namespace Helpers;

    template<class T, size_t N>
    class Allocator
    {
        using object_type = T;
        using pointer = object_type*;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        // TODO: to aligned storage?
        std::byte rawMemory[N * sizeof(Integer)] {};
        Integer *pool = reinterpret_cast<Integer*>(rawMemory);

        // FIXME
        // static_assert(pool % alignof(T) == 0);


        int tail {0};
        std::array<size_t, N> available {};

    public:

        Allocator()
        {
            tail = N - 1;
            //std::cout << reinterpret_cast<long>(&pool)  << std::endl;
            for (size_t idx = 0; idx < N; ++idx) {
                available[idx] = idx;
                std::cout << "\t" << idx << " --> " << reinterpret_cast<long>(&pool[idx])  << std::endl;
            }
        }

        template<typename ... Args>
        pointer AllocateAndConstruct(Args ... params)
        {
            if (tail < 0) {
                std::cerr << "Out ot space" << std::endl;
                return nullptr;
            }

            const size_t offset = available[tail--];
            try {
                pointer ptr =  new (&pool[offset]) object_type { std::forward<Args>(params)... };
                // std::cout << ptr << std::endl;
                return ptr;
            } catch (...)
            {
                std::cerr << "Exc" << std::endl;
                ++tail;
                throw;
            }
        }

        void DestroyAndDeallocate(pointer ptr) // + validate
        {
            if (nullptr == ptr)
                throw std::runtime_error("nullptr ptr");
            /*if (0 == (ptr - pool)  % sizeof(object_type) )
                throw std::runtime_error("wrong alignment ptr");*/
            if (&pool[0] > ptr)
                throw std::runtime_error("Not in the allocated block 1");
            if (ptr > &pool[N - 1])
                throw std::runtime_error("Not in the allocated block 2");

            /*
            if (!(nullptr != ptr && 0 == (ptr - pool)  % sizeof(object_type) && ptr >= pool && (pool + N - 1) >= ptr)) {
                throw std::runtime_error("alien ptr");
            }*/

            const size_t offset = ptr - pool;
            // ptr->~object_type();
            std::destroy_at(ptr);
            available[++tail] = offset;
        }
    };
}

namespace Memory::CustomStackAllocator::Tests
{
    void SimpleTest()
    {
        Allocator<Integer, 5> allocator;

        std::cout << "-----------------------------------------------------\n";
        std::array<Integer*, 5> objs {};
        for (int testId  = 0; testId < 10; ++testId)
        {
            for (int i = 0; i < 5; ++i)
                objs[i] = allocator.AllocateAndConstruct((i + 1) * 10);
            for (const auto ptr: objs)
                allocator.DestroyAndDeallocate(ptr);
        }
        std::cout << "-----------------------------------------------------\n";
    }
}

void Memory::CustomStackAllocator::TestAll()
{
    Tests::SimpleTest();
}