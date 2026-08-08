/**============================================================================
Name        : SubscriberDifferentEvents.cpp
Created on  : 08.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SubscriberDifferentEvents.cpp
============================================================================**/

#include "PubSub.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>


/*
 * Pub/Sub Pattern — Type-Based Event Bus
 *
 * Pub/Sub (Publish–Subscribe) is a messaging pattern where the publisher
 * does not know which objects will receive the published message.
 *
 * The publisher sends an event to the EventBus:
 *
 *     bus.publish(UserCreated { ... });
 *
 * Subscribers register themselves for a particular event type:
 *
 *     bus.subscribe<UserCreated>(logger);
 *     bus.subscribe<UserCreated>(metrics);
 *
 * The EventBus keeps the relationship between event types and subscribers.
 * Therefore, the publisher depends only on the EventBus and does not know
 * anything about Logger, Metrics, Audit or any other subscriber.
 *
 * The example uses the event type itself as the topic. UserCreated, UserDeleted
 * and UserRenamed are therefore different topics.
 *
 * The important distinction from CommandBus is that a command normally has
 * one logical handler, while an event can have multiple subscribers:
 *
 *     Command:
 *
 *         CommandBus
 *              |
 *              v
 *           Handler
 *
 *     Pub/Sub:
 *
 *         EventBus
 *          /    |    \
 *         v     v     v
 *       Logger Metrics Audit
 *
 * This implementation deliberately does not use std::function. Each subscriber
 * is represented by a small type-erased object stored by the EventBus.
 *
 * The implementation is synchronous: publish() immediately invokes all
 * subscribers registered for the event type.
 *
 * Subscriber lifetime:
 * --------------------
 * The EventBus does not own subscribers. A subscriber must remain alive while
 * it is registered in the EventBus.
 *
 * The Subscription object returned by subscribe() can be used to unsubscribe.
 * Destroying Subscription automatically removes the subscription.
 *
 * This example is intentionally single-threaded. A production implementation
 * that supports concurrent subscribe(), unsubscribe() and publish() would need
 * additional synchronization and lifetime management.
 */


namespace
{
    //struct IEvent{};
    using SubscriptionId = size_t;

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

    struct IEventBus
    {
        virtual ~IEventBus() = default;

    protected:

        virtual void unsubscribe(std::type_index type, SubscriptionId id) noexcept = 0;

        struct ISubscriber
        {
            virtual ~ISubscriber() = default;
            virtual void invoke(const void* event) = 0;

            [[nodiscard]]
            virtual SubscriptionId getId() const noexcept = 0;
        };

        friend struct Subscription;
    };

    struct Subscription
    {
        Subscription() noexcept = default;

        Subscription(const std::shared_ptr<IEventBus>& bus, const std::type_index type, const SubscriptionId id) noexcept
            : eventBus { bus }, type { type }, id { id } {
        }

        Subscription(const Subscription&) = delete;
        Subscription& operator=(const Subscription&) = delete;

        Subscription(Subscription&& other) noexcept :
            eventBus { std::move(other.eventBus) },
            type { other.type },
            id { other.id } {
        }

        Subscription& operator=(Subscription&& other) noexcept
        {
            reset();
            eventBus = std::move(other.eventBus);
            type = other.type;
            id = other.id;

            return *this;
        }

        ~Subscription() {
            reset();
        }

        void reset() noexcept
        {
            if (const auto bus = eventBus.lock(); bus) {
                bus->unsubscribe(type, id);
            }
            eventBus.reset();
        }

    private:

        std::weak_ptr<IEventBus> eventBus;
        std::type_index type { typeid(void) };
        SubscriptionId id { 0 };
    };

    struct EventBus final: IEventBus, std::enable_shared_from_this<EventBus>
    {
        template<typename Event, typename SubType>
        class Subscriber final : public ISubscriber
        {
        public:

            Subscriber(const SubscriptionId id, SubType& subscriber) noexcept
                : id { id }, subscriber { subscriber } {
            }

            void invoke(const void* event) override{
                subscriber.handle(*static_cast<const Event*>(event));
            }

            [[nodiscard]]
            SubscriptionId getId() const noexcept override{
                return id;
            }

        private:

            SubscriptionId id;
            SubType& subscriber;
        };

