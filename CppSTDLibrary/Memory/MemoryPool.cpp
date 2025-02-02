/**============================================================================
Name        : MemoryPool.cpp
Created on  : 01.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MemoryPool.cpp
============================================================================**/

#include <cstddef>
#include <stack>
#include <memory>
#include <iostream>

#include "MemoryPool.h"

namespace MemoryPool
{
    template <typename T, size_t PoolSize>
    class MemoryPool
    {

        T* pool;
        std::stack<size_t> freeIndices;

    public:

        MemoryPool() : pool(new T[PoolSize]), freeIndices()
        {
            for (size_t i = 0; i < PoolSize; ++i) {
                freeIndices.push(i);
            }
        }

        ~MemoryPool() {
            delete[] pool;
        }


        T* allocate()
        {
            if (freeIndices.empty()) {
                throw std::bad_alloc();
            }

            size_t index = freeIndices.top();
            freeIndices.pop();
            return &pool[index];
        }

        void deallocate(T* ptr)
        {
            size_t index = ptr - pool;
            freeIndices.push(index);
        }
    };

    struct SmallObject
    {
        int data[2];
        SmallObject() { std::cout << "SmallObject␣constructed\n"; }
        ~SmallObject() { std::cout << "SmallObject␣destroyed\n"; }
    };
}

void MemoryPool::TestAll()
{
    MemoryPool<SmallObject, 10> pool;
    auto obj1 = pool.allocate();
    new (obj1) SmallObject(); // Placement new
    pool.deallocate(obj1);

}