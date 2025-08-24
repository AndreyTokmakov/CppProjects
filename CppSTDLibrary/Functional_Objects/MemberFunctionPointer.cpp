/**============================================================================
Name        : MemberFunctionPointer.cpp
Created on  : 07.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MemberFunctionPointer.cpp
============================================================================**/

#include "MemberFunctionPointer.hpp"

#include <iostream>
#include <utility>
#include <functional>

namespace MemberFunctionPointer
{
    struct Base
    {
        std::string value;

        explicit Base(std::string str) : value(std::move(str)) {
        }

        Base(const Base& obj) : value(obj.value) {
            std::cout << "Base::Base(" << value << ") copy contructor." << std::endl;
        }

        Base(Base&& obj) noexcept : value(std::move(obj.value)) {
            std::cout << "Base::Base(" << value << ") move contructor." << std::endl;
        }

        Base& operator=(const Base& obj)  {
            std::cout << "Base::Base(" << value << ")." << std::endl;
            if (this != &obj) {
                value = obj.value;
            }
            return *this;
        }
        Base& operator=(Base&& obj) noexcept {
            std::cout << "Move assignment oprator Base::Base(" << value << ")." << std::endl;
            if (this != &obj) {
                value = std::move(obj.value);
            }
            return *this;
        }

        virtual void doSomething(const std::string& str)
        {
            std::cout << __FUNCTION__ << std::endl;
            std::cout << str << std::endl;
        }
    };

    struct Derived : public Base
    {
        explicit Derived(std::string str) : Base(std::move(str)) {
        }

        void doSomething(const std::string& str) override
        {
            std::cout << __FUNCTION__ << std::endl;
            std::cout << str << std::endl;
        }
    };
}


namespace MemberFunctionPointer
{

    void CallMethod_ByPointer()
    {
        using methodPtr = void (Base::*)(const std::string& str);
        methodPtr func = &Base::doSomething;

        {
            std::cout << "\n----------------------------- Calling Base class method by pointer: ---------------------\n" << std::endl;
            Base obj("Test");
            (obj.*func)("Some input parameter");
        }
        {
            std::cout << "\n----------------------------- Calling Derived class method by pointer: ---------------------\n" << std::endl;
            Derived obj("Test");
            (obj.*func)("Some input parameter");
        }
    }

    void CallMethod_BIND()
    {
        {
            std::cout << "\n----------------------------- Calling Base class method using BIND: ---------------------\n" << std::endl;
            Base obj("Test");
            auto func = std::bind(&Base::doSomething, &obj, std::placeholders::_1);
            func("Some input parameter");
        }
        {
            std::cout << "\n----------------------------- Calling Derived class method using BIND: ---------------------\n" << std::endl;
            Derived obj("Test");
            auto func = std::bind(&Base::doSomething, &obj, std::placeholders::_1);
            func("Some input parameter");
        }
    }

    void CallMethod_Mem_Fn()
    {
        auto func = std::mem_fn(&Base::doSomething);
        {
            std::cout << "----------------------------- Calling Base class method using std::mem_fn: ---------------------\n" << std::endl;
            Base obj("Test");
            func(obj, "Some input parameter1");
        }
        {
            std::cout << "\n----------------------------- Calling Derived class method using std::mem_fn: ---------------------\n" << std::endl;
            Derived obj("Test");
            func(obj, "Some input parameter2");
        }
    }
};


namespace MemberFunctionPointer::FunctionPointers
{
    struct S
    {
        int value {1};

        explicit S(int val): value {val} { std::cout << "S::S(" << value << ")" << std::endl; }
        ~S() { std::cout << "S::~S()" << std::endl; }

        [[nodiscard]]
        constexpr int getValueOne() { return value; }

        [[nodiscard]]
        constexpr int getValueTwo() { return value * 2; }
    };

    void functionPointer_Type_Test()
    {
        /** Defining type **/
        using fun_ptr_t = int (S::*)();
        S s {1};

        fun_ptr_t getValOne = &S::getValueOne;

        {
            auto v = (s.*getValOne)();
            std::cout << v << std::endl;
        }

        getValOne = &S::getValueTwo;

        {
            auto v = (s.*getValOne)();
            std::cout << v << std::endl;
        }
    }
}


namespace MemberFunctionPointer::MemFuncTemplate
{
    template<typename ObjectType,
             typename ReturnType,
             typename ...Args>
    using MemFuncPtr = ReturnType (ObjectType::*)(Args...);


    struct Base
    {
        void fn() { std::cout << "Base::fn()\n"; }
        virtual void fn_v() { std::cout << "Base::fn_v()\n"; }

        void fn(int) { std::cout << "Base::fn(int)\n"; }

        void call() { fn(); }
        void call_v() { fn_v(); }
    };

    struct Derived : Base
    {
        void fn() { std::cout << "Derived::fn\n"; }
        void fn_v() override { std::cout << "Derived::fn_v\n"; }
    };

    void Test()
    {
        Base base;
        Derived derived;

        MemFuncPtr<Base, void, int> mem_fun_ptr = &Base::fn;
        (derived.*mem_fun_ptr)(42); // Base::fn(int)

        MemFuncPtr<Derived, void> mem_fun_ptr_d = &Derived::fn;
        (derived.*mem_fun_ptr_d)(); // Derived::fn()
    }
}

void MemberFunctionPointer::TestAll()
{
    // MemberFunctionPointer::CallMethod_ByPointer();
    // MemberFunctionPointer::CallMethod_BIND();
    // MemberFunctionPointer::CallMethod_Mem_Fn();

    // FunctionPointers::functionPointer_Type_Test();

    MemFuncTemplate::Test();
};
