/**============================================================================
Name        : SimpleSubscriberCallback.cpp
Created on  : 08.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SimpleSubscriberCallback.cpp
============================================================================**/

#include "PubSub.hpp"

#include <iostream>
#include <unordered_map>
#include <functional>
#include <memory>
#include <ranges>
#include <utility>

namespace
{
    struct Subject: std::enable_shared_from_this<Subject>
    {
        using Callback = std::function<void(int)>;
        using SubscriptionId = uint64_t;

        struct Subscription
        {
            Subscription(const std::shared_ptr<Subject>& sub, const SubscriptionId id) :
                wPtrSubj { sub }, idSub { id }  {
                /** **/
            }

            ~Subscription() {
                reset();
            }

            Subscription(const Subscription&) = delete;
            Subscription& operator=(const Subscription&) = delete;

            Subscription(Subscription&& other) noexcept:
                wPtrSubj { std::move(other.wPtrSubj) },
                idSub { std::exchange(other.idSub, 0) } {
                /** **/
            }

            Subscription& operator=(Subscription&& other) noexcept
            {
                reset();
                wPtrSubj =  std::move(other.wPtrSubj);
                idSub = std::exchange(other.idSub, 0);
                return *this;
            }

            void reset() noexcept
            {
                if (const std::shared_ptr<Subject> subject = wPtrSubj.lock(); subject) {
                    subject->unsubscribe(idSub);
                    std::cout << "Unsubscribed " << idSub << "\n";
                } else {
                    std::cout << idSub << " expired\n";
                }

                wPtrSubj.reset();
                idSub = SubscriptionId{0};
            }

        private:
            std::weak_ptr<Subject> wPtrSubj;
            SubscriptionId idSub { 0 };
        };

        [[nodiscard]]
        Subscription makeSubscription(const Callback& callback)
        {
            const SubscriptionId id = subscribe(callback);
            return { shared_from_this(), id };
        }

        void notify(const int value) {
            for (const auto &callback: observers | std::views::values) {
                callback(value);
            }
        }

    private:

        void unsubscribe(const SubscriptionId id) {
            observers.erase(id);
        }

        SubscriptionId subscribe(const Callback &cb)
        {
            const SubscriptionId id = nextId++;
            observers[id] = cb;
            return id;
        }

    private:

        std::unordered_map<SubscriptionId, Callback> observers;
        SubscriptionId nextId = 0;
    };


}

void published_subscriber::simple_subscriber_callback::TestAll()
{
    const std::shared_ptr<Subject> subject = std::make_shared<Subject>();
    {
        Subject::Subscription sub = subject->makeSubscription([](const int x){
            std::cout << "Scoped observer: " << x << "\n";
        });
        subject->notify(5);
    } // здесь sub уничтожается -> auto-unsubscribe
    subject->notify(10);  // уже никто не слушает
}
