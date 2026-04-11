/**============================================================================
Name        : Atomic.cpp
Created on  : 30.09.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <queue>
#include <chrono>
#include <future>
#include <syncstream>

#include "Atomic.h"
#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << " "

namespace Atomic::Atomic_INT
{

    void Test1()
    {
        int v1 = 0;
        std::atomic<int> v2{ 0 };

        auto add_v1 = [&]()-> int{
            return ++v1;
            /* Generated x86-64 assembly:
                mov     eax, DWORD PTR v1[rip]
                add     eax, 1
                mov     DWORD PTR v1[rip], eax
            */
        };

        [[maybe_unused]]
        auto add_v2 = [&]()-> int {
            return v2.fetch_add(1);
            /* Generated x86-64 assembly:
                mov     eax, 1
                lock xadd       DWORD PTR _ZL2v2[rip], eax
            */
        };

        auto fut = std::async(add_v1);
    }

    void Fetch_Add_Test()
    {
        std::atomic<int> counter { 0 };
        while (counter.load() < 10)
        {
            auto old = counter.fetch_add(1);
            std::cout << old << " -> " << counter.load() << std::endl;
        }
    }

    void Test_2_MemoryOrderRelaxed() {
        std::atomic<size_t> counter{ 0 };

        auto process = [&]()-> void {
            counter.fetch_add(1, std::memory_order_relaxed);
        };

        auto print_metrics = [&]()-> void {
            std::cout << "Number of requests = " << counter.load(std::memory_order_relaxed) << "\n";
        };

        auto task1 = std::async(process);
        auto task2 = std::async(print_metrics);
        task1.wait();
        task2.wait();
    }

    //-------------------------------------------------------------------------------------//

    template <typename T>
    T fetch_mult(std::atomic<T>& shared, T mult) {
        T oldValue = shared.load();
        while (false == shared.compare_exchange_strong(oldValue, oldValue * mult));
        return oldValue;
    }

    void Test3() {
        std::atomic<int> myInt{ 5 };
        std::cout << myInt << std::endl;
        fetch_mult(myInt, 5);
        std::cout << myInt << std::endl;
    }

    void Test4() {
        constexpr int COUNT = 10'000'000;
        std::atomic_uint32_t atomInit{ 0 };
        // int atomInit{ 0 };

        std::cout << atomInit << std::endl;

        std::vector<std::future<void>> tasks;
        for (int i = 0; i < 20; ++i) {
            tasks.emplace_back(std::async([&] {
                for (int i = 0; i < COUNT; i++)atomInit++;
            }));
        }
        std::for_each(tasks.cbegin(), tasks.cend(), [](auto& F) {F.wait(); });
        std::cout << atomInit << std::endl;
    }


    void Compare_Test() {

        std::atomic_uint32_t atomInit{ 0 };
        //


    }
};

namespace Atomic::Atomic_CumtomClasses {

    // a simple global linked list :
    struct Node {
        int value;
        Node* next;
    };

    std::atomic<Node*> list_head(nullptr);

    // append an element to the list
    void append(int val) {
        Node* oldHead = list_head;
        Node* newNode = new Node{ val,oldHead };

        // what follows is equivalent to: list_head = newNode, but in a thread-safe way:
        while (false == list_head.compare_exchange_weak(oldHead, newNode))
            newNode->next = oldHead;
    }

    void Test() {
        std::vector<std::future<void>> tasks;
        for (auto i = 0; i < 10; ++i)
            tasks.emplace_back(std::async(append, i));
        for (const auto& fut : tasks)
            fut.wait();

        // print contents:
        for (Node* it = list_head; it != nullptr; it = it->next)
            std::cout << ' ' << it->value;
        std::cout << '\n';

        // cleanup:
        Node* it;
        while ((it = list_head)) {
            list_head = it->next;
            delete it;
        }
    }
}

namespace Atomic::Atomic_Boolean {

