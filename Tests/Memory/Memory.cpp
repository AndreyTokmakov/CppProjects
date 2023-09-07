/**============================================================================
Name        : Memory.cpp
Created on  : 18.02.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Memory test
============================================================================**/

#include "Memory.h"

#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <cstring>

#include "../Helpers/Utilities.h"
#include "../Helpers/Long.h"
#include "../Helpers/Object.h"


namespace Memory
{
    struct Object
    {
        Object() { std::cout << "Object()" << std::endl; }
        ~Object() { std::cout << "~Object()" << std::endl; }

        Object(const Object&) { std::cout << "Object(const Object& obj)" << std::endl; }
        Object(Object&&) noexcept { std::cout << "Object(Object&& obj) noexcept" << std::endl; }

        Object& operator==(const Object&) {
            std::cout << "Object& operator==(const Object&)" << std::endl;
            return *this;
        }

        Object& operator==(Object&&) noexcept {
            std::cout << "Object& operator==(Object&&) noexcept" << std::endl;
            return *this;
        }
    };

    class BadClass
    {
    private:
        Memory::Object* obj = new Object();
        // std::unique_ptr<Memory::Object> obj { std::make_unique<Memory::Object>() };

    public:

        BadClass() {
            throw 1;
        }

        ~BadClass() {
            delete obj;
        }
    };

    void CleanUP_Exception_Test()
    {
        try {
            BadClass b;
        }
        catch (...) {
            std::cout << "Ops" << std::endl;
        }
    }
}


namespace Memory
{
    using session = int;

    auto session_factory(int id) {
        return std::make_shared<session>(id);
    }

    // https://ibob.bg/blog/2023/01/01/tracking-shared-ptr-leaks/
    void SharedPtrLeak()
    {
        std::shared_ptr<session> leak;

        std::vector<std::weak_ptr<session>> registry;

        constexpr int N = 20;
        srand(unsigned(std::time(nullptr)));
        auto i_to_leak = rand() % (2 * N);

        // std::cout << "i_to_leak = " << i_to_leak << std::endl;

        for (int i = 0; i < N; ++i) {
            auto sptr = session_factory(i);
            registry.push_back(sptr);
            if (i == i_to_leak) {
                leak = sptr;
                // std::cout << "Expect " << leak << " to leak\n";
            }
        }

        for (auto& w : registry) {
            if (w.use_count()) {
                std::cout << "found a leak in " << w.lock() << "\n";
            }
        }

        std::cout << "Done\n";
    }
}

namespace Memory
{
    struct ARPHeader
    {
        uint16_t htype {0};
        uint16_t ptype {0};
        uint8_t  hlen {};
        uint8_t  plen {};
        uint16_t opcode {0};
        uint8_t  sender_mac[6]{};
        uint32_t sender_ip {};
        uint8_t  target_mac[6]{};
        uint32_t target_ip {};

    public:
        [[nodiscard]]
        bool SetSenderMACAddress([[maybe_unused]] std::string_view mac) const {
            return htype != 0;
        }

    } __attribute__((packed, aligned(1))) ;


    void initMemset(ARPHeader* arpHeader)
    {
        memset(arpHeader, 0, sizeof(ARPHeader));
    }

    void initAssignment(ARPHeader* arpHeader)
    {
        *arpHeader = {};
    }

    void Memset_vs_Assignment()
    {
        std::unique_ptr<ARPHeader> apr { std::make_unique<ARPHeader>()};

        apr->target_ip = 12345;
        std::cout << apr->target_ip << std::endl;

        // initMemset(apr.get());
        initAssignment(apr.get());

        std::cout << apr->target_ip << std::endl;
    }