        using Subscribers = std::vector<std::unique_ptr<ISubscriber>>;

    public:

        EventBus() = default;

        template<typename Event, typename SubType>
        [[nodiscard]]
        Subscription subscribe(SubType& subscriber)
        {
            const std::type_index type = std::type_index { typeid(Event) };
            const SubscriptionId id = nextId++;

            Subscribers& subscribers = getSubscribers(type);

            subscribers.push_back(std::make_unique<Subscriber<Event, SubType>>(id, subscriber));
            return Subscription { shared_from_this(), type, id };
        }

        template<typename Event>
        void publish(const Event& event)
        {
            const auto iterator = subscribersTable.find(std::type_index { typeid(Event) });
            if (iterator == subscribersTable.end()) {
                return;
            }
            for (const auto& subscriber : iterator->second){
                subscriber->invoke(&event);
            }
        }

    protected:

        void unsubscribe(const std::type_index type, const SubscriptionId id) noexcept override
        {
            const auto iterator = subscribersTable.find(type);
            if (iterator == subscribersTable.end()) {
                return;
            }

            Subscribers& subscribersForType = iterator->second;
            const auto subscriber = std::ranges::find_if(subscribersForType, [id](const auto& item) {
                return item->getId() == id;
            });
            if (subscriber != subscribersForType.end()) {
                subscribersForType.erase(subscriber);
            }
        }

    private:

        [[nodiscard]]
        Subscribers& getSubscribers(const std::type_index type){
            return subscribersTable[type];
        }

    private:

        std::unordered_map<std::type_index, Subscribers> subscribersTable;
        SubscriptionId nextId { 0 };

        friend struct Subscription;
    };

    struct UserLogger
    {
        static void handle(const UserCreated& event) {
            std::cout << "UserLogger: user created: " << event.id << ", " << event.name << '\n';
        }

        static void handle(const UserDeleted& event) {
            std::cout << "UserLogger: user deleted: " << event.id << '\n';
        }
    };

    struct UserMetrics
    {
        void handle(const UserCreated&) noexcept{
            ++createdUsers;
        }

        void handle(const UserDeleted&) noexcept {
            ++deletedUsers;
        }

        [[nodiscard]]
        std::size_t getCreatedUsers() const noexcept {
            return createdUsers;
        }

        [[nodiscard]]
        std::size_t getDeletedUsers() const noexcept {
            return deletedUsers;
        }

    private:

        std::size_t createdUsers { 0 };
        std::size_t deletedUsers { 0 };
    };

    struct AuditLog
    {
        static void handle(const UserCreated& event) {
            std::cout << "AuditLog: created user " << event.id << '\n';
        }

        static void handle(const UserRenamed& event) {
            std::cout << "AuditLog: renamed user "<< event.id << " from " << event.oldName<< " to " << event.newName << '\n';
        }
    };

}


void published_subscriber::subscriber_different_events::TestAll()
{
    const std::shared_ptr<EventBus> bus = std::make_shared<EventBus>();

    UserLogger logger;
    UserMetrics metrics;
    AuditLog audit;

    auto createdLogSubscription = bus->subscribe<UserCreated>(logger);
    auto createdMetricsSubscription = bus->subscribe<UserCreated>(metrics);
    auto deletedLogSubscription = bus->subscribe<UserDeleted>(logger);
    auto deletedMetricsSubscription = bus->subscribe<UserDeleted>(metrics);
    auto createdAuditSubscription = bus->subscribe<UserCreated>(audit);
    auto renamedAuditSubscription = bus->subscribe<UserRenamed>(audit);

    bus->publish(UserCreated { 42, "Alice" });
    bus->publish(UserDeleted { 42 });
    bus->publish(UserRenamed { 42, "Alice", "Alice Smith" });

    std::cout << "Created users: " << metrics.getCreatedUsers() << '\n';
    std::cout << "Deleted users: "<< metrics.getDeletedUsers() << '\n';

    createdAuditSubscription.reset();

    bus->publish(UserCreated { .id = 100, .name = "Bob" });
}

/**
UserLogger: user created: 42, Alice
AuditLog: created user 42
UserLogger: user deleted: 42
AuditLog: renamed user 42 from Alice to Alice Smith
Created users: 1
Deleted users: 1
UserLogger: user created: 100, Bob
**.