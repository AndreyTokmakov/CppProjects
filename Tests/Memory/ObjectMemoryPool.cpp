/**============================================================================
Name        : ObjectMemoryPool.cpp
Created on  : 03.11.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ObjectMemoryPool.cpp
============================================================================**/

#include "ObjectMemoryPool.h"

#include <iostream>
#include <memory>
#include <vector>
#include <numeric>
#include <cmath>

#include <thread>
#include <syncstream>


#define LOG std::osyncstream(std::cout) << now()
#define ERR std::osyncstream(std::cerr) << now()


namespace
{
    using namespace std::chrono;

    constexpr std::string_view FORMAT { "[%d-%02d-%02d %02d:%02d:%02d.%06ld] " };

    [[nodiscard]]
    std::string now(const time_point<system_clock>& timestamp = system_clock::now()) noexcept
    {
        const time_t time { system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(&(buffer.front()), FORMAT.data(),
                                          tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                                          duration_cast<microseconds>(timestamp - time_point_cast<seconds>(timestamp)).count());
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
    }
}


namespace ObjectMemoryPool
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

        std::mutex mutex;
        std::vector<pointer> pool;

        static inline thread_local std::vector<pointer> available;

        static inline constexpr size_type DEFAULT_CHUNK_SIZE { 5 };
        static inline constexpr size_type GROWTH_STRATEGY { 2 };

        size_type newBlockSize { DEFAULT_CHUNK_SIZE };
        size_type capacity { 0 };

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
            ObjectPool* pool {nullptr};

            void operator()(pointer object) const noexcept
            {
                std::destroy_at(object);

                /// Return object mem pointer back to pool
                pool->available.push_back(object);
            }
        };

    public:
        using ObjectPtr = std::unique_ptr<object_type, Deleter>;

    public:
        ObjectPool() = default;

        explicit ObjectPool(const Allocator& allocator) : m_allocator{ allocator } {
            // Trivial
        }

        ~ObjectPool()
        {   // Note: this implementation assumes that all objects handed out by this
            // pool have been returned to the pool before the pool is destroyed.
            // The following statement asserts if that is not the case.
            assert(available.size() == DEFAULT_CHUNK_SIZE * (std::pow(2, pool.size()) - 1));

            // Deallocate all allocated memory.
            size_t chunkSize{ DEFAULT_CHUNK_SIZE };
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
            // If there are no free objects, allocate a new chunk.
            if (available.empty()) {
                addChunk();
            }

            // Get a free object.
            const pointer objectPtr { available.back() };

            pointer obj = new (objectPtr) object_type { std::forward<Args>(args)... };

            // Remove the object from the list of free objects.
            available.pop_back();

            // Wrap the initialized object and return it.
            return std::unique_ptr<object_type, Deleter> { objectPtr, Deleter{this}};
        }


        [[nodiscard]]
        size_type Capacity() const noexcept {
            return capacity;
        }
    };
}


namespace ThreadLocalStorage
{
    struct Worker
    {
        static inline thread_local std::vector<std::string> params = []{
            LOG << "Vector is created for thread " << std::this_thread::get_id() << std::endl;
            return std::vector<std::string>{};
        }();

        std::vector<std::jthread> pool;

        void executor()
        {
            LOG << "Starting job" << std::endl;

            for (int i = 0; i < 4; ++i)
                params.emplace_back(now());

            std::this_thread::sleep_for(std::chrono::seconds(1u));

            for (const std::string& s: params)
                std::cout << s << ' ';
            std::cout << std::endl;
        }

        void start()
        {
            for (uint32_t idx = 0; idx < 4; ++idx)
            {
                std::this_thread::sleep_for(std::chrono::microseconds (250u));
                pool.emplace_back(&Worker::executor, this);
            }
        }

        ~Worker()
        {
            for (auto& T: pool)
                T.join();
        }
    };

}

void ObjectMemoryPool::TestAll()
{

    ThreadLocalStorage::Worker worker {};
    worker.start();

}