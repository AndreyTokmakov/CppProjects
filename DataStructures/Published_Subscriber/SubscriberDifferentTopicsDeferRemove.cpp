/**============================================================================
Name        : SubscriberDifferentTopicsDeferRemove.cpp
Created on  : 08.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SubscriberDifferentTopicsDeferRemove.cpp
============================================================================**/

#include "PubSub.hpp"

#include <iostream>
#include <format>
#include <string_view>
#include <utility>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <map>
#include <memory>
#include <atomic>

namespace
{
    using EventTypeId = std::uint32_t;
    using SubscriptionId = std::size_t;

    struct TypeIdGenerator
    {
        template<typename>
        [[nodiscard]]
        static EventTypeId get() noexcept
        {
            static const EventTypeId typeId = generate();
            return typeId;
        }

    private:

        [[nodiscard]]
        static EventTypeId generate() noexcept
        {
            static std::atomic<EventTypeId> currentId { 0 };
            return currentId.fetch_add(1, std::memory_order_relaxed);
        }
    };

    template<typename T>
    [[nodiscard]]
    EventTypeId getTypeId() noexcept {
        return TypeIdGenerator::get<T>();
    }

    struct TaskCreated
    {
        int taskId;
        std::string_view name;
    };

    struct TaskCompleted
    {
        int taskId;
        std::size_t durationMs;
    };

    struct TaskFailed
    {
        int taskId;
        int errorCode;
    };

    struct TaskCancelled
    {
        int taskId;
        std::string_view reason;
    };

    struct IEventBus
    {
        virtual ~IEventBus() = default;

    protected:

        virtual void unsubscribe(EventTypeId typeId, SubscriptionId id) noexcept = 0;
        friend class Subscription;
    };

    class Subscription
    {
    public:

        Subscription() noexcept = default;

        Subscription(std::shared_ptr<IEventBus>&& eventBus,
                     const EventTypeId typeId,
                     const SubscriptionId id) noexcept:
            eventBus { eventBus },
            typeId { typeId },
            id { id } {
        }

        Subscription(const Subscription&) = delete;
        Subscription& operator=(const Subscription&) = delete;

        Subscription(Subscription&& other) noexcept:
            eventBus { std::move(other.eventBus) },
            typeId { other.typeId },
            id { other.id } {
        }

        Subscription& operator=(Subscription&& other) noexcept
        {
            reset();
            eventBus = std::move(other.eventBus);
            typeId = other.typeId;
            id = other.id;
            return *this;
        }

        ~Subscription() {
            reset();
        }

        void reset() noexcept
        {
            if (const auto bus = eventBus.lock(); bus) {
                bus->unsubscribe(typeId, id);
            }
            eventBus.reset();
        }

    private:

        std::weak_ptr<IEventBus> eventBus;
        EventTypeId typeId { 0 };
        SubscriptionId id { 0 };
    };

    struct ISubscription
    {
        virtual ~ISubscription() = default;

        [[nodiscard]]
        virtual SubscriptionId getId() const noexcept = 0;

        [[nodiscard]]
        virtual bool isActive() const noexcept = 0;

        virtual void deactivate() noexcept = 0;
        virtual void invoke(const void* event) const = 0;
    };

    using Topic = std::vector<std::unique_ptr<ISubscription>>;

    class EventBus final : public IEventBus,
                           public std::enable_shared_from_this<EventBus>
    {
        template<typename Event, typename Handler>
        class Subscriber final : public ISubscription
        {
        public:

            Subscriber(const SubscriptionId id, Handler& handler) noexcept:
                id { id }, handler { handler } {
            }

            [[nodiscard]]
            SubscriptionId getId() const noexcept override {
                return id;
            }

            [[nodiscard]]
            bool isActive() const noexcept override {
                return active;
            }

            void deactivate() noexcept override {
                active = false;
            }

            void invoke(const void* event) const override {
                handler.handle(*static_cast<const Event*>(event));
            }

        private:

            SubscriptionId id;
            Handler& handler;
            bool active { true };
        };

    public:

        template<typename Event, typename Handler>
        [[nodiscard]]
        Subscription subscribe(Handler& handler)
        {
            const EventTypeId typeId = getTypeId<Event>();
            const SubscriptionId id = nextId++;

            subscriptionsTable[typeId].push_back(std::make_unique<Subscriber<Event, Handler>>(id, handler));
            return Subscription { shared_from_this(), typeId, id };
        }

        template<typename Event>
        void publish(const Event& event)
        {
            const EventTypeId typeId = getTypeId<Event>();
            const auto iterator = subscriptionsTable.find(typeId);
            if (iterator == subscriptionsTable.end()) {
                return;
            }

            ++publishDepth;

            const Topic& topic = iterator->second;
            const std::size_t initialSize = topic.size();

            for (std::size_t index = 0; index < initialSize; ++index)
            {
                if (ISubscription& subscription = *topic[index]; subscription.isActive()) {
                    subscription.invoke(&event);
                }
            }

            --publishDepth;
            if (publishDepth == 0) {
                cleanup();
            }
        }

    protected:

        void unsubscribe(const EventTypeId typeId,
                         const SubscriptionId id) noexcept override
        {
            const auto iterator = subscriptionsTable.find(typeId);
            if (iterator == subscriptionsTable.end()) {
                return;
            }

            Topic& topic = iterator->second;
            const auto subscriber = std::ranges::find_if(topic, [id](const auto& item) {
                return item->getId() == id;
            });
            if (subscriber == topic.end()) {
                return;
            }

            (*subscriber)->deactivate();
            if (publishDepth == 0) {
                cleanup(typeId);
            }
        }

    private:

        void cleanup() noexcept
        {
            for (auto iterator = subscriptionsTable.begin(); iterator != subscriptionsTable.end();)
            {
                Topic& topic = iterator->second;
                std::erase_if(topic, [](const auto& subscription) {
                    return !subscription->isActive();
                });
                if (topic.empty()) {
                    iterator = subscriptionsTable.erase(iterator);
                } else {
                    ++iterator;
                }
            }
        }

        void cleanup(const EventTypeId typeId) noexcept
        {
            const auto iterator = subscriptionsTable.find(typeId);
            if (iterator == subscriptionsTable.end()) {
                return;
            }

            Topic& topic = iterator->second;
            std::erase_if(topic,[](const auto& subscription) {
                return !subscription->isActive();
            });
            if (topic.empty()) {
                subscriptionsTable.erase(iterator);
            }
        }

    private:

        std::map<EventTypeId, Topic> subscriptionsTable;
        SubscriptionId nextId { 0 };
        std::size_t publishDepth { 0 };
    };
}

