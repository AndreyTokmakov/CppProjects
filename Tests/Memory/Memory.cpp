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
#include <functional>

#include "../Helpers/Helpers.h"
#include "CustomStackAllocator.h"
#include "AlignedStackAllocator.h"
#include "UsingCustomAllocator_List.h"
#include "ObjectMemoryPool.h"

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



namespace Memory::UniquePtrExperiments {
    using namespace Helpers;

    void PointerToObjectOnStack() {
        auto longDeleter = [](Integer *ptr) {
            std::cout << "Skip deletion for Long(" << ptr << ")\n";
            //delete ptr;
        };

        Integer l1(10);

        {
            std::unique_ptr<Integer, decltype(longDeleter)> lPtr{&l1, longDeleter};
        }

        std::cout << "Done\n";

    }

    template<typename T>
    struct CustomDeleter {
        CustomDeleter() = default;

        CustomDeleter(const CustomDeleter &) = default;

        CustomDeleter(CustomDeleter &) = default;

        CustomDeleter(CustomDeleter &&) noexcept = default;

        void operator()(T *p) const { delete p; };
    };

    void CustomDeleterTests() {

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

    void handle_RValue_One(std::unique_ptr<Integer>&& integer)
    {
    }

    void handle_RValue_Two(std::unique_ptr<Integer>&& integer)
    {
        std::unique_ptr<Integer> dest = std::move(integer);
    }

    void handle_LValue(std::unique_ptr<Integer> integer)
    {
    }

    void Pass_Unique_Ptr_Object()
    {
        {
            std::unique_ptr<Integer> var{std::make_unique<Integer>(111)};
            std::cout << std::string(160, '=') << std::endl;
            handle_RValue_One(std::move(var));
            std::cout << std::string(160, '=') << std::endl;
        }

        std::cout << std::endl;

        {
            std::unique_ptr<Integer> var{std::make_unique<Integer>(222)};
            std::cout << std::string(160, '=') << std::endl;
            handle_RValue_Two(std::move(var));
            std::cout << std::string(160, '=') << std::endl;
        }

        std::cout << std::endl;
        {
            std::unique_ptr<Integer> var{std::make_unique<Integer>(333)};
            std::cout << std::string(160, '=') << std::endl;
            handle_LValue(std::move(var));
            std::cout << std::string(160, '=') << std::endl;
        }
    }
}

namespace Memory::PlacementNew
{

