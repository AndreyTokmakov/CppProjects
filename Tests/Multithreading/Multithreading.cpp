/**============================================================================
Name        : Multithreading.cpp
Created on  : 30.10.22.
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Multithreading
//============================================================================**/

#include "Multithreading.h"
#include "Queue.h"


#include <iostream>
#include <stop_token>
#include <future>
#include <thread>
#include <vector>
#include <list>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <atomic>
#include <deque>
#include <concepts>
#include <functional>
#include <syncstream>
#include <random>

#include "../Helpers/Helpers.h"

using namespace std::literals; // for duration literals

namespace PairUtils = UnorderedPair::Utilities;

namespace SynchStream
{
    struct synch_stream final : public std::stringstream {
    private:
        static inline std::mutex mtx;
        const static inline std::thread::id mainThreadId { std::this_thread::get_id() };
        constexpr static inline std::string_view FORMAT { "%Y-%m-%d %X" };

    private:

        [[nodiscard]]
        static std::string getCurrentTime() noexcept {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), FORMAT.data());
            return ss.str();
        }

    public:
        ~synch_stream() override {
            const auto currId = std::this_thread::get_id();

            // TODO: Prepare string before lock
            std::string info("[");
            info.append(getCurrentTime()).append("] Thread [");

            std::lock_guard<std::mutex> lock{mtx};

            std::cout << info;
            if (mainThreadId == currId)
                std::cout << std::setiosflags(std::ios::left) << std::setw(9) << "Main";
            else
                std::cout << "Id: " << std::setiosflags(std::ios::left) << std::setw(5) << currId;

            std::cout << "] " << rdbuf();
            std::cout.flush();
        }
    };
}

namespace Multithreading
{
    void func(std::stop_token st, int num)
    {
        const auto id = std::this_thread::get_id();

        std::cout << "call func(" << num << ")\n";

        std::stop_callback cb1 {st, [num, id]{
            std::cout << "- STOP1 requested in func(" << num
                      << (id == std::this_thread::get_id() ? ")\n" : ") in main thread\n");
        }};

        std::this_thread::sleep_for(9ms);

        // register a second callback:
        std::stop_callback cb2 {st, [num, id] {
            std::cout << "- STOP2 requested in func(" << num
                      << (id == std::this_thread::get_id() ? ")\n" : ") in main thread\n");
        }};

        std::this_thread::sleep_for(2ms);
    }
}

namespace Multithreading::Experiments
{

    const std::thread::id mainThreadId = std::this_thread::get_id();
    std::mutex mtxPrint;

    template<typename ... _Types>
    void Debug(_Types&& ... params) {
        auto add_space = []<typename T>(const T& arg) {
            std::cout << ' ';
            return arg;
        };

        const auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");

        std::lock_guard<std::mutex> lock {mtxPrint};
        const auto threadID = std::this_thread::get_id();

        std::cout  << "[" << ss.str() << "] Thread [";
        if (mainThreadId == threadID)
            std::cout  << std::setiosflags(std::ios::left) << std::setw(9) << "Main";
        else
            std::cout  << "Id: " << std::setiosflags(std::ios::left) << std::setw(5) << threadID;
        std::cout  << "] ";
        (std::cout << ... << add_space(std::forward<_Types>(params))) << std::endl;
    }




    void CalcTeethContactPoints()
    {
        std::mutex mtx;
        std::unordered_map<PairUtils::UnorderedPair<int>,
                           std::array<double, 3>,
                           PairUtils::PairHashUnordered<int>> contactPoints;

        constexpr std::array<int, 14> lowerIDs { 47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37 };
        constexpr std::array<int, 14> upperIDs { 17, 16, 15, 14, 13, 12, 11, 21, 22, 23, 24, 25, 26, 27 };


        constexpr int bucketSize = 4; // NOTE: Threads count
        std::vector<std::vector<std::pair<int, int>>> toothPairs(bucketSize);
        for (int bucket = 0; const auto& ids: { lowerIDs, upperIDs }) {
            for (size_t size = ids.size(), i = 1; i < size; ++i) {
                toothPairs[bucket++].emplace_back(ids[i - 1], ids[i]);
                bucket = (bucketSize == bucket) ? 0 : bucket;
            }
        }

        auto handler = [&] (const std::vector<std::pair<int, int>>& bucket) {
            for (const auto &[id1, id2]: bucket) {
                {
                    std::lock_guard<std::mutex> lock {mtx};
                    // TODO: Calculate contact point
                    std::array<double,3> pt {1, 2, 3};
                    contactPoints.emplace(PairUtils::UnorderedPair<int>{id1, id2}, pt);
                    std::cout << id1 << ", " << id2 << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(250u));
            }
        };

        std::vector<std::future<void>> workers;
        for (const auto& bucket: toothPairs)
            workers.emplace_back(std::async(handler, bucket));

        std::for_each(workers.cbegin(), workers.cend(), [](const auto& task) {
            task.wait();
        });
    }

