/**============================================================================
Name        : FreeList_ThreadSafe.cpp
Created on  : 18.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FreeList_ThreadSafe.cpp
============================================================================**/

#include "FreeList_ThreadSafe.hpp"

#include <atomic>
#include <cstddef>
#include <new>
#include <vector>

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cstring>
#include <chrono>


namespace
{
    class FreeList
    {
        static constexpr std::size_t BlockSize      = 4096;
        static constexpr std::size_t BlocksPerChunk = 64;

        struct Node
        {
            Node* next;
        };

    public:

        FreeList() = default;

        ~FreeList()
        {
            for (void* chunk : chunks) {
                ::operator delete(chunk, std::align_val_t{BlockSize});
            }
        }

        void* allocate(const std::size_t size,
                       const std::size_t alignment = alignof(std::max_align_t))
        {
            if (size > BlockSize || alignment > BlockSize) {
                return ::operator new(size, std::align_val_t{alignment});
            }

            Node* head = freeList.load(std::memory_order_acquire);
            while (head)
            {
                Node* next = head->next;
                if (freeList.compare_exchange_weak(head,next, std::memory_order_acq_rel, std::memory_order_acquire))
                {
                    return head;
                }
            }

            refill();
            return allocate(size, alignment);
        }

        void deallocate(void* ptr,
                        const std::size_t size,
                        const std::size_t alignment = alignof(std::max_align_t))
        {
            if (!ptr)
                return;
            if (size > BlockSize || alignment > BlockSize) {
                return ::operator delete(ptr, std::align_val_t{alignment});
            }

            Node* node = static_cast<Node*>(ptr);
            Node* head = freeList.load(std::memory_order_acquire);
            do {
                node->next = head;
            }
            while (!freeList.compare_exchange_weak(head,node, std::memory_order_release,std::memory_order_acquire));
        }

    private:

        void refill()
        {
            constexpr std::size_t chunkSize = BlockSize * BlocksPerChunk;
            void* raw = ::operator new(chunkSize, std::align_val_t{BlockSize});

            chunks.push_back(raw);
            char* chunk = static_cast<char*>(raw);
            for (std::size_t i = 0; i < BlocksPerChunk; ++i)
            {
                Node* node = reinterpret_cast<Node*>(chunk + i * BlockSize);
                deallocate(node, BlockSize);
            }
        }

        std::atomic<Node*> freeList { nullptr };
        std::vector<void*> chunks;
    };
}

namespace
{

    struct TestRunner
    {
        int passed = 0;
        int failed = 0;

        void run(const char* name, bool (*test)())
        {
            bool result = false;
            try {
                result = test();
            } catch (...) {
                result = false;
            }

            if (result) {
                ++passed;
                std::cout << "[OK]   " << name << "\n";
            } else {
                ++failed;
                std::cout << "[FAIL] " << name << "\n";
            }
        }

        void summary() const
        {
            std::cout << "\n==== SUMMARY ====\n";
            std::cout << "Passed: " << passed << "\n";
            std::cout << "Failed: " << failed << "\n";
        }
    };

    bool test_basic_allocate()
    {
        FreeList pool;

        void* p = pool.allocate(128);
        if (!p) return false;

        std::memset(p, 0xAB, 128);
        pool.deallocate(p, 128);

        return true;
    }

    bool test_reuse()
    {
        FreeList pool;

        void* p1 = pool.allocate(64);
        pool.deallocate(p1, 64);
        void* p2 = pool.allocate(64);

        return p1 == p2;
    }

    bool test_large_fallback()
    {
        FreeList pool;

        void* p = pool.allocate(8192); // > BlockSize
        if (!p) return false;

        std::memset(p, 0xCC, 8192);
        pool.deallocate(p, 8192);

        return true;
    }

    bool test_alignment()
    {
        FreeList pool;

        void* p = pool.allocate(256, 64);
        if (!p) return false;

        uintptr_t addr = reinterpret_cast<uintptr_t>(p);
        bool aligned = (addr % 64) == 0;

        pool.deallocate(p, 256, 64);
        return aligned;
    }

    bool test_stress_single_thread()
    {
        FreeList pool;
        constexpr int N = 100000;

        std::vector<void*> ptrs;
        ptrs.reserve(N);

        for (int i = 0; i < N; ++i)
        {
            void* p = pool.allocate(128);
            if (!p) return false;
            ptrs.push_back(p);
        }

        for (void* p : ptrs)
            pool.deallocate(p, 128);

        return true;
    }

    bool test_multithreaded()
    {
        FreeList pool;

        constexpr int Threads = 8;
        constexpr int Iter = 50000;

        std::atomic<bool> failed{false};

        auto worker = [&]()
        {
            for (int i = 0; i < Iter; ++i)
            {
                void* p = pool.allocate(256);
                if (!p)
                {
                    failed = true;
                    return;
                }

                std::memset(p, 0x11, 256);
                pool.deallocate(p, 256);
            }
        };

        std::vector<std::thread> threads;

        for (int i = 0; i < Threads; ++i)
            threads.emplace_back(worker);

        for (auto& t : threads)
            t.join();

        return !failed.load();
    }

    bool test_many_refills()
    {
        FreeList pool;
        constexpr int N = 10000;

        std::vector<void*> ptrs;
        for (int i = 0; i < N; ++i)
        {
            void* p = pool.allocate(4096);
            if (!p) return false;
            ptrs.push_back(p);
        }

        for (void* p : ptrs)
            pool.deallocate(p, 4096);

        return true;
    }

    void runTests()
    {
        TestRunner runner;

        runner.run("Basic allocate", test_basic_allocate);
        runner.run("Reuse memory", test_reuse);
        runner.run("Large fallback", test_large_fallback);
        runner.run("Alignment", test_alignment);
        runner.run("Single-thread stress", test_stress_single_thread);
        runner.run("Multi-thread stress", test_multithreaded);
        runner.run("Many refills", test_many_refills);
        runner.summary();
    }
}


void free_list_thread_safe::TestAll()
{
    runTests();
}
