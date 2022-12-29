//============================================================================
// Name        : Future.cpp
// Created on  : 07.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Future src class
//============================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <future>
#include <vector>

#include "Future.h"
#include "../ThreadHelperUtilities/ThreadHelperUtilities.h"

namespace Future::CallClassMethod {

    class ThreadTestClass {
    protected:
        std::future<bool> __future;

    protected:
        bool doStomething(std::string_view message, unsigned int timeout = 10) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            THREAD_INFO << "Input message : " << message << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            THREAD_INFO << "Done." << std::endl;
            return true;
        }


    public:
        void start() {
            //this->__future = std::async(ThreadTestClass::doStomething, "Inout Text", 10)
            this->__future = std::async(std::launch::async, &ThreadTestClass::doStomething, this, "Some_Test_Input_Param", 5);
            THREAD_INFO << "doStomething() launched async" << std::endl;
        }

        void get() {
            THREAD_INFO << "Before get" << std::endl;
            this->__future.get();
            THREAD_INFO << "After get" << std::endl;
        }
    };

    void Test()
    {

        ThreadTestClass T;
        THREAD_INFO << "Starting..." << std::endl;
        T.start();
        THREAD_INFO << "Waiting" << std::endl;
        T.get();
        THREAD_INFO << "Done" << std::endl;
    }

};

namespace Future {

    bool doStomething(std::string_view message, unsigned int timeout = 10) {
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        THREAD_INFO << "Input message : " << message << std::endl;
        return true;
    }

    std::string sleepAndReturnText(unsigned int timeout = 10) {
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        return "Task lasted for " + std::to_string(timeout) + " seconds.";
    }

    int countdown(int from, int to) {
        for (int i = from; i != to; --i) {
            std::cout << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << "Lift off! " << std::endl;
        return from - to;
    }

    int get_int() {
        int x;
        /** throw on failbit set **/
        std::cin.exceptions(std::ios::failbit);
        THREAD_INFO << "Please, enter an integer value: ";
        /** sets failbit if invalid. **/
        std::cin >> x;
        return x;
    }

    ///////////////////////////////////////

    void Future_CreateAndPush_ToVector() {

        std::vector<std::future<int>> jobs;
        jobs.emplace_back(std::async(countdown, 5, 0));


        std::cout << "Do something while waiting for function to set future : ";
        std::chrono::milliseconds span(100);
        while (jobs.back().wait_for(span) == std::future_status::timeout)
            std::cout << ". " << std::flush;

        /** retrieve return value : **/
        auto result = jobs.back().get();
        std::cout << "Task result : " << result << std::endl;
    }

    void FutureAsyncRun() {
        // call function asynchronously:
        std::future<bool> fut = std::async(doStomething, "444444443", 3);

        THREAD_INFO << "Do something while waiting for function to set future : ";
        std::chrono::milliseconds span(500);
        while (fut.wait_for(span) == std::future_status::timeout)
            std::cout << ". " << std::flush;

        /** retrieve return value : **/
        auto result = fut.get();
        THREAD_INFO << "Task result : " << result << std::endl;
    }

    void Future_WaitFor() {
        const unsigned int timeout = 7;
        std::future<std::string> future = std::async(std::launch::async, [](unsigned int timeout)-> std::string {
            THREAD_INFO << "Sleeping for " << timeout << " seconds...." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            THREAD_INFO << "Done." << std::endl;
            return std::string("Task lasted for " + std::to_string(timeout) + " seconds.");
        }, timeout);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        THREAD_INFO << "waiting..." << std::endl;

        std::future_status status;
        do {
            status = future.wait_for(std::chrono::seconds(1));
            if (status == std::future_status::deferred) {
                THREAD_INFO << "deferred" << std::endl;
            }
            else if (status == std::future_status::timeout) {
                THREAD_INFO << "timeout" << std::endl;
            }
            else if (status == std::future_status::ready) {
                THREAD_INFO << "ready!" << std::endl;
            }
        } while (status != std::future_status::ready);

        THREAD_INFO << "result is " << future.get() << std::endl;
    }

    void Future_WaitFor_2() {
        const unsigned int timeout = 7;
        std::future<std::string> future = std::async(std::launch::async, [](unsigned int timeout)-> std::string {
            THREAD_INFO << "Sleeping for " << timeout << " seconds..." << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(timeout));

            std::cout << std::endl;
            THREAD_INFO << "Done" << std::endl;
            return std::string("Task lasted for " + std::to_string(timeout) + " seconds.");
        }, timeout);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        THREAD_INFO << "Do something while waiting for function to set future: ";

        std::chrono::milliseconds span(170);
        while (future.wait_for(span) == std::future_status::timeout)
            std::cout << ". " << std::flush;

        /** retrieve return value : **/
        auto result = future.get();
        THREAD_INFO << "Done. Future result : " << result << std::endl;
    }

