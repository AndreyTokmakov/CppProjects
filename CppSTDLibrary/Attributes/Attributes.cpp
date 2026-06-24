//============================================================================
// Name        : Attributes.cpp
// Created on  : 26.12.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ attributes src
//============================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <functional>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <numeric>
#include <list>
#include <vector>
#include <iterator>
#include <chrono>
#include <memory>
#include <thread>
#include <format>

#include "Attributes.h"
#include "Assume.h"
#include "../Helpers/Helpers.h"

namespace Attributes::Fallthrough {

    enum class Color {
        black = 0,
        white = 1,
        white1 = 1,
        gray = 2,
    };

    void PrintColor(Color color) {
        switch (color) {
            case Color::gray:
                std::cout << "white                             [[fallthrough]]" << std::endl;
                [[fallthrough]];
            case Color::black:
                std::cout << "black" << std::endl;
                break;
            case Color::white:
                std::cout << "white" << std::endl;
                break;
        }
    }

    void Fallthrough_Test() {
        std::cout << "Color::black" << std::endl;
        const Color color1 = Color::black;
        PrintColor(color1);

        std::cout << "\nColor::black" << std::endl;
        const Color color2 = Color::white;
        PrintColor(color2);

        std::cout << "\nColor::gray" << std::endl;
        const Color color3 = Color::gray;
        PrintColor(color3);
    }
};


namespace Attributes::Nodiscard
{

    [[nodiscard]]
    unsigned short GetResultCode() {
        return 101;
    }

    [[nodiscard]]
    auto allocate_memory() {
        return "Here is your memory. Don't forget to release it!";
    }

    unsigned short DoSystemCall() {
        return 1;
    }

    void NoDiscardTest(void) {
        // We should get the WARNING here
#if 0
        GetResultCode();
#endif

        // No WARNING here
        auto resultCode = GetResultCode();
        (void)resultCode;
    }


    class [[nodiscard("Please do not discard Base object. They fill so alone....")]]
    Base {
    };


    class Object {
    public:
        [[nodiscard("Please do not discard Object objects too.")]]
        Object() {}
    };

    auto getBaseObject() {
        return Base();
    }

    void NoDiscardTest2() {
        /*
        auto mem = allocate_memory();
        std::cout << mem << std::endl;
        */

        allocate_memory();
    }

    struct [[nodiscard("Please check error code")]] Error {
    };

    Error function() {
        return {};
    }


    void NoDiscard_Class() {
        getBaseObject();
    }

    void NoDiscard_Class2() {
        Base();
    }

    void NoDiscard_Class_IgnoreError() {
        function(); // // Warning, ignoring return value marked with nodiscard
    }


    auto getObject() {
        return Object();
    }

    void acquire_resource() {}
    void release_resource() {}

    struct ResourceHandle
    {
        [[nodiscard("Memory leak")]]
        ResourceHandle() { acquire_resource(); };
        ~ResourceHandle() { release_resource(); }
    };

    void NoDiscard_Constructor() {
        Object();
    }

    void NoDiscard_Constructor2() {
        getObject();
    }

    void NoDiscard_Constructor3() {
                          // Warning, ignoring constructor declared with nodiscard
        ResourceHandle{}; // resource acquired and immediately released
    }

    void MyBeUnused()
    {
        // We should get the 'Unused variable' WARNING here
        [[maybe_unused]]
        auto resultCode1 = DoSystemCall();

        // No WARNING here
        [[maybe_unused]]
        auto resultCode2 = DoSystemCall();
    }


    //==============================================================

    enum class [[nodiscard("Dont forget to check result")]] ErrorCode {
        OK,
        Fatal,
        System,
        FileIssue
    };

    ErrorCode OpenFile(std::string_view fileName) {
        return ErrorCode::OK;
    }

    void Test_Discard_Enum_Return_Value()
    {
        OpenFile("21324");
    }
}


namespace Attributes::MyBeUnused {

    void foo(int val, [[maybe_unused]] std::string msg)
    {
#ifdef DEBUG
        log(msg);
#endif
    }

    class MyStruct {
        char c;
        int i;

        [[maybe_unused]]
        char makeLargerSize[100];
    };
}


namespace Attributes:: Deprecated { //[[deprecated]]

    // We can deprecate whole namespace
    namespace [[deprecated]] OldApi {

        const unsigned int getSomeConstant() {
            return 123;
        }
    }


    enum class City {
        Berlin = 0,
        NewYork = 1,
        Mumbai = 2,
        Bombay [[deprecated]] = Mumbai,
    };



    void Namespace() {
#if 0
        auto value = OldApi::getSomeConstant();
#endif
    }

    void Enum() {
        City city1 = City::Berlin; // OK

#if 0
        City city2 = City::Bombay;
#endif
    }
}

namespace Attributes::NoUniqueAddress
{
    struct Empty {
    }; // empty class

    struct X {
        int i;
        Empty e;
    };

    struct Y {
        int i;
        [[no_unique_address]] Empty e;
    };

