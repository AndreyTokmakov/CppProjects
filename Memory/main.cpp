/**============================================================================
Name        : OpenCV.cpp
Created on  : 10.01.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Memory C++ project
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>
#include <chrono>
#include <thread>
#include <chrono>
#include <syncstream>

#include "ObjectPool.h"
#include "Experiments.h"
#include "Common.h"
#include "PerfUtilities.hpp"
#include <tcmalloc/tcmalloc.h>

namespace
{
    template<size_t N>
    class Object
    {
        char buffer[N]{ 0 };

    public:
        Object() = default;

        // Object(const Object& obj) {}
        // Object& operator=(Object& right) {}

        // Object(Object&& obj) noexcept {}
        // Object& operator=(Object&& right) noexcept {}
    };


    using TypeTiny   = Object<sizeof(int)>;
    using TypeSmall  = Object<128>;
    using TypeMedium = Object<1024>;
    using TypeLarge  = Object<1024 * 64>;

    using TestType   = TypeLarge;

    constexpr int32_t tests = 100;
    constexpr int32_t allocations = 1024;

}



namespace Tests
{

    void benchmarkPool(const int32_t iterations,
                       const int32_t allocations,
                       const int32_t threadsCount = 8)
    {
        Memory::ObjectPool<TestType> objectPool;

        {
            PerfUtilities::ScopedTimer timer {"Pool"};
            std::vector<std::jthread> workers;
            for (int32_t i = 0; i < threadsCount; ++i) {
                workers.emplace_back([&] {
                    std::vector<decltype(objectPool)::ObjectPtr> created;
                    created.reserve(allocations);
                    for (int32_t x = 0; x < iterations; ++x) {
                        for (int32_t n = 0; n < allocations; ++n) {
                            created.push_back(objectPool.acquireObject());
                        }
                        created.clear();
                    }
                });
            }
            workers.clear();
        }
    }

    void benchmarkNoPool(const int32_t iterations,
                         const int32_t allocations,
                         const int32_t threadsCount = 8)
    {
        std::vector<std::jthread> workers;

        PerfUtilities::ScopedTimer timer{"NoPool"};
        for (int32_t i = 0; i < threadsCount; ++i)
        {
            workers.emplace_back([&]{
                std::vector<std::unique_ptr<TestType>> created;
                created.reserve(allocations);
                for (int32_t x = 0; x < iterations; ++x)
                {
                    for (int32_t n = 0; n < allocations; ++n) {
                        auto ptr = std::make_unique<TestType>();
                        created.push_back(std::move(ptr));
                    }
                    created.clear();
                }
            });
        }

        workers.clear();
    }
}


namespace Memory::GoodPools_Tests
{

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

        static constexpr size_type DEFAULT_CHUNK_SIZE { 5 };
        static constexpr size_type GROWTH_STRATEGY { 2 };
        size_type _new_block_size { DEFAULT_CHUNK_SIZE };

        void addChunk()
        {
            // Allocate a new chunk of uninitialized memory
            pointer newBlock { m_allocator.allocate(_new_block_size) };

            // Keep all allocated blocks in 'pool' to delete them later:
            pool.push_back(newBlock);

            available.resize(_new_block_size);
            std::iota(std::begin(available), std::end(available), newBlock);
            _new_block_size *= GROWTH_STRATEGY;
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

        virtual ~ObjectPool()
        {
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
            // If there are no free objects, allocate a new chunk.
            if (available.empty()) {
                addChunk();
            }

            // Get a free object.
            const pointer objectPtr { available.back() };

            // Initialize, i.e. construct, an instance of T in an uninitialized block of memory
            // using placement new, and perfectly forward any provided arguments to the constructor.
            pointer obj = new (objectPtr) object_type { std::forward<Args>(args)... };

            // Remove the object from the list of free objects.
            available.pop_back();

            // Wrap the initialized object and return it.
            return std::unique_ptr<object_type, Deleter> { objectPtr, Deleter{this}};
        }
    };

    void PerformanceTests()
    {

        {
            PerfUtilities::ScopedTimer timer {"NoPool"};

            std::vector<std::unique_ptr<TestType>> store;
            store.reserve(allocations);

            for (int32_t t = 0; t < tests; ++t)
            {
                for (int32_t n = 0; n < allocations; ++n)
                    store.push_back(std::make_unique<TestType>());
                store.clear();
            }
        }

        {
            ObjectPool<TestType> pool{};
            {
                PerfUtilities::ScopedTimer timer{"Pool 1"};

                std::vector<decltype(pool)::ObjectPtr> store;
                store.reserve(allocations);

                for (int32_t t = 0; t < tests; ++t) {
                    for (int32_t n = 0; n < allocations; ++n)
                        store.push_back(pool.acquireObject());
                    store.clear();
                }
            }
        }

        {
            Memory::ObjectPool<TestType> pool;
            const PerfUtilities::ScopedTimer timer {"Pool 2"};

            std::vector<decltype(pool)::ObjectPtr> store;
            store.reserve(allocations);

            for (int32_t t = 0; t < tests; ++t)
            {
                for (int32_t n = 0; n < allocations; ++n)
                    store.push_back(pool.acquireObject());
                store.clear();
            }
        }
    }
}






int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Experiments::TestAll();

    Tests::benchmarkPool(1'000, 1'000, 8);
    Tests::benchmarkNoPool(1'000, 1'000, 8);



    // Memory::GoodPools_Tests::PerformanceTests();


    return EXIT_SUCCESS;
}



// bazel build --cxxopt='-std=c++17'

// TCMalloc:
//  - https://github.com/google/tcmalloc/blob/master/docs/quickstart.md

// git clone https://github.com/google/tcmalloc.git
// git clone https://github.com/abseil/abseil-cpp.git