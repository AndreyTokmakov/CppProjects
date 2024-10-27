//============================================================================
// Name        : Threads.cpp
// Created on  : 07.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Threads src class
//============================================================================

#include "../ThreadHelperUtilities/ThreadHelperUtilities.h"

#include <condition_variable>
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

#include "Threads.h"

namespace Threads
{
    struct DisplayThread
    {
        void operator()() {
            THREAD_INFO << "DisplayThread entered. Sleeping 2 seconds." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            THREAD_INFO << "DisplayThread done" << std::endl;
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
            THREAD_INFO << "Handler name: " << this->name << ". Count: " << this->count << std::endl;
            for (int i = 1; i <= this->count; i++) {
                std::this_thread::sleep_for(std::chrono::seconds(1u));
                THREAD_INFO << i << "  of " << this->count << std::endl;
            }
        }

        void Task1(const std::string& _name, int count)
        {
            THREAD_INFO << "Handler name: " << _name << ". Count: " << count << std::endl;
            for (int i = 1; i <= count; i++) {
                std::this_thread::sleep_for(std::chrono::seconds(1u));
                THREAD_INFO << i << " of " << count << std::endl;
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
        THREAD_INFO << "Started" << std::endl;

        ThreadHandler handler("TestThreadHandler", 5);
        std::thread thread(&ThreadHandler::Task, &handler);
        thread.join();

        THREAD_INFO << "Completed" << std::endl;
    }

    void Thead_as_ClassMethod_Params()
    {
        THREAD_INFO << "Started" << std::endl;

        ThreadHandler handler("TestThreadHandler", 5);
        std::thread thread(&ThreadHandler::Task1, &handler , "PARAM-1", 10);
        thread.join();

        THREAD_INFO << "Completed" << std::endl;
    }

    void Thead_as_ClassMethod_1()
    {
        THREAD_INFO << "Started" << std::endl;

        ThreadHandler handler("TestThreadHandler", 5);
        handler.RunTask();

        THREAD_INFO << "Completed" << std::endl;
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
            THREAD_INFO << "Value: " << num << ", text: " << text << std::endl;
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
            THREAD_INFO << "Value: " << num << ", text: " << text << ", fVal: " << fVal << std::endl;
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
            THREAD_INFO << " Entered. Sleeping 2 seconds." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            THREAD_INFO << " Done" << std::endl;
        };

        THREAD_INFO << " Started" << std::endl;
        std::thread thread(func);

        thread.join();
        THREAD_INFO << " Completed" << std::endl;
    }

    void Create_Simple_Thread_Lambda()
    {
        THREAD_INFO << " Started" << std::endl;
        std::thread thread([](void)-> void {
            THREAD_INFO << " Entered. Sleeping 2 seconds." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            THREAD_INFO << " Done" << std::endl;
        });

        thread.join();
        THREAD_INFO << " Completed" << std::endl;
    }

    void Create_Simple_Thread_Params()
    {
        const auto func = [](unsigned short timeout)-> void {
            THREAD_INFO << "Entered. Sleeping " << timeout << " seconds." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            THREAD_INFO << "Done" << std::endl;
        };

        THREAD_INFO << "Started" << std::endl;
        std::thread thread(func, 5);

        thread.join();
        THREAD_INFO << "Completed" << std::endl;
    }


    void Create_Simple_Thread_Params_By_Reference()
    {
        const auto func = [](int& value)-> void {
            THREAD_INFO << "Starting thread" << std::endl;
            for (int i = 0; i < 10; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100u));
                value++;
            }
            THREAD_INFO << "Done" << std::endl;
        };

        int value{ 10 };
        THREAD_INFO << "Starting thread: value = " << value << std::endl;
        std::thread thread(func, std::ref(value));

        thread.join();
        THREAD_INFO << "Completed: value = " << value << std::endl;
    }

    void Create_Simple_Thread_FunctionObjects() {
        THREAD_INFO << "Started" << std::endl;
        std::thread thread((DisplayThread()));
        thread.join();
        THREAD_INFO << "Completed" << std::endl;
    }

    void CreateThread_Function_accept_Function() {

        auto handler = [](auto func,  std::string text)-> void {
            THREAD_INFO << "Starting thread" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2u));
            func(text);
            THREAD_INFO << "Done" << std::endl;
        };

        auto callback = [](const std::string& text)-> void {
            THREAD_INFO << "Callback: " << text << std::endl;
        };

        THREAD_INFO << "Started" << std::endl;
        std::thread thread(handler, callback,  "text");
        thread.join();
        THREAD_INFO << "Completed" << std::endl;
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
            THREAD_INFO << "Rage counter for " << thread_name << ": " << thread_local_counter << '\n';
        };

        std::thread a(increase_rage, "a"), b(increase_rage, "b");
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            THREAD_INFO << "Rage counter for main: " << thread_local_counter << '\n';
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
        THREAD_INFO << "Starting thread/n";
        std::this_thread::sleep_for(std::chrono::seconds(4u));
        THREAD_INFO << "Thread throwing a runtime_error exception...\n";
        throw std::runtime_error{ "Exception from thread" };
    }

    void threadFunc(std::exception_ptr& err)
    {
        try {
            do_some_work();
        }
        catch (...) {
            THREAD_INFO << "Thread caught exception, returning exception...\n";
            err = std::current_exception();
        }
    }

    void doWorkInThread()
    {
        std::exception_ptr error;
        std::thread T{ threadFunc, std::ref(error) };
        T.join();

        if (error) {
            THREAD_INFO << "Main thread received exception, rethrowing it...\n";
            rethrow_exception(error);
        }
        else {
            THREAD_INFO << "Main thread did not receive any exception.\n";
        }
    }

    void Run_Test()
    {
        try {
            doWorkInThread();
        }
        catch (const std::exception& e) {
            THREAD_INFO << "Main function caught: '" << e.what() << "'\n";
        }
    }
}

void Threads::TEST_ALL()
{
    // Thead_as_ClassMethod();
    // Thead_as_ClassMethod_Params();
    // Thead_as_ClassMethod_1();

    // Create_Thread_Tuple_as_Params();
    Create_Thread_Tuple_as_Params_2();

    // Create_Simple_Thread();
    // Create_Simple_Thread_Lambda();
    // Create_Simple_Thread_Params();
    // Create_Simple_Thread_Params_By_Reference();
    // Create_Simple_Thread_FunctionObjects();

    // CreateThread_Function_accept_Function();

    // ThreadLocalStorage::Test();
    // ThreadLocalStorage::Test2();
    // ThreadLocalStorage::Test3();

    // VariousTests::Test();

    // HandlingExceptions::Run_Test();
}
