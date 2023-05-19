//============================================================================
// Name        : Atomic.cpp
// Created on  : 30.09.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Atomic src class
//============================================================================

#include "Atomic.h"
#include <iostream>
#include <atomic>         // std::atomic, std::atomic_flag, ATOMIC_FLAG_INIT
#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <chrono>
#include <future>
#include <cassert>
#include <syncstream>

#include "../ThreadHelperUtilities/ThreadHelperUtilities.h"

namespace Atomic::Atomic_INT
{

    void Test1() {
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
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            THREAD_INFO << "The answer: " << data[0] << std::endl;
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

    void SetValue() {
        std::atomic<bool> b{false};

        bool x = b.load(std::memory_order_acquire);
        std::cout << "b = " <<  std::boolalpha << b  << ", x = " << x << std::endl;

        b.store(true);
        std::cout << "b = " << std::boolalpha << b << " , x = " << x << std::endl;

        x = b.exchange(false, std::memory_order_acq_rel);
        std::cout << "b = " << std::boolalpha << b << ", x = " << x << std::endl;
    }


    void Is_Lock_Free() {
        //
    }


    void Consumer_Producer() {

        std::atomic<bool> run { true };
        auto task = std::async([&run]() {
            while (run) {
                THREAD_INFO << "Running\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
        });

        std::this_thread::sleep_for(std::chrono::seconds(5));
        run.store(false);
        THREAD_INFO << "Done\n";
    }
}

namespace Atomic::AtomicFlag {

    class spinlock_mutex {
    private:
        std::atomic_flag flag { ATOMIC_FLAG_INIT };

    public:
        ~spinlock_mutex() {
            flag.clear(std::memory_order_release);
        }

        void lock() {
            while (flag.test_and_set(std::memory_order_acquire)) {
                // THREAD_INFO << "Locked"  << std::endl;
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

        THREAD_INFO << "Counter = " << counter << std::endl;
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

        THREAD_INFO << "Counter = " << counter << std::endl;
    }

    //-----------------------------------------------------------------------------//

    void Test_and_Set() {
        std::atomic_flag flag = ATOMIC_FLAG_INIT;

        auto handler = [&](size_t n)-> void {
            for (size_t i = 0; i < 100; ++i) {
                while (flag.test_and_set(std::memory_order_acquire));  // acquire lock
                THREAD_INFO << "Output from thread " << n << "\n";
                flag.clear(std::memory_order_release);                 // release lock
            }
        };

        THREAD_INFO << "Test started.\n";
        std::vector<std::future<void>> tasks;
        for (size_t i = 0; i < 10; ++i)
            tasks.emplace_back(std::async(handler, i));
        for (const auto& fut : tasks)
            fut.wait();
        THREAD_INFO << "Test completed.\n";
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

namespace Atomic::Compare {

    void CompareExchangeWeak()
    {
        std::atomic<int> value{ 20 };
        std::cout << value << std::endl;

        int val1 = 30;
        int val2 = 40;

        auto exchanged = value.compare_exchange_weak(val1, val2);
        std::cout << "value = " << value << ". exchange succeeded = " << std::boolalpha << exchanged << std::endl;

        val1 = 20;

        exchanged = value.compare_exchange_weak(val1, val2);
        std::cout << "value = " << value << ". exchanged succeeded = " << std::boolalpha << exchanged << std::endl;
    }

    void CompareExchangeStrong()
    {
        std::atomic<int> value{ 20 };
        std::cout << value << std::endl;

        int val1 = 30;
        int val2 = 40;

        auto exchanged = value.compare_exchange_strong(val1, val2);
        std::cout << "value = " << value << ". exchange succeeded = " << std::boolalpha << exchanged << std::endl;

        val1 = 20;

        exchanged = value.compare_exchange_strong(val1, val2);
        std::cout << "value = " << value << ". exchanged succeeded = " << std::boolalpha << exchanged << std::endl;
    }

}


namespace Atomic::Cpp_20_Features {

    void Wait()
    {
        constexpr int INITIAL_VALUE{ 0 };
        std::atomic<int> variable { INITIAL_VALUE };

        auto waiter = std::async(std::launch::async, [&variable]() -> void {
            THREAD_INFO << "Waiting until variable changes its value: value = " << variable << std::endl;
            variable.wait(INITIAL_VALUE);
            THREAD_INFO << "Waiting Done!!!: value = " << variable << std::endl;
        });

        auto task = std::async(std::launch::async, [&variable]() -> void {
            THREAD_INFO << "Starting task" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(2));
            variable.store(INITIAL_VALUE);
            THREAD_INFO << "Task 1 completed: value = " << variable << std::endl;

            variable.notify_all();

            std::this_thread::sleep_for(std::chrono::seconds(2));
            variable.store(INITIAL_VALUE  + 1);
            THREAD_INFO << "Task 2 completed: value = " << variable << std::endl;

            variable.notify_all();
        });
    }

    void Notify_One()
    {
        constexpr int INTIAL_VALUE{ 0 };
        std::atomic<int> value{ INTIAL_VALUE };

        auto task = std::async([&value]() -> void {
            THREAD_INFO << "Starting task" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(5));

            value.store(INTIAL_VALUE + 1);
            value.notify_one();

            THREAD_INFO << "Task  completed: value = " << value << std::endl;
        });

        std::vector<std::future<void>> tasks;
        for (int i = 0; i < 2; ++i) {
            const std::string name { "Thread_" + std::to_string(i) };
            tasks.emplace_back(std::async([&value, name]() -> void {
                THREAD_INFO << "Waiting until value != 10 . . . " << std::endl;
                value.wait(INTIAL_VALUE);
                THREAD_INFO << name << " done" << std::endl;
            }));
        }
    }


    void Notify_All()
    {
        constexpr int INTIAL_VALUE{ 0 };
        std::atomic<int> value{ INTIAL_VALUE };

        auto task = std::async([&value]() -> void {
            THREAD_INFO << "Starting task" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(5));

            value.store(INTIAL_VALUE + 1);
            value.notify_all();

            THREAD_INFO << "Task  completed: value = " << value << std::endl;
        });

        std::vector<std::future<void>> tasks;
        for (int i = 0; i < 2; ++i) {
            const std::string name{ "Thread_" + std::to_string(i) };
            tasks.emplace_back(std::async([&value, name]() -> void {
                THREAD_INFO << "Waiting until value != 10 . . . " << std::endl;
                value.wait(INTIAL_VALUE);
                THREAD_INFO << name << " done" << std::endl;
            }));
        }
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

void Atomic::TEST_ALL()
{
    AtomicFlag::Spinlock_Test();
    // AtomicFlag::Spinlock_Test_Guard();
    // AtomicFlag::Test_and_Set();

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

    // Compare::CompareExchangeWeak();
    // Compare::CompareExchangeStrong();

    // Cpp_20_Features::Wait();
    // Cpp_20_Features::Notify_One();
    // Cpp_20_Features::Notify_All();

    // ModificationOrder::Test_Default();
    ModificationOrder::Test_Relaxed();

    // PingPongGame::Test();
}
