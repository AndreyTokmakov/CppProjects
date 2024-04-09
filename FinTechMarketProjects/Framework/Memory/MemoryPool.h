/**============================================================================
Name        : MemoryPool.h
Created on  : 09.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MemoryPool.h
============================================================================**/

#ifndef CPPPROJECTS_MEMORYPOOL_H
#define CPPPROJECTS_MEMORYPOOL_H

#include <vector>
#include <memory>
#include <cassert>
#include <numeric>

namespace MemoryPool
{
    template <typename Ty, typename Allocator = std::allocator<Ty>>
    class ObjectPool final
    {
        using object_type = Ty;
        using pointer = object_type*;
        using size_type = typename std::vector<pointer>::size_type;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

    private:
        std::vector<pointer> pool;
        std::vector<pointer> available;

        static constexpr size_type DEFAULT_CHUNK_SIZE { 10 };
        static constexpr size_type GROWTH_STRATEGY { 2 };

        size_type newBlockSize { DEFAULT_CHUNK_SIZE };
        size_type capacity { 0 };

        std::mutex mtx;

        void addChunk()
        {
            // Allocate a new chunk of uninitialized memory
            pointer newBlock { m_allocator.allocate(newBlockSize) };

            // Keep all allocated blocks in 'pool' to delete them later:
            pool.push_back(newBlock);

            available.resize(newBlockSize);
            std::iota(std::begin(available), std::end(available), newBlock);

            capacity += newBlockSize;
            newBlockSize *= GROWTH_STRATEGY;
        }

        // The allocator to use for allocating and deallocating chunks.
        Allocator m_allocator;

    protected:

        struct Deleter final
        {
            ObjectPool& pool;

            void operator()(pointer object) const noexcept
            {
                std::destroy_at(object);
                std::lock_guard<std::mutex> lock { pool.mtx };

                // Return object mem pointer back to pool
                pool.available.push_back(object);
            }
        };

    public:
        using ObjectPtr = std::unique_ptr<object_type, Deleter>;

    public:
        ObjectPool() = default;

        explicit ObjectPool(const Allocator& allocator) : m_allocator { allocator } {
            addChunk();
        }

        virtual ~ObjectPool()
        {   // Note: this implementation assumes that all objects handed out by this
            // pool have been returned to the pool before the pool is destroyed.
            // The following statement asserts if that is not the case.
            assert(available.size() == DEFAULT_CHUNK_SIZE * (std::pow(2, pool.size()) - 1));

            // Deallocate all allocated memory.
            size_t chunkSize { DEFAULT_CHUNK_SIZE };
            for (auto* chunk : pool) {
                m_allocator.deallocate(chunk, chunkSize);
                chunkSize *= GROWTH_STRATEGY;
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
            pointer objectPtr {nullptr};

            {
                std::lock_guard<std::mutex> lock { mtx };
                // If there are no free objects, allocate a new chunk.
                if (available.empty()) {
                    addChunk();
                }

                // Get a free object.
                objectPtr = available.back();

                // Remove the object from the list of free objects.
                available.pop_back();
            }

            // Initialize, i.e. construct, an instance of T in an uninitialized block of memory
            // using placement new, and perfectly forward any provided arguments to the constructor.
            new (objectPtr) object_type { std::forward<Args>(args)... };

            // Wrap the initialized object and return it.
            return ObjectPtr { objectPtr, Deleter { *this } };
        }

        [[nodiscard]]
        inline size_type Capacity() const noexcept {
            return capacity;
        }
    };
}

#endif //CPPPROJECTS_MEMORYPOOL_H
