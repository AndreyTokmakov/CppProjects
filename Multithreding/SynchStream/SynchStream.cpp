//
// Created by andtokm on 23.08.22.
//

#include <iostream>
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <deque>
#include <list>
#include <iomanip>

#include "SynchStream.h"

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

namespace SynchStream::Tests
{
    void Test1()
    {
        std::atomic<int> counter = 0;
        std::vector<std::future<void>> task;
        for (int i = 0; i < 5; ++i){
            task.emplace_back(std::async([&] {
                synch_stream() << "Counter value = " << ++counter << std::endl;
            }));
        }

        std::for_each(task.begin(), task.end(), [] (const auto& T){
            T.wait();
        });
    }
}

void SynchStream::TEST_ALL() {

    // synch_stream() << "Test" << std::endl;

    Tests::Test1();
};