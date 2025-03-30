/**============================================================================
Name        : Concepts.cpp
Created on  : 18.11.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Concepts.cpp
============================================================================**/

#include "Concepts.h"

#include <iostream>

namespace Concepts::DependencyInjection
{
    template<typename Type>
    concept Interface = requires (Type obj) {
        { obj.func() } -> std::same_as<void>;
    };

    struct DefaultImpl
    {
        void func() {
            std::cout << "DefaultImpl::func()" << std::endl;
        }
    };

    template<typename ...>
    inline auto InjectedInterface = DefaultImpl {};

    template<typename ... Args>
    requires (sizeof...(Args) == 0)
    void call_func()
    {
        Interface auto& iface = InjectedInterface<Args...>;
        iface.func();
    }

    struct InjectedImplOne
    {
        void func() {
            std::cout << "InjectedImpl_One::func()" << std::endl;
        }
    };

    template<>
    inline auto InjectedInterface<> = InjectedImplOne {};

    void MyFunc()
    {
        call_func();
    }
}


namespace Concepts::TypeErasure
{
    template <class T>
    concept HasCallFunction = requires(T obj) {
        { obj.call() } -> std::convertible_to<void>;
    };

    template<HasCallFunction callableObject>
    void invoke(callableObject obj)
    {
        obj.call();
    }

    struct ClassWithCallable {
        void call() {std::cout << "Functor::call()" << std::endl; }
    };


    template<HasCallFunction Impl>
    struct Wrapper
    {
        explicit Wrapper(Impl&& obj) noexcept : object { std::forward<Impl>(obj) } {
        }

        void call() {
            return object.call();
        }

    private:
        Impl object {};
    };

    void Test()
    {
        Wrapper<ClassWithCallable> w1 {ClassWithCallable{}};
        w1.call();
    }

}

void Concepts::TestAll()
{
    // DependencyInjection::MyFunc();
    TypeErasure::Test();
}