//============================================================================
// Name        : Threads.cpp
// Created on  : 07.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Threads src class
//============================================================================

#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <string>
#include <chrono>
#include <future>
#include <exception>
#include <functional>
#include <semaphore>
#include <utility>
#include <type_traits>
#include <syncstream>

#include "Threads.h"
#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << utilities::datetime::getCurrentTime() << " "

namespace Threads
{
    struct DisplayThread
    {
        void operator()() {
            LOG << "DisplayThread entered. Sleeping 2 seconds." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            LOG << "DisplayThread done" << std::endl;
        }
    };

    struct ThreadHandler
    {
        std::string name;
        int count;

    public:
        ThreadHandler(std::string  n, int c) : name(std::move(n)), count(c) {}

    public:

        void Task()
        {
            LOG << "Handler name: " << this->name << ". Count: " << this->count << std::endl;
            for (int i = 1; i <= this->count; i++) {
                std::this_thread::sleep_for(std::chrono::seconds(1u));
                LOG << i << "  of " << this->count << std::endl;
            }
        }

        void Task1(const std::string& _name, int count)
        {
            LOG << "Handler name: " << _name << ". Count: " << count << std::endl;
            for (int i = 1; i <= count; i++) {
                std::this_thread::sleep_for(std::chrono::seconds(1u));
                LOG << i << " of " << count << std::endl;
            }
        }

    public:
        void RunTask() {
            std::thread thread(&ThreadHandler::Task1, this, this->name, this->count);
            thread.detach();
        }
    };

    //////////////////////////////////////////

    void Thead_as_ClassMethod()
    {
        LOG << "Started" << std::endl;

        ThreadHandler handler("TestThreadHandler", 5);
        std::thread thread(&ThreadHandler::Task, &handler);
        thread.join();

        LOG << "Completed" << std::endl;
    }

    void Thead_as_ClassMethod_Params()
    {
        LOG << "Started" << std::endl;

        ThreadHandler handler("TestThreadHandler", 5);
        std::thread thread(&ThreadHandler::Task1, &handler , "PARAM-1", 10);
        thread.join();

        LOG << "Completed" << std::endl;
    }

    void Thead_as_ClassMethod_1()
    {
        LOG << "Started" << std::endl;

        ThreadHandler handler("TestThreadHandler", 5);
        handler.RunTask();

        LOG << "Completed" << std::endl;
    }

    template <typename Func, typename TupleType, size_t... Indices>
    constexpr std::jthread create_thread_impl(Func&& task,
                                              TupleType&& tup,
                                              std::index_sequence<Indices...>)
    {
        return std::jthread(std::forward<Func>(task),
                            std::get<Indices>(std::forward<TupleType>(tup))...);
    }

    void Create_Thread_Tuple_as_Params()
    {
        auto task = [](int num, const std::string& text) {
            LOG << "Value: " << num << ", text: " << text << std::endl;
        };

        std::tuple<int, std::string> param = std::make_tuple(12, "One1");

        using _Indices = std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(param)>>>;
        std::jthread T = create_thread_impl(task,
                                            std::forward<decltype(param)>(param),
                                            _Indices{});
    }

    void Create_Thread_Tuple_as_Params_2()
    {
        auto task = [](int num, const std::string& text, float fVal) {
            LOG << "Value: " << num << ", text: " << text << ", fVal: " << fVal << std::endl;
        };

        std::tuple param = std::make_tuple(12, "One1", 0.1);

        using _Indices = std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(param)>>>;
        std::jthread T = create_thread_impl(task,
                                            std::forward<decltype(param)>(param),
                                            _Indices{});
    }

    void Create_Simple_Thread()
    {
        const auto func = []()-> void {
            LOG << " Entered. Sleeping 2 seconds." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            LOG << " Done" << std::endl;
        };

        LOG << " Started" << std::endl;
        std::thread thread(func);

        thread.join();
        LOG << " Completed" << std::endl;
    }

    void Create_Simple_Thread_Lambda()
    {
        LOG << " Started" << std::endl;
        std::thread thread([](void)-> void {
            LOG << " Entered. Sleeping 2 seconds." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            LOG << " Done" << std::endl;
        });

        thread.join();
        LOG << " Completed" << std::endl;
    }

    void Create_Simple_Thread_Params()
    {
        const auto func = [](unsigned short timeout)-> void {
            LOG << "Entered. Sleeping " << timeout << " seconds." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            LOG << "Done" << std::endl;
        };

        LOG << "Started" << std::endl;
        std::thread thread(func, 5);

        thread.join();
        LOG << "Completed" << std::endl;
    }


