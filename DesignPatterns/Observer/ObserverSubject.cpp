/**============================================================================
Name        : ObserverSubject.cpp
Created on  : 12.01.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ObserverSubject pattern test
============================================================================**/

#include "Observer.h"

#include <print>
#include <algorithm>
#include <list>

namespace Observer::SubjectObserver
{
    struct IObserver
    {
        virtual ~IObserver() = default;
        virtual void notify() const = 0;
    };

    struct Subject
    {
        void registerObserver(IObserver* observer) {
            observers.push_back(observer);
        }

        void unregisterObserver(IObserver* observer) {
            observers.remove(observer);
        }

        void notifyObservers() const {
            for (auto observer: observers)
                observer->notify();
        }

    private:
        std::list<IObserver*> observers;
    };

    struct ConcreteObserverA : public IObserver
    {
        explicit ConcreteObserverA(Subject& srcObject) : subject { srcObject } {
            subject.registerObserver(this);
        }

        void notify() const override {
            std::println("ConcreteObserverA::notify");
        }
    private:
        Subject& subject;
    };

    struct ConcreteObserverB : public IObserver
    {
        explicit ConcreteObserverB(Subject& srcObject) : subject { srcObject } {
            subject.registerObserver(this);
        }
        void notify() const override {
            std::println("ConcreteObserverB::notify");
        }
    private:
        Subject& subject;
    };

}

void SubjectObserver_Test()
{
    using namespace Observer::SubjectObserver;

    Subject subject;
    ConcreteObserverA observerA(subject);
    ConcreteObserverB observerB(subject);

    subject.notifyObservers();
    std::println("    subject.unregisterObserver(observerA)");
    subject.unregisterObserver(&observerA);
    subject.notifyObservers();
}
