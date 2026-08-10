/**============================================================================
Name        : MultiLevelQueue.cpp
Created on  : 10.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MultiLevelQueue.cpp
============================================================================**/

#include "Queues.hpp"

#include <algorithm>
#include <iostream>
#include <array>
#include <cstddef>
#include <memory>
#include <queue>
#include <utility>

namespace
{
    enum class Priority : std::size_t
    {
        Critical = 0,
        High,
        Normal,
        Low,
        Count
    };

    struct ITask
    {
        virtual ~ITask() = default;
        virtual void execute() = 0;
    };

    class MultiLevelQueue
    {
        using Task = std::unique_ptr<ITask>;
        using Queue = std::queue<Task>;

        static constexpr std::size_t PriorityCount = static_cast<std::size_t>(Priority::Count);

    public:

        void push(const Priority priority, Task task) {
            queues[getIndex(priority)].push(std::move(task));
        }

        [[nodiscard]]
        Task pop()
        {
            Queue& queue = getNextQueue();
            Task task = std::move(queue.front());
            queue.pop();
            return task;
        }

        [[nodiscard]]
        bool empty() const noexcept
        {
            return not std::ranges::any_of(queues, [](const auto& queue) {
                return not queue.empty();
            });
        }

        [[nodiscard]]
        std::size_t size() const noexcept
        {
            std::size_t result { 0 };
            for (const Queue& queue : queues) {
                result += queue.size();
            }

            return result;
        }

    private:

        [[nodiscard]]
        static constexpr std::size_t getIndex(const Priority priority) noexcept {
            return static_cast<std::size_t>(priority);
        }

        [[nodiscard]]
        Queue& getNextQueue() noexcept
        {
            for (std::size_t index = 0; index < PriorityCount; ++index)
            {
                Queue& queue = queues[index];
                if (!queue.empty()) {
                    return queue;
                }
            }

            return queues[0];
        }

    private:

        std::array<Queue, PriorityCount> queues;
    };

    class PrintTask final : public ITask
    {
    public:

        explicit PrintTask(const char* message) noexcept: message { message } {
        }

        void execute() override{
            std::cout << message << '\n';
        }

    private:

        const char* message;
    };
}



void queues::multi_level_queue::TestAll()
{
    MultiLevelQueue queue;

    queue.push(Priority::Low,std::make_unique<PrintTask>("Low #1"));
    queue.push(Priority::Normal,std::make_unique<PrintTask>("Normal #1"));
    queue.push(Priority::High,std::make_unique<PrintTask>("High #1"));
    queue.push(Priority::Critical,std::make_unique<PrintTask>("Critical #1"));
    queue.push(Priority::Low,std::make_unique<PrintTask>("Low #2"));
    queue.push(Priority::Normal,std::make_unique<PrintTask>("Normal #2"));
    queue.push(Priority::Critical,std::make_unique<PrintTask>("Critical #2"));
    queue.push(Priority::High,std::make_unique<PrintTask>("High #2"));

    while (!queue.empty())
    {
        const std::unique_ptr<ITask> task = queue.pop();
        task->execute();
    }
}

/**
Critical #1
Critical #2
High #1
High #2
Normal #1
Normal #2
Low #1
Low #2 **/
