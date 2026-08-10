/**============================================================================
Name        : PriorityDispatcher.cpp
Created on  : 10.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PriorityDispatcher.cpp
============================================================================**/

#include "Queues.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <queue>
#include <string_view>
#include <utility>
#include <vector>

/**
 * @file PriorityDispatcher.cpp
 *
 * @brief Demonstrates a priority-based task dispatcher.
 *
 * This example demonstrates a Priority Dispatcher architecture where tasks
 * are submitted to a central dispatcher and executed according to their
 * priority.
 *
 * The implementation separates three responsibilities:
 *
 *   - ITask
 *       Defines the common interface for executable tasks. A task exposes
 *       its priority and a monotonically increasing sequence number.
 *
 *   - PriorityQueue
 *       Owns pending tasks and determines which task should be executed next.
 *       Tasks with a higher priority are processed first. Tasks with the same
 *       priority are processed in FIFO order using their sequence number.
 *
 *   - PriorityDispatcher
 *       Provides the public interface for submitting tasks and dispatches
 *       pending tasks by repeatedly taking the highest-priority task from
 *       PriorityQueue and executing it.
 *
 * The priority ordering is:
 *
 *     Critical > High > Normal > Low
 *
 * When two tasks have the same priority, their sequence numbers determine
 * their order:
 *
 *     sequence 0 -> sequence 1 -> sequence 2 -> ...
 *
 * The implementation intentionally does not use std::function. Tasks are
 * represented by polymorphic objects implementing ITask, which allows the
 * dispatcher to work with different task types through a common interface.
 *
 * PriorityQueue is implemented on top of std::vector and the standard heap
 * algorithms std::push_heap() and std::pop_heap().
 *
 * This is preferable to using std::priority_queue<std::unique_ptr<ITask>>
 * directly when pop() must transfer ownership of the top element. The
 * std::priority_queue::top() function returns const T&, which prevents
 * moving a std::unique_ptr out of the top element.
 *
 * Using std::vector together with std::pop_heap() moves the highest-priority
 * element to the end of the container. The element can then be safely moved
 * out of the vector before it is removed.
 *
 * The resulting architecture is:
 *
 *     PriorityDispatcher
 *             |
 *             | submit()
 *             v
 *       PriorityQueue
 *             |
 *             | pop()
 *             v
 *           ITask
 *             |
 *             | execute()
 *             v
 *       ConcreteTask
 *
 * This example demonstrates the core mechanism only. A production dispatcher
 * could additionally provide worker threads, bounded queues, cancellation,
 * deadlines, starvation prevention, task aging, or multiple priority levels.
 */

namespace
{
    enum class Priority : std::uint8_t
    {
        Low = 0,
        Normal = 1,
        High = 2,
        Critical = 3
    };

    struct  ITask
    {
        virtual ~ITask() = default;

        virtual void execute() = 0;

        [[nodiscard]]
        virtual Priority getPriority() const noexcept = 0;

        [[nodiscard]]
        virtual std::size_t getSequence() const noexcept = 0;
    };

    class TaskBase : public ITask
    {
    public:

        TaskBase(const Priority priority,
                 const std::size_t sequence) noexcept:
            priority { priority }, sequence { sequence } {
        }

        [[nodiscard]]
        Priority getPriority() const noexcept override {
            return priority;
        }

        [[nodiscard]]
        std::size_t getSequence() const noexcept override {
            return sequence;
        }

    private:

        Priority priority;
        std::size_t sequence;
    };

    class PriorityQueue
    {
        struct Comparator
        {
            [[nodiscard]]
            bool operator()(const std::unique_ptr<ITask>& left,
                            const std::unique_ptr<ITask>& right) const noexcept
            {
                if (left->getPriority() != right->getPriority()) {
                    return left->getPriority() < right->getPriority();
                }

                return left->getSequence() > right->getSequence();
            }
        };

    public:

        void push(std::unique_ptr<ITask> task)
        {
            tasks.push_back(std::move(task));
            std::push_heap(tasks.begin(), tasks.end(), comparator);
        }

        [[nodiscard]]
        std::unique_ptr<ITask> pop()
        {
            std::pop_heap(tasks.begin(), tasks.end(), comparator);
            std::unique_ptr<ITask> task = std::move(tasks.back());
            tasks.pop_back();
            return task;
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return tasks.empty();
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return tasks.size();
        }

    private:

        std::vector<std::unique_ptr<ITask>> tasks;
        Comparator comparator;
    };

    class PriorityDispatcher
    {
    public:

        template<typename TaskType, typename... Args>
        void submit(const Priority priority, Args&&... args)
        {
            const std::size_t sequence = nextSequence++;
            queue.push(std::make_unique<TaskType>(priority, sequence, std::forward<Args>(args)...));
        }

        void dispatch()
        {
            while (!queue.empty())
            {
                const std::unique_ptr<ITask> task = queue.pop();
                task->execute();
            }
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return queue.empty();
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return queue.size();
        }

    private:

        PriorityQueue queue;
        std::size_t nextSequence { 0 };
    };
}

namespace
{
    class PrintTask final : public TaskBase
    {
    public:

        PrintTask(const Priority priority,
                  const std::size_t sequence,
                  const std::string_view message) noexcept:
            TaskBase { priority, sequence }, message { message } {
        }

        void execute() override{
            std::cout << "PrintTask: " << message << '\n';
        }

    private:

        std::string_view message;
    };

    class CalculationTask final : public TaskBase
    {
    public:

        CalculationTask(
            const Priority priority,
            const std::size_t sequence,
            const int value) noexcept
            : TaskBase { priority, sequence }, value { value } {
        }

        void execute() override
        {
            std::cout << "CalculationTask: "
                      << value
                      << " * "
                      << value
                      << " = "
                      << value * value
                      << '\n';
        }

    private:

        int value;
    };
}

void queues::priority_dispatcher::TestAll()
{
    PriorityDispatcher dispatcher;

    dispatcher.submit<PrintTask>(
        Priority::Low,
        "Low priority task");

    dispatcher.submit<PrintTask>(
        Priority::Critical,
        "Critical priority task");

    dispatcher.submit<CalculationTask>(
        Priority::Normal,
        10);

    dispatcher.submit<PrintTask>(
        Priority::High,
        "High priority task");

    dispatcher.submit<PrintTask>(
        Priority::Critical,
        "Another critical task");

    dispatcher.submit<CalculationTask>(
        Priority::Low,
        20);

    dispatcher.dispatch();
}
