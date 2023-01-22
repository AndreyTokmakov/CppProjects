//============================================================================
// Name        : Function.cpp
// Created on  : 07.08.22.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Function
//============================================================================

#include "Function.h"

#include <iostream>
#include <memory>

template<typename T>
class function;


template<typename ReturnType, typename ... Args>
class function<ReturnType (Args...)>
{
public:
    /// Function(Ret (*f)(Args ...)) : callable { std::make_unique<CallableImpl<Ret (*)(Args ...)>>(f)} { }

    template<class FunctionalObject>
    explicit function(FunctionalObject funcObj)
            : callable { std::make_unique<CallableImpl<FunctionalObject>>(funcObj)} {
    }

    ReturnType operator()(Args ... params) {
        return callable->call(params ...);
    }

    // TODO: Add copy constructor

private:
    struct ICallable {
        virtual ReturnType call(Args ...) = 0;
        virtual ~ICallable() = default;

        // TODO: Finish rule of 5
        // TODO: clone() ??
    };

    template<typename Callable>
    struct CallableImpl: public ICallable {
        explicit CallableImpl(Callable callable_): callable { std::move(callable_) } {
        }

        ReturnType call(Args ... params) override {
            return callable(params ...);
        }

        Callable callable;
    };

    std::unique_ptr<ICallable> callable {};
};

namespace
{
    int sum(int a, int b) {
        return a + b;
    }
}

void Function::Test()
{

    function<int (int, int)> func (sum);
    function<int (int, int)> func1 ([](int a, int b) { return  a + b; });

    std::cout << func(1, 2) << std::endl;
    std::cout << func1(1, 2) << std::endl;

}

