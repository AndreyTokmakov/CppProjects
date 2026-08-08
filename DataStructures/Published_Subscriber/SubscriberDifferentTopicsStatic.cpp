/**============================================================================
Name        : SubscriberDifferentTopicsStatic.cpp
Created on  : 08.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SubscriberDifferentTopicsStatic.cpp
============================================================================**/

#include "PubSub.hpp"

#include <iostream>
#include <format>
#include <string_view>
#include <utility>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <concepts>
#include <memory>

namespace
{
    using SubscriptionId = std::size_t;

    enum class Topic: uint8_t
    {
        UserCreated,
        UserDeleted,
        UserRenamed
    };

    struct UserCreated
    {
        int id;
        std::string_view name;
    };

    struct UserDeleted
    {
        int id;
    };

    struct UserRenamed
    {
        int id;
        std::string_view oldName;
        std::string_view newName;
    };

    template<Topic>
    struct EventTraits;

    template<>
    struct EventTraits<Topic::UserCreated> {
        using Type = UserCreated;
    };

    template<>
    struct EventTraits<Topic::UserDeleted> {
        using Type = UserDeleted;
    };

    template<>
    struct EventTraits<Topic::UserRenamed> {
        using Type = UserRenamed;
    };

    template<Topic TopicType>
    using EventType = typename EventTraits<TopicType>::Type;

    struct IEventBus
    {
        virtual ~IEventBus() = default;

    protected:

        struct ISubscriber
        {
            virtual ~ISubscriber() = default;
            virtual void invoke(const void* event) const = 0;

            [[nodiscard]]
            virtual SubscriptionId getId() const noexcept = 0;
        };

        virtual void unsubscribe(Topic topic, SubscriptionId id) noexcept = 0;
        friend struct Subscription;
    };

    struct Subscription
    {
        Subscription() noexcept = default;

        Subscription(std::shared_ptr<IEventBus>&& bus,
                     const Topic topic,
                     const SubscriptionId id) noexcept:
            eventBus { bus }, topic { topic }, id { id } {
        }

        Subscription(const Subscription&) = delete;
        Subscription& operator=(const Subscription&) = delete;

        Subscription(Subscription&& other) noexcept:
            eventBus { std::move(other.eventBus) },
            topic { other.topic },
            id { other.id } {
        }

        Subscription& operator=(Subscription&& other) noexcept
        {
            reset();
            eventBus = std::move(other.eventBus);
            topic = other.topic;
            id = other.id;
            return *this;
        }

        ~Subscription() {
            reset();
        }

        void reset() noexcept
        {
            if (const auto bus = eventBus.lock(); bus) {
                bus->unsubscribe(topic, id);
            }
            eventBus.reset();
        }

    private:

        std::weak_ptr<IEventBus> eventBus;
        Topic topic { Topic::UserCreated };
        SubscriptionId id { 0 };
    };

    class EventBus final : public IEventBus,
                           public std::enable_shared_from_this<EventBus>
    {
        template<Topic TopicType, typename SubscriberType>
        class Subscriber final : public ISubscriber
        {
        public:

            Subscriber(const SubscriptionId id, SubscriberType& subscriber) noexcept:
                id { id }, subscriber { subscriber } {
            }

            void invoke(const void* event) const override {
                subscriber.handle(*static_cast<const EventType<TopicType>*>(event));
            }

            [[nodiscard]]
            SubscriptionId getId() const noexcept override {
                return id;
            }

        private:

            SubscriptionId id;
            SubscriberType& subscriber;
        };

        template<Topic TopicType>
        struct Channel
        {
            std::vector<std::unique_ptr<ISubscriber>> subscribers;
        };

    public:

        template<Topic TopicType, typename SubscriberType>
        [[nodiscard]]
        Subscription subscribe(SubscriberType& subscriber)
        {
            Channel<TopicType>& channel = getChannel<TopicType>();
            const SubscriptionId id = nextId++;

            channel.subscribers.push_back(std::make_unique<Subscriber<TopicType, SubscriberType>>(id, subscriber));
            return Subscription { shared_from_this(), TopicType, id };
        }

        template<Topic TopicType>
        void publish(const EventType<TopicType>& event)
        {
            Channel<TopicType>& channel = getChannel<TopicType>();
            for (const auto& subscriber : channel.subscribers) {
                subscriber->invoke(&event);
            }
        }

    protected:

        void unsubscribe(const Topic topic, const SubscriptionId id) noexcept override
        {
            switch (topic)
            {
                case Topic::UserCreated:
                    unsubscribe<Topic::UserCreated>(id);
                    break;
                case Topic::UserDeleted:
                    unsubscribe<Topic::UserDeleted>(id);
                    break;
                case Topic::UserRenamed:
                    unsubscribe<Topic::UserRenamed>(id);
                    break;
            }
        }

    private:

        template<Topic TopicType>
        [[nodiscard]]
        Channel<TopicType>& getChannel()
        {
            static Channel<TopicType> channel;
            return channel;
        }

        template<Topic TopicType>
        void unsubscribe(const SubscriptionId id) noexcept
        {
            Channel<TopicType>& channel = getChannel<TopicType>();
            const auto iterator = std::ranges::find_if(channel.subscribers,[id](const auto& subscriber) {
                return subscriber->getId() == id;
            });
            if (iterator != channel.subscribers.end()) {
                channel.subscribers.erase(iterator);
            }
        }

        SubscriptionId nextId { 0 };
    };
}