    struct Z {
        char c;
        [[no_unique_address]] Empty e1, e2;
    };

    struct W {
        char c[2];
        [[no_unique_address]] Empty e1, e2;
    };

    void Test() {
        // the size of any object of empty class type is at least 1
        static_assert(sizeof(Empty) >= 1);

        // at least one more byte is needed to give e a unique address
        static_assert(sizeof(X) >= sizeof(int) + 1);

        // empty member optimized out
        std::cout << "sizeof(Y) == sizeof(int) is " << std::boolalpha
                  << (sizeof(Y) == sizeof(int)) << '\n';

        // e1 and e2 cannot share the same address because they have the same type, even though they are
        // marked with [[no_unique_address]]. However, either may share address with c.
        static_assert(sizeof(Z) == 2);

        // e1 and e2 cannot have the same address, but one of them can share with c[0] and the other with c[1]
        //static_assert(sizeof(W) == 2);
    }

    //--------------------------------------------------------------

    template<typename T, typename Deleter>
    class UniquePtrBad
     {
        T *pointer = nullptr;
        Deleter deleter {};

    public:
        /** Some logic **/

        ~UniquePtrBad() {
            deleter(pointer);
        }
    };

    template<typename T, typename Deleter>
    class UniquePtrGood
    {
        T *pointer = nullptr;

        [[no_unique_address]]
        Deleter deleter {};

    public:
        /** Some logic **/

        ~UniquePtrGood() {
            deleter(pointer);
        }
    };

    template<typename T>
    struct Deleter
    {
        void operator()(T *ptr) {
            delete ptr;
        }
    };

    template<typename T>
    struct DeleterNotEmpty
    {
        int value {0};

        void operator()(T *ptr) {
            delete ptr;
        }
    };


    void Test2()
    {
        using Type = std::string;
        using Deleter = Deleter<Type>;
        using DeleterNotEmpty = DeleterNotEmpty<Type>;

        UniquePtrBad<Type, Deleter> ptrBad1;
        UniquePtrBad<Type, DeleterNotEmpty> ptrBad2;

        UniquePtrGood<Type, Deleter> ptrGood1;
        UniquePtrGood<Type, DeleterNotEmpty> ptrGood2;

        static_assert(sizeof(ptrBad1) == 2 * sizeof(Type*));
        static_assert(sizeof(ptrBad2) == 2 * sizeof(Type*));

        static_assert(sizeof(ptrGood1) == sizeof(Type*));     /// <--- Here is the profit
        static_assert(sizeof(ptrGood2) == 2 * sizeof(Type*));
    }


    struct EmptyClass {};
    struct HasMember { int m; };

    // The listed values are valid for x86_64/Linux/Clang and may differ on other platforms.

    template <typename Member>
    struct Basic
    {
        Member m;
        int v;
    };

    // sizeof(Basic<Empty>) == 8 ((1+3) + 4)
    // sizeof(Basic<HasMember>) == 8 (4 + 4)

    template <typename Member>
    struct Compressed
    {
        [[no_unique_address]] Member m;
        int v;
    };

    // sizeof(Compressed<Empty>) == 4 (0 + 4)
    // sizeof(Compressed<HasMember>) == 8 (4 + 4)

    template <typename Member>
    struct MaybeEmpty
    {
        [[no_unique_address]] Member m;
    };

    void Demo()
    {
        std::cout << std::format("sizeof(Basic<Empty>) == {}\n", sizeof(Basic<Empty>));                   // --> 8
        std::cout << std::format("sizeof(Basic<HasMember>) == {}\n", sizeof(Basic<HasMember>));           // --> 8

        std::cout << std::format("sizeof(Compressed<Empty>) == {}\n", sizeof(Compressed<Empty>));         // --> 4
        std::cout << std::format("sizeof(Compressed<HasMember>) == {}\n", sizeof(Compressed<HasMember>)); // --> 8

        std::cout << std::format("sizeof(MaybeEmpty<Empty>) == {}\n", sizeof(MaybeEmpty<Empty>));         // --> 1
        std::cout << std::format("sizeof(MaybeEmpty<HasMember>) == {}\n", sizeof(MaybeEmpty<HasMember>)); // --> 4
    }
}


void Attributes::TestAll()
{

    // Fallthrough::Fallthrough_Test();

    // Nodiscard::NoDiscardTest();
    // Nodiscard::NoDiscardTest2();

    // Nodiscard::NoDiscard_Class();
    // Nodiscard::NoDiscard_Class2();
    // Nodiscard::NoDiscard_Class_IgnoreError();

    // Nodiscard::NoDiscard_Constructor();
    // Nodiscard::NoDiscard_Constructor2();
    // Nodiscard::NoDiscard_Constructor3();

    // Nodiscard::Test_Discard_Enum_Return_Value();

    // Deprecated::Namespace();
    // Deprecated::Enum();

    // NoUniqueAddress::Test();
    // NoUniqueAddress::Test2();
    // NoUniqueAddress::Demo();

    Assume::TestAll();
};