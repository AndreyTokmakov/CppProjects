//============================================================================
// Name        : ObjectPoolsExtended.cpp
// Created on  :
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ObjectPoolsExtended
//============================================================================

#include "ObjectPools.h"

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <memory>
#include <vector>
#include <new>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <vector>
#include <array>
#include <cassert>
#include <numeric>
#include <memory_resource>
#include <cmath>

namespace ObjectPoolsExtended {

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


    template <typename Ty, typename Allocator = std::allocator<Ty>>
    class ObjectPool final
    {
    private:
        using object_type = Ty;
        using pointer = object_type*;
        using size_type = typename std::vector<pointer>::size_type;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

    private:
        std::vector<pointer> pool;
        std::vector<pointer> available;

        static constexpr size_type DEFAULT_CHUNK_SIZE{ 5 };

        size_type newBlockSize { DEFAULT_CHUNK_SIZE };
        size_type size {0};
        size_type capacity{ 0 };

        void addChunk()
        {
            // Allocate a new chunk of uninitialized memory
            pointer newBlock { m_allocator.allocate(newBlockSize) };

            // Keep all allocated blocks in 'pool' to delete them later:
            pool.push_back(newBlock);

            available.resize(newBlockSize);
            std::iota(std::begin(available), std::end(available), newBlock);

            capacity += newBlockSize;
            newBlockSize *= 2; // TODO: 2 --> const
        }

        // The allocator to use for allocating and deallocating chunks.
        Allocator m_allocator;

    protected:

        struct Deleter {
            ObjectPool* pool {nullptr};

            void operator()(pointer object) const noexcept {
                std::destroy_at(object);
                // Return object mem pointer back to pool
                pool->available.push_back(object);
                --pool->size;
            }
        };

    public:
        ObjectPool() = default;

        explicit ObjectPool(const Allocator& allocator) : m_allocator{ allocator } {
            // Trivial
        }

        virtual ~ObjectPool() {
            // Note: this implementation assumes that all objects handed out by this
            // pool have been returned to the pool before the pool is destroyed.
            // The following statement asserts if that is not the case.
            assert(available.size() == DEFAULT_CHUNK_SIZE * (std::pow(2, pool.size()) - 1));

            // Deallocate all allocated memory.
            size_t chunkSize{ DEFAULT_CHUNK_SIZE };
            for (auto* chunk : pool) {
                m_allocator.deallocate(chunk, chunkSize);
                chunkSize *= 2;
            }
        }

        // Allow move construction and move assignment.
        ObjectPool(ObjectPool&& src) noexcept = default;
        ObjectPool& operator=(ObjectPool&& rhs) noexcept = default;

        // Prevent copy construction and copy assignment.
        ObjectPool(const ObjectPool& src) = delete;
        ObjectPool& operator=(const ObjectPool& rhs) = delete;

        // Reserves and returns an object from the pool. Arguments can be
        // provided which are perfectly forwarded to a constructor of T.
        template<typename... Args>
        std::unique_ptr<object_type, Deleter> acquireObject(Args... args)
        {
            // If there are no free objects, allocate a new chunk.
            if (available.empty()) {
                addChunk();
            }

            // Get a free object.
            pointer objectPtr { available.back() };

            // Initialize, i.e. construct, an instance of T in an uninitialized block of memory
            // using placement new, and perfectly forward any provided arguments to the constructor.
            new(objectPtr) object_type{ std::forward<Args>(args)... };

            // Remove the object from the list of free objects.
            available.pop_back();
            ++size;

            // Wrap the initialized object and return it.
            return std::unique_ptr<object_type, Deleter> { objectPtr, Deleter{this}};
        }

        [[nodiscard]]
        size_type Size() const noexcept {
            return size;
        }

        [[nodiscard]]
        size_type Capacity() const noexcept {
            return capacity;
        }
    };
}

void Extended_Test()
{
    using namespace ObjectPoolsExtended;

    using TestType = TypeLarge;
    // using TestType = TestTypes::TypeMedium;
    ObjectPool<TestType> pool{};

    std::cout << "Pool address = " << &pool << std::endl;
    std::cout << "Capacity = " << pool.Capacity() << std::endl;

    auto object1{ pool.acquireObject() };
    auto object2{ pool.acquireObject() };
    auto object3{ pool.acquireObject() };

    std::cout << "Capacity = " << pool.Capacity() << ". Size = " << pool.Size() << std::endl;

    auto object4{ pool.acquireObject() };
    auto object5{ pool.acquireObject() };
    auto object6{ pool.acquireObject() };

    std::cout << "Capacity = " << pool.Capacity() << ". Size = " << pool.Size() << std::endl;

    auto object7{ pool.acquireObject() };
    auto object8{ pool.acquireObject() };
    auto object9{ pool.acquireObject() };

    std::cout << "Capacity = " << pool.Capacity() << ". Size = " << pool.Size() << std::endl;

    {
        auto obj1 { pool.acquireObject() };
        auto obj2 { pool.acquireObject() };
        auto obj3 { pool.acquireObject() };
        std::cout << "Capacity = " << pool.Capacity() << ". Size = " << pool.Size() << std::endl;
    }

    std::cout << "Capacity = " << pool.Capacity() << ". Size = " << pool.Size() << std::endl;

    std::cout << typeid(object3).name() << std::endl;
}