    // ----------------------------------------------------------------------------

    template <class T, size_t N>
    class LockFreeQueue
    {
        std::array<T, N> buffer{};  // Used by both threads
        std::atomic<size_t> size {0}; // Used by both threads
        size_t read_pos { 0 };
        size_t write_pos { 0 };

    public:
        LockFreeQueue() : size {0}, read_pos{0}, write_pos{0} {
            // assert(size_.is_lock_free());
            if (!size.is_lock_free()) {
                std::cout << "ERROR!\n";
            }
        }

        [[nodiscard]]
        size_t Size() const {
            return size.load();
        }

        [[nodiscard]]
        T& front() {
            const size_t s = size.load();
            if (s == 0) {
                throw std::underflow_error("Queue is empty");
            }
            return buffer[read_pos];
        }

        void push(const T& t) {
            if (size.load() >= N) {
                throw std::overflow_error("Queue is full");
            }
            buffer[write_pos] = t;
            write_pos = (write_pos + 1) % N;
            size.fetch_add(1);
        }

        void pop() {
            if (size.load() == 0) {
                throw std::underflow_error("Queue is empty");
            }
            read_pos = (read_pos + 1) % N;
            size.fetch_sub(1);
        }
    };

    void LockFreeTest() {

        LockFreeQueue<int, 5> queue {};

        for (int i = 0; i < 10; ++i) {
            std::cout << "Pushing " << i << std::endl;
            queue.push(i);
            std::cout << "front " << queue.front() << ", Size = " << queue.Size() << std::endl;
        }

        std::cout << queue.Size() << std::endl;
    }
}

namespace Multithreading::SwitchingThreads
{
    using namespace SynchStream;

    void Test()
    {
        std::mutex mtx1, mtx2;
        size_t counter = 0;

        const auto func1 = [&](uint32_t timeout) -> void {
            for (size_t i = 0, v = 0; i < 5; ++i)
            {
                mtx1.lock();

                v = ++counter;
                std::this_thread::sleep_for(std::chrono::microseconds (timeout));

                // synch_stream()  << v << std::endl;
                std::cout << "T1: " <<  v << std::endl;
                mtx2.unlock();
            }
        };

        const auto func2 = [&](uint32_t timeout) -> void {
            for (size_t i = 0, v = 0; i < 5; ++i)
            {
                mtx2.lock();

                v = ++counter;
                std::this_thread::sleep_for(std::chrono::microseconds (timeout));

                // synch_stream()  << v << std::endl;
                std::cout << "T2: " <<  v << std::endl;



                mtx1.unlock();
            }
        };


        mtx2.lock();
        std::thread T1(func1, 1), T2(func2, 1);

        T1.join();
        T2.join();
    }
}


// https://leetcode.com/problems/print-in-order/submissions/
/**

public class Foo {
    public void first() { print("first"); }
    public void second() { print("second"); }
    public void third() { print("third"); }
}

The same instance of Foo will be passed to three different threads.
Thread A will call first(), thread B will call second(), and thread C will call third().
Design a mechanism and modify the program to ensure that second() is executed
after first(), and third() is executed after second().
 */
namespace Multithreading::SwitchingThreads_SpinLock
{
    struct Worker
    {
        std::atomic<uint32_t> turnSwitch;

        explicit Worker(uint32_t turn = 1): turnSwitch {turn} {
        }

        void spinLock(uint32_t order) {
            while (turnSwitch.load(std::memory_order_acquire) != order) {
            }
        }

