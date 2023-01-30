/**============================================================================
Name        : Multithreading.cpp
Created on  : 30.10.22.
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Multithreading
//============================================================================**/

#include "Multithreading.h"

#include <iostream>
#include <stop_token>
#include <future>
#include <thread>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <atomic>

#include "../Helpers/Utilities.h"

using namespace std::literals; // for duration literals


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
        std::unordered_map<Utilities::UnorderedPair<int>,
                std::array<double, 3>,
                Utilities::PairHashUnordered<int>> contactPoints;

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
                    contactPoints.emplace(Utilities::UnorderedPair<int>{id1, id2}, pt);
                    std::cout << id1 << ", " << id2 << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
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
    class LockFreeQueue {
    public:
        LockFreeQueue() : size_{0}, read_pos_{0}, write_pos_{0} {
            //assert(size_.is_lock_free());
            if (!size_.is_lock_free()) {
                std::cout << "ERROR!\n";
            }
        }

        auto size() const {
            return size_.load();
        }

        auto push(const T& t) {
            if (size_.load() >= N) {
                throw std::overflow_error("Queue is full");
            }
            buffer_[write_pos_] = t;
            write_pos_ = (write_pos_ + 1) % N;
            size_.fetch_add(1);
        }

        auto& front() const {
            const auto s = size_.load();
            if (s == 0) {
                throw std::underflow_error("Queue is empty");
            }
            return buffer_[read_pos_];
        }

        auto pop() {
            if (size_.load() == 0) {
                throw std::underflow_error("Queue is empty");
            }
            read_pos_ = (read_pos_ + 1) % N;
            size_.fetch_sub(1);
        }

    private:
        std::array<T, N> buffer_{}; // Used by both threads
        std::atomic<size_t> size_{}; // Used by both threads
        size_t read_pos_ = 0;
        size_t write_pos_ = 0;
    };

    void LockFreeTest() {

        LockFreeQueue<int, 5> queue {};
        queue.push(1);
        queue.push(2);

        std::cout << queue.size() << std::endl;
    }
}

namespace Multithreading::SwitchingThreads
{
    using namespace SynchStream;

    void Test()
    {
        std::mutex mtx1, mtx2;
        size_t counter = 0;

        const auto func1 = [&](int timeout) -> void {
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

        const auto func2 = [&](int timeout) -> void {
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
            while (!std::atomic_compare_exchange_weak(&head, &new_node->next,new_node)) { /** **/ };
        }

        std::shared_ptr<T> pop()
        {
            std::shared_ptr<node> old_head = std::atomic_load(&head);
            while (old_head && !std::atomic_compare_exchange_weak(&head,
                                                                  &old_head,std::atomic_load(&old_head->next))) { /** **/ };
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

void Multithreading::TestAll()
{

    /*
    // create stop_source and stop_token:
    std::stop_source ssrc;
    std::stop_token stok{ssrc.get_token()};

    // register callback:
    std::stop_callback cb{stok, []{
        std::cout << "- STOP requested in main()\n" << std::flush;
    }};

    // in the background call func() a couple of times:
    auto fut = std::async([stok] {
        for (int num = 1; num < 10; ++num) {
            func(stok, num);
        }
    });

    // after a while, request stop:
    std::this_thread::sleep_for(120ms);
    ssrc.request_stop();
    */

    SwitchingThreads::Test();


    // Experiments::CalcTeethContactPoints();
    // Experiments::Debug("33", 333);
}