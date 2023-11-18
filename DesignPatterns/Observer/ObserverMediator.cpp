/**============================================================================
Name        : ObserverMediator.cpp
Created on  : November 18, 2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Observer-Mediator pattern test
============================================================================**/

#ifndef OBSERVER_MEDIATOR_PATTERN_INCLUDE_GUARD_H
#define OBSERVER_MEDIATOR_PATTERN_INCLUDE_GUARD_H

#include <iostream>
#include <memory>
#include <list>
#include <utility>
#include <vector>
#include <unordered_map>
#include <format>


namespace ObserverMediator
{
    template<typename T>
    struct IObserver
    {
        virtual void update(const T& source) const = 0;
        virtual ~IObserver() = default;
    };

    template<typename T>
    struct IObservable
    {
        virtual IObserver<T>* addObserver(IObserver<T>* observer) = 0;
        virtual void notify() noexcept = 0;
        virtual ~IObservable() = default;
    };

    template<typename T>
    class ObservableBase: public IObservable<T>
    {
        std::list<IObserver<T>*> observers;

    protected:
        [[nodiscard, maybe_unused]]
        inline constexpr T& underlying() noexcept {
            return static_cast<T&>(*this);
        }

        [[nodiscard, maybe_unused]]
        inline constexpr const T& underlying() const noexcept {
            return static_cast<const T&>(*this);
        }

    public:

        IObserver<T>* addObserver(IObserver<T>* observer) override {
            observers.push_back(observer);
            return observer;
        }

        void notify() noexcept override
        {
            for (const IObserver<T>* observer : observers) {
                observer->update(underlying());
            }
        }
    };

    struct Entry: public ObservableBase<Entry>
    {
        std::string name;

        explicit Entry(std::string name): name { std::move(name) } {
            std::cout << std::format("Entry({}) created.", name) << std::endl;
        }

        void setName(std::string n) noexcept {
            name = std::move(n);
            notify();
        }
    };

    class Repository
    {
        std::unordered_map<std::string, std::shared_ptr<Entry>> storage;

        struct UpdateEntryObserver: IObserver<Entry>
        {
            Repository& repo;

            explicit UpdateEntryObserver(Repository& repository): repo {repository}{
            }

            void update(const Entry& event) const override {
                repo.onEntryUpdate(event);
            }
        };

    public:

        std::shared_ptr<Entry> getEntry(const std::string& name)
        {
            if (auto iter = storage.find(name); storage.end() != iter)
                return iter->second;
            else {
                auto [it, ok] = storage.emplace(name, std::make_shared<Entry>(""));
                it->second->addObserver(new UpdateEntryObserver(*this));
                return it->second;
            }
        }

        void print()
        {
            for (auto& [name, entry]: storage)
                std::cout << std::format("Name {}, value: {}", name, entry->name) << std::endl;
        }

    private:

        void onEntryUpdate(const Entry& event)
        {
            std::cout << "Repository::onEntryUpdate() called" << std::endl;
            std::cout << "Event name: " << event.name << std::endl;
        }
    };
}

void ObserverMediator_Test()
{
    using namespace ObserverMediator;
    Repository repository;

    std::shared_ptr<Entry> obj1 = repository.getEntry("One");
    std::shared_ptr<Entry> obj2 = repository.getEntry("Two");

    obj1->setName("One_Name");
    obj2->setName("Two_Name");
    obj1->setName("One_Name_1");
}

#endif // !OBSERVER_MEDIATOR_PATTERN_INCLUDE_GUARD_H