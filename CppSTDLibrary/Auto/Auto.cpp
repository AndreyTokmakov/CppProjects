//============================================================================
// Name        : Auto.cpp
// Created on  : 28.02.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Auto src class
//============================================================================

#include <iostream>
#include <string>
#include <thread>
#include <type_traits>

#include "Auto.h"

namespace Auto {

    class Base {

    };

    class Derived: public Base {

    };

    Base buildBase() {
        return {};
    }

    auto print_type = [](auto T) -> void {
        std::cout << typeid(T).name() << "\n" << std::endl;
    };

    void foo1(const Derived& obj) {

        //std::cout << std::is_reference_v<obj> << std::endl;
        print_type(obj);

        auto a = static_cast<const Base&>(obj);
        print_type(a);

        auto &b = static_cast<const Base&>(obj);
        print_type(b);

        decltype(auto) c = static_cast<const Base&>(obj);
        print_type(c);
    }


    void SimpleTest() {
        auto a = 1;
        print_type(a);

        auto b = "Test";
        print_type(b);

        auto c = buildBase();
        print_type(c);

        Derived d;
        foo1(d);
    }

}

namespace Auto
{
    struct Object
    {
        Object() {
            // std::cout << "Object::Object()" << std::endl;
        }

        ~Object() {
            // std::cout << "~Object::Object()" << std::endl;
        }

        Object(const Object&) { std::cout << "Object::Object(const Object&)" << std::endl; }
        Object(Object&&) noexcept { std::cout << "Object::Object(Object&&)" << std::endl; }

        Object& operator=(const Object&) {
            std::cout << "Object::Object(const Object&)" << std::endl;
            return *this;
        }

        Object& operator=(Object&&) noexcept {
            std::cout << "Object::Object(Object&&)" << std::endl;
            return *this;
        }
    };

    Object tmp;

    Object& getObject() {
        return tmp;
    }

    void Test_GetReference()
    {
        Object& _ = getObject();
    }

    void Test_LoseReference_Copy()
    {
        // 'auto obj' --> 'Object obj' [not a ref]
        auto obj = getObject();
    }
}

namespace Auto::Deduction_Rules
{
    int foo(int x){
        return x;
    }

    void examples()
    {
        {
            // auto a = 5, b = 0.1;  /** Compiler error **/
        }

        {
            auto a { 0uz };
            static_assert(std::is_same_v<decltype(a), size_t>);
        }

        {
            int x;
            auto a = &x; // --> int*
            static_assert(std::is_same_v<decltype(a), int*>);
        }

        {
            int x[5];
            auto a = x; // --> int*
            static_assert(std::is_same_v<decltype(a), int*>);
        }

        {
            auto a = nullptr; // --> nullptr_t
            static_assert(std::is_same_v<decltype(a), nullptr_t>);
        }

        {
            auto a = {1, 2, 3}; // std::initializer_list<int>>
            static_assert(std::is_same_v<decltype(a), std::initializer_list<int>>);
        }

        {
            auto a = {17}; // std::initializer_list<int>>
            static_assert(std::is_same_v<decltype(a), std::initializer_list<int>>);

        }

        {
            // auto a {1, 2, 3};  /** Compiler error **/
        }

        {
            auto a = foo;  /// ---> Function pointer: int(*)(int
            static_assert(std::is_same_v<decltype(a), int(*)(int) >);
        }

        {
            decltype(auto) a = (foo);
            static_assert(std::is_same_v<decltype(a), int(&)(int) >);
        }

        {
            volatile const int x = 1;
            auto a = x;
            static_assert(std::is_same_v<decltype(a), int>);

            // We strip all modifiers off x, so that results in an int.
            // Thus, auto is deduced as int. auto always drops top-level CV qualifiers.
        }

        {
            volatile const int x = 1;
            decltype(auto) a = x;
            static_assert(std::is_same_v<decltype(a), volatile const int>);
        }

        {
            volatile const int x = 1;
            auto a = x;
            static_assert(std::is_same_v<decltype(a), int>);
        }

        {
            const int& x{42};
            auto& a = x;
            static_assert(std::is_same_v<decltype(a), const int&>);
        }

        {
            int x;
            int& y = x;
            auto a = y;

            static_assert(std::is_same_v<decltype(a), int>);
        }

        {
            int x;
            auto& a = x;
            static_assert(std::is_same_v<decltype(a), int&>);
        }

        {
            int x;
            auto&& a = x;

            static_assert(std::is_same_v<decltype(a), int&>);
            // auto&& v is a forwarding reference. x is an lvalue and lvalues bind to lvalue references. Here, we get an lvalue reference.
        }

        {
            auto func = [] () -> int {
                return 1;
            };

            auto&& a = func();
            static_assert(std::is_same_v<decltype(a), int&&>);

            // The return value of the function call is a prvalue.
            // It will bind to an rvalue reference. So, the type of v is deduced as int&&.
        }

        {
            int x = 0;
            auto func = [&] () -> int& {
                return x;
            };

            auto&& a = func();
            static_assert(std::is_same_v<decltype(a), int&>);

            // The result of the function call y() is an lvalue, so the type of v is deduced to be int&.
        }

        {
            constexpr int x = 0;
            auto a = (x);

            static_assert(std::is_same_v<decltype(a), int>);
        }
        {
            int x = 0;
            decltype(auto) a = (x);

            static_assert(std::is_same_v<decltype(a), int&>);
            // The expression (x) is an lvalue, so decltype(auto) deduces to an lvalue reference of type int&.
        }

        {
            int x = 0;
            auto a = std::move(x);
            static_assert(std::is_same_v<decltype(a), int>);
        }

        {
            int x;
            decltype(auto) a = std::move(x);
            static_assert(std::is_same_v<decltype(a), int&&>);
        }

        {
            struct Base {
                auto foo() {
                    return this;
                };
            };

            class Derived : public Base {};

            Derived d;
            auto a = d.foo();

            static_assert(std::is_same_v<decltype(a), Base*>);
        }
    }
}


