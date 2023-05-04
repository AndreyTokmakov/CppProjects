//============================================================================
// Name        : Synchronization.h
// Created on  : 23.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Lock synchronization src src
//============================================================================

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <thread>
#include <future>
#include <fstream>
#include <shared_mutex>

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <sstream>
#include <unordered_map>

#include <ranges>
#include <span>
#include <chrono>
#include <cstdlib>
#include <ctime>

#include <optional>
#include <syncstream>

#include "Synchronization.h"
#include "../ThreadHelperUtilities/ThreadHelperUtilities.h"

namespace Synchronization {

    void Test_Unsynch() {
        int counter = 0;
        THREAD_INFO << "started." << std::endl;

        auto incremetor = [&]()-> void {
            for (int i = 0; i < 1000000; ++i) {
                counter = counter + 1;
            }
        };

        std::vector<std::thread> jobs;
        for (int i = 0; i++ < 5;)
            jobs.emplace_back(incremetor);
        std::for_each(jobs.begin(), jobs.end(), [](auto& T) {T.join(); });
        std::cout << "Result = " << counter << std::endl;
    }

    void Mutex_Lock_Test_1() {
        int counter = 0;
        std::mutex mtx;

        THREAD_INFO << "started." << std::endl;

        auto incremetor = [&]()-> void {
            for (int i = 0; i < 100; ++i) {
                //mtx.lock();
                ++counter;
                THREAD_INFO  << "Counter: " << counter << std::endl;
                //mtx.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        };

        std::vector<std::thread> jobs;
        for (int i = 0; i++ < 5;)
            jobs.emplace_back(incremetor);
        std::for_each(jobs.begin(), jobs.end(), [](auto& T) {T.join(); });
    }

    void LockGuard_Test_1() {
        unsigned int sharedVariable = 0;
        std::mutex mtx;

        THREAD_INFO << "started." << std::endl;

        auto safe_increment = [&]()-> void {
            const std::lock_guard<std::mutex> lock(mtx);
            ++sharedVariable;
            THREAD_INFO << sharedVariable << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // g_i_mutex is automatically released when lock goes out of scope
        };

        THREAD_INFO << "Start value: " << sharedVariable << std::endl;

        std::vector<std::future<void>> jobs;
        for (int i = 0; i < 3; i++)
            jobs.emplace_back(std::async(safe_increment));
        std::for_each(jobs.begin(), jobs.end(), [](auto& T) {T.wait(); });

        THREAD_INFO << "End value: " << sharedVariable << std::endl;
    }

    void LockGuard_Test_2() {
        std::mutex mtx;
        THREAD_INFO << "started." << std::endl;

        auto print_even = [](int x)-> void {
            if (x % 2 == 0) {
                THREAD_INFO << "is even" << std::endl;
            } else {
                throw (std::logic_error("not even"));
            }
        };

        auto print_thread_id = [&](int id)-> void {
            try { // using a local lock_guard to lock mtx guarantees unlocking on destruction / exception:
                std::lock_guard<std::mutex> lock(mtx);
                print_even(id);
            }
            catch (const std::logic_error& exc) {
                THREAD_INFO << "[exception caught]: " << exc.what() << std::endl;
            }
        };

        std::vector<std::thread> jobs;
        for (int i = 0; i++ < 5;)
            jobs.emplace_back(print_thread_id, i + 1);
        std::for_each(jobs.begin(), jobs.end(), [](auto& T) {T.join(); });
        THREAD_INFO << "Done." << std::endl;
    }
}


namespace Synchronization::UniqueLock
{
    void UniqueLock_Lock() {
        std::mutex mtx;
        THREAD_INFO << "started." << std::endl;

        auto print_block = [&](int n, char c)-> void {
            // critical section (exclusive access to std::cout signaled by lifetime of lck):
            std::unique_lock<std::mutex> lock(mtx);
            for (int i = 0; i < n; ++i) {
                std::cout << c;
            }
            std::cout << std::endl;
        };

        std::future<void> future1 = std::async(print_block, 50, '*');
        std::future<void> future2 = std::async(print_block, 50, '$');
        future1.wait();
        future2.wait();
    }

    void UniqueLock_UnLock() {
        std::mutex mtx;

        [[maybe_unused]]
        unsigned long counter = 0;
        THREAD_INFO << "started." << std::endl;

        auto print_thread_id = [&](int id)-> void {
            std::unique_lock<std::mutex> lock(mtx, std::defer_lock);
            // critical section (exclusive access to std::cout signaled by locking lck):
            lock.lock();
            THREAD_INFO << id << std::endl;
            lock.unlock();
        };

        std::vector<std::thread> jobs;
        for (int i = 0; i++ < 10;)
            jobs.emplace_back(print_thread_id, i + 1);
        std::for_each(jobs.begin(), jobs.end(), [](auto& T) {T.join(); });
        THREAD_INFO << "Done." << std::endl;
    }

    void UniqueLock_UnLock_2() {
        std::mutex mtx;
        unsigned long counter = 0;
        const unsigned short thread_max = 1000;

        auto increment_counter = [&]()-> void {
            std::unique_lock<std::mutex> lck(mtx, std::defer_lock);
            lck.lock();
            counter = counter + 1;
            std::cout << counter << std::endl;
            lck.unlock();
        };

        std::cout << "Starting jobs.." << std::endl;

        std::vector<std::future<void>> jobs;
        jobs.reserve(thread_max);
        for (int i = 0; i < thread_max; i++)
            jobs.emplace_back(std::async(increment_counter));

        std::cout << "Waiting for " << jobs.size() << " jobs...." << std::endl;
        for (const std::future<void>& fut : jobs)
            fut.wait();

        std::cout << "All done\nCounter = " << counter << std::endl;
    }

    void UniqueLock_UnLock_2_1() {
        std::mutex mtx;
        unsigned long counter = 0;
        const unsigned short thread_max = 1000;

        auto increment_counter = [&]()-> void {
            std::unique_lock<std::mutex> lck(mtx, std::defer_lock);
            lck.lock();
            counter = counter + 1;
            std::cout << counter << std::endl;
            lck.unlock();
        };

        std::cout << "Starting jobs.." << std::endl;

        std::vector<std::future<void>> jobs(thread_max);
        // std::future<void> jobs[thread_max];
        for (int i = 0; i < thread_max; i++)
            jobs[i] = std::async(increment_counter);

        std::cout << "Waiting ..." << std::endl;
        for (const std::future<void>& fut : jobs)
            fut.wait();

        std::cout << "All done\nCounter = " << counter << std::endl;
    }

    void UniqueLock_Release()
    {
        std::mutex mtx;
        int count = 0;

        auto print_count_and_unlock = [&](std::mutex* p_mtx)-> void {
            std::cout << "count: " << count << std::endl;
            p_mtx->unlock();
        };

        auto task = [&]()-> void {
            std::unique_lock<std::mutex> lck(mtx);
            ++count;
            print_count_and_unlock(lck.release());
        };

        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i)
            threads.emplace_back(task);
        for (auto& x : threads)
            x.join();
    }

    void UniqueLock_OwnsLock() {
        std::mutex mtx;
        auto print_star = [&]()-> void {
            std::unique_lock<std::mutex> lck(mtx, std::try_to_lock);
            if (lck.owns_lock()) { // print '*' if successfully locked, 'x' otherwise:
                std::cout << '*';
            }
            else {
                std::cout << 'x';
            }
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 500; ++i)
            threads.emplace_back(print_star);

        for (auto& x : threads)
            x.join();
    }

    //-------------------------------------------------------------------------

    void prepare_data() {
        THREAD_INFO << " Prepate data\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    void handle_data() {
        THREAD_INFO << " Handle data\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    std::unique_lock<std::mutex> processData() {
        std::mutex mtx;
        std::unique_lock<std::mutex> lock(mtx);
        prepare_data();
        return lock;
    }

    void Transfer_Ownership() {
        std::unique_lock<std::mutex> lk(processData());
        handle_data();
        THREAD_INFO << " Done\n";
    }
}

namespace Synchronization::SharedMutext {