namespace
{
    struct TaskLogger
    {
        static void handle(const TaskCreated& event) {
            std::cout << "Logger: created task " << event.taskId << " (" << event.name << ")\n";
        }

        static void handle(const TaskCompleted& event) {
            std::cout << "Logger: completed task " << event.taskId << " in " << event.durationMs << " ms\n";
        }

        static void handle(const TaskFailed& event) {
            std::cout << "Logger: task " << event.taskId<< " failed with error " << event.errorCode << '\n';
        }
    };

    struct TaskMetrics
    {
        void handle(const TaskCreated&) noexcept{
            ++createdTasks;
        }

        void handle(const TaskCompleted& event) noexcept {
            ++completedTasks;
            totalDurationMs += event.durationMs;
        }

        void handle(const TaskFailed&) noexcept {
            ++failedTasks;
        }

        [[nodiscard]]
        std::size_t getCreatedTasks() const noexcept {
            return createdTasks;
        }

        [[nodiscard]]
        std::size_t getCompletedTasks() const noexcept {
            return completedTasks;
        }

        [[nodiscard]]
        std::size_t getFailedTasks() const noexcept {
            return failedTasks;
        }

        [[nodiscard]]
        std::size_t getTotalDurationMs() const noexcept{
            return totalDurationMs;
        }

    private:

        std::size_t createdTasks { 0 };
        std::size_t completedTasks { 0 };
        std::size_t failedTasks { 0 };
        std::size_t totalDurationMs { 0 };
    };

    struct TaskMonitor
    {
        static void handle(const TaskFailed& event) {
            std::cout << "Monitor: task " << event.taskId << " failed, error " << event.errorCode << '\n';
        }

        static void handle(const TaskCancelled& event) {
            std::cout << "Monitor: task " << event.taskId << " cancelled: " << event.reason << '\n';
        }
    };

    struct SelfUnsubscriber
    {
        Subscription* subscription { nullptr };
        std::size_t calls { 0 };

        void handle(const TaskCompleted& event)
        {
            ++calls;

            std::cout << "OneShot: task " << event.taskId << " completed\n";
            if (calls == 1 && subscription != nullptr) {
                subscription->reset();
            }
        }
    };
}

void published_subscriber::subscriber_different_topics_defer_remove::TestAll()
{
    const std::shared_ptr<EventBus> bus = std::make_shared<EventBus>();

    TaskLogger logger;
    TaskMetrics metrics;
    TaskMonitor monitor;
    SelfUnsubscriber oneShot;

    auto createdLogger = bus->subscribe<TaskCreated>(logger);
    auto completedLogger = bus->subscribe<TaskCompleted>(logger);
    auto failedLogger = bus->subscribe<TaskFailed>(logger);

    auto createdMetrics = bus->subscribe<TaskCreated>(metrics);
    auto completedMetrics = bus->subscribe<TaskCompleted>(metrics);
    auto failedMetrics = bus->subscribe<TaskFailed>(metrics);

    auto failedMonitor = bus->subscribe<TaskFailed>(monitor);
    auto cancelledMonitor = bus->subscribe<TaskCancelled>(monitor);

    auto oneShotSubscription = bus->subscribe<TaskCompleted>(oneShot);
    oneShot.subscription = &oneShotSubscription;

    std::cout << "First publish:\n";

    bus->publish(TaskCreated { .taskId = 101, .name = "Compile project" });

    std::cout << "\nFirst TaskCompleted publish:\n";

    bus->publish(TaskCompleted { .taskId = 101, .durationMs = 420 });

    std::cout << "\nSecond TaskCompleted publish:\n";

    bus->publish(TaskCompleted { .taskId = 102, .durationMs = 250 });

    std::cout << "\nFailure:\n";

    bus->publish(TaskFailed { .taskId = 103, .errorCode = 500 });

    std::cout << "\nCancellation:\n";

    bus->publish(TaskCancelled {
            .taskId = 104,
            .reason = "User requested cancellation"
        });

    std::cout << "\nMetrics:\n";
    std::cout << "Created tasks: "<< metrics.getCreatedTasks() << '\n';
    std::cout << "Completed tasks: "<< metrics.getCompletedTasks() << '\n';
    std::cout << "Failed tasks: " << metrics.getFailedTasks() << '\n';
    std::cout << "Total duration: "<< metrics.getTotalDurationMs() << " ms\n";
    std::cout << "One-shot calls: " << oneShot.calls << '\n';
}