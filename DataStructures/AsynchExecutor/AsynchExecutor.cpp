/**============================================================================
Name        : AsynchExecutor.cpp
Created on  : 14.11.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AsynchExecutor.cpp
============================================================================**/

#include <iostream>

#include <syncstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <functional>


#include "AsynchExecutor.hpp"

namespace
{
    using namespace std::chrono;
    using namespace std::chrono_literals;

    constexpr std::string_view formatMSeconds { "%d-%02d-%02d %02d:%02d:%02d.%06ld" };

    [[nodiscard]]
    std::string getCurrentTime(const time_point<system_clock>& timestamp = system_clock::now())
    {
        const time_t time { system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(buffer.data(), formatMSeconds.data(),
                                          tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                                          duration_cast<microseconds>(timestamp - time_point_cast<seconds>(timestamp)).count()
        );
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
    }

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "
#define ERR  std::osyncstream { std::cerr } << getCurrentTime() << " "
}

namespace rest_client_asynch_executor_demo
{
    struct Order
    {
        std::uint64_t id { 0 };
        std::string symbol;
        double price { 0.0 };
        double quantity { 0.0 };
    };

    struct OrderResult
    {
        std::uint64_t orderId { 0 };
        bool success { false };
        std::string message;
    };

    class RestClient
    {
    public:
        explicit RestClient(const std::size_t threadCount)
        {
            if (threadCount == 0) {
                throw std::invalid_argument("Thread count must be greater than zero");
            }
            try {
                for (std::size_t i = 0; i < threadCount; ++i) {
                    workers.emplace_back(&RestClient::workerLoop, this, stopSource);
                }
            }
            catch (...) {
                const auto _ = stopSource.request_stop();
                condition.notify_all();
                for (auto& worker : workers) {
                    worker.join();
                }
                throw;
            }
        }

        ~RestClient()
        {
            shutdown();
        }

        RestClient(const RestClient&) = delete;
        RestClient& operator=(const RestClient&) = delete;

        RestClient(RestClient&&) = delete;
        RestClient& operator=(RestClient&&) = delete;

        template<typename Callback>
        void sendOrder(Order order, Callback&& callback)
        {
            auto task = [order = std::move(order), callback = std::forward<Callback>(callback)]() mutable
            {
                // Simulate network request.
                std::this_thread::sleep_for(500ms);

                OrderResult result {
                    .orderId = order.id,
                    .success = true,
                    .message = "Order accepted"
                };

                callback(result);
            };

            enqueue(std::move(task));
        }

        void shutdown()
        {
            const auto _ = stopSource.request_stop();
            condition.notify_all();
            for (auto& worker : workers)
            {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

    private:
        void enqueue(std::function<void()> task)
        {
            {
                std::lock_guard lock(mutex);
                if (stopSource.stop_requested()) {
                    throw std::runtime_error("RestClient is shutting down");
                }
                tasks.push(std::move(task));
            }

            condition.notify_one();
        }

        void workerLoop(const std::stop_source& source)
        {
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock lock(mutex);
                    condition.wait(lock, [this, &source]() {
                        return source.stop_requested() || !tasks.empty();
                    });

                    if (source.stop_requested()) {
                        return;
                    }

                    task = std::move(tasks.front());
                    tasks.pop();
                }

                try {
                    task();
                } catch (const std::exception& e) {
                    ERR << "Worker exception: " << e.what() << '\n';
                } catch (...) {
                    ERR << "Worker exception: unknown exception\n";
                }
            }
        }

    private:

        std::vector<std::jthread> workers;
        std::stop_source stopSource;
        std::queue<std::function<void()>> tasks;
        std::mutex mutex;
        std::condition_variable condition;
    };

    class Manager
    {
    public:
        explicit Manager(RestClient& restClient) noexcept: restClient(restClient)
        {
        }

        void submitOrder(Order order)
        {
            restClient.sendOrder(std::move(order), [this](const OrderResult& result)
            {
                onOrderCompleted(result);
            });
        }

    private:

        void onOrderCompleted(const OrderResult& result)
        {
            if (result.success){
                LOG << "Order #" << result.orderId<< " completed: " << result.message << '\n';
            } else {
                LOG << "Order #" << result.orderId << " failed: " << result.message << '\n';
            }
        }

    private:
        RestClient& restClient;
    };

    void runDemo()
    {
        RestClient restClient { 1 };
        Manager manager(restClient);

        manager.submitOrder({
            .id = 1001,
            .symbol = "BTCUSDT",
            .price = 67500.0,
            .quantity = 0.15
        });
        LOG << "Order submitted\n";

        manager.submitOrder({
            .id = 1002,
            .symbol = "ETHUSDT",
            .price = 3500.0,
            .quantity = 2.0
        });
        LOG << "Order submitted\n";

        LOG << "Continue operations....\n";
        std::this_thread::sleep_for(3s);
    }
}


void asynch_executor::testAll()
{
    rest_client_asynch_executor_demo::runDemo();
}