    std::future<std::string> createFuture(unsigned int timeout) {
        std::cout << "Creating future function called." << std::endl;
        std::future<std::string> the_future = std::async(std::launch::async, [](unsigned int timeout)-> std::string {
            std::cout << "Sleeping for " << timeout << " seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            return std::string("Task lasted for " + std::to_string(timeout) + " seconds.");
        }, timeout);
        // Return the future object
        return std::move(the_future);
    }

    void Future_WaitFor_3() {
        const unsigned int timeout = 7;
        std::future<std::string> future = createFuture(timeout);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Do something while waiting for function to set future: ";

        std::chrono::milliseconds span(170);
        while (future.wait_for(span) == std::future_status::timeout)
            std::cout << ". " << std::flush;

        /** retrieve return value : **/
        auto result = future.get();
        std::cout << "Done\nFuture result : " << result << std::endl;
    }

    void Future_WaitUntil() {
        unsigned int timeout = 5;
        std::chrono::system_clock::time_point five_seconds_timeout =
                std::chrono::system_clock::now() + std::chrono::seconds(timeout);
        std::future<std::string> job_for_3_sec = std::async(sleepAndReturnText, 3);
        std::future<std::string> job_for_8_sec = std::async(sleepAndReturnText, 8);

        /*
        // Make a future that that takes 1 second to completed.
        std::promise<int> p1;
        std::future<int> f_completes = p1.get_future();
        std::thread([](std::promise<int> p1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            p1.set_value_at_thread_exit(9);
        },std::move(p1)).detach();

        // Make a future that that takes 5 seconds to completed.
        std::promise<int> p2;
        std::future<int> f_times_out = p2.get_future();
        std::thread([](std::promise<int> p2){
            std::this_thread::sleep_for(std::chrono::seconds(5));
            p2.set_value_at_thread_exit(8);
        }, std::move(p2)).detach();
        */

        std::cout << "Waiting for " << timeout << " seconds..." << std::endl;

        if (std::future_status::ready == job_for_3_sec.wait_until(five_seconds_timeout)) {
            std::cout << "job_for_3_sec: " << job_for_3_sec.get() << std::endl;
        }
        else {
            std::cout << "job_for_3_sec did not complete!" << std::endl;
        }

        if (std::future_status::ready == job_for_8_sec.wait_until(five_seconds_timeout)) {
            std::cout << "job_for_8_sec: " << job_for_8_sec.get() << std::endl;
        }
        else {
            std::cout << "job_for_8_sec did not complete!" << std::endl;
        }

        std::cout << "Done!" << std::endl;
    }

    void FutureWait() {
        // call function asynchronously:
        std::future<bool> fut = std::async(doStomething, "Input Text", 3);

        THREAD_INFO << "Waiting for Future....." << std::endl;
        fut.wait();
        THREAD_INFO << "Future done" << std::endl;;

        if (fut.get()) { // guaranteed to be ready (and not block) after wait returns
            THREAD_INFO << "Result = TRUE" << std::endl;
        }
        else {
            THREAD_INFO << "Result = FALSE" << std::endl;
        }
    }

