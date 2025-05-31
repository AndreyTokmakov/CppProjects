/**============================================================================
Name        : Experiments.cpp
Created on  : 31.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Experiments.cpp
============================================================================**/

#include "Experiments.h"
#include "Common.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <numeric>
#include <syncstream>

namespace Experiments
{
    template <typename Ty, typename Allocator = std::allocator<Ty>>
    struct ObjectPool
    {
        using object_type = Ty;
        using pointer = object_type*;
        using size_type = typename std::vector<pointer>::size_type;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        struct Deleter final
        {
            static inline ObjectPool* pool { nullptr };

            void operator()(pointer object) const noexcept
            {
                std::destroy_at(object);
                pool->available.push_back(object);
                LOG << "Object at " << object << " returned back to  " << &(pool->available) << std::endl;
            }
        };

        using wrapped_pointer = std::unique_ptr<object_type, Deleter>;

        ObjectPool() {
            deleter.pool = this;
        }

        explicit ObjectPool(const Allocator& allocator) : m_allocator { allocator } {
            deleter.pool = this;
        }

        template<typename... Args>
        wrapped_pointer acquireObject(Args... args)
        {
            if (available.empty()) {
                addChunk(available);
            }

            pointer obj = new (available.back()) object_type { std::forward<Args>(args)... };
            available.pop_back();

            LOG << "Object at " << obj << " taken from  " << &(available) << std::endl;

            return wrapped_pointer { obj, deleter };
        }

        void addChunk(std::vector<pointer>& poolLocal)
        {
            constexpr int newBlockSize = 10;
            const pointer newBlock { m_allocator.allocate(newBlockSize) };
            poolLocal.resize(newBlockSize);
            std::iota(std::begin(poolLocal), std::end(poolLocal), newBlock);
            LOG << "Local poll " << &(available) << " has been resized. size = " << poolLocal.size() << std::endl;
        }

    private:
        Allocator m_allocator;
        Deleter deleter;

        // TODO: Rename
        static inline thread_local std::vector<pointer> available;
    };


    void test()
    {
        ObjectPool<int> objectPool;
        std::vector<std::jthread> workers;
        for (int i = 0; i < 8; ++i) {
            workers.emplace_back([&]{
                std::vector<ObjectPool<int>::wrapped_pointer> created;
                for (int n = 0; n < 5; ++n) {
                    auto ptr = objectPool.acquireObject(i);
                    created.push_back(std::move(ptr));
                    std::this_thread::sleep_for(std::chrono::milliseconds (250u));
                }
            });
        }

        for (auto& T: workers)
            T.join();
    }
}

void Experiments:: TestAll()
{
    Experiments::test();
}