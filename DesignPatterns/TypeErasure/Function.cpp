/**============================================================================
Name        : Function.cpp
Created on  : 11.01.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Function.cpp
============================================================================**/

#include "TypeErasure.hpp"

#include <iostream>
#include <print>
#include <functional>
#include <memory>


namespace
{
    template<typename R, typename... Args>
    class Function
    {
        template<typename Ret, typename... Ts>
        struct Concept
        {
            virtual Ret operator()(Ts&&... args) = 0;
            virtual ~Concept() = default;
        };

        template<typename Func, typename Ret, typename... Ts>
        struct ConceptImpl : Concept<Ret, Args...>
        {
            explicit ConceptImpl(Func func): function { func } {
            }

            R operator()(Ts&& ... args) override {
                return std::invoke(function, std::forward<Ts>(args)...);
            }

            Func function;
        };

    public:

        template<typename Func>
            requires std::invocable<Func, Args...>
        Function(Func func) : funcImpl { std::make_unique<ConceptImpl<Func, R, Args...>>(func) } {
        }

        R operator()(Args&&... args){
            return  std::invoke(*funcImpl, std::forward<Args>(args)...);
        }

    private:
        std::unique_ptr<Concept<R, Args...>> funcImpl;
    };
}

namespace
{
    void print_num(int i){
        std::println("{}", i);
    }

    auto display_lambda = [](int i){
        std::println("{}", i);
    };

    struct PrintFunctor
    {
        void operator()(int i) const {
            std::println("{}", i);
        }
    };
}

void TypeErasure_Function::Test()
{
    Function<void, int> f_print_num = print_num;
    Function<void, int> f_display_lambda = display_lambda;
    Function<void, int> f_print_functor = PrintFunctor{};

    f_print_num(42);
    f_display_lambda(5);
    f_print_functor(17);

    // 42
    // 5
    // 17
}