    void FutureAsyncRunAndExit() {
        // call function asynchronously:
        std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
        std::future<bool> fut = std::async(doStomething, "Done", 3);
        std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void FutureGet() {
        std::future<int> fut = std::async(get_int);
        try {
            int x = fut.get();
            THREAD_INFO << "You entered: " << x << '\n';
        }
        catch (const std::exception& exc) {
            THREAD_INFO << "[exception caught]" << std::endl;
            std::cout << exc.what() << std::endl;
        }
    }



    void FutureGet_2() {
        // call function asynchronously:
        std::future<bool> fut = std::async(doStomething, "Input Text", 3);
        std::cout << "Running async task. And waiting for result. " << std::endl;
        if (fut.get()) { // guaranteed to be ready (and not block) after wait returns
            std::cout << "Result = TRUE" << std::endl;
        }
        else {
            std::cout << "Result = FALSE" << std::endl;
        }
    }

    void Future_Lambda() {
        constexpr unsigned short timeout = 5;
        std::future<std::string_view> future = std::async([]( unsigned short timeout)-> std::string_view {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // For pritty output
            THREAD_INFO << "Async task started. Sleeping " << timeout << " seconds" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            THREAD_INFO << "Task completed." << std::endl;
            return std::string_view("Task completed");
        }, timeout);

        THREAD_INFO << "Running async task. And waiting for result. " << std::endl;
        std::string_view result = future.get();
        THREAD_INFO << "Result: " << result << std::endl;
    }


    void Future_Move() {
        constexpr unsigned short timeout = 5;
        std::future<std::string_view> future = std::async([](unsigned short timeout)-> std::string_view {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // For pritty output
            THREAD_INFO << "Async task started. Sleeping " << timeout << " seconds" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            THREAD_INFO << "Task completed." << std::endl;
            return std::string_view("Task completed");
        }, timeout);


        std::future<std::string_view> future2 = std::move(future);

        future2.wait();
    }


    void FutureErrorTest1()
    {
        std::future<int> empty;
        try {
            [[maybe_unused]]
            int n = empty.get(); // The behavior is undefined, but some implementations throw std::future_error
        }
        catch (const std::future_error& e) {
            THREAD_INFO << "Error code: " << e.code() << ". Message: " << e.what() << std::endl;
        }
    }

    void FutureErrorTest2() {
        std::promise<int> prom;
        try {
            auto result = prom.get_future();
            /** This line throws std::future_error.**/
            result = prom.get_future();
        }
        catch (const std::future_error& e) {
            THREAD_INFO << "future_error caught. " << std::endl;
            THREAD_INFO << "   code: " << e.code().message() << std::endl;
            THREAD_INFO << "   what: " << e.what() << std::endl;
        }
    }

    void Future_Valid_Test()
    {
        constexpr unsigned short timeout = 5;
        constexpr auto some_task = [](unsigned short timeout)-> std::string_view {
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            return std::string_view("We've_Done!!!");
        };

        std::future<std::string_view> foo, bar;

        foo = std::async(some_task, timeout);

        if (foo.valid())
            std::cout << "foo is valid." << std::endl;
        else
            std::cout << "foo is not valid" << std::endl;

        if (bar.valid())
            std::cout << "bar is valid." << std::endl;
        else
            std::cout << "bar is not valid" << std::endl;

        std::cout << "\nMoving 'Foo' --> 'Bar'\n" << std::endl;
        bar = std::move(foo);

        if (foo.valid())
            std::cout << "foo's value: " << foo.get() << std::endl;
        else
            std::cout << "foo is not valid" << std::endl;

        if (bar.valid())
            std::cout << "bar's value: " << bar.get() << std::endl;
        else
            std::cout << "bar is not valid" << std::endl;
    }

    void Furute_Asynch_vs_Defered() {
        auto some_task = [](unsigned long timeout)-> void {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            THREAD_INFO << "Task started. Sleeping for " << timeout << " seconds\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            THREAD_INFO << "Done!\n";
        };

        constexpr unsigned long timeout = 5; // 5 seconds
        std::future<void> futureAsync = std::async(std::launch::async, some_task, timeout);

        THREAD_INFO << "After std::launch::async task. Point 1\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
        THREAD_INFO << "After std::launch::async task. Point 2\n";

        futureAsync.wait();

        // std::launch::deferred
        // the task is executed on the calling thread the first time its result is requested (lazy evaluation)

        std::future<void> futureDeferred = std::async(std::launch::deferred, some_task, timeout);

        THREAD_INFO << "After std::launch::deferred task. Point 1\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
        THREAD_INFO << "After std::launch::deferred task. Point 2\n";

        futureDeferred.get();
    }
}

namespace Future::SharedFuture
{
    void Test()
    {
        std::promise<void> ready_promise, thread1Ready, thread2Ready;
        std::shared_future<void> sharedFuture(ready_promise.get_future());
        std::chrono::time_point<std::chrono::high_resolution_clock> start;

        auto func1 = [&](unsigned int timeout)-> std::chrono::duration<double, std::milli> {
            THREAD_INFO << "Started." << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            thread1Ready.set_value();
            THREAD_INFO << "Ready." << std::endl;

            sharedFuture.wait(); // waits for the signal from main()
            THREAD_INFO << "Done" << std::endl;

            return std::chrono::high_resolution_clock::now() - start;
        };

        auto func2 = [&](unsigned int timeout)-> std::chrono::duration<double, std::milli> {
            THREAD_INFO << "Started" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            thread2Ready.set_value();
            THREAD_INFO << "Ready." << std::endl;

            sharedFuture.wait(); // waits for the signal from main()
            THREAD_INFO << "Done" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // For pritty output
            return std::chrono::high_resolution_clock::now() - start;
        };


        unsigned int timeout1 = 2, timeout2 = 3;
        THREAD_INFO << "Starting threads...." << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // For pritty output
        auto result1 = std::async(std::launch::async, func1, timeout1);

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // For pritty output
        auto result2 = std::async(std::launch::async, func2, timeout2);

        THREAD_INFO << "Wait for the threads to become ready..." << std::endl;
        thread1Ready.get_future().wait();
        thread2Ready.get_future().wait();

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // For pritty output
        THREAD_INFO << "Workers are ready!" << std::endl;

        // the threads are ready, start the clock
        start = std::chrono::high_resolution_clock::now();

        // signal the threads to go
        std::this_thread::sleep_for(std::chrono::seconds(5));
        ready_promise.set_value();

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // For pritty output
        THREAD_INFO << "Thread 1 received the signal "
                    << result1.get().count() << " ms after start\n"
                    << "Thread 2 received the signal "
                    << result2.get().count() << " ms after start\n";
    }

