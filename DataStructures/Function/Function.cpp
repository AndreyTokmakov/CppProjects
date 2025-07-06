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
#include <exception>
#include <functional>

namespace Impl_One
{
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

        struct ICallable
        {
            virtual ReturnType call(Args ...) = 0;
            virtual ~ICallable() = default;

            // TODO: Finish rule of 5
            // TODO: clone() ??
        };

        template<typename Callable>
        struct CallableImpl: public ICallable
        {
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

    void demo()
    {

        function<int (int, int)> func (sum);
        function<int (int, int)> func1 ([](int a, int b) { return  a + b; });

        std::cout << func(1, 2) << std::endl;
        std::cout << func1(1, 2) << std::endl;
    }
}

namespace Impl_Two
{
    template<typename Signature>
    class Function;

    template<typename ReturnType, typename... Args>
    class Function<ReturnType(Args...)>
    {
        struct ICallable
        {
            virtual ~ICallable() = default;
            virtual ReturnType invoke(Args... args) = 0;
            //virtual std::unique_ptr<ICallable> clone() const = 0;
        };

        template<typename F>
        struct Model : ICallable {
            F func;

            explicit Model(F&& f) : func(std::forward<F>(f)) {}

            ReturnType invoke(Args... args) override {
                return func(std::forward<Args>(args)...);
            }

            /*
            std::unique_ptr<ICallable> clone() const override {
                return std::make_unique<Model<F>>(func);
            }*/
        };

        std::unique_ptr<ICallable> impl;

    public:
        Function() = default;

        template<typename F>
        Function(F&& f) : impl { std::make_unique<Model<std::decay_t<F>>>(std::forward<F>(f)) } {
        }

        Function(const Function& other) : impl(other.impl ? other.impl->clone() : nullptr) {}

        Function& operator=(const Function& other) {
            if (this != &other) {
                impl = other.impl ? other.impl->clone() : nullptr;
            }
            return *this;
        }

        Function(Function&&) noexcept = default;
        Function& operator=(Function&&) noexcept = default;

        ReturnType operator()(Args&& ... args) const
        {
            if (!impl) {
                throw std::bad_function_call();
            }
            return impl->invoke(std::forward<Args>(args)...);
        }

        explicit operator bool() const noexcept {
            return static_cast<bool>(impl);
        }
    };

    void demo()
    {
        Function<void(int)> f = [](int x) {
            std::cout << "Lambda: " << x << "\n";
        };

        f(42);
    }
}

void Function::Test()
{

    // Impl_One::demo();
    Impl_Two::demo();
}

