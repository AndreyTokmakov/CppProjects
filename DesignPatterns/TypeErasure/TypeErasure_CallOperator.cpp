/**============================================================================
Name        : TypeErasure_CallOperator.cpp
Created on  : 05.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TypeErasure_CallOperator.cpp
============================================================================**/

#include <memory>
#include <iostream>

namespace TypeErasure_CallOperator
{
    struct PlusZero
    {
        int operator()(int) const {
            return 0;
        }
    };

    struct PlusOne
    {
        [[nodiscard]]
        int call(int x) const { return x+1; }

        int operator()(int x) const {
            return call(x);
        }
    };

    struct PlusTwo
    {
        [[nodiscard]]
        int call(int x) const { return x+2; }

        int operator()(int x) const {
            return call(x);
        }
    };

    struct AbstractCallback
    {
        [[nodiscard]]
        virtual int call(int) const = 0;

        virtual ~AbstractCallback() = default;
    };

    template<class T>
    struct WrappingCallback : AbstractCallback
    {
        T callbackImpl;

        explicit WrappingCallback(T &&cb) : callbackImpl { std::move(cb) } {
        }

        [[nodiscard]]
        int call(int x) const override {
            return callbackImpl(x);
        }
    };

    struct Callback {
        std::unique_ptr<AbstractCallback> ptr;

        template<class T>
        Callback(T t) {
            ptr = std::make_unique<WrappingCallback<T>>(std::move(t));
        }

        int operator()(int x) const {
            return ptr->call(x);
        }
    };

    int client(const Callback& callback) {
        return callback(1);
    }
}


void TypeErasure_CallOperator_Test()
{
    using namespace TypeErasure_CallOperator;

    std::cout << client(PlusZero{}) << std::endl;
    std::cout << client([](int x) { return x; }) << std::endl;
    std::cout << client(PlusOne{}) << std::endl;
    std::cout << client(PlusTwo{}) << std::endl;
}