    void WaitFor_CheckStatus()
    {
        THREAD_INFO << "Test started." << std::endl;

        // TODO: if 'shared_future' --> 'future' we'll have a CRUSH !!!
        std::shared_future<int> someSharedJob = std::async(std::launch::async, [](unsigned int timeout) {
            THREAD_INFO << "Sleeping..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            THREAD_INFO << "Done. " << std::endl;
            return static_cast<int>(timeout);
        }, 5);

        auto waiter = [&]() {
            THREAD_INFO << "Waiting..." << std::endl;
            std::chrono::milliseconds span(500);
            std::future_status status = someSharedJob.wait_for(span);
            while (status != std::future_status::ready) {
                if (status == std::future_status::deferred) {
                    THREAD_INFO << "Deferred\n";
                }
                else if (status == std::future_status::timeout) {
                    THREAD_INFO << "Timeout\n";
                }
                status = someSharedJob.wait_for(span);
            }
            THREAD_INFO << "Done. Result is " << someSharedJob.get() << std::endl;
        };

        auto waitTask1 = std::async(waiter);
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // For pritty output
        auto waitTask2 = std::async(waiter);

        waitTask1.get();
        waitTask2.get();

        THREAD_INFO << "Done. " << std::endl;
    }
}

namespace Future::CollectionFutures {

    void Emplace_To_Vector() {

        auto handler = [](size_t timeout) {
            THREAD_INFO << "Job started." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            THREAD_INFO << "Job done." << std::endl;
        };

        std::vector<std::future<void>> jobs;

        {
            jobs.emplace_back(std::async(handler, 5));
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            jobs.emplace_back(std::async(handler, 10));
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            jobs.emplace_back(std::async(handler, 30));
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        std::cout << "Do something while waiting for function to set future : ";
        std::chrono::milliseconds span(100);

        // print '.' while any of jobs status == std::future_status::timeout
        while (std::any_of(jobs.cbegin(), jobs.cend(), [&span](const auto& T) {
            return T.wait_for(span) == std::future_status::timeout;
        })) std::cout << ". " << std::flush;
    }
}


namespace Future::Tests {

    void RunAsyncTest() {

        std::string_view text = "X";
        std::cout << text << std::endl;

        std::async(std::launch::async, [&] {
            THREAD_INFO << "Job 1 started." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            text = "Y";
            THREAD_INFO << "Job 1 done." << std::endl;
        }).wait();


        std::async(std::launch::async, [&] {
            THREAD_INFO << "Job 2 started." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            text = "Z";
            THREAD_INFO << "Job 2 done." << std::endl;
        }).wait();

        std::cout << text << std::endl;
    }
}

void Future::TEST_ALL()
{
    Future_CreateAndPush_ToVector();

    // FutureAsyncRun();

    // FutureAsyncRunAndExit();

    // FutureGet();
    // FutureGet_2();
    // Future_Lambda();
    // Future_Move();

    // FutureWait();

    // FutureErrorTest1();
    // FutureErrorTest2();

    // Future_WaitFor();
    // Future_WaitFor_2();
    // Future_WaitFor_3();
    // Future_WaitUntil();
    // Future_Valid_Test();

    // Furute_Asynch_vs_Defered();

    // SharedFuture::Test();
    // SharedFuture::WaitFor_CheckStatus();

    // CollectionFutures::Emplace_To_Vector();

    // CallClassMethod::Test();

    // Tests::RunAsyncTest();
}