    void Bool_Load_Test() {
        std::vector<int> data;
        std::atomic<bool> data_ready(false);

        auto reader_thread = [&]()->void {
            while (false == data_ready.load()) {
                std::this_thread::sleep_for(std::chrono::seconds(1u));
            }
            LOG << "The answer: " << data[0] << std::endl;
        };

        auto writer_thread = [&]()->void {

            data.push_back(42);
            data_ready = true;
        };

        auto task1 = std::async(reader_thread);
        auto task2 = std::async(writer_thread);
        task1.wait();
        task2.wait();
    }

    void SetValue()
    {
        std::atomic<bool> b{false};

        bool x = b.load(std::memory_order_acquire);
        std::cout << "b = " <<  std::boolalpha << b  << ", x = " << x << std::endl;

        b.store(true);
        std::cout << "b = " << std::boolalpha << b << " , x = " << x << std::endl;

        x = b.exchange(false, std::memory_order_acq_rel);
        std::cout << "b = " << std::boolalpha << b << ", x = " << x << std::endl;
    }

    void Consumer_Producer() {

        std::atomic<bool> run { true };
        auto task = std::async([&run]() {
            while (run) {
                LOG << "Running\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(250U));
            }
        });

        std::this_thread::sleep_for(std::chrono::seconds(5U));
        run.store(false);
        LOG << "Done\n";
    }
}

namespace Atomic::AtomicFlag
{

    class spinlock_mutex {
    private:
        std::atomic_flag flag { ATOMIC_FLAG_INIT };

    public:
        ~spinlock_mutex() {
            flag.clear(std::memory_order_release);
        }

        void lock() {
            while (flag.test_and_set(std::memory_order_acquire)) {
                // LOG << "Locked"  << std::endl;
            }
        }
        void unlock() {
            flag.clear(std::memory_order_release);
        }
    };

    void Spinlock_Test() {
        spinlock_mutex mtx;
        unsigned long counter = 0;

        auto worker = [&]()->void {
            mtx.lock();
            for (int i = 0; i < 1'000'000; i++) {
                counter++;
            }
            mtx.unlock();
        };

        std::vector<std::future<void>> tasks;
        for (auto i = 0; i < 32; ++i)
            tasks.emplace_back(std::async(std::launch::async, worker));
        for (const auto& fut : tasks)
            fut.wait();

        LOG << "Counter = " << counter << std::endl;
    }

    void Spinlock_Test_Guard() {
        spinlock_mutex mtx;
        unsigned long counter = 0;

        auto worker = [&]()->void {
            //std::lock_guard< spinlock_mutex>(mtx);
            //mtx.lock();
            for (int i = 0; i < 10000; i++) {
                counter++;
            }
            //mtx.unlock();
        };

        std::vector<std::future<void>> tasks;
        for (auto i = 0; i < 10; ++i)
            tasks.emplace_back(std::async(worker));
        for (const auto& fut : tasks)
            fut.wait();

        LOG << "Counter = " << counter << std::endl;
    }

    //-----------------------------------------------------------------------------//

    void Test_and_Set()
    {
        std::atomic_flag flag = ATOMIC_FLAG_INIT;

        auto handler = [&](size_t n)-> void {
            for (size_t i = 0; i < 100; ++i) {
                while (flag.test_and_set(std::memory_order_acquire));  // acquire lock
                LOG << "Output from thread " << n << "\n";
                flag.clear(std::memory_order_release);                 // release lock
            }
        };

        LOG << "Test started.\n";
        std::vector<std::future<void>> tasks;
        for (size_t i = 0; i < 10; ++i)
            tasks.emplace_back(std::async(handler, i));
        for (const auto& fut : tasks)
            fut.wait();
        LOG << "Test completed.\n";
    }

    void Wait_Notify()
    {
        std::vector<int> data {};
        std::atomic_flag atomicFlag {};

        auto prepare = [&]() {
            data.insert(data.end(), {0, 1, 0, 3});
            std::osyncstream {std::cout} << "Prepared" << std::endl;
            atomicFlag.test_and_set();
            atomicFlag.notify_one();
        };

        auto complete = [&] {
            std::osyncstream {std::cout} << "Worker: Waiting for data." << std::endl;
            atomicFlag.wait(false);
            data[2] = 2;
            std::osyncstream {std::cout} <<  "Waiter: Complete the work." << std::endl;
            for (auto i: data)
                std::cout << i << " ";
            std::cout << std::endl;
        };

        std::jthread t2(complete);
        std::jthread t1(prepare);
    }
}