namespace
{
    struct UserLogger
    {
        void handle(const UserCreated& event) const {
            std::cout << "Logger: user created: " << event.id << ", " << event.name << '\n';
        }

        void handle(const UserDeleted& event) const {
            std::cout << "Logger: user deleted: " << event.id << '\n';
        }

        void handle(const UserRenamed& event) const {
            std::cout << "Logger: user renamed: " << event.id << " from " << event.oldName << " to " << event.newName << '\n';
        }
    };

    struct UserMetrics
    {
        void handle(const UserCreated&) noexcept {
            ++createdUsers;
        }

        void handle(const UserDeleted&) noexcept {
            ++deletedUsers;
        }

        void handle(const UserRenamed&) noexcept {
            ++renamedUsers;
        }

        [[nodiscard]]
        std::size_t getCreatedUsers() const noexcept {
            return createdUsers;
        }

        [[nodiscard]]
        std::size_t getDeletedUsers() const noexcept {
            return deletedUsers;
        }

        [[nodiscard]]
        std::size_t getRenamedUsers() const noexcept {
            return renamedUsers;
        }

    private:

        std::size_t createdUsers { 0 };
        std::size_t deletedUsers { 0 };
        std::size_t renamedUsers { 0 };
    };

    struct AuditLog
    {
        void handle(const UserCreated& event) const {
            std::cout << "Audit: created user " << event.id << '\n';
        }

        void handle(const UserRenamed& event) const {
            std::cout << "Audit: renamed user " << event.id << '\n';
        }
    };
}

void published_subscriber::subscriber_different_topics_static::TestAll()
{
    const std::shared_ptr<EventBus> bus = std::make_shared<EventBus>();

    UserLogger logger;
    UserMetrics metrics;
    AuditLog audit;

    auto loggerCreated = bus->subscribe<Topic::UserCreated>(logger);
    auto metricsCreated = bus->subscribe<Topic::UserCreated>(metrics);

    auto loggerDeleted = bus->subscribe<Topic::UserDeleted>(logger);
    auto metricsDeleted = bus->subscribe<Topic::UserDeleted>(metrics);

    auto auditCreated = bus->subscribe<Topic::UserCreated>(audit);
    auto auditRenamed = bus->subscribe<Topic::UserRenamed>(audit);

    bus->publish<Topic::UserCreated>(UserCreated { .id = 42,.name = "Alice"});
    bus->publish<Topic::UserDeleted>(UserDeleted { .id = 42 });
    bus->publish<Topic::UserRenamed>(UserRenamed { .id = 42, .oldName = "Alice", .newName = "Alice Smith"});

    std::cout << '\n';

    std::cout << "Created users: " << metrics.getCreatedUsers() << '\n';
    std::cout << "Deleted users: " << metrics.getDeletedUsers() << '\n';
    std::cout << "Renamed users: " << metrics.getRenamedUsers() << '\n';

    std::cout << '\n';

    auditCreated.reset();
    bus->publish<Topic::UserCreated>(UserCreated { .id = 100, .name = "Bob"});
}