    void Test_NoShare() {
        std::mutex mtx;
        int i = 10;

        auto reader = [&]()->void {
            std::unique_lock<std::mutex> slk(mtx);
            THREAD_INFO << "Read i as " << i << "..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            THREAD_INFO << "Woke up..." << std::endl;
        };

        std::future<void> future1 = std::async(reader);
        std::future<void> future2 = std::async(reader);
        future1.wait();
        future2.wait();
    }

    void Synchronized_Read_Write_Test() {
        std::string file = "Original content."; // Simulates a file
        std::mutex output_mutex;      // mtx that protects output operations.
        std::shared_mutex mutex; // reader/writer mtx

        auto read = [&mutex, &file, &output_mutex](int id) {
            std::string content;
            {
                std::shared_lock lock(mutex, std::defer_lock); // Do not lock it first.
                lock.lock(); // Lock it here.
                content = file;
            }
            std::lock_guard lock(output_mutex);
            THREAD_INFO << "Contents read by reader #" << id << ": " << content << std::endl;
        };

        auto write = [&mutex, &file, &output_mutex]() {
            {
                std::lock_guard file_lock(mutex);
                file = "New content";
            }
            std::lock_guard output_lock(output_mutex);
            THREAD_INFO << "New content saved." << std::endl;
        };

        THREAD_INFO << "Two readers reading from file." << std::endl;
        THREAD_INFO << "A writer competes with them." << std::endl;

        std::thread reader1(read, 1);
        std::thread reader2(read, 2);
        std::thread writer(write);
        reader1.join();
        reader2.join();
        writer.join();

        THREAD_INFO << "The first few operations to file are done.\n";

        reader1 = std::thread(read, 3);
        reader1.join();
    }

