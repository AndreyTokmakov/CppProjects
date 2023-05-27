//============================================================================
// Name        : Latch.cpp
// Created on  : 02.12.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Latch src class
//============================================================================

#include "Latch.h"

#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include <latch>
#include <future>
#include <thread>
#include <chrono>
#include <iomanip>

#include "../ThreadHelperUtilities/ThreadHelperUtilities.h"


namespace Latch::Utils
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

namespace Latch {

    void Wait_Test()
    {
        int max_workers = 4;
        std::latch completion_latch(max_workers);

        std::vector<std::jthread> workers;
        auto task = [&](unsigned int timeout)-> void {
            Utils::synch_stream{} << "Waiting for " << timeout << " seconds.\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            completion_latch.count_down();
        };

        Utils::synch_stream{} << "Starting threads....\n";
        while (max_workers--)
            workers.emplace_back(task, rand() % 8);


        Utils::synch_stream{} << "Block with latch.wait() until work is done.\n";
        completion_latch.wait();
        Utils::synch_stream{} << "\n ****** Latch.wait() done. ***** \n\n";
    }

    void TryWait_Test()
    {
        int max_workers = 4;
        std::latch completion_latch(max_workers);

        std::vector<std::jthread> workers;
        auto task = [&](unsigned int timeout)-> void {
            Utils::synch_stream{} << "Waiting for " << timeout << " seconds.\n";
            std::this_thread::sleep_for(std::chrono::seconds(timeout));
            Utils::synch_stream{} << "Thread done\n";
            completion_latch.count_down();

        };

        Utils::synch_stream{}<< "Starting threads....\n";
        while (max_workers--)
            workers.emplace_back(task, rand() % 8);


        Utils::synch_stream{} << "Block with latch.wait() until work is done.\n";

        while (!completion_latch.try_wait()) {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            // std::cout << ".";
        }
        Utils::synch_stream{} << "Done\n";
    }
};

void Latch::TEST_ALL()
{
    // Wait_Test();
    TryWait_Test();
}
