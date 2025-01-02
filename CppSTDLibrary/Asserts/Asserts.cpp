/**============================================================================
Name        : Asserts.cpp
Created on  : 06.07.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <cassert>
#include <functional>
#include <string_view>
#include <memory>

#include "Asserts.h"

namespace Asserts::StaticAsserts
{
    template <typename T, size_t Size>
    class Vector
    {
        static_assert(Size > 3, "Size is too small");
        T points[Size] {};
    };

    template <typename T1, typename T2>
    auto add(T1 t1, T2 t2) -> decltype(t1 + t2)
    {
        static_assert(std::is_integral_v<T1>, "Type T1 must be integral");
        static_assert(std::is_integral_v<T2>, "Type T2 must be integral");

        return t1 + t2;
    }

    void Test1()
    {
        [[maybe_unused]]
        Vector<int, 16> a1;

        [[maybe_unused]]
        Vector<double, 5> a2;

        // Here we'll get error if size = 3

        //[[maybe_unused]]
        //Vector<double, 3> a3;
    }

    void Test2()
    {
        // Compile ERROR !!!!
        // std::cout << add(1, 3.14) << std::endl;
        //std::cout << add("one", 2) << std::endl;

        // Compile OK
        std::cout << add(1, 2) << std::endl;
        std::cout << add(3, 2) << std::endl;
    }

    class A {};

    template <class T>
    T f(T i) {
        static_assert(std::is_integral_v<T>, "Integer type required");
        return i;
    }

    void Test3()
    {
        std::cout << std::boolalpha;
        std::cout << std::is_integral_v<A> << std::endl;
        std::cout << std::is_integral_v<float> << std::endl;
        std::cout << std::is_integral_v<int> << std::endl;
        std::cout << f(123) << std::endl;

        // ERROR
        // std::cout << f("") << std::endl;
    }

    void Assert_With_Constexpr_Formated_Message()
    {
        static_assert(sizeof(A) == 1, std::format("Unexpected sizeof: expected 1, got {}", sizeof(A)));
    }
};

namespace Asserts::Assert_RunTime
{

    void Test1()
    {
        assert(2 + 2 == 4);
        std::cout << "Assert 1 OK" << std::endl;

        assert(2 + 2 == 5);
        std::cout << "Assert 2 OK" << std::endl;
    }

    void Test2()
    {
        assert(2 + 2 == 4), "Error text 1";
        std::cout << "Assert 1 OK" << std::endl;

        assert(2 + 2 == 5), "Error text 2";
        std::cout << "Assert 2 OK" << std::endl;
    }

    void Test3()
    {
        assert((2 * 2 == 4) && "TEXT_1");
        assert((2 * 2 == 5) && "TEXT_2");
    }

    void Test4()
    {
        assert((2 * 2 > 4) && "ERRR");
    }

    void Test5()
    {
        std::unique_ptr<int> intPtr = std::make_unique<int>(123);
        std::cout << *intPtr << std::endl;
        assert(ptr && "Ptr is NULL");

        std::unique_ptr<int> intPtrNull = nullptr;
        assert(intPtrNull && "Ptr is NULL");
    }
}

void Asserts::TestAll()
{
    // StaticAsserts::Test1();
    // StaticAsserts::Test2();
    // StaticAsserts::Test3();
    StaticAsserts::Assert_With_Constexpr_Formated_Message();

    // Assert_RunTime::Test1();
    // Assert_RunTime::Test2();
    // Assert_RunTime::Test3();
    // Assert_RunTime::Test4();
    // Assert_RunTime::Test5();
}
