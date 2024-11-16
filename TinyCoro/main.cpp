/**============================================================================
Name        : main.cpp
Created on  : 29.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TinyCoro library tests
============================================================================**/

#include <string_view>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <iomanip>
#include <fstream>
#include <charconv>
#include <optional>
#include <algorithm>
#include <numeric>
#include <format>
#include <print>

#include "Common.h"
#include <tinycoro/tinycoro_all.h>


namespace Examples
{
    void Sleep(auto& scheduler)
    {
        auto sleep = [](auto duration) -> tinycoro::Task<int32_t> {
            for (auto start = std::chrono::system_clock::now(); std::chrono::system_clock::now() - start < duration;)
            {
                co_await std::suspend_always{};
            }
            co_return 42;
        };

        auto task = [&sleep]() -> tinycoro::Task<int32_t> {
            auto val = co_await sleep(1s);
            co_return val;

            // co_return co_await sleep(1s);     // or write short like this
        };

        auto future = scheduler.Enqueue(task());
        SyncOut() << "co_return => " << future.get() << '\n';
    }

    void MultiTasks(auto& scheduler)
    {
        auto task = []() -> tinycoro::Task<void> {
            SyncOut() << "  Coro starting..." << "  Thread id : " << std::this_thread::get_id() << '\n';
            co_return;
        };

        tinycoro::GetAll(scheduler, task(), task(), task());
        SyncOut() << "GetAll co_return => void" << '\n';
    }
}



int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    tinycoro::Scheduler scheduler {std::thread::hardware_concurrency()};

    // Examples::Sleep(scheduler);
    Examples::MultiTasks(scheduler);


    return EXIT_SUCCESS;
}