void Auto::TestAll()
{
    // Auto::SimpleTest();
    // Test_GetReference();
    // Test_LoseReference_Copy();


    Deduction_Rules::examples();
};

/** https://www.volatileint.dev/posts/auto-type-deduction-gauntlet/#advanced **/

/**  ------------------------------------ Basics ------------------------------------

auto v = 5;

    Type: int
    Explanation: Straightforward type deduction from an integer constant.

auto v = 0.1;

    Type: double
    Explanation: Notably different than integers. Floating points default to the larger double instead of float.

int x;               auto v = x;

    Type: int
    Explanation: Simple type derived from the assigned-from variable.

auto v = 5, w = 0.1;

    Type: Fails to compile.
    Explanation: All types in an expression defined with auto have to be the same.

int x;              auto v = &x;

    Type: int*
    Explanation: Auto will deduce pointers.

auto v = nullptr;

    Type: std::nullptr_t
    Explanation: nullptr has its own type.

auto v = { 1, 2, 3 };

    Type: std::initializer_list<int>
    Explanation: It might seem like this should create a container, but it won’t!

int x[5];           auto v = x;

    Type: int*
    Explanation: C-style arrays decay to a pointer. The decay happens before auto is evaluated.

int foo(int x) {     auto v = foo;
    return x;
}

    Type: int (*) (int)
    Explanation: auto can deduce function pointers.
**/

/**  ------------------------------------ Intermediate ---------------------------------------

volatile const int x = 1;     auto v = x;

    Type: int
    Explanation: auto drops top-level CV qualifiers.

volatile const int x = 1;      auto v = &x;

    Type: volatile const int*
    Explanation: CV qualifiers applied to pointed-to or referred-to types are maintained.

int x;       int& y = x;       auto v = y;

    Type: int
    Explanation: auto will never deduce a reference on its own.

int x;                         auto& v = x;

    Type: int&
    Explanation: lvalue references are deduced via auto&.

int x[5];                      auto& v = x;

    Type: int (&) [5]
    Explanation: When binding arrays to references, they don’t decay. So auto deduces the actual array type.

int foo(const int x) {          auto v = foo;
    return x;
}

    Type: int (*) (int)
    Explanation: Remember - CV qualifiers on parameters are thrown away during function resolution!
**/

/** -------------------------------------- Advanced--------------------------------------------

int x;    auto&& v = x;

    Type: int&
    Explanation: A forwarding reference like auto&& can bind to lvalue or rvalue expressions. Here we get an lvalue reference because x is an lvalue.

auto x = [] () -> int {
    return 1;
};
auto&& v = x();

    Type: int&&
    Explanation: x() returns a prvalue, and prvalues assigned to forwarding references yield an rvalue reference.

int x;         auto y = [&] () -> int& {            auto&& v = y();
                  return x;
               };

    Type: int&
    Explanation: This time x() returns an lvalue, and lvalues assigned to forwarding references yields an lvalue reference.

int x;          decltype(auto) v = (x);

    Type: int&
    Explanation: (x) is an expression. decltype(expression) yields an lvalue reference when the expression is an lvalue.

struct Foo {};        auto&& v = Foo{};

    Type: Foo&&
    Explanation: prvalues like Foo{} will bind to an rvalue reference.

struct Foo {};       decltype(auto) v = Foo{};

    Type: Foo
    Explanation: For any prvalue expression e, decltype(e) evaluates to the type of e.

int x;               decltype(auto) v = std::move(x);

    Type: int&&
    Explanation: For any xvalue expression e, decltype(e) evalutes to an rvalue reference to the type of e.

int foo(int x) {       decltype(auto) v = foo;
    return x;
}

    Type: Fails to compile.
    Explanation: Function id-expressions do not decay to pointers when evaluating with decltype!

int foo(int x) {        decltype(auto) v = (foo);
    return x;
}

    Type: int (&) (int)
    Explanation: Parenthesized function symbol expressions are deduced as a reference to the function.


struct Base {                  struct Derived : public Base {           Derived d;
    auto foo() {               };                                       auto v = d.foo();
        return this;
    };
};

    Type: Base*
    Explanation: foo is defined in the Base class, so this has to refer to a Base*, even when foo is called from a child class.
**/