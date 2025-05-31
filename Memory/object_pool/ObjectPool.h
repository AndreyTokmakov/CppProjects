/**============================================================================
Name        : ObjectPool.h
Created on  : 31.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ObjectPool.h
============================================================================**/

#ifndef CPPPROJECTS_MEMORY_OBJECTPOOL_H
#define CPPPROJECTS_MEMORY_OBJECTPOOL_H

#include <memory>
#include <vector>
#include <numeric>
#include <mutex>
#include <cmath>

namespace Memory
{

    template <typename Ty, typename Allocator = std::allocator<Ty>>
    class ObjectPool final
    {
        using object_type = Ty;
        using pointer = object_type*;
        using size_type = typename std::vector<pointer>::size_type;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        static constexpr size_type defaultBlockSize { 64 };

        struct Deleter final
        {
            static inline ObjectPool* pool { nullptr };

            void operator()(pointer object) const noexcept
            {
                std::destroy_at(object);

                /** Return object mem pointer back to pool **/
                pool->available.push_back(object);
            }
        };

    public:

        using ObjectPtr = std::unique_ptr<object_type, Deleter>;

        ObjectPool() {
            deleter.pool = this;
        }

        explicit ObjectPool(const Allocator& allocator) : m_allocator { allocator } {
            deleter.pool = this;
        }

        ~ObjectPool()
        {
            // Deallocate all allocated memory.
            for (pointer chunk : pool) {
                m_allocator.deallocate(chunk, defaultBlockSize);
            }
        }

        /** Allow move construction and move assignment **/
        ObjectPool(ObjectPool&& src) noexcept = default;
        ObjectPool& operator=(ObjectPool&& rhs) noexcept = default;

        /** Prevent copy construction and copy assignment **/
        ObjectPool(const ObjectPool& src) = delete;
        ObjectPool& operator=(const ObjectPool& rhs) = delete;

        template<typename... Args>
        std::unique_ptr<object_type, Deleter> acquireObject(Args... args)
        {
            if (available.empty()) {
                addChunk(available);
            }

            pointer obj = new (available.back()) object_type { std::forward<Args>(args)... };
            available.pop_back();

            /** Wrap the initialized object and return it **/
            return std::unique_ptr<object_type, Deleter> { obj, deleter };
        }

        [[nodiscard]]
        size_type Capacity() const noexcept {
            return capacity;
        }

    private:

        void addChunk(std::vector<pointer>& poolLocal)
        {
            // Allocate a new chunk of uninitialized memory
            const pointer newBlock { m_allocator.allocate(defaultBlockSize) };

            // Keep all allocated blocks in 'pool' to delete them later:
            {
                std::unique_lock<std::mutex> lock { mutex };
                // Keep all allocated blocks in 'pool' to delete them later:
                pool.push_back(newBlock);
            }

            poolLocal.resize(defaultBlockSize);
            std::iota(std::begin(poolLocal), std::end(poolLocal), newBlock);
        }

    private:
        Allocator m_allocator;
        mutable std::mutex mutex;
        std::vector<pointer> pool;

        size_type capacity { 0 };

        Deleter deleter;

        /** Objects allocated and available per thread. **/
        static inline thread_local std::vector<pointer> available;

    };
}

#endif //CPPPROJECTS_MEMORY_OBJECTPOOL_H
