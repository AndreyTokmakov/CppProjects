/**============================================================================
Name        : Invoke.cpp
Created on  : 01.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Invoke
============================================================================**/

#include "Invoke.h"

#include <iostream>
#include <string>
#include <functional>
#include <utility>

namespace
{
    struct Functor
    {
        void operator() (const int a) const
        {
            std::cout << a << std::endl;
        }
    };


    struct S
    {
        int x { 0 };

        [[nodiscard]]
        int foo(const int i) const  {
            return i * x;
        }
    };


    struct Worker
    {
        int value;

        explicit Worker(const int val) : value(val) {
        }

        void add_and_print(const std::string& text) const {
            std::cout << text << std::endl;
        }

        [[nodiscard]]
        int getValue() const {
            return this->value;
        }

        void printValue() const {
            std::cout << "Value :" << this->value << std::endl;
        }
    };
}

namespace Invoke_Objects_Methods_Manually
{
    struct Data
    {
        int field = 42;

        [[nodiscard]]
        int memberFunction(const int value) const {
            return value * field;
        }
    };

    void call()
    {
        auto methodPtr = &Data::memberFunction;
        auto fieldPtr = &Data::field;

        Data data { 10 };
        auto obj_field = std::invoke(fieldPtr, data);
        std::cout << std::invoke(methodPtr, data, 13) << std::endl;
    }
}


namespace Callbacks
{
    template <typename Callback, typename... Args>
    auto process_and_call(Callback&& callback, Args&&... args)
    {
        // some processing
        return std::invoke(std::forward<Callback>(callback), std::forward<Args>(args)...);
    }

    void invoke_class_method()
    {
        S realObj;
        auto result = process_and_call(&S::foo, realObj, 42);
        std::cout << result << std::endl;
    }
}


namespace Invoke
{

    void Invoke_Functor()
    {
        const auto sum = [](int a, int b)-> int { return a + b; };

        std::cout << "is_invocable<Functor(): "<< std::boolalpha << std::is_invocable<Functor()>::value << std::endl;
        std::cout << std::invoke(sum, 10, 20) << std::endl;
        std::invoke(Functor(), 42);
        std::invoke([]() { std::cout << "hello" << std::endl;; });
    }

    void Access_Member()
    {
        const Worker utils(12345);
        // invoke (access) a data member
        std::cout << "Value : " << std::invoke(&Worker::value, utils) << std::endl;
    }
}


namespace Utilities::Invoke
{




    void Call_Class_Method()
    {
        const Worker utils(314159);

        utils.printValue();
        std::invoke(&Worker::add_and_print, utils, "Some_Text");
        utils.printValue();
    }



    void Is_Invocable() {
        const auto sum = [](int a, int b)-> int { return a + b; };

        std::cout << std::boolalpha << std::is_invocable<decltype(sum)>::value << std::endl;
        std::cout << std::boolalpha << std::is_invocable<int>::value << std::endl;
    }

    // --------------------------------------------------------------------------


    class SomeType
    {
        void putInfo(int value, std::string_view text)
        {
            std::cout << "PrivateInfo: Value = " << value << ", Text: " << text << std::endl;
        }

        template<typename Method, typename... Args>
        void delegate(Method func, Args&&... params)
        {
            std::invoke(func, this, std::forward<Args>(params)...);
        }

    public:

        void info()
        {
            std::invoke(&SomeType::putInfo, this, 101, "Text");
        }

        void invokeWithDelegate()
        {
            delegate(&SomeType::putInfo, 102, "Text2");
        }

    };

    void Invoke_Class_Method_FromMethod()
    {
        SomeType{}.info();
    }

    void Invoke_Class_Method_FromMethod_Delegate()
    {
        SomeType{}.invokeWithDelegate();
    }

    void Call_Lambda()
    {
        auto printer = [] <typename ... Types>(Types&& ... args) {
            auto add_space = []<typename T>(const T& arg) {
                std::cout << ' ';
                return arg;
            };
            (std::cout << ... << add_space(std::forward<Types>(args))) << std::endl;
        };

        // printer(1);
        // printer(1, "Two");
        // printer(1, "Two", "III");

        std::invoke(printer, 1);
        std::invoke(printer, 1, "Two");
        std::invoke(printer, 1, "Two", "III");
    }
}


void Invoke::TestAll()
{
    // Invoke::Access_Member();
    // Invoke::Call_Class_Method();
    // Invoke::Is_Invocable();
    // Invoke::Invoke_Functor();
    // Invoke::Invoke_Class_Method_FromMethod();
    // Invoke::Invoke_Class_Method_FromMethod_Delegate();
    // Invoke::Call_Lambda();

    // Callbacks::invoke_class_method();

    Invoke_Objects_Methods_Manually::call();
}
