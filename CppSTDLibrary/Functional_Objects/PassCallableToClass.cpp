/**============================================================================
Name        : PassCallableToClass.cpp
Created on  : 24.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PassCallableToClass.cpp
============================================================================**/

#include "PassCallableToClass.hpp"

#include <iostream>
#include <utility>


namespace PassCallableToClass::Template_Based
{
    /**
     * Template-based (compile-time, no overhead)
     * If you know the callable type at compile time, you can make the class a template:
    **/

    template <typename Callable>
    struct Worker
    {
        Worker(Callable&& c) : callable(std::forward<Callable>(c)) {}

        void run() {
            callable();
        }

    private:
        Callable callable;
    };

    void demo()
    {
        auto lambda = [] { std::cout << "Hello from lambda\n"; };
        Worker worker(std::move(lambda));
        worker.run();
    }
}

namespace PassCallableToClass::Raw_Function_Pointer
{
    /**
     * Store raw function pointer
     * If you only need simple free/static functions:
    **/

    struct Worker
    {
        using Func = void(*)();

        explicit Worker(Func f) : func(f) {}

        void run() {
            func();
        }

    private:
        Func func;
    };

    void hello() {
        std::cout << "Hello from function pointer\n";
    }

    void demo()
    {
        Worker w(hello);
        w.run();
    }
}

namespace PassCallableToClass::CRTP
{
    template <typename Derived>
    struct WorkerBase
    {
        void run() {
            static_cast<Derived*>(this)->call();
        }
    };

    struct Worker : public WorkerBase<Worker>
    {
        void call() {
            std::cout << "Custom implementation\n";
        }
    };

    void demo()
    {
        Worker w;
        w.run();
    }
}

/*
namespace PassCallableToClass::Auto_Lambda
{
    class Worker {
    public:
        template <typename Callable>
        Worker(Callable&& c) : callable([c = std::forward<Callable>(c)](){ c(); }) {}

        void run() {
            callable();
        }

    private:
        void (*callable)();  // could be optimized further
    };

    void demo()
    {
        auto lambda = [] { std::cout << "Hello from lambda\n"; };
        Worker worker(std::move(lambda));
        worker.run();
    }
}
*/


void PassCallableToClass::TestAll()
{
    // Template_Based::demo();
    // Raw_Function_Pointer::demo();
    // CRTP::demo();

    /// Auto_Lambda::demo();
}