    void CreateObjects()
    {
        using T = Helpers::Integer;
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

#if 0

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

namespace Memory::MakeUnique_ForOverwrite
{
    // Output Fibonacci numbers to an output iterator.
    template<typename OutputIt>
    OutputIt fibonacci(OutputIt first, OutputIt last)
    {
        for (int a = 0, b = 1; first != last; ++first)
        {
            *first = b;
            b += std::exchange(a, b);
        }
        return first;
    }


    void AllocateArray_AndInitialize()
    {
        constexpr size_t len { 10 };
        const std::unique_ptr<int[]> values = std::make_unique_for_overwrite<int[]>(len);

        fibonacci(values.get(), values.get() + len);

        std::cout << "make_unique_for_overwrite<int[]>(10), fibonacci(...): [" << values[0];
        for (std::size_t i = 1; i < 10; ++i)
            std::cout << ", " << values[i];
        std::cout << "]\n";
    }
};

namespace Memory::UniquePtr_BAD
{
    template<typename T>
    struct VectorPtr
    {
        using pointer_type = std::unique_ptr<T[]>;
        using size_type = std::vector<T>::size_type;

        pointer_type data_ = nullptr;
        size_type length_ = 0;

        explicit VectorPtr(std::vector<T>& vector):
            data_(std::make_unique<T[]>(vector.size())), length_(vector.size())
        {
            std::copy(vector.begin(), vector.end(), data_.get());
        }

        T& operator[](size_type index)
        {
            if (index >= length_)
                throw std::out_of_range("Out of range");
            return data_[index];
        }

        [[nodiscard]]
        constexpr size_type size() const
        {
            return length_;
        }
    };

    void Test()
    {
        /*
        using Helpers::Integer;

        using PtrType = int*;

        std::vector<Helpers::Integer> myVec { Helpers::Integer(1), Helpers::Integer(2), Helpers::Integer(3) };
        // std::vector<PtrType> myVec { new int(1), new int(2), new int(3)};

        VectorPtr<Helpers::Integer> ptr(myVec);
        */

        std::vector<int> myVec { 1, 2 ,3, 4, 5 };
        VectorPtr<int> ptr(myVec);

        std::cout << "myVec size: " << myVec.size() << ", ptr size: " << ptr.size()  << std::endl;
        myVec.push_back(6);
        std::cout << "myVec size: " << myVec.size() << ", ptr size: " << ptr.size()  << std::endl;
    }
}


#if 0
namespace Memory::CustomAllocatorTest
{
    using namespace Helpers;

    template<class T, size_t N>
    class Allocator
    {
        using object_type = T;
        using pointer = object_type*;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        // TODO: to aligned storage?
        std::byte rawMemory[N * sizeof(Integer)] {};
        Integer *pool = reinterpret_cast<Integer*>(rawMemory);

        // FIXME
        // static_assert(pool % alignof(T) == 0);


        int tail {0};
        std::array<size_t, N> available {};

    public:

        Allocator()
        {
            tail = N - 1;
            std::cout << reinterpret_cast<long>(&pool)  << std::endl;
            for (size_t idx = 0; idx < N; ++idx) {
                available[idx] = idx;
                std::cout << "\t" << idx << " --> " << reinterpret_cast<long>(&pool[idx])  << std::endl;
            }
        }

        template<typename ... Args>
        pointer AllocateAndConstruct(Args ... params)
        {
            if (tail < 0) {
                std::cerr << "Out ot space" << std::endl;
                return nullptr;
            }

            const size_t offset = available[tail--];
            try {
                pointer ptr =  new (&pool[offset]) object_type { std::forward<Args>(params)... };
                // std::cout << ptr << std::endl;
                return ptr;
            } catch (...)
            {
                std::cerr << "Exc" << std::endl;
                ++tail;
                throw;
            }
        }

        void DestroyAndDeallocate(pointer ptr) // + validate
        {
            if (nullptr == ptr)
                throw std::runtime_error("nullptr ptr");
            /*if (0 == (ptr - pool)  % sizeof(object_type) )
                throw std::runtime_error("wrong alignment ptr");*/
            if (&pool[0] > ptr)
                throw std::runtime_error("Not in the allocated block 1");
            if (ptr > &pool[N - 1])
                throw std::runtime_error("Not in the allocated block 2");

            /*
            if (!(nullptr != ptr && 0 == (ptr - pool)  % sizeof(object_type) && ptr >= pool && (pool + N - 1) >= ptr)) {
                throw std::runtime_error("alien ptr");
            }*/

            const size_t offset = ptr - pool;
            // ptr->~object_type();
            std::destroy_at(ptr);
            available[++tail] = offset;
        }
    };

    void Tests()
    {
        Allocator<Integer, 5> allocator;

        std::array<Integer*, 5> objs {};
        for (int testId  = 0; testId < 10; ++testId)
        {
            for (int i = 0; i < 5; ++i)
                objs[i] = allocator.AllocateAndConstruct((i + 1) * 10);
            for (const auto ptr: objs)
                allocator.DestroyAndDeallocate(ptr);
        }


        /*
        std::array<int, 3> storage {1,2,3};
        // std::vector<int> storage {1,2,3};
        std::cout << reinterpret_cast<long>(&storage)  << std::endl;
        for (size_t idx = 0; idx < storage.size(); ++idx)
            std::cout << storage[idx] << " -> " <<reinterpret_cast<long>(&storage[idx])  << std::endl;
`       */
    }
}
#endif


namespace VectorOfUniquePointers_CustomDeleter
{
    struct Client
    {
        int value {0};

        explicit Client(int v) : value {v} { std::cout << "Client(" << value << ") created\n"; }

        Client(const Client&)  { std::cout << "Client(" << value << ") copy constructor\n"; }
        Client& operator=(const Client&)  { std::cout << "Client(" << value << ") copy assignment\n"; return *this; }

        Client(Client&&) noexcept  { std::cout << "Client(" << value << ") move constructor\n"; }
        Client& operator==(Client&&) noexcept { std::cout << "Client(" << value << ") move assignment\n"; return *this; }

        ~Client()    { std::cout << "Client(" << value << ") destroyed\n"; }
        void close() { std::cout << "Client(" << value << ") closed\n"; }
        void info()  { std::cout << "Client(" << value << ") info\n"; }
    };

    template<typename T>
    struct Closer
    {
        void operator()(T *ptr) const {
            ptr->close();
            delete ptr;
        }
    };

    using ClientDeleter = Closer<Client>;

    void handle(Client& client)
    {
        client.info();
    }

    void Test()
    {
        std::vector<std::unique_ptr<Client, ClientDeleter>> clients;

        for (int i = 0; i < 5; ++i)
            clients.push_back(std::unique_ptr<Client, ClientDeleter>(new Client{i}, ClientDeleter{}));
    }

    void Test_PasRef()
    {
        {
            auto client = std::unique_ptr<Client, ClientDeleter>(new Client{123}, ClientDeleter{});
            handle(*client);

        }
        std::cout << std::string(100, '=') << std::endl;
        {
            Client client{2};
            handle(client);
            client.close();
        }
    }

}

namespace VectorOfUniquePointers_KeepReference_StoredInVector
{
    struct Client
    {
        int value {0};

        explicit Client(int v) : value {v} { std::cout << "Client(" << value << ") created\n"; }
        ~Client() { std::cout << "Client(" << value << ") destroyed\n"; }
        void close() { std::cout << "Client(" << value << ") closed\n"; }

        void info() {
            std::cout << "info Client(" << value << ")\n";
        }
    };

    template<typename T>
    struct Closer
    {
        void operator()(T *ptr) const {
            ptr->close();
            delete ptr;
        }
    };

    using ClientDeleter = Closer<Client>;

    void Test()
    {
        std::vector<std::unique_ptr<Client, ClientDeleter>> clients;

        std::function<void(void)> callback;

        {
            auto client = std::unique_ptr<Client, ClientDeleter>(new Client{123}, ClientDeleter{});
            callback = [client_ref = client.get()] {
                client_ref->info();
            };

            clients.push_back(std::move(client));
        }

        std::cout << std::string(100, '=') << std::endl;

        callback();
        callback();

        clients.back()->value = 100500;

        callback();

        std::cout << std::string(100, '=') << std::endl;
    }
}


namespace Memory::Unique_Ptr_Deleter_Experiments
{
    using Integer = Helpers::Integer;

    struct Pool
    {

    };

    struct Deleter
    {
        static inline std::array<char, 32> buffer {0};

        void operator()(const Integer* ptr) const
        {
            std::cout << "Deleter(" << ptr->getValue() << ") deleted\n";
            delete ptr;
        }
    };


    void Deleter_With_Members()
    {
        // std::unique_ptr<Integer> intPtr { std::make_unique<Integer>(101) };
        std::unique_ptr<Integer, Deleter> intPtr { std::unique_ptr<Integer, Deleter> { new Integer(101), Deleter{} } };

        static_assert(8 == sizeof(intPtr));
    }
}


void Memory::TestAll()
{
    // ObjectMemoryPool::TestAll();

    // CleanUP_Exception_Test();
    // SharedPtrLeak();

    // UniquePtrExperiments::PointerToObjectOnStack();
    // UniquePtrExperiments::CustomDeleterTests();
    // UniquePtrExperiments::Pass_Unique_Ptr_Object();

    // Memset_vs_Assignment();
    // Memset_vs_Assignment_Perf();
    // Double_Delete_Nullptr();

    // SharedPtr_BadUsage_DoubleDelete();
    // SharedPtr_BadUsage_DoubleDelete_FIX_EmptyDeleter();
    // Shared_Weak_UsageCount();

    // AllocateShared_And_Trace();
    // AllocateMakeShared_And_Trace();


    Unique_Ptr_Deleter_Experiments::Deleter_With_Members();

    // PlacementNew::CreateObjects();

    // RestrictObjectHeapCreation::CreateObjects_PrivateFunc();

    // SharedPtr_MemoryAllocationTests::TestAllocations();

    // MakeUnique_ForOverwrite::AllocateArray_AndInitialize();

    // UniquePtr_BAD::Test();


    // AlignedStackAllocator::TestAll();
    // CustomStackAllocator::TestAll();
    // UsingCustomAllocator_List::TestAll();

    // VectorOfUniquePointers_CustomDeleter::Test();
    // VectorOfUniquePointers_CustomDeleter::Test_PasRef();
    // VectorOfUniquePointers_KeepReference_StoredInVector::Test();

}