namespace Atomic::Atomic_Objects {

    class Object {
    private:
        int value {0};

    public:
        inline int getValue() const noexcept {
            return this->value;
        }

        inline void setValue(int v) noexcept {
            this->value = v;
        }

        inline void increment() noexcept {
            ++this->value;
        }
    };

    void Test_BAD() {
        Object* obj = new Object();

        auto worker = [&obj]()->void {
            for (int i = 0; i < 10000; i++)
                obj->increment();
        };

        std::vector<std::future<void>> tasks;
        for (auto i = 0; i < 10; ++i)
            tasks.emplace_back(std::async(worker));
        for (const auto& fut : tasks)
            fut.wait();

        std::cout << obj->getValue() << std::endl;
        delete obj;
    }

    void Test_Good() {

        /*
        std::atomic<Object> atomic_obj;
        auto worker = [&atomic_obj]()->void {
            for (int i = 0; i < 10000; i++)
                atomic_obj.e
        };

        std::vector<std::future<void>> tasks;
        for (auto i = 0; i < 10; ++i)
            tasks.emplace_back(std::async(worker));
        for (const auto& fut : tasks)
            fut.wait();

        //std::cout << atomic_obj->getValue() << std::endl;
        delete obj;
        */

    }
}

namespace Atomic::Atomic_SharedPtr {

    class Object {
    private:
        int value{ 0 };

    public:
        inline int getValue() const noexcept {
            return this->value;
        }

        inline void setValue(int v) noexcept {
            this->value = v;
        }

        inline void increment() noexcept {
            ++this->value;
        }
    };

    void Test_BAD() {
        std::shared_ptr<Object> ptr {std::make_shared<Object>()};

        auto worker = [&ptr]()->void {
            for (int i = 0; i < 10000; i++)
                ptr->increment();
        };

        std::vector<std::future<void>> tasks;
        for (auto i = 0; i < 10; ++i)
            tasks.emplace_back(std::async(worker));
        for (const auto& fut : tasks)
            fut.wait();

        std::cout << ptr->getValue() << std::endl;
    }

    void Test_GOOD() {
        //std::atomic<std::shared_ptr<Object>> atomic_ptr{ std::make_shared<Object>() };

        // IMPLEMENT THIS
    }
}


namespace Atomic::AtomicRef {

    class SomeHeavyObjectToBeCopied {
    public:
        int counter{ 0 };
    };

    constexpr int countLimit = 10'000'000;
    constexpr int threadCount = 52;


    void NoAtomicIncrement() {
        SomeHeavyObjectToBeCopied resource;
        std::vector<std::thread> jobs;

        for (int i = 0; i < threadCount; ++i)
            jobs.emplace_back([&resource]() {
                for (int i = 0; i < countLimit; ++i) {
                    resource.counter++;
                }
            });

        for (std::thread& T : jobs)
            T.join();

        // assert(counter == countlimit * 10);
        std::cout << resource.counter << std::endl;
    }

    void AtomicIncrement() {
        SomeHeavyObjectToBeCopied resource;
        std::atomic_ref<int> atomicResFef{ resource.counter };
        std::vector<std::thread> jobs;

        for (int i = 0; i < threadCount; ++i)
            jobs.emplace_back([&atomicResFef]() {
                for (int i = 0; i < countLimit; ++i) {
                    atomicResFef++;
                }
            });

        for (std::thread& T : jobs)
            T.join();

        // assert(counter == countlimit * 10);
        std::cout << resource.counter << std::endl;
    }
}

namespace Atomic::PingPongGame
{
    struct Engine
    {
        static constexpr int MaxCountTimes {1'000'000};
        std::atomic_flag pass;
        std::atomic_int counter;