    void Create_Simple_Thread_Params_By_Reference()
    {
        const auto func = [](int& value)-> void {
            LOG << "Starting thread" << std::endl;
            for (int i = 0; i < 10; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100u));
                value++;
            }
            LOG << "Done" << std::endl;
        };

        int value{ 10 };
        LOG << "Starting thread: value = " << value << std::endl;
        std::thread thread(func, std::ref(value));

        thread.join();
        LOG << "Completed: value = " << value << std::endl;
    }

    void Create_Simple_Thread_FunctionObjects() {
        LOG << "Started" << std::endl;
        std::thread thread((DisplayThread()));
        thread.join();
        LOG << "Completed" << std::endl;
    }

    void CreateThread_Function_accept_Function() {

        auto handler = [](auto func,  std::string text)-> void {
            LOG << "Starting thread" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            func(text);
            LOG << "Done" << std::endl;
        };

        auto callback = [](const std::string& text)-> void {
            LOG << "Callback: " << text << std::endl;
        };

        LOG << "Started" << std::endl;
        std::thread thread(handler, callback,  "text");
        thread.join();
        LOG << "Completed" << std::endl;
    }


    void CreateThread_LambdaWithState()
    {
        auto func = [counter = 0] mutable -> void {
            ++counter;
            std::osyncstream { std::cout } << counter << std::endl;
        };

        func();

        std::jthread t1(func);
        std::jthread t2(func);
    }

    struct Worker
    {
        std::atomic<int32_t> counter { 0 };

        void doSomething() {
            const int32_t prev = counter.fetch_add(1, std::memory_order::relaxed);
            std::osyncstream { std::cout } << (1 + prev) << std::endl;
        }
    };

    void CreateThread_ClassMethod_Callback()
    {
        Worker worker;

        std::jthread t1(&Worker::doSomething, &worker);
        std::jthread t2(&Worker::doSomething, &worker);
    }
}

namespace Threads::ThreadLocalStorage
{
    struct Params
    {
        // unique in program
        inline static std::string gName = "global";

        // unique per thread
        inline static thread_local std::string tName = "tls";

        // for each object:
        std::string lName = "local";

    public:
        void print(const std::string& msg) const
        {
            std::cout << msg << std::endl;
            std::cout << "- gName: " << gName << std::endl;
            std::cout << "- tName: " << tName << std::endl;
            std::cout << "- lName: " << lName << std::endl;
        }
    };

    inline thread_local Params params;

    void foo()
    {
        params.print("foo() begin:");
        params.gName = "thread2 name";
        params.tName = "thread2 name";
        params.lName = "thread2 name";
        params.print("foo() end:");
    }

    void Test()
    {
        params.print("main() begin:");
        params.gName = "thread1 name";
        params.tName = "thread1 name";
        params.lName = "thread1 name";
        params.print("main() later:");

        std::thread t(foo);
        t.join();

        params.print("main() end:");
    }

    //-------------------------------------------------------------------//

    void Test2()
    {
        thread_local unsigned int thread_local_counter = 1;
        std::mutex cout_mutex;

        auto increase_rage = [&cout_mutex](const std::string& thread_name)-> void {
            ++thread_local_counter; // modifying outside a lock is okay; this is a thread-local variable
            std::lock_guard<std::mutex> lock(cout_mutex);
            LOG << "Rage counter for " << thread_name << ": " << thread_local_counter << '\n';
        };

        std::thread a(increase_rage, "a"), b(increase_rage, "b");
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            LOG << "Rage counter for main: " << thread_local_counter << '\n';
        }

        a.join();
        b.join();
    }



    struct Worker
    {
        inline static thread_local std::vector<int> numbers {};

        void process(int values)
        {
            for (int i = 0; i < values; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds (100u));
                numbers.push_back(i);
            }
            std::cout << "Address: " << &numbers << ", size: " << numbers.size() << std::endl;
        }

        void run()
        {
            std::jthread thread1(&Worker::process, this, 5);
            std::jthread thread2(&Worker::process, this, 5 * 2);
        }
    };


    void Test3()
    {
        Worker w;
        w.run();
    }
}

namespace Threads::VariousTests {

    void Test() {
        // std::jthread t;

    }
}

namespace Threads::HandlingExceptions {

    void do_some_work()
    {
        LOG << "Starting thread/n";
        std::this_thread::sleep_for(std::chrono::seconds(4u));
        LOG << "Thread throwing a runtime_error exception...\n";
        throw std::runtime_error{ "Exception from thread" };
    }

    void threadFunc(std::exception_ptr& err)
    {
        try {
            do_some_work();
        }
        catch (...) {
            LOG << "Thread caught exception, returning exception...\n";
            err = std::current_exception();
        }
    }

    void doWorkInThread()
    {
        std::exception_ptr error;
        std::thread T{ threadFunc, std::ref(error) };
        T.join();

        if (error) {
            LOG << "Main thread received exception, rethrowing it...\n";
            rethrow_exception(error);
        }
        else {
            LOG << "Main thread did not receive any exception.\n";
        }
    }

    void Run_Test()
    {
        try {
            doWorkInThread();
        }
        catch (const std::exception& e) {
            LOG << "Main function caught: '" << e.what() << "'\n";
        }
    }
}

void Threads::TEST_ALL()
{
    // Thead_as_ClassMethod();
    // Thead_as_ClassMethod_Params();
    // Thead_as_ClassMethod_1();

    // Create_Thread_Tuple_as_Params();
    // Create_Thread_Tuple_as_Params_2();
    // Create_Simple_Thread();
    // Create_Simple_Thread_Lambda();
    // Create_Simple_Thread_Params();
    // Create_Simple_Thread_Params_By_Reference();
    // Create_Simple_Thread_FunctionObjects();
    // CreateThread_LambdaWithState();
    CreateThread_ClassMethod_Callback();

    // CreateThread_Function_accept_Function();

    // ThreadLocalStorage::Test();
    // ThreadLocalStorage::Test2();
    // ThreadLocalStorage::Test3();

    // VariousTests::Test();

    // HandlingExceptions::Run_Test();
}