    void Read_Write_Test_Blocking()
    {
        std::shared_mutex mtx;
        auto reader = [&mtx]()-> void {
            while (true) {
                {
                    std::shared_lock lock(mtx);
                    std::osyncstream{std::cout} << "Reader: entered" << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    std::osyncstream{std::cout} << "Reader: exited" << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds (100));
            }
        };

        auto writer = [&mtx]()-> void {
            while (true) {
                std::lock_guard lock(mtx);
                std::osyncstream {std::cout}  << "Writer: entered" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(10));
                std::osyncstream {std::cout}  << "Writer: exited" << std::endl;
            }
        };


        std::jthread writerThread1(writer);
        std::jthread readerThread1(reader), readerThread2(reader), readerThread3(reader);
    }
}

namespace Synchronization::SharedTimedMutext {

    class DNSCache {
    protected:
        using dns_entry = std::string;
        std::unordered_map<std::string, dns_entry> entries;
        mutable std::shared_mutex entry_mutex;

    public:
        std::optional<dns_entry> find_entry(const std::string& domain) const {
            std::shared_lock<std::shared_mutex> lock_shared(entry_mutex);
            THREAD_INFO << "Reading cache." << std::endl;
            const auto it = entries.find(domain);
            return (it == entries.end()) ? std::nullopt : std::make_optional<dns_entry>(it->second);
        }

