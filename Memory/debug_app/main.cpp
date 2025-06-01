/**============================================================================
Name        : main.cpp
Created on  : 31.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "../object_pool/ObjectPool.h"
#include <iostream>
#include <thread>

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

    using TestType   = TypeMedium;

    constexpr int32_t tests = 100;

}

namespace
{
    void benchmarkPool(const int32_t iterations,
                       const int32_t allocations,
                       const int32_t threadsCount = 8)
    {
        Memory::ObjectPool<TestType> objectPool;

        {
            std::vector<std::jthread> workers;
            workers.reserve(threadsCount);

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
}

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{

    benchmarkPool(10'000, 1'000, 1);


    return EXIT_SUCCESS;
}