        void ping()
        {
            int counterLocal = 0;
            while (counterLocal <= MaxCountTimes) {
                pass.wait(false);
                pass.clear();
                counterLocal++;
                counter++;
                pass.notify_one();
                // std::osyncstream {std::cout}  << "ping: " << counterLocal << std::endl;
            }
            // std::osyncstream {std::cout} << "ping: done!!!" << std::endl;
        }

        void pong()
        {
            int counterLocal = 0;
            while (counterLocal < MaxCountTimes) {
                pass.wait(true);
                pass.test_and_set();
                counterLocal++;
                pass.notify_one();
                // std::osyncstream {std::cout}  << "pong: " << counterLocal << std::endl;
            }
            // std::osyncstream {std::cout} << "pong: done!!!" << std::endl;
        }

        void start()
        {
            pass.test_and_set();
            pass.notify_one();
        }
    };


    void Test()
    {
        Engine engine;
        engine.start();

        std::thread ping = std::thread(&Engine::ping, &engine);
        std::thread pong = std::thread(&Engine::pong, &engine);

        if (ping.joinable()) {
            ping.join();
        }
        if (pong.joinable()) {
            pong.join();
        }

        std::cout << "Done\n";
    }
}


namespace Atomic::ModificationOrder
{
    void Test_Default()
    {
        size_t counter {0};
        std::atomic<int> var {0};

        for (int n = 0; n < 100; ++n)
        {
            std::future<void> updater = std::async(std::launch::async, [&]{
                for (int i = 0; i < 100; ++i) {
                    var.fetch_add(1);  // A
                    var.fetch_sub(1);  // B
                    ++counter;
                }
            });

            std::future<void> reader = std::async(std::launch::async, [&]{
                for (int i = 0; i < 100; ++i) {
                    int varLoc = var.load();
                    if (0 != varLoc) {
                        // Reader threads will never see a modification order with (B) before (A)
                        std::cout << "Yoooo. varLoc = " << varLoc << std::endl;
                    }
                }
            });

            updater.wait();
            reader.wait();
        }

        std::cout << "Var = " << var.load() << ". counter = " << counter << std::endl;
    }

    void Test_Relaxed()
    {
        size_t counter {0};
        std::atomic<int> var {0};

        for (int n = 0; n < 100; ++n)
        {
            std::future<void> updater = std::async(std::launch::async, [&]{
                for (int i = 0; i < 100; ++i) {
                    var.fetch_add(1, std::memory_order_relaxed);  // A
                    var.fetch_sub(1, std::memory_order_relaxed);  // B
                    ++counter;
                }
            });

            std::future<void> reader = std::async(std::launch::async, [&]{
                for (int i = 0; i < 100; ++i) {
                    int varLoc = var.load();
                    if (0 != varLoc) {
                        // Reader threads will never see a modification order with (B) before (A)
                        std::cout << "Yoooo. varLoc = " << varLoc << std::endl;
                    }
                }
            });

            updater.wait();
            reader.wait();
        }

        std::cout << "Var = " << var.load() << ". counter = " << counter << std::endl;
    }
};

void Atomic::TestAll()
{
    // AtomicFlag::Spinlock_Test();
    // AtomicFlag::Spinlock_Test_Guard();
    // AtomicFlag::Test_and_Set();
    // AtomicFlag::Wait_Notify();

    // Atomic_Boolean::Bool_Load_Test();
    // Atomic_Boolean::SetValue();
    // Atomic_Boolean::Consumer_Producer();

    // Atomic_INT::Test1();
    // Atomic_INT::Fetch_Add_Test();
    // Atomic_INT::Test_2_MemoryOrderRelaxed();
    // Atomic_INT::Test3();
    // Atomic_INT::Test4();
    // Atomic_INT::Compare_Test();

    // Atomic_CumtomClasses::Test();

    // Atomic_Objects::Test_BAD();
    // Atomic_Objects::Test_Good();

    // Atomic_SharedPtr::Test_BAD();

    // AtomicRef::NoAtomicIncrement();
    // AtomicRef::AtomicIncrement();


    // ModificationOrder::Test_Default();
    // ModificationOrder::Test_Relaxed();

    // PingPongGame::Test();
}