        void update_or_add_entry(const std::string& domain, const dns_entry& dns_details) {
            std::lock_guard<std::shared_mutex> lock(entry_mutex);
            THREAD_INFO << "Updating cache." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2)); //4Test
            entries[domain] = dns_details;
            THREAD_INFO << "Update done" << std::endl;
        }
    };

    void Test()
    {
        std::shared_timed_mutex m;
        int i = 10;

        auto reader = [&]()->void {
            // both the threads get access to the integer i
            std::shared_lock<std::shared_timed_mutex> slk(m);
            THREAD_INFO << "Read i as " << i << "..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            THREAD_INFO << "Woke up..." << std::endl;
        };

        std::future<void> future1 = std::async(reader);
        std::future<void> future2 = std::async(reader);
        future1.wait();
        future2.wait();
    }


    void Read_Write_Test()
    {
        DNSCache cache;

        auto reader = [&cache](const std::string& domain, int timeout)-> void {
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                cache.find_entry(domain);
            }
        };

        auto writer = [&cache](const std::string& domain, int timeout)-> void {
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(timeout));
                cache.update_or_add_entry(domain, "some_info");
            }
        };

        std::vector<std::future<void>> readers, writers;

        readers.emplace_back(std::async(reader, "www.ya.ru", 1));
        readers.emplace_back(std::async(reader, "www.google.com", 3));
        readers.emplace_back(std::async(reader, "www.mail.ru", 2));
        writers.emplace_back(std::async(writer, "www.mail.ru", 5));

        readers.back().wait();
    }

    void Try_Lock_Until() {
        std::shared_timed_mutex mtx;

        auto func = [&] ()-> void {
            THREAD_INFO << "Entered" << std::endl;
            auto now = std::chrono::steady_clock::now();

            [[maybe_unused]]
            auto result = mtx.try_lock_until(now + std::chrono::seconds(2));
            THREAD_INFO << "Done" << std::endl;
        };

        std::lock_guard<std::shared_timed_mutex> lock(mtx);
        std::thread thread(func);
        thread.join();
    }


    void Try_Lock_For() {
        std::mutex cout_mutex;
        std::timed_mutex mtx;

        auto task = [&](int id)-> void {
            std::ostringstream stream;

            for (int i = 0; i < 3; ++i) {
                if (mtx.try_lock_for(std::chrono::milliseconds(100))) {
                    stream << "success ";
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    mtx.unlock();
                }
                else {
                    stream << "failed ";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            std::lock_guard<std::mutex> lock(cout_mutex);
            THREAD_INFO << "[" << id << "] " << stream.str() << "\n";
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 5; ++i)
            threads.emplace_back(task, i);
        for (auto& i : threads)
            i.join();
    }
}


namespace Synchronization::TimedMutex {

    void TryLockUntil()
    {
        std::timed_mutex mtx;
        std::lock_guard<std::timed_mutex> lock(mtx);

        auto task = std::async([&](unsigned short timeout)-> void {
            THREAD_INFO << "Task started.\n";
            auto now = std::chrono::steady_clock::now();

            [[maybe_unused]]
            auto result = mtx.try_lock_until(now + std::chrono::seconds(timeout));
            THREAD_INFO << "Task completed.\n";
        }, 5);
        task.wait();
    }

    void TryLockUntil_1()
    {
        std::timed_mutex mtx;

        auto job1 = std::async([&](unsigned int timeout)->void {
            std::scoped_lock<std::timed_mutex> lock(mtx);
            THREAD_INFO << "Locked for " << timeout << " milliseconds" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
            THREAD_INFO << "Release." << std::endl;
        }, 5000);


        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        auto job2 = std::async([&](unsigned int timeout)->void {
            for (int i = 0; i < 10; ++i) {
                auto now = std::chrono::steady_clock::now();
                if (mtx.try_lock_until(now + std::chrono::milliseconds(timeout))) {
                    THREAD_INFO << "Locked ok!" << std::endl;
                    break;
                } else {
                    THREAD_INFO << "Failed to acquire lock." << std::endl;
                }
            }
        }, 1000);

        job1.wait();
        job2.wait();
        THREAD_INFO << "Done." << std::endl;
    }

    void TryLockFor() {
        std::mutex cout_mutex;
        std::timed_mutex mtx;

        auto task = [&](int id)-> void {
            std::ostringstream stream;

            for (int i = 0; i < 3; ++i) {
                if (mtx.try_lock_for(std::chrono::milliseconds(100))) {
                    stream << "success ";
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    mtx.unlock();
                }
                else {
                    stream << "failed ";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            std::lock_guard<std::mutex> lock(cout_mutex);
            THREAD_INFO << "[" << id << "] " << stream.str() << "\n";
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 5; ++i)
            threads.emplace_back(task, i);
        for (auto& i : threads)
            i.join();
    }

    void TryLockFor_1() {
        std::timed_mutex mtx;

        auto job1 = std::async([&](unsigned int timeout)->void {
            std::scoped_lock<std::timed_mutex> lock(mtx);
            THREAD_INFO << "Locked for " << timeout << " milliseconds" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
            THREAD_INFO << "Release." << std::endl;
        }, 5000);


        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        auto job2 = std::async([&](unsigned int timeout)->void {
            for (int i = 0; i < 10; ++i) {
                if (mtx.try_lock_for(std::chrono::milliseconds(timeout))) {
                    THREAD_INFO << "Locked ok!" << std::endl;
                    break;
                } else {
                    THREAD_INFO << "Failed to acquire lock." << std::endl;
                }
            }
        }, 1000);

        job1.wait();
        job2.wait();
        THREAD_INFO << "Done." << std::endl;
    }

    void LimitTime_Using_UniqueLock()
    {
        std::timed_mutex mtx {};
        const std::jthread holder = std::jthread([&mtx] {
            std::lock_guard<std::timed_mutex> lock {mtx};
            THREAD_INFO << "Blocking mutex for 2 sec" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        });

        auto task = [&mtx]{
            std::unique_lock<std::timed_mutex> lock {mtx, std::chrono::milliseconds (200)};
            if (!lock.owns_lock())
                THREAD_INFO << "Failed to get lock." << std::endl;
            else
                THREAD_INFO << "Got the lock." << std::endl;
            return;
        };

        for (int i = 0; i < 3; ++i) {
            auto T1 = std::jthread(task);
            std::this_thread::sleep_for(std::chrono::milliseconds(1200));
        }
    }
}

namespace Synchronization::ScopedLock {

    class Employee {
    public:
        std::string id;
        std::vector<std::string> lunch_partners;
        std::mutex mtx;

    public:
        Employee(const std::string& id) : id(id) {
        }

        std::string output() const
        {
            std::string ret = "Employee " + id + " has lunch partners: ";
            for (const auto& partner : lunch_partners)
                ret += partner + " ";
            return ret;
        }
    };

    void send_mail([[maybe_unused]] const Employee& emp1,
                   [[maybe_unused]] const Employee& emp11) {
        // simulate a time-consuming messaging operation
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void assign_lunch_partner(Employee &e1, Employee &e2)
    {
        static std::mutex io_mutex;
        {
            std::lock_guard<std::mutex> lk(io_mutex);
            std::cout << e1.id << " and " << e2.id << " are waiting for locks" << std::endl;
        }

        {
            // use std::scoped_lock to acquire two locks without worrying about other calls to
            // assign_lunch_partner deadlocking us and it also provides a convenient RAII-style mechanism

            std::scoped_lock lock(e1.mtx, e2.mtx);

            /** Equivalent code 1 (using std::lock and std::lock_guard): **/
            // std::lock(e1.m, e2.m);
            // std::lock_guard<std::mtx> lk1(e1.m, std::adopt_lock);
            // std::lock_guard<std::mtx> lk2(e2.m, std::adopt_lock);

            /** Equivalent code 2 (if unique_locks are needed, e.g. for condition variables): **/
            // std::unique_lock<std::mtx> lk1(e1.m, std::defer_lock);
            // std::unique_lock<std::mtx> lk2(e2.m, std::defer_lock);
            // std::lock(lk1, lk2);

            {
                std::lock_guard<std::mutex> lock(io_mutex);
                std::cout << e1.id << " and " << e2.id << " got locks" << std::endl;
            }
            e1.lunch_partners.push_back(e2.id);
            e2.lunch_partners.push_back(e1.id);
        }

        send_mail(e1, e2);
        send_mail(e2, e1);
    }

    void Complex_Test()
    {
        Employee alice("alice"), bob("bob"), christina("christina"), dave("dave");

        // assign in parallel threads because mailing users about lunch assignments takes a long time
        std::vector<std::thread> threads;
        threads.emplace_back(assign_lunch_partner, std::ref(alice), std::ref(bob));
        threads.emplace_back(assign_lunch_partner, std::ref(christina), std::ref(bob));
        threads.emplace_back(assign_lunch_partner, std::ref(christina), std::ref(alice));
        threads.emplace_back(assign_lunch_partner, std::ref(dave), std::ref(bob));

        for (auto &thread : threads)
            thread.join();

        std::cout << alice.output() << '\n' << bob.output() << '\n' << christina.output() << '\n' << dave.output() << '\n';
    }

    //----------------------------------------------------------------------------------------------//

    struct SyncCounter {
        unsigned long long counter = 0;
        std::mutex mtx;
    };

    void Simple_Test() {
        SyncCounter counter1, counter2;
        THREAD_INFO << "started." << std::endl;

        auto increment = [](SyncCounter& counter)-> void {
            for (int i = 0; i < 100000; i++) {
                std::scoped_lock<std::mutex> lock(counter.mtx);
                counter.counter++;
            }
        };


        std::vector<std::thread> jobs;
        jobs.emplace_back(increment, std::ref(counter1));
        // jobs.emplace_back(increment, std::reference_wrapper(counter1));



        std::for_each(jobs.begin(), jobs.end(), [](auto& T) {
            T.join();
        });

        THREAD_INFO << "Done." << std::endl;
        THREAD_INFO << "Counter1 = " << counter1.counter << std::endl;
        THREAD_INFO << "Counter2 = " << counter2.counter << std::endl;
    }

    //-----------------------------------------------------------------------------------------------------//

    void do_some_work(const std::string& message)
    {
        {
            static std::mutex cout_lock;
            auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::lock_guard guard(cout_lock);
            std::cout << message << " @ " << ctime(&t);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 10));
    }

    void Good_Example()
    {
        std::mutex m1, m2;

        std::thread t1([&]() {
            while (true) {
                std::scoped_lock guard(m1, m2);
                do_some_work("Thread 1");
            }
        });

        std::thread t2([&]() {
            while (true) {
                std::scoped_lock guard(m2, m1);
                do_some_work("Thread 2");
            }
        });

        t1.join();
        t2.join();
    }
}

namespace Synchronization::RecursiveMutex
{

    struct NonRecursive
    {
        void push_back(int value)
        {
            std::unique_lock lock(mux_);
            /// We already hold mux_, so we couldn't call reserve()
            if (size_ == capacity_)
                reserve_impl(capacity_ == 0 ? 64 : capacity_ * 2);
            data_[size_++] = value;
        }

        void reserve(size_t cnt) {
            std::unique_lock lock(mux_);
            reserve_impl(cnt);
        }

    private:

        /// reserve_impl expects mux_ to be held by the caller
        void reserve_impl(size_t cnt) {
            auto new_data = std::make_unique<int[]>(cnt);
            std::copy_n(data_.get(), size_, new_data.get());
            data_ = std::move(new_data);
            capacity_ = cnt;
            size_ = std::min(size_, capacity_);
        }

        std::mutex mux_;
        std::unique_ptr<int[]> data_;
        size_t size_ = 0;
        size_t capacity_ = 0;
    };

    struct Recursive
    {
        void push_back(int value)
        {
            std::unique_lock lock(mux_);
            // holding a recursive mutex multiple times is fine
            if (size_ == capacity_)
                reserve(capacity_ == 0 ? 64 : capacity_ * 2);
            data_[size_++] = value;
        }

        void reserve(size_t cnt)
        {
            std::unique_lock lock(mux_);
            auto new_data = std::make_unique<int[]>(cnt);
            std::copy_n(data_.get(), size_, new_data.get());
            data_ = std::move(new_data);
            capacity_ = cnt;
            size_ = std::min(size_, capacity_);
        }

    private:
        std::recursive_mutex mux_;
        std::unique_ptr<int[]> data_;
        size_t size_ = 0;
        size_t capacity_ = 0;
    };

    void main()
    {
        NonRecursive non;
        for (int i = 0; i < 200; i++)
            non.push_back(i);
        non.reserve(2);  // malloc(): corrupted top size

        std::cout << "-----------------\n";

        Recursive rec;
        for (int i = 0; i < 200; i++)
            rec.push_back(i);
        rec.reserve(2);
    }
}

void Synchronization::TEST_ALL()
{
    // Test_Unsynch();

    // Mutex_Lock_Test_1();

    // LockGuard_Test_1();
    // LockGuard_Test_2();


    // UniqueLock::Transfer_Ownership();
    // UniqueLock::UniqueLock_OwnsLock();
    // UniqueLock::UniqueLock_Lock();
    // UniqueLock::UniqueLock_UnLock();
    // UniqueLock::UniqueLock_UnLock_2();
    // UniqueLock::UniqueLock_Release();


    // SharedMutext::Test_NoShare();
    // SharedMutext::Synchronized_Read_Write_Test();
    SharedMutext::Read_Write_Test_Blocking();


    // TimedMutex::TryLockFor();
    // TimedMutex::TryLockFor_1();
    // TimedMutex::TryLockUntil();
    // TimedMutex::TryLockUntil_1();
    // TimedMutex::TryLockUntil_1();
    // TimedMutex::LimitTime_Using_UniqueLock();




    // SharedTimedMutext::Test();
    // SharedTimedMutext::Read_Write_Test();
    // SharedTimedMutext::Try_Lock_Until();
    // SharedTimedMutext::Try_Lock_For();


    // ScopedLock::Complex_Test();
    // ScopedLock::Simple_Test();
    // ScopedLock::Good_Example();


    // RecursiveMutex::main();
};