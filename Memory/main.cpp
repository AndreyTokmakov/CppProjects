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

#include "ObjectPool/ObjectPool.h"
#include "Experiments/Experiments.h"
#include "Common/Common.h"
#include "PerfUtilities.h"
#include <tcmalloc/tcmalloc.h>



namespace Experimental
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
        ObjectPool<int> pool;
        std::vector<std::jthread> workers;
        for (int i = 0; i < 8; ++i) {
            workers.emplace_back([&]{
                std::vector<ObjectPool<int>::wrapped_pointer> created;
                for (int n = 0; n < 5; ++n) {
                    auto ptr = pool.acquireObject(i);
                    created.push_back(std::move(ptr));
                    std::this_thread::sleep_for(std::chrono::milliseconds (250u));
                }
            });
        }

        for (auto& T: workers)
            T.join();
    }
}

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    Experimental::test();

    /*
    {
        PerfUtilities::ScopedTimer timer{"Test"};
        std::this_thread::sleep_for(std::chrono::seconds(1u));
    }*/




    return EXIT_SUCCESS;
}

// bazel build --cxxopt='-std=c++17'

// TCMalloc:
//  - https://github.com/google/tcmalloc/blob/master/docs/quickstart.md

// git clone https://github.com/google/tcmalloc.git
// git clone https://github.com/abseil/abseil-cpp.git