        void first()
        {
            std::cout << "First" << std::endl;
            turnSwitch.store(2, std::memory_order_release); // turnSwitch.store(2) or turnSwitch = 2
        }

        void second()
        {
            spinLock(2);
            std::cout << "Second" << std::endl;
            turnSwitch.store(3, std::memory_order_release);
        }

        void third()
        {
            spinLock(3);
            std::cout << "Third" << std::endl;
        }
    };


    int32_t getRandomInt(int32_t from = 0, int32_t until = 100)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distribution(from, until);
        return distribution(gen);
    }

    void singleThreadTest()
    {
        Worker worker;

        worker.first();
        worker.second();
        worker.third();
    }

    void multiThreadTest()
    {
        Worker worker;

        std::vector<std::future<void>> jobs;
        for (size_t idx = 1; idx <= 3; ++idx) {
            jobs.emplace_back(std::async(std::launch::async, [&worker, idx]()
            {
                const uint32_t sleepTime = getRandomInt(0, 5);
                std::this_thread::sleep_for(std::chrono::seconds(sleepTime));

                switch (idx) {
                    case 1: worker.first(); break;
                    case 2: worker.second(); break;
                    case 3: worker.third(); break;
                }
            }));
        }

        for (const auto& T: jobs)
            T.wait();
    }

    void TestAll()
    {

        // singleThreadTest();
        multiThreadTest();


        /*
        for (int i = 0; i < 10; ++i)
            std::cout << getRandomInt(0, 5) << std::endl;
        */
    }
}

namespace Multithreading::LockFree
{
    template<typename T>
    class lock_free_stack
    {
    private:
        struct node {
            std::shared_ptr<T> data;
            std::shared_ptr<node> next;
            explicit node(T const& data_): data(std::make_shared<T>(data_)) { }
        };

        std::shared_ptr<node> head;

    public:
        void push(T const& data)
        {
            std::shared_ptr<node> const new_node = std::make_shared<node>(data);
            new_node->next = std::atomic_load(&head);
            while (!std::atomic_compare_exchange_weak(&head, &new_node->next,new_node)) {
                /** **/
            };
        }

        std::shared_ptr<T> pop()
        {
            std::shared_ptr<node> old_head = std::atomic_load(&head);
            while (old_head && !std::atomic_compare_exchange_weak(&head,&old_head,std::atomic_load(&old_head->next))) {
                /** **/
            };
            if (old_head) {
                std::atomic_store(&old_head->next,std::shared_ptr<node>());
                return old_head->data;
            }
            return std::shared_ptr<T>();
        }

        ~lock_free_stack(){
            while(pop());
        }
    };


    void testStack() {
        // lock_free_stack<int> stack {};
    }
}


namespace Multithreading::Pools
{
    template<typename Task = std::function<void()>>
    struct thread_pool
    {
        // TODO: Refactor?? use template like std::func
        using TaskType = Task;

        mutable std::mutex mutex;

        std::deque<TaskType> queue;
        // std::list<TaskType> queue;

        std::condition_variable updated;

        std::atomic_bool run { true };

        std::vector<std::jthread> workers {};

        /** Maximum number of request handler workers: **/
        // static inline const size_t THREADS_COUNT { std::thread::hardware_concurrency() };
        static inline const size_t THREADS_COUNT { 3 };

        /** Maximum number of request handler workers: **/
        static inline const size_t MAX_CAPACITY { 1 };

        static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> TIMEOUT =
                std::chrono::milliseconds(2000u);

        static inline const std::chrono::duration<int64_t, std::ratio<1, 1000>> POLL_TIMEOUT =
                std::chrono::milliseconds(250u);

    private:

        template<class Rep, class Period>
        bool wait_for_and_pop(TaskType &task,
                              const std::chrono::duration<Rep, Period> &timeout) noexcept {
            std::unique_lock<std::mutex> lock(mutex);
            if (!updated.wait_for(lock, timeout, [this] { return !queue.empty(); }))
                return false;
            task = std::move(queue.front());
            queue.pop_front();
            lock.unlock();
            updated.notify_all();
            return true;
        }

    private:

        void worker_thread()
        {
            TaskType task;
            while (run) {
                if (auto result = wait_for_and_pop(task, TIMEOUT); result) {
                    task();
                }
            }
        }

    public:
        thread_pool()
        {
            try {
                for (size_t i = 0; i < THREADS_COUNT; ++i) {
                    workers.emplace_back(&thread_pool::worker_thread, this);
                }
            } catch (...) {
                run = false;
                throw;
            }
        }

        ~thread_pool() {
            run = false;
        }

        [[nodiscard("Its not for free")]]
        bool empty() const noexcept {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.empty();
        }

        [[nodiscard("Its not for free")]]
        size_t size() const noexcept {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.size();
        }

        void submit(TaskType &&new_value) noexcept
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (MAX_CAPACITY >= queue.size()) {
                while (!updated.wait_for(lock, POLL_TIMEOUT, [this] { return MAX_CAPACITY > queue.size(); })) { /** **/ }
            }
            queue.push_back(std::move(new_value));
            lock.unlock();
            updated.notify_one();
        }

        // TODO: Make it work!
        template<typename... Args>
        void emplace(Args &&... args) noexcept
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                queue.emplace_back(std::forward<Args>(args)...);
            }
            updated.notify_one();
        }
    };

    void Test()
    {
        thread_pool pool;

        auto task = []() {
            std::osyncstream(std::cout) << "Starting job\n";
            std::this_thread::sleep_for(std::chrono::seconds(1u));
            std::osyncstream(std::cout) << "Job done\n";
        };

        std::vector<std::future<void>> workers;
        for (int i = 0; i < 25; ++i) {
            workers.emplace_back(std::async([&] { pool.submit(task); } ));
        }
        std::for_each(workers.cbegin(), workers.cend(), [](const auto &task) {
            task.wait();
        });
    }
};


namespace Multithreading::SimpleThreadSafeCollection
{
    template<typename F>
    concept FunctionPointer = std::is_member_function_pointer_v<F>;

    template<typename T>
    struct ListWrapper
    {
        using value_type = T;


        mutable std::mutex mtx;
        std::list<value_type> list;

        template<typename Method, typename... Args>
        void callMethodSynchronized(Method func, Args&&... params)
        {
            std::lock_guard<std::mutex> lock {mtx};
            std::osyncstream(std::cout) << "Size: " << list.size();

            std::invoke(func, this, std::forward<Args>(params)...);

            std::osyncstream(std::cout) << " --> " << list.size() << std::endl;
        }

        void __push_back(const value_type& v) {
            list.push_back(v);
        }

        void push_back(const value_type& v) {
            callMethodSynchronized(&ListWrapper<T>::__push_back, v);
        }

        void __push_front(const value_type& v) {
            list.push_front(v);
        }

        void push_front(const value_type& v) {
            callMethodSynchronized(&ListWrapper<T>::__push_front, v);
        }

        void __pop_back() {
            list.pop_back();
        }

        void pop_back(const value_type& v) {
            callMethodSynchronized(&ListWrapper<T>::__pop_back, v);
        }

        void __pop_front(){
            list.pop_front();
        }

        void pop_front(const value_type& v) {
            callMethodSynchronized(&ListWrapper<T>::__pop_front, v);
        }
    };

    void Test()
    {
        ListWrapper<int> wrapper;

        std::vector<std::future<void>> workers;
        for (int i = 0; i < 10; ++i) {
            workers.emplace_back(std::async([&] { wrapper.push_back(i); } ));
        }
        std::for_each(workers.cbegin(), workers.cend(), [](const auto &task) {
            task.wait();
        });

        std::cout << "Done\n";
        std::this_thread::sleep_for(std::chrono::seconds(5u));
    }
}

namespace FalseSharingExperiments
{
#define START_TIME_MEASURE auto start = std::chrono::high_resolution_clock::now();
#define STOP_TIME_MEASURE  { auto end = std::chrono::high_resolution_clock::now(); \
                           auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
						   std::cout << "Result: " << duration << " microseconds" << std::endl;}

    constexpr size_t ITER_COUNT { 10'000'000 };
    constexpr size_t COUNT { 12 };

    struct Stats {
        int a { 0 };
        int b { 0 };
        int c { 0 };
        int d { 0 };
    };

    struct StatsAligned {
        alignas(std::hardware_destructive_interference_size) int a { 0 };
        alignas(std::hardware_destructive_interference_size) int b { 0 };
        alignas(std::hardware_destructive_interference_size) int c { 0 };
        alignas(std::hardware_destructive_interference_size) int d { 0 };
    };


    template<typename T>
    void task(T& var)
    {
        for (size_t i = 0; i < ITER_COUNT; ++i) {
            for (size_t n = 0; n < ITER_COUNT; ++n) {
                ++var;
            }
        }
    }

    template<typename T>
    void test()
    {
        std::vector<T> store (COUNT);
        std::vector<std::thread> threads {};
        for (T& entry: store) {
            threads.emplace_back(task<int>, std::ref(entry.a));
            threads.emplace_back(task<int>, std::ref(entry.b));
            threads.emplace_back(task<int>, std::ref(entry.c));
            threads.emplace_back(task<int>, std::ref(entry.d));
        }

        START_TIME_MEASURE;
        std::for_each(threads.begin(), threads.end(), [](std::thread& job ) { job.join(); });
        STOP_TIME_MEASURE;
    }

    void Benchmark()
    {
        test<Stats>();
        test<StatsAligned>();
    }
};


namespace Future
{
    void WaitForFuture_RangeBasedLoop_Copy()
    {
        auto calculate = [](uint32_t timeout) {
            std::osyncstream(std::cout) << "Starting job\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            std::osyncstream(std::cout) << "Job done\n";
            return 10;
        };


        std::vector<std::future<int>> workers;
        workers.emplace_back(std::async(std::launch::async, calculate, 10));
        workers.emplace_back(std::async(std::launch::async, calculate, 5));

        int sum = 0;
        for (auto&   T: workers)
            sum += T.get();

        std::cout << sum << std::endl;
    }
}

namespace WaitFree
{
    template<typename T, size_t N>
    class WaitFreeQueue
    {
        T data[N];
        std::atomic<size_t> readSequence { 0 };
        std::atomic<size_t> writeSequence { 1 };

    public:

        bool tryWrite(T value)
        {
            const auto nextWriteIndex = writeSequence % N;
            const auto currentReadIndex = readSequence % N;
            const bool noRoomLeft = (nextWriteIndex == currentReadIndex);
            if (noRoomLeft) {
                return false;
            }
            data[nextWriteIndex] = std::move(value);
            writeSequence.store(nextWriteIndex + 1);
            return true;
        }

        T* tryRead()
        {
            const auto nextReadIndex = (readSequence + 1) % N;
            const auto nextWriteIndex = writeSequence % N;
            const bool noNewData = (nextReadIndex == nextWriteIndex);
            if (noNewData) {
                return nullptr;
            }
            readSequence.store(nextReadIndex);
            return & data[nextReadIndex];
        }
    };

    void Test()
    {

    }
}


namespace Thread_CPU_Affinity
{
    bool setThreadCore(const uint32_t coreId) noexcept
    {
        cpu_set_t cpuSet {};
        CPU_ZERO(&cpuSet);
        CPU_SET(coreId, &cpuSet);
        return 0 == pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuSet);
    }

    int32_t getGpu() noexcept
    {
        return sched_getcpu();
    }

    void thread_affinity_worker(int coreId)
    {
        const auto threadId { std::this_thread::get_id() };
        if (!setThreadCore(coreId))
        {
            std::cerr << "Failed to set core " << coreId << " for " << threadId << std::endl;
            return;
        }

        for(int i = 0; i < 4; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds (250u));
            std::osyncstream(std::cout) << threadId << " | " << i << ": on CPU " << getGpu() << "\n";
        }
    }

    void testThreadAffinity()
    {
        std::jthread thread1(thread_affinity_worker, 1);
        std::jthread thread2(thread_affinity_worker, 2);
    }
}





void Multithreading::TestAll()
{
    // Queue::TestAll();

    // WaitFree::Test();

    // SwitchingThreads::Test();

    // Experiments::CalcTeethContactPoints();
    // Experiments::Debug("33", 333);

    // Experiments::LockFreeTest();  <-------------- FIXME

    // Pools::Test();

    // SimpleThreadSafeCollection::Test();

    // SwitchingThreads_SpinLock::TestAll();

    // FalseSharingExperiments::Benchmark();

    // Future::WaitForFuture_RangeBasedLoop_Copy();

    Thread_CPU_Affinity::testThreadAffinity();
}