    void Memset_vs_Assignment_Perf()
    {
        std::unique_ptr<ARPHeader> apr { std::make_unique<ARPHeader>()};
        constexpr size_t iterCount {1'000'00};

        {
            auto start = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < iterCount; ++i)
            {
                for (int n = 0; n < iterCount; n++)
                {
                    initAssignment(apr.get());
                    // initMemset(apr.get());
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "Result: " << duration << " microseconds" << std::endl;
        }

        {
            auto start = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < iterCount; ++i)
            {
                for (int n = 0; n < iterCount; n++)
                {
                    // initAssignment(apr.get());
                    initMemset(apr.get());
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "Result: " << duration << " microseconds" << std::endl;
        }
    }

    void Double_Delete_Nullptr()
    {
        int* iPtr = new int(100500);

        std::cout << *iPtr << " at " << iPtr << std::endl;

        {
            delete iPtr;
            iPtr = nullptr;
        }

        delete iPtr;
    }

    void SharedPtr_BadUsage_DoubleDelete()
    {
        std::shared_ptr<int> i(new int(2));
        {
            std::shared_ptr<int> i2(i.get());
        }
    }

    void SharedPtr_BadUsage_DoubleDelete_FIX_EmptyDeleter()
    {
        std::shared_ptr<int> i(new int(2));
        {
            auto empty_deleter = [](int* ptr){
                std::cout << "Doing nothing with " << ptr << std::endl;
            };
            std::shared_ptr<int> i2(i.get(), empty_deleter);
        }
        // Now OK
    }

    template<class T>
    struct TracingAllocator
    {
        using value_type = T;
        using pointer = value_type*;

        TracingAllocator() = default;

        template<class U>
        constexpr explicit TracingAllocator(const TracingAllocator<U>&) noexcept {
        }

        [[nodiscard]]
        pointer allocate(std::size_t n)
        {
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(value_type))
                throw std::bad_array_new_length();

            if (pointer ptr = static_cast<pointer>(std::malloc(n * sizeof(value_type))))
            {
                trace(ptr, n);
                return ptr;
            }

            throw std::bad_alloc();
        }

        void deallocate(pointer ptr, std::size_t n) noexcept
        {
            trace(ptr, n, 0);
            std::free(ptr);
        }

    private:

        void trace(pointer ptr, std::size_t n, bool alloc = true) const
        {
            std::cout << (alloc ? "Alloc: " : "Dealloc: ") << sizeof(value_type) * n
                      << " bytes at " << std::hex << std::showbase
                      << reinterpret_cast<void*>(ptr) << std::dec << '\n';
        }
    };


    void AllocateShared_And_Trace()
    {
        TracingAllocator<int> traceAllocator;
        auto deleter = [&traceAllocator](int* ptr){
            traceAllocator.deallocate(ptr, 1);
        };

        std::shared_ptr<int> sharedInt (traceAllocator.allocate(1), deleter, traceAllocator);
    }

    void AllocateMakeShared_And_Trace()
    {
        TracingAllocator<int> traceAllocator;
        auto deleter = [&traceAllocator](int* ptr){
            traceAllocator.deallocate(ptr, 1);
        };

        std::shared_ptr<int> sharedInt (traceAllocator.allocate(1), deleter, traceAllocator);
    }

    void Shared_Weak_UsageCount()
    {
        // shard_counter = 1, weak_counter = 0
        std::shared_ptr<int> sharedInt = std::make_shared<int>();
        
        std::cout << "shared use count: " << sharedInt.use_count() << std::endl;

        // shard_counter = 1, weak_counter = 1
        std::weak_ptr<int> weakInt = sharedInt;

        std::cout << "shared use count: " << sharedInt.use_count()
                  << " | weak use count: " << weakInt.use_count() << std::endl;
    }
}



namespace Memory::UniquePtrExperiments
{
    using namespace Helpers;

    void PointerToObjectOnStack()
    {
        auto longDeleter = [](Long* ptr) {
            std::cout << "Skip deletion for Long(" << ptr << ")\n";
            //delete ptr;
        };

        Long l1(10);

        {
            std::unique_ptr<Long, decltype(longDeleter)> lPtr{&l1, longDeleter};
        }

        std::cout << "Done\n";

    }

    template <typename T>
    struct CustomDeleter {
        CustomDeleter() = default;
        CustomDeleter(const CustomDeleter&) = default;
        CustomDeleter(CustomDeleter&) = default;
        CustomDeleter(CustomDeleter&&)  noexcept = default;
        void operator()(T* p) const { delete p; };
    };

    void CustomDeleterTests()
    {

        {
            std::cout << "Example 1\n";
            std::unique_ptr<int, CustomDeleter<int>> foo_unique(new int(), CustomDeleter<int>()); // move CustomDeleter
            std::unique_ptr<int, CustomDeleter<int>> f2 = std::move(foo_unique); // move CustomDeleter
        }

        {
            std::cout << "Example 2\n";
            CustomDeleter<int> deleter;
            std::unique_ptr<int, CustomDeleter<int> &> f3(new int(), deleter); // reference CustomDeleter

            std::unique_ptr<int, CustomDeleter<int> &> f4 = std::move(f3); // reference CustomDeleter
            // std::unique_ptr<int, CustomDeleter<int>&&> f41 = std::move(f4); // Won't compile

            std::unique_ptr<int, CustomDeleter<int>> f5 = std::move(f4); // non-const copy CustomDeleter
            // std::unique_ptr<Foo, CustomDeleter<int>&> f6 = std::move(f5); // Won't compile
        }

        {
            std::cout << "Example 3\n";
            CustomDeleter<int> deleter;

            std::unique_ptr<int, const CustomDeleter<int> &> f7(new int(), deleter); // reference CustomDeleter
            std::unique_ptr<int, CustomDeleter<int>> f8 = std::move(f7); // copy CustomDeleter
            //std::unique_ptr<int, CustomDeleter<int>&> f9 = std::move(f8); // Won't compile
        }
    }
}

namespace Memory::PlacementNew
{

    void CreateObjects()
    {
        using T = Helpers::Long;
        constexpr size_t max_size = 10;

        decltype(auto) memBlock = operator new[](max_size * sizeof(T));
        T *pool = static_cast<T*>(memBlock);
        for (int pos = 0; pos < 10; ++pos)
            new (&pool[pos]) T {pos};

        // for (int i = 9; i >= 0; --i)
        //    pool[i].~T();

        T* ptr = std::launder(reinterpret_cast<T*>(pool));
        std::destroy(ptr, ptr + max_size);

        operator delete[](memBlock);
    }
}

namespace Memory::RestrictObjectHeapCreation
{

    template <typename... Ts>
    struct LocalObject* createObject(Ts&&... params);

    struct LocalObject
    {
        int value { 0 };
        std::string name;

        LocalObject(int v, std::string s): value {v}, name { std::move(s)} {
            std::cout << "LocalObject(" << value << ", " << std::quoted(name) << ")\n";
        }

        ~LocalObject() {
            std::cout << "~LocalObject(" << value << ", " << std::quoted(name) << ")\n";
        }

    private:

        void* operator new(size_t size)
        {
            std::cout << "LocalObject created on Heap. Size = " << size << std::endl;
            decltype(auto) memBlock = operator new[](size);
            // decltype(auto) memBlock1 = malloc(size);
            return memBlock;
        }

        template <typename... Ts>
        friend LocalObject* createObject(Ts&&... params);
    };

    template <typename... Ts>
    struct LocalObject* createObject(Ts&&... params)
    {
        return new LocalObject(std::forward<Ts>(params)...);
    }

    void CreateObjects_PrivateFunc()
    {
        /** Will not compile **/
        // LocalObject* obj = new LocalObject(10, "sdsds");

        LocalObject* obj = createObject(12, "dsdsdsd");

        delete obj;
    }
}

#if 1

void* operator new(size_t count) {
    decltype(auto) ptr = malloc(count);
    std::cout << count << " bytes allocated at address " << ptr << std::endl;
    return ptr;
}

void operator delete(void* ptr) noexcept {
    std::cout << "Deallocating " << ptr << std::endl;
    free(ptr);
}

namespace Memory::SharedPtr_MemoryAllocationTests
{
    using Helpers::Long;

    void TestAllocations()
    {
        std::cout << "----------------------------------------------------\n";
        {

            std::shared_ptr<Long> sharedLong = std::shared_ptr<Long> { new Long {123}};

        }
        std::cout << "----------------------------------------------------\n";
        {

            std::shared_ptr<Long> sharedLong = std::make_shared<Long> (123);

        }
        std::cout << "----------------------------------------------------\n";
    }
}
#endif


void Memory::TestAll()
{

    // CleanUP_Exception_Test();
    // SharedPtrLeak();

    // UniquePtrExperiments::PointerToObjectOnStack();
    // UniquePtrExperiments::CustomDeleterTests();

    // Memset_vs_Assignment();
    // Memset_vs_Assignment_Perf();
    // Double_Delete_Nullptr();

    // SharedPtr_BadUsage_DoubleDelete();
    // SharedPtr_BadUsage_DoubleDelete_FIX_EmptyDeleter();
    // Shared_Weak_UsageCount();

    // AllocateShared_And_Trace();
    // AllocateMakeShared_And_Trace();

    // PlacementNew::CreateObjects();

    // RestrictObjectHeapCreation::CreateObjects_PrivateFunc();


    SharedPtr_MemoryAllocationTests::TestAllocations();
}
