/**============================================================================
Name        : ObjectPool_Stack_FixedSize.cpp
Created on  : 20.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ObjectPool Stack FixedSize
============================================================================**/

#include "ObjectPool_Stack_FixedSize.h"

#include <iostream>
#include <array>
#include <vector>
#include <numeric>
#include <memory>

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


namespace ObjectPool_Stack_FixedSize
{
    template<class T, size_t Capacity>
    struct Pool
    {
        using object_type = T;
        using pointer = object_type*;
        using size_type = size_t;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        struct Deleter final
        {
			static inline Pool* pool { nullptr };

			void operator()(pointer object) const noexcept
			{
				std::destroy_at(object);
				pool->available.push_back(object);
			}
		};

        Pool()
        {
            deleter.pool = this;
            available.resize(Capacity);
            std::iota(available.begin(), available.end(), ptrPool);
        }

        template<typename ... Args>
        std::unique_ptr<object_type, Deleter> acquireObject(Args ... params)
        {
            if (available.empty()) {
                // TODO: Fallback to system allocator
                return nullptr;
            }

            pointer obj = new (available.back()) object_type { std::forward<Args>(params)... };
            available.pop_back();

            return std::unique_ptr<object_type, Deleter> { obj, deleter };
        }

        std::array<object_type, Capacity> buffer {};
        pointer ptrPool { reinterpret_cast<pointer>(buffer.data()) };
        std::vector<pointer> available;
        Deleter deleter;
    };
}



void ObjectPool_Stack_FixedSize::TestAll()
{
    Pool<int, 10> pool;

    std::cout << pool.available.size() << std::endl;

    {
        auto obj = pool.acquireObject(12345);
        std::cout << *obj << std::endl;

        std::cout << pool.available.size() << std::endl;
    }

    std::cout << pool.available.size() << std::endl;
}
