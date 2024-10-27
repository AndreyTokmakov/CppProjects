/**============================================================================
Name        : ThreadPools.cpp
Created on  : 22.10.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ThreadPools src class.cpp
============================================================================**/

#include <iostream>
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds
#include <functional>
#include <thread>
#include <string>
#include <vector>
#include <deque>
#include <syncstream>
#include "ThreadPools.h"


#define LOG std::osyncstream(std::cout) << now()
#define ERR std::osyncstream(std::cerr) << now()

namespace
{
    using namespace std::chrono;


    constexpr std::string_view FORMAT { "[%d-%02d-%02d %02d:%02d:%02d.%06ld] " };

    [[nodiscard]]
    std::string now(const time_point<system_clock>& timestamp = system_clock::now()) noexcept
    {
        const time_t time { system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(&(buffer.front()), FORMAT.data(),
                                          tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                                          duration_cast<microseconds>(timestamp - time_point_cast<seconds>(timestamp)).count());
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
    }
}

template<typename T>
struct ThreadPool;

template<typename ReturnType, typename ... Args>
struct ThreadPool<ReturnType (Args...)>
{
    using Task = std::function<ReturnType (Args...)>;
    using PackagedTask = std::packaged_task<ReturnType (Args...)>;
    using Future = std::future<ReturnType>;


    mutable std::mutex mutex;
    std::deque<PackagedTask> queue;   // TODO: rename ??
    std::condition_variable updated;  // TODO: rename ??
    std::vector<std::jthread> workers {};
    std::stop_source source;

    /** Maximum number of workers: **/
    static inline const size_t threadsCount { std::thread::hardware_concurrency() };

    static inline const duration<int64_t, std::ratio<1, 1000>> pollTimeout = milliseconds(500);

private:

    template<class Rep, class Period>
    bool wait_for_and_pop(PackagedTask & task,
                          const duration<Rep, Period> &timeout) noexcept
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (!updated.wait_for(lock, timeout, [this] { return !queue.empty();}))
            return false;

        task = std::move(queue.front());
        queue.pop_front();
        return true;
    }

    // TODO: Try-catch ???
    void executor(const std::stop_source& source)
    {
        PackagedTask task; // FIXME: Use alias | template
        while (!source.stop_requested())
        {
            if (auto result = wait_for_and_pop(task, pollTimeout); result) {
                task(3);
            }
        }
    }

public:

    ThreadPool()
    {
        workers.reserve(threadsCount);
        for (size_t i = 0; i < threadsCount; ++i) {
            workers.emplace_back(&ThreadPool::executor, this, source);
        }
    }

    [[nodiscard("Its not for free")]]
    bool empty() const noexcept
    {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

    [[nodiscard("Its not for free")]]
    size_t size() const noexcept
    {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }

    Future submit(Task&& task) noexcept
    {
        PackagedTask packagedTask(task);
        Future futureResult = packagedTask.get_future();
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push_back(std::move(packagedTask));
        }
        updated.notify_all(); // TODO:  one / all ?
        return futureResult;
    }

#if 0
    // TODO: Make it work!
    template<typename... Args>
    void emplace(Args &&... args) noexcept
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.emplace_backd(std::forward<Args>(args)...);
        }
        updated.notify_all(); // TODO:  one / all ?
    }
#endif

};


// TODO: FIXME -- User RValue ref ????
template<typename ... Args>
struct Params
{
    std::tuple<Args...> tup;

    template<typename ... Types>
    explicit Params(Types ... params)
        : tup { std::make_tuple<Args...>( std::forward<Args>(params) ...) }
    {

        std::cout << std::get<0>(tup) << std::endl;
        std::cout << std::get<1>(tup) << std::endl;
        std::cout << std::get<2>(tup) << std::endl;
    }
};


void ThreadPools::TestAll()
{
    /*
    using RetType = int;

    auto func = [](int timeout) -> RetType {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        LOG << "Starting job" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        LOG << "Job  done\n";
        //return std::string("Task completed");
        return 12345;
    };

    ThreadPool<int(int)> pool;
    std::vector<std::future<RetType>> results;
    for (int i = 0; i < 5; i++)
    {
        results.push_back(pool.submit(func));
    }

    LOG << "Waiting" << std::endl;
    for ( auto& F: results)
    {
        auto result = F.get();
        LOG<< result << std::endl;
    }
    LOG << "Jobs completed" << std::endl;

    const bool done = pool.source.request_stop();
    LOG << "Done: " << std::boolalpha << done << std::endl;
    */

/*
    std::tuple params = std::make_tuple(1,'2', std::string{"3"});
    std::cout << std::get<0>(params) << std::endl;
    std::cout << std::get<1>(params) << std::endl;
    std::cout << std::get<2>(params) << std::endl;
*/

    Params<int, char, std::string> params {123, 'X', std::string{"qwerty"}};



};