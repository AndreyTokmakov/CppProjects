//============================================================================
// Name        : Tests.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Tests C++ project
//============================================================================

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>
#include <ranges>

#include <thread>
#include <future>

#include <mutex>
#include <atomic>

#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>

#include <utility>
#include <vector>
#include <any>
#include <list>
#include <map>
#include <algorithm>
#include <array>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <span>
#include <random>
#include <execution>

// #include <format>
#include <concepts>

#include "Integer.h"
#include "MemoryPool.h"

#include "Collections/Collections.h"
#include "VirtualCall_vs_TypeErasure/VirtualCall_vs_TypeErasure.h"


using Utilities::Integer;

namespace Utilities {

    class Long {
    protected:
        int value;

    public:
        Long() : value(0) {
        }

        explicit Long(int val) : value(val) {
        }

        Long(const Long &obj) {
            this->value = obj.value;
        }

        Long& operator=(const Long& right) {
            if (&right != this) {
                value = right.value;
            }
            return *this;
        }

        Long(Long && obj) noexcept : value(std::exchange(obj.value, 0)) {
        }

        Long& operator=(Long&& right) noexcept {
            if (this != &right) {
                this->value = std::exchange(right.value, 0);
            }
            return *this;
        }

        virtual ~Long() {
        }
    };
}

namespace PerformanceTests {

#define START_TIME_MEASURE auto start = std::chrono::high_resolution_clock::now();
#define STOP_TIME_MEASURE  { auto end = std::chrono::high_resolution_clock::now(); \
                           auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
						   std::cout << "Result: " << duration << " microseconds" << std::endl;}


#define STOP_TIME_MEASURE_EX(T)  { auto end = std::chrono::high_resolution_clock::now(); \
                           auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
						   std::cout << "Result: " << duration << " microseconds" << std::endl; }


#define FUNC_CHOOSER(_f1, _f2, _f3, ...) _f3
#define FUNC_RECOMPOSER(argsWithParentheses) FUNC_CHOOSER argsWithParentheses

#define PRINT_RESULT_1(x)    STOP_TIME_MEASURE_EX(x)
#define PRINT_RESULT_0()     PRINT_RESULT_1("")

#define CHOOSE_FROM_ARG_COUNT(...) FUNC_RECOMPOSER((__VA_ARGS__, PRINT_RESULT_1, ))
#define NO_ARG_EXPANDER() ,,PRINT_RESULT_0
#define MACRO_CHOOSER(...) CHOOSE_FROM_ARG_COUNT(NO_ARG_EXPANDER __VA_ARGS__ ())
#define PRINT_RESULT(...) MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)


    /*
    #define FUNC_CHOOSER(_f1, _f2, _f3, ...) _f3
    #define FUNC_RECOMPOSER(argsWithParentheses) FUNC_CHOOSER argsWithParentheses

    #define PRINT_RESULT_2(x, y) PRINT_RESULT_INTERNAL(x, y)
    #define PRINT_RESULT_1(x)    PRINT_RESULT_2("", "")
    #define PRINT_RESULT_0()     PRINT_RESULT_1("")

    #define CHOOSE_FROM_ARG_COUNT(...) FUNC_RECOMPOSER((__VA_ARGS__, PRINT_RESULT_2, PRINT_RESULT_1, ))
    #define NO_ARG_EXPANDER() ,,PRINT_RESULT_0
    #define MACRO_CHOOSER(...) CHOOSE_FROM_ARG_COUNT(NO_ARG_EXPANDER __VA_ARGS__ ())
    #define PRINT_RESULT(...) MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
    #define PRINT_RESULT PRINT_RESULT()
    */
}

//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::Buffers {

#pragma optimize( "", off )
    void ArrayOfBytes_vs_CharBuffer()
    {
        constexpr int COUNT = 100000000;
        constexpr int SIZE = 1000;

        {
            std::array<std::byte, SIZE> buffer{};
            {
                std::byte b{};
                START_TIME_MEASURE;
                for (int n = 0; n < COUNT; n++) {
                    for (int i = 0; n < COUNT; n++) {
                        buffer[i];
                    }
                }
                STOP_TIME_MEASURE;
            }
        }

        {
            char buffer[SIZE];
            {
                char c;
                START_TIME_MEASURE;
                for (int n = 0; n < COUNT; n++) {
                    for (int i = 0; n < COUNT; n++) {
                        buffer[i];
                    }
                }
                STOP_TIME_MEASURE;
            }
        }

        {
            std::array<std::byte, SIZE> buffer{};
            {
                std::byte b{};
                START_TIME_MEASURE;
                for (int n = 0; n < COUNT; n++) {
                    for (int i = 0; n < COUNT; n++) {
                        buffer[i];
                    }
                }
                STOP_TIME_MEASURE;
            }
        }
    }
#pragma optimize( "", on )
}

//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::Span {

    void handle_vector_copy(std::vector<int> v) {
        auto x = v.size();
    }

    void handle_vector_ref(const std::vector<int>& v) {
        auto x = v.size();
    }

    void handle_vector_ptr(const std::vector<int>* v) {
        auto x = v->size();
    }

    void handle_vector_span(const std::span<int> v) {
        auto x = v.size();
    }

#pragma optimize( "", off )
    void Pass_Vector_As_Param()
    {
        constexpr int count = 10000000;
        constexpr int size = 1000;
        std::vector<int> numbers = [=]() {
            std::vector<int> vect(size);
            for (int i = 0; i < size; i++)
                vect[i] = i;
            return vect;
        }();

        {
            START_TIME_MEASURE;
            for (int n = 0; n < count; n++) {
                handle_vector_copy(numbers);
            }
            STOP_TIME_MEASURE;
        }

        {
            const std::vector<int>& vec_ref = numbers;
            START_TIME_MEASURE;
            for (int n = 0; n < count; n++) {
                handle_vector_ref(vec_ref);
            }
            STOP_TIME_MEASURE;
        }

        {
            auto ptr = &numbers;
            START_TIME_MEASURE;
            for (int n = 0; n < count; n++) {
                handle_vector_ptr(ptr);
            }
            STOP_TIME_MEASURE;
        }

        // SPAN:

        {
            START_TIME_MEASURE;
            for (int n = 0; n < count; n++) {
                handle_vector_span(numbers);
            }
            STOP_TIME_MEASURE;
        }

        {
            std::span<int> sp(numbers);
            START_TIME_MEASURE;
            for (int n = 0; n < count; n++) {
                handle_vector_span(sp);
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )
}


//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::Ranges {

#pragma optimize( "", off )
    void Filter_Test() {

        constexpr int size = 1000;
        const std::vector<int> numbers = [=]() {
            std::vector<int> vect(size);
            for (int i = 0; i < size; i++)
                vect[i] = i;
            return vect;
        }();

        {
            int sum = 0;
            START_TIME_MEASURE;
            for (int n = 0; n < 1000000; n++) {
                for (int i = 0; i < size; i++) {
                    if (0 == numbers[i] % 2)
                        sum += numbers[i];
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            int sum = 0;

            START_TIME_MEASURE;
            for (int n = 0; n < 1000000; n++) {
                for (auto v : numbers | std::ranges::views::filter([](auto v){ return 0 == v % 2; })) {
                    sum += v;
                }
            }
            STOP_TIME_MEASURE;
        }

        /*
        constexpr std::array<int, 10> numbers = build_array<int, 10>();
        auto evens = numbers | std::ranges::views::filter([](int v) -> bool { return 0 == v % 2; });
        std::ranges::for_each(evens, [](auto v) { std::cout << v << ' '; });
        std::cout << "\n";
        */
    }
#pragma optimize( "", on )
}

//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::Pass_ConstStringRef_AsParameter {

    class Foo {
        int i;
        std::string s;
    public:
        template<typename STR>
        Foo(int i, STR&& s) : i(i), s(std::forward<STR>(s)) { }
    };

    class Foo2 {
        int i;
        std::string s;
    public:
        template<typename... T>
        explicit Foo2(int i, T&&... t) : i(i), s(std::forward<T>(t)...) { }
    };

    class Foo3 {
        int i;
        std::string s;
    public:
        Foo3(int i, std::string str) : i(i), s(std::move(str)) {}
    };

    class Foo4 {
        int i;
        std::string s;
    public:
        Foo4(int i, std::string&& str) : i(i), s(std::move(str)) {}
    };

    class FooBad {
        int i;
        std::string s;
    public:
        FooBad(int i, const std::string& str) : i(i), s(str) {}
    };


    //------------------------------------------------------------------------//

    void Construct_ForwardTest()
    {
        volatile unsigned int N1 = 10000, N2 = 10000;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    Foo f(1, "Test");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    Foo2 f(1, "Test");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    Foo3 f(1, "Test");
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    Foo4 f(1, "Test");
                }
            }
            STOP_TIME_MEASURE;
        }


        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    FooBad f(1, "Test");
                }
            }
            STOP_TIME_MEASURE;
        }
    }
    //-------------------------------------------------------------------------------------------------------//

    class Bar {
        std::vector<Foo> foos;
    public:
        template<typename... T>
        void add(T&&... t) {
            foos.emplace_back(std::forward<T>(t)...);
        }
    };

    class BarBad {
        std::vector<FooBad> foos;
    public:
        void add(int i, const std::string& s) {
            foos.emplace_back(i, s);
        }
    };

    class BarX {
        std::vector<Foo2> foos;
    public:
        void add(int i, std::string s) {
            foos.emplace_back(i, s);
        }
    };

    void Emplace_StringParameter_ToVector()
    {
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 1000; i++) {
                Bar bar;
                for (int n = 0; n < 100000; n++) {
                    bar.add(1, "TET");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 1000; i++) {
                BarBad bar;
                for (int n = 0; n < 100000; n++) {
                    bar.add(1, "TET");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 1000; i++) {
                BarX bar;
                for (int n = 0; n < 100000; n++) {
                    bar.add(1, "TET");
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}

//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::StringBasedObject {

    class TestClass0 {
        std::string s;

    public:
        TestClass0(std::string&& str) : s(str) { }
    };

    class TestClass01 {
        std::string s;

    public:
        TestClass01(std::string&& str) : s(std::move(s)) { }
    };

    class TestClass1 {
        std::string s;

    public:
        template<typename STR>
        TestClass1(STR&& s) : s(std::forward<STR>(s)) { }
    };

    class TestClass2 {
        std::string s;

    public:
        template<typename... T>
        TestClass2(T&&... t) : s(std::forward<T>(t)...) { }
    };

    class TestClass3 {
        std::string s;

    public:
        TestClass3(const std::string& str) : s(str) {}
    };

    class TestClass4 {
        std::string s;

    public:
        TestClass4(std::string str) : s(std::move(s)) {}
    };

    class TestClass5 {
        std::string s;

    public:
        TestClass5(const char* buff) : s(buff) {}
    };

    class TestClass6 {
        std::string s;

    public:
        TestClass6(std::string_view str) : s { str .data()} {}
    };

#pragma optimize( "", off )
    void RunTest()
    {
        volatile unsigned int N1 = 10000, N2 = 10000;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    TestClass0 f("Test");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    TestClass01 f("Test");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    TestClass1 f("Test");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    TestClass2 f("Test");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    TestClass3 f("Test");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    TestClass4 f("Test");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    TestClass5 f("Test");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    TestClass6 f("Test");
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )

    void RunTest_LocalString()
    {
        volatile unsigned int N1 = 10000, N2 = 10000;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    std::string str("Test_111111111111111111111111111111111111");
                    TestClass0 f(std::move(str));
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    std::string str("Test_111111111111111111111111111111111111");
                    TestClass01 f(std::move(str));
                }
            }
            STOP_TIME_MEASURE;
        }



        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    std::string str("Test_111111111111111111111111111111111111");
                    TestClass1 f(std::move(str));
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    std::string str("Test_111111111111111111111111111111111111");
                    TestClass2 f(std::move(str));
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    std::string str("Test_111111111111111111111111111111111111");
                    TestClass3 f(str);
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    std::string str("Test_111111111111111111111111111111111111");
                    TestClass4 f(str);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    std::string str("Test_111111111111111111111111111111111111");
                    TestClass4 f(std::move(str));
                }
            }
            STOP_TIME_MEASURE;
        }



        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    char buffer[] = "Test_111111111111111111111111111111111111";
                    TestClass5 f(buffer);
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}

//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::SharedPtr_PassTests {

    class Base {
    private:
        int val = 0;

    public:
        Base(int v) : val(v) {
        }
    };

    class Derived : public Base {
    public:
        Derived(int v) : Base(v) {
        }
    };


    void IncrementByValue(std::shared_ptr<Utilities::Integer> int_var) {
        //int_var->increment();
        //std::cout << "use_count = " << int_var.use_count() << std::endl;
    }

    void IncrementByValue_Weak(std::weak_ptr<Utilities::Integer> int_var) {
        //int_var.get
        //std::cout << "use_count = " << int_var.use_count() << std::endl;
    }

    void IncrementByRef(std::shared_ptr<Integer>& int_var) {
        int_var->increment();
        //std::cout << "use_count = " << int_var.use_count() << std::endl;
    }

    void IncrementByRef_WEAK(std::weak_ptr<Utilities::Integer> int_var) {
        //int_var.get
        //std::cout << "use_count = " << int_var.use_count() << std::endl;
    }

    void HandleBase(std::shared_ptr<Base> ptr) {
    }

    void HandleDerived(std::shared_ptr<Derived> ptr) {
    }


    void PassByValue_ToBaseClass() {
        START_TIME_MEASURE;
        for (int i = 0; i < 10; i++) {
            std::shared_ptr<Derived> ptr = std::make_shared<Derived>(0);
            for (int n = 0; n < 100000000; n++) {
                HandleBase(ptr);
            }
        }
        STOP_TIME_MEASURE;
    }

    void PassByValue_ToDerivedClass() {
        START_TIME_MEASURE;
        for (int i = 0; i < 10; i++) {
            std::shared_ptr<Derived> ptr = std::make_shared<Derived>(0);
            for (int n = 0; n < 100000000; n++) {
                HandleDerived(ptr);
            }
        }
        STOP_TIME_MEASURE;
    }

    void Pass_ByValue_vs_PassRef() {
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10; i++) {
                std::shared_ptr<Integer> int_var = std::make_shared<Integer>(0);
                for (int n = 0; n < 100000000; n++) {
                    IncrementByValue(int_var);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10; i++) {
                std::shared_ptr<Integer> int_var = std::make_shared<Integer>(0);
                for (int n = 0; n < 100000000; n++) {
                    IncrementByRef(int_var);
                }
            }
            STOP_TIME_MEASURE;
        }
    }

    void Pass_ByValue_vs_PassRef_WEAK() {
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10; i++) {
                std::shared_ptr<Integer> int_var = std::make_shared<Integer>(0);
                for (int n = 0; n < 100000000; n++) {
                    IncrementByValue(int_var);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10; i++) {
                std::shared_ptr<Integer> int_var = std::make_shared<Integer>(0);
                for (int n = 0; n < 100000000; n++) {
                    IncrementByValue_Weak(int_var);
                }
            }
            STOP_TIME_MEASURE;
        }
    }

    void Put_SharedPtr_To_Vector() {
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000; i++) {
                std::vector<std::shared_ptr<Integer>> sharedIntegers;
                for (int n = 0; n < 100; n++) {
                    sharedIntegers.push_back(std::make_shared<Integer>(i));
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000; i++) {
                std::vector<std::shared_ptr<Integer>> sharedIntegers;
                for (int n = 0; n < 100; n++) {
                    sharedIntegers.emplace_back(std::make_shared<Integer>(i));
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000; i++) {
                std::vector<std::shared_ptr<Integer>> sharedIntegers;
                for (int n = 0; n < 100; n++) {
                    sharedIntegers.emplace_back(new Integer(i));
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000; i++) {
                std::vector<std::shared_ptr<Integer>> sharedIntegers;
                for (int n = 0; n < 100; n++) {
                    std::shared_ptr<Integer> x = sharedIntegers.emplace_back(std::make_shared<Integer>(i));
                }
            }
            STOP_TIME_MEASURE;
        }
    }

#pragma optimize( "", off )
    void Create_Shared_vs_New()
    {
        constexpr int N = 10000, M = 1000;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N; i++) {
                for (int n = 0; n < M; n++) {
                    auto ptr = std::make_shared<Integer>(n);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N; i++) {
                for (int n = 0; n < M; n++) {
                    auto ptr = std::make_unique<Integer>(n);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N; i++) {
                for (int n = 0; n < M; n++) {
                    auto *ptr = new Integer(n);
                    delete ptr;
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N; i++) {
                for (int n = 0; n < M; n++) {
                    Integer intger(n);
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )
};

//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::UniquedPtr_PassTests {

    void IncrementByRawPointer(Integer* int_var, int count) {
        if (0 == count)
            return;
        int_var->increment();
        IncrementByRawPointer(int_var, count - 1);
    }

    void IncrementByValue(std::unique_ptr<Integer> int_var, int count) {
        if (0 == count)
            return;
        int_var->increment();
        IncrementByValue(std::move(int_var), count - 1);
    }

    void IncrementByRef(std::unique_ptr<Integer>& int_var, int count) {
        if (0 == count)
            return;
        int_var->increment();
        IncrementByRef(int_var, count - 1);
    }

    struct Base {
        char buffer[1024 * 1024]{};
    };

#pragma optimize( "", off )
    void MakeUniquw_vs_NEW()
    {
        constexpr int TESTS_COUNT = 1'00;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < TESTS_COUNT; i++) {
                for (int n = 0; n < TESTS_COUNT; ++n) {
                    std::unique_ptr<Base> ptr{ std::unique_ptr< Base >(new Base()) };
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < TESTS_COUNT; i++) {
                for (int n = 0; n < TESTS_COUNT; ++n) {
                    std::unique_ptr<Base> ptr{ std::make_unique<Base>() };
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )

    void PassByRawPointer() {
        START_TIME_MEASURE;
        for (int i = 0; i < 100000; i++) {
            Integer* int_var = new Integer(0);
            IncrementByRawPointer(int_var, 10000);
            delete int_var;

        }
        STOP_TIME_MEASURE;
    }

    void PassByValueTest() {
        START_TIME_MEASURE;
        for (int i = 0; i < 100000; i++) {
            std::unique_ptr<Integer> int_var = std::make_unique<Integer>(0);
            IncrementByValue(std::move(int_var), 10000);

        }
        STOP_TIME_MEASURE;
    }

    void PassByRefTest() {

        START_TIME_MEASURE;
        for (int i = 0; i < 100000; i++) {
            std::unique_ptr<Integer> int_var = std::make_unique<Integer>(0);
            IncrementByRef(int_var, 10000);
        }
        STOP_TIME_MEASURE;
    }
};


//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::Const_vs_NonConst_Variables {

    constexpr int max_count = 10000;

    void ConstLocalVariable() {
        {
            START_TIME_MEASURE;
            for (int i = 0; i < max_count; i++) {
                for (int n = 0; n < max_count; n++) {
                    const std::string s = "SomeShortString22222222222222222222222222222222";
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < max_count; i++) {
                for (int n = 0; n < max_count; n++) {
                    std::string s = "SomeShortString22222222222222222222222222222222";
                }
            }
            STOP_TIME_MEASURE;
        }
    }

    void Find_in_ConstexprArray()
    {
        constexpr std::array<int, 1000> data = []()->std::array<int, 1000>  {
            std::array<int, 1000> x;
            std::iota(x.begin(), x.end(), 0);
            return x;
        }();


        {
            START_TIME_MEASURE;
            for (int i = 0; i < max_count; i++) {
                for (int n = 0; n < 100; n++) {
                    // constexpr
                    auto x = std::find_if(data.begin(), data.end(), [&](const auto x) { return x == 10; } );
                }
            }
            STOP_TIME_MEASURE;
        }

    }
};

//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::AccessVariable_Map_vs_ArrayOnStack {

    void AccessValueFromStdArray() {
        int tmp[256] = { 0 };
        tmp[123] = 222;

        START_TIME_MEASURE;
        for (int i = 0; i < 10000; i++) {
            volatile int v = tmp[123];
            for (int n = 0; n < 1000000; n++) {
                v = tmp[123];
            }
        }
        STOP_TIME_MEASURE;

    }

    void AccessValueFromMap() {
        std::unordered_set<int> tmp;
        int i = 256;
        while (i)
            tmp.insert(i--);

        START_TIME_MEASURE;
        for (int i = 0; i < 10000; i++) {
            auto it = tmp.find(123);
            for (int n = 0; n < 1000000; n++) {
                it = tmp.find(123);
            }
        }
        STOP_TIME_MEASURE;
    }
}

namespace PerformanceTests::Noexcept_Move_Performance {

    // TODO! !!!!!!!!!!!!!!!!!!!!!!!!!!!!

    class Widget {
    private:
        int i{ 0 };
        std::string s{};
        int* ptr{ nullptr };

    public:
        Widget() {
            ptr = new int(1);
        }
        Widget(int i, const std::string& str, int* ptr) : i(i), s(str), ptr(ptr) {
        }
        // Move constructor
        Widget(Widget&& w) noexcept : i(std::move(w.i)),
                                      s(std::move(w.s)),
                                      ptr(std::move(w.ptr)) {
            w.ptr = nullptr;
        }
    };


    class Widget_Except {
    private:
        int i{ 0 };
        std::string s{};
        int* ptr{ nullptr };

    public:
        Widget_Except() {
            ptr = new int(1);
        }
        Widget_Except(int i, const std::string& str, int* ptr) : i(i), s(str), ptr(ptr) {
        }
        // Move constructor
        Widget_Except(Widget_Except&& w) : i(std::move(w.i)),
                                           s(std::move(w.s)),
                                           ptr(std::move(w.ptr)) {
            w.ptr = nullptr;
        }
    };

    void Test()
    {
        {
            std::string s("Long string that needs to be copied");
            std::vector<Widget> v{};
            constexpr size_t N(1000000);

            std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
            start = std::chrono::high_resolution_clock::now();

            for (size_t i = 0UL; i < N; ++i) {
                Widget w{ 1, s, nullptr };
                v.push_back(std::move(w));
            }

            end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> elapsedTime(end - start);
            const double seconds(elapsedTime.count());

            std::cout << " Runtime: " << seconds << std::endl;
        }
        {
            std::string s("Long string that needs to be copied");
            std::vector<Widget_Except> v{};
            constexpr size_t N(1000000);

            std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
            start = std::chrono::high_resolution_clock::now();

            for (size_t i = 0UL; i < N; ++i) {
                Widget_Except w{ 1, s, nullptr };
                v.push_back(std::move(w));
            }

            end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> elapsedTime(end - start);
            const double seconds(elapsedTime.count());

            std::cout << " Runtime: " << seconds << std::endl;
        }
    }
}


//----------------------------------------------------------------------------------------------

namespace PerformanceTests::Move_Experiments {

    constexpr static int COUNT = 10'000;
    constexpr static int TESTS = 10'000;

    void Move_vs_Copy_VectorToMap() {

        {
            START_TIME_MEASURE;
            std::map<std::string, std::vector<Integer>> map{};
            for (int n = 0; n < TESTS; n++) {
                std::vector<Integer> items{};
                items.reserve(COUNT);
                for (int i = 0; i < COUNT; ++i) {
                    items.emplace_back(i);
                }
                map["key"] = std::move(items); // INFO: This is faster
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            std::map<std::string, std::vector<Integer>> map{};
            for (int n = 0; n < TESTS; n++) {
                std::vector<Integer> items{};
                items.reserve(COUNT);
                for (int i = 0; i < COUNT; ++i) {
                    items.emplace_back(i);
                }
                map["key"] = items;
                // map.c
            }
            STOP_TIME_MEASURE;
        }
    }
}

//----------------------------------------------------------------------------------------------//


namespace PerformanceTests::Exceptions
{

    const int randomRange = 50;  // Give me a number between 0 and 2.
    const int errorInt = 0;     // Stop every time the number is 0.

    int getRandom() {
        return rand() % randomRange;
    }

    void exitWithBasicException() {
        if (getRandom() == errorInt) {
            throw - 2;
        }
    }

    void exitWithMessageException() {
        if (getRandom() == errorInt) {
            throw std::runtime_error("Halt! Who goes there?");
        }
    }

    void exitWithReturn() {
        if (getRandom() == errorInt) {
            return;
        }
    }

    int exitWithErrorCode() {
        if (getRandom() == errorInt) {
            return -1;
        }
        return 0;
    }


    void Tests()
    {
        constexpr size_t MAX_COUNT = 1'000'000;
        {
            START_TIME_MEASURE;
            for (size_t i = 0; i < MAX_COUNT; i++) {
                try {
                    exitWithBasicException();
                }
                catch (int ex) {
                    // Caught! Carry on next iteration.
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (size_t i = 0; i < MAX_COUNT; i++) {
                try {
                    exitWithMessageException();
                }
                catch (const std::runtime_error& ex) {
                    // Caught! Carry on next iteration.
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (size_t i = 0; i < MAX_COUNT; i++) {
                exitWithReturn();
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (size_t i = 0; i < MAX_COUNT; i++) {
                auto err = exitWithErrorCode();
                if (err < 0) {
                    // `handle_error()` ...
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}


//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::FinalMethods {

    class Base {
    public:
        virtual void Final() = 0;
        virtual void NotFinal() = 0;
    };

    class Derived : public Base {
    private:
        unsigned long long count = 0;

    public:
        void Final() override final {
            count++;
        }
        virtual void NotFinal() override final {
            count++;
        }
    };

    void TestFinalMethod()
    {
        START_TIME_MEASURE;
        Derived* d = new Derived();
        for (int i = 0; i < 10000; ++i) {
            for (int i = 0; i < 10000; ++i) {
                d->Final();
            }
        }
        delete d;
        STOP_TIME_MEASURE;
    }

    void TestNotFinalMethod()
    {
        START_TIME_MEASURE;
        Derived* d = new Derived();
        for (int i = 0; i < 10000; ++i) {
            for (int i = 0; i < 10000; ++i) {
                d->NotFinal();
            }
        }
        delete d;
        STOP_TIME_MEASURE;
    }
}

//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::MoveOperatios::Utilities {


    class Object_Move {
    private:
        std::string s;

    public:
        Object_Move(std::string t_s) : s(std::move(t_s)) {
        }
    };


    class Object_ConstRef {
    private:
        std::string s;

    public:
        Object_ConstRef(const std::string& t_s) : s(t_s) {
        }
    };

    class Object_RValue_Forward {
    private:
        std::string s;

    public:
        template<typename STR>
        Object_RValue_Forward(STR&& str) : s(std::forward<STR>(str)) {
        }
    };

    class Object_UniversalRef_Forward {
    private:
        std::string s;

    public:
        template<typename... T>
        Object_UniversalRef_Forward(T&&... params) : s(std::forward<T>(params)...) {
        }
    };
}

namespace PerformanceTests::MoveOperatios {

    using namespace MoveOperatios::Utilities;

    void Construct_String_Object()
    {
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000000; ++i) {
                std::string s = std::string("a not very short string");
                Object_Move o(s);
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000000; ++i) {
                std::string s = std::string("a not very short string");
                Object_Move o(std::move(s));
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000000; ++i) {
                std::string s{ "a not very short string" };
                Object_ConstRef o(s);
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000000; ++i) {
                Object_ConstRef o("a not very short string");
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000000; ++i) {
                Object_RValue_Forward o("a not very short string");
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000000; ++i) {
                Object_UniversalRef_Forward o("a not very short string");
            }
            STOP_TIME_MEASURE;
        }
    }
}

//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::MemoryPoolsTests {

    class Long {
    protected:
        int value;
        char buff[1024 * 1024];

    public:
        Long() : value(0) {
        }

        explicit Long(int val) : value(val) {
        }

        Long(const Long &obj) {
            this->value = obj.value;
        }

        Long& operator=(const Long& right) {
            if (&right != this) {
                value = right.value;
            }
            return *this;
        }

        Long(Long && obj) noexcept : value(std::exchange(obj.value, 0)) {
        }

        Long& operator=(Long&& right) noexcept {
            if (this != &right) {
                this->value = std::exchange(right.value, 0);
            }
            return *this;
        }

        virtual ~Long() {
        }
    };

    ////////////////////////////////////////////////////////

    template<size_t N>
    class Object {
        char buffer[N]  {0};

    public:
        Object() {}

        Object(const Object &obj) {}
        Object& operator=(Object& right) {}

        Object(Object && obj) noexcept {}
        Object& operator=(Object&& right) noexcept {}
    };

    using TypeTiny   = Object<sizeof(int)>;
    using TypeSmall  = Object<128>;
    using TypeMedium = Object<1024>;
    using TypeLarge  = Object<1024 * 64>;

    using TestType = TypeLarge;

    void Test() {

        constexpr size_t size = 256;
        MemoryPool::Pool<TestType, size> pool;
        //MemoryPool::Pool_AlignedStorage<TestType, size> pool;

        TestType* ints[size];
        constexpr size_t MAX_COUNT = 100;

        {
            START_TIME_MEASURE;
            for (int i = 0; i < MAX_COUNT; i++) {
                for (int n = 0; n < MAX_COUNT; n++) {
                    for (size_t k = 0; k < size; k++)
                        ints[k] = new TestType;
                    for (size_t k = 0; k < size; k++)
                        delete ints[k];
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < MAX_COUNT; i++) {
                for (int n = 0; n < MAX_COUNT; n++) {
                    for (size_t k = 0; k < size; k++)
                        ints[k] = pool.getObject();
                    for (size_t k = 0; k < size; k++)
                        pool.deleteObject(ints[k]);
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}


//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::CollectionsTests::Vector {

    void PushBack_vs_Emplace_String() {
        using type = std::string;

        {
            START_TIME_MEASURE;
            std::vector<type> vect;
            vect.reserve(10000);
            for (int i = 0; i < 10000; ++i) {
                for (int n = 0; n < 10000; ++n) {
                    vect.push_back("Longggggggggggggggggggggg stringggggggggggg");
                }
                vect.clear();
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            std::vector<type> vect;
            vect.reserve(10000);
            for (int i = 0; i < 10000; ++i) {
                for (int n = 0; n < 10000; ++n) {
                    vect.emplace_back("Longggggggggggggggggggggg stringggggggggggg");
                }
                vect.clear();
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            std::vector<type> vect;
            vect.reserve(10000);
            for (int i = 0; i < 10000; ++i) {
                for (int n = 0; n < 10000; ++n) {
                    vect.emplace_back(std::move("Longggggggggggggggggggggg stringggggggggggg"));
                }
                vect.clear();
            }
            STOP_TIME_MEASURE;
        }
    }

    void PushBack_vs_Emplace_ConstCharPtr() {
        using type = const char*;

        {
            START_TIME_MEASURE;
            std::vector<type> vect;
            vect.reserve(10000);
            for (int i = 0; i < 10000; ++i) {
                for (int n = 0; n < 10000; ++n) {
                    vect.push_back("Longggggggggggggggggggggg stringggggggggggg");
                }
                vect.clear();
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            std::vector<type> vect;
            vect.reserve(10000);
            for (int i = 0; i < 10000; ++i) {
                for (int n = 0; n < 10000; ++n) {
                    vect.emplace_back("Longggggggggggggggggggggg stringggggggggggg");
                }
                vect.clear();
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            std::vector<type> vect;
            vect.reserve(10000);
            for (int i = 0; i < 10000; ++i) {
                for (int n = 0; n < 10000; ++n) {
                    vect.emplace_back(std::move("Longggggggggggggggggggggg stringggggggggggg"));
                }
                vect.clear();
            }
            STOP_TIME_MEASURE;
        }
    }
}


//----------------------------------------------------------------------------------------------//

namespace PerformanceTests::Strings {

    void Handle_String(const std::string& str) {
        auto size = str.size();
    }

    void Handle_StringValue(std::string str) {
        auto size = str.size();
    }

    void Handle_String_RValue(std::string&& str) {
        auto size = str.size();
    }

    void Handle_String_View(std::string_view str) {
        auto size = str.size();
    }



    void Handle_String_BuildStr(const std::string& str) {
        std::string text(str);
    }

    void Handle_String_View_BuildStr(const std::string_view str) {
        std::string text(str);
    }

    void Handle_CharSequence_Pointer(const char* ptr) {
        std::string text(ptr);
    }

//#define TEST_STRING "Short_String"
#define TEST_STRING "Lonmgggggggggggggggggggg_Stringgggggggggggggggg"

    void handle__const_string_ref(const std::string& s) {
        std::string str {s};
    }

    void handle__const_char_ptr(const char* s) {
        std::string str {s};
    }

    void handle_string_RValue_Ref(std::string&& s) {
        std::string str {std::move(s)};
    }

    void handle_string_view(std::string_view s) {
        std::string str {s};
    }

    void PassStringToFunction()
    {
        volatile unsigned int N1 = 10000, N2 = 10000;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    std::string s { "123456789012345678901234567890"};
                    handle__const_string_ref (s);
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    std::string s { "123456789012345678901234567890"};
                    handle__const_char_ptr (s.c_str());
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    std::string s { "123456789012345678901234567890"};
                    handle_string_RValue_Ref (std::move(s));
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    char s[] = "123456789012345678901234567890";
                    handle_string_RValue_Ref (s);
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    std::string s { "123456789012345678901234567890"};
                    handle_string_view (s);
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < N1; i++) {
                for (int n = 0; n < N2; n++) {
                    char s[] = "123456789012345678901234567890";
                    handle_string_view (s);
                }
            }
            STOP_TIME_MEASURE;
        }
    }



#pragma optimize( "", off )
    void StringView_String_PassToFunction_InPlace()
    {
        constexpr size_t TEST_ITER_COUNT = 3000;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_String(TEST_STRING);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_StringValue(TEST_STRING);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_String_RValue(TEST_STRING);
                }
            }
            STOP_TIME_MEASURE;
        }

        std::cout << "String views\n";

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_String_View(TEST_STRING);
                }
            }
            STOP_TIME_MEASURE;
        }

        std::cout << "CharSequence pointer\n";

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_CharSequence_Pointer("Longggggggggggggggggggggg12345stringggggggggggg");
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )


#pragma optimize( "", off )
    void StringView_String_PassToFunction_FromString()
    {
        constexpr size_t TEST_ITER_COUNT = 1000;
        std::string text = "Short_String";

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_String(text);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_StringValue(text);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_String_RValue(std::move(text));
                }
            }
            STOP_TIME_MEASURE;
        }

        std::cout << "String views\n";

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_String_View(text);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_String_View(text.c_str());
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )

#pragma optimize( "", off )
    void StringView_String_PassToFunction_StringPart()
    {
        constexpr size_t TEST_ITER_COUNT = 1'000;
        constexpr size_t beg = 5, end = 25;

        std::string text = "123456789123456789123456789123456789123456789123456789123456789123456789";

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_String(text.substr(beg, end - beg));
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_StringValue(text.substr(beg, end - beg));
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TEST_ITER_COUNT; ++i) {
                for (int n = 0; n < TEST_ITER_COUNT; ++n) {
                    Handle_String_View(std::string_view(text.begin() + beg, text.begin() + end));
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )


#pragma optimize( "", off )
    void Substring_Search_StringView_vs_Array()
    {
        std::string_view strView{ "12345_123123414324234532523535" };
        std::string str{ "12345_123123414324234532523535" };
        char data[]{ "12345_123123414324234532523535" };
        constexpr size_t TESTS_COUNT = 10'00;

        auto sub = strView.substr(0, 5);

        auto complex_check = [](const char data[]) -> bool {
            return (data[0] == 'a' || data[0] == 'A') && (data[1] == 'b' || data[1] == 'B') &&
                   data[2] == '3' && data[3] == '4' && data[4] == '5';
        };

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TESTS_COUNT; i++) {
                for (int n = 0; n < TESTS_COUNT; n++)
                {
                    auto x = (0 == str.substr(0, 5).compare("12345"));
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TESTS_COUNT; i++) {
                for (int n = 0; n < TESTS_COUNT; n++)
                {
                    auto x = (0 == strView.substr(0, 5).compare("12345"));
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TESTS_COUNT; i++) {
                for (int n = 0; n < TESTS_COUNT; n++)
                {
                    auto x = (0 == sub.compare("12345"));
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TESTS_COUNT; i++) {
                for (int n = 0; n < TESTS_COUNT; n++) {
                    auto x = complex_check(data);
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TESTS_COUNT; i++) {
                for (int n = 0; n < TESTS_COUNT; n++) {
                    auto x = (data[0] == '1' && data[1] == '2' && data[2] == '3' && data[3] == '4' && data[4] == '5');
                }
            }
            STOP_TIME_MEASURE;
        }

    }
#pragma optimize( "", on )


    std::string get_value() {
        return std::string("SSO_String");
    }

    void ReassignementTest() {
        {
            START_TIME_MEASURE;
            std::string val; // default construct
            for (int i = 0; i < 10000; ++i) {
                for (int n = 0; n < 10000; ++n) {
                    val = get_value(); // copy/move assignment
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000; ++i) {
                for (int n = 0; n < 10000; ++n) {
                    std::string val = get_value(); // direct-init/RVO
                }
            }
            STOP_TIME_MEASURE;
        }
    }

    void Append_Performace_Tests() {
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                std::string text;
                for (int n = 0; n < 1000; ++n) {
                    text.append("Value1");
                    text.append("Value2");
                    text.append("Value3");
                    text.append("Value4");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                std::string text;
                for (int n = 0; n < 1000; ++n) {
                    text.append("Value1").append("Value2").append("Value3").append("Value4");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            // APPEND WITH RESERVE
            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                std::string text;
                text.reserve(1000 * 6 * 4);
                for (int n = 0; n < 1000; ++n) {
                    text.append("Value1").append("Value2").append("Value3").append("Value4");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                std::string text;
                for (int n = 0; n < 1000; ++n) {
                    text = +"Value1";
                    text = +"Value2";
                    text = +"Value3";
                    text = +"Value4";
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                std::string text = "";
                for (int n = 0; n < 1000; ++n) {
                    text += std::string("Value1") + std::string("Value2") + std::string("Value3") + std::string("Value4");
                }
            }
            STOP_TIME_MEASURE;
        }
    }

    std::string get_some_string() {
        return std::string("123456789");
    }

    void ProcessString(std::string text) {
    }

    void ProcessStringDefault1(std::string text = "123456789") {
    }

    void ProcessStringDefault2(std::string text = std::string("123456789")) {
    }

    void ProcessStringByRef(const std::string& text = std::string("123456789")) {
    }

    void RVO_For_Default_Parameters() {
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                for (int n = 0; n < 1000; ++n) {
                    ProcessString("123456789");
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                for (int n = 0; n < 1000; ++n) {
                    ProcessString(get_some_string());
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                for (int n = 0; n < 1000; ++n) {
                    ProcessStringDefault1();
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                for (int n = 0; n < 1000; ++n) {
                    ProcessStringDefault2();
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            std::string str = "123456789";
            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                for (int n = 0; n < 1000; ++n) {
                    ProcessString(str);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            std::string str = "123456789";
            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                for (int n = 0; n < 1000; ++n) {
                    ProcessStringByRef(str);
                }
            }
            STOP_TIME_MEASURE;
        }
        {

            START_TIME_MEASURE;
            for (int i = 0; i < 100000; ++i) {
                for (int n = 0; n < 1000; ++n) {
                    ProcessStringByRef();
                }
            }
            STOP_TIME_MEASURE;
        }
    }

    //-------------------------------------------------//

    void SplitString_Find(const std::string& str,
                          std::vector<std::string>& cont,
                          const std::string& delimiter) {
        size_t pos = 0, prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos) {
            cont.emplace_back(str.begin() + prev, str.begin() + pos);
            prev = pos + delimiter.length();
        }
        cont.emplace_back(str.substr(prev, pos - prev));
    }


    template <class Container>
    void SplitString_Stream(const std::string& str, Container& cont) {
        std::istringstream iss(str);
        std::copy(std::istream_iterator<std::string>(iss),
                  std::istream_iterator<std::string>(),
                  std::back_inserter(cont));
    }

    void Split_String_Tests() {
        const std::string base = "11111111a 22222222222b 3333333333333c 4444444444d 55555555555f 6666666666666g";
        std::vector<std::string> parts;

        constexpr int TESTS = 1000;

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TESTS; i++) {
                for (int n = 0; n < TESTS; n++) {

                    SplitString_Find(base, parts, " ");
                }
                parts.clear();
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < TESTS; i++) {
                for (int n = 0; n < TESTS; n++) {
                    SplitString_Stream(base, parts);
                }
                parts.clear();
            }
            STOP_TIME_MEASURE;
        }
    }

    //--------------------------------------------------------------------------------------------------//

    template <typename ...Args>
    std::string stringWithFormat(std::string&& format, Args && ...args) {
        size_t size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
        std::string output(size + 1, '\0');
        std::sprintf(output.data(), format.c_str(), std::forward<Args>(args)...);
        return output;
    }

    template <typename ...Args>
    std::string stringWithFormat_Move(std::string&& format, Args && ...args) {
        size_t size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
        std::string output(size + 1, '\0');
        std::sprintf(output.data(), format.c_str(), std::forward<Args>(args)...);
        return std::move(output);
    }

    void Format() {
        constexpr size_t TESTS_COUNT = 10'000;

        {
            START_TIME_MEASURE;
            for (size_t i = 0; i < TESTS_COUNT; i++) {
                for (size_t n = 0; n < TESTS_COUNT; n++) {
                    std::string result = stringWithFormat("String: %s, Value: %d", "Str_Text", 123);
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (size_t i = 0; i < TESTS_COUNT; i++) {
                for (size_t n = 0; n < TESTS_COUNT; n++) {
                    std::string result = stringWithFormat_Move("String: %s, Value: %d", "Str_Text", 123);
                }
            }
            STOP_TIME_MEASURE;
        }
    }


    void Create_From_TwoString() {
        const std::string s1("OneOneOneOneOneOneOneOneOneOneOneOneOneOneOneOneOne");
        const std::string s2("TwoTwoTwoTwoTwoTwoTwoTwoTwoTwoTwoTwoTwoTwoTwoTwoTwo");
        constexpr size_t TESTS_COUNT = 1'000;

        {
            START_TIME_MEASURE;
            for (size_t i = 0; i < TESTS_COUNT; i++) {
                for (size_t n = 0; n < TESTS_COUNT; n++) {
                    std::string str(s1 + s2);
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (size_t i = 0; i < TESTS_COUNT; i++) {
                for (size_t n = 0; n < TESTS_COUNT; n++) {
                    std::string str{ std::string(s1 + s2) };
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------//

namespace PerformanceTests::Strings_HTTP {

    const std::string request = R"(GET /favicon.ico HTTP/1.1
Host: 127.0.0.1:52525
Connection: keep-alive
Pragma: no-cache
Cache-Control: no-cache
User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36
Accept: image/avif,image/webp,image/apng,image/*,*/*;q=0.8
Sec-Fetch-Site: same-origin
Sec-Fetch-Mode: no-cors
Sec-Fetch-Dest: image
Referer: http://127.0.0.1:52525/
Accept-Encoding: gzip, deflate, br
Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7
Cookie: _ga=GA1.1.299330640.1605022705; PGADMIN_KEY=26c6d221-b0a7-4ae6-b030-2aebf7032cf6; PGADMIN_LANGUAGE=en)";

    const std::string method_string_request = R"(GET /favicon.ico HTTP/1.1)";

    enum class Method {
        None = 0,
        GET = 1,
        HEAD = 2,
        POST = 3,
        PUT = 4,
        // DELETE = 5,
        CONNECT = 6,
        OPTIONS = 7,
        TRACE = 8,
        PATCH = 9
    };

    // Some local HTTP specific constants:
    namespace {
        /** HTTP headers delimiter **/
        inline static const std::string delimiter = "\n"; // "\r\n"

        /** Separates http request method and path: **/
        inline static const std::string path_separator = " /";

        /** Separates http request path and HTTP version: **/
        inline static const std::string http_verion_separator = " HTTP/";

        using HttpMethodDef = std::pair<std::string_view, Method>;
        static constexpr std::array<HttpMethodDef, 8> http_methods_mapping{ {
            {std::string_view("GET"),     Method::GET},
            {std::string_view("POST"),    Method::POST},
            {std::string_view("HEAD"),    Method::HEAD},
            {std::string_view("PUT"),     Method::PUT},
            //{std::string_view("DELETE"),  Method::DELETE},
            {std::string_view("CONNECT"), Method::CONNECT},
            {std::string_view("OPTIONS"), Method::OPTIONS},
            {std::string_view("TRACE"),   Method::TRACE},
            {std::string_view("PATCH"),   Method::PATCH}
        } };
    }

    Method HttpMethodFromString_OldStyle(const std::string& method) {
        if (0 == method.compare("GET"))
            return Method::GET;
        else if (0 == method.compare("HEAD"))
            return Method::HEAD;
        else if (0 == method.compare("POST"))
            return Method::POST;
        else if (0 == method.compare("PUT"))
            return Method::PUT;
            // else if (0 == method.compare("DELETE"))
            // 	  return Method::DELETE;
        else if (0 == method.compare("CONNECT"))
            return Method::CONNECT;
        else if (0 == method.compare("OPTIONS"))
            return Method::OPTIONS;
        else if (0 == method.compare("TRACE"))
            return Method::TRACE;
        else if (0 == method.compare("PATCH"))
            return Method::PATCH;
        return Method::None;
    }

    Method HttpMethodFromString(const std::string& method_string) {
        for (const auto& def : http_methods_mapping) {
            for (size_t i = 0; i < method_string.size() && i <= def.first.size(); ++i) {
                if (' ' == method_string[i])
                    return def.second;
                if (def.first[i] != method_string[i])
                    break;
            }
        }
        return Method::None;
    }

    void OldStyle_Test() {
        size_t pos = method_string_request.find_first_of(path_separator);
        if (std::string::npos == pos)
            return;

        std::string tmp = method_string_request.substr(0, pos);
        Method method = HttpMethodFromString_OldStyle(tmp);
    }

    void NewStyle_Test() {
        Method method = HttpMethodFromString(method_string_request);
    }

#pragma optimize( "", off )
    void Compare_HTTP_Method_Parsers() {
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000; i++) {
                for (int n = 0; n < 10000; n++) {
                    OldStyle_Test();
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000; i++) {
                for (int n = 0; n < 10000; n++) {
                    NewStyle_Test();
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )
}

//----------------------------- PolymorphicMemoryResources: ---------------------------------------------------//
namespace PerformanceTests::PolymorphicMemoryResources {

    void PMR_Vector_Test() {
        // using Type = int;
        using Type = Utilities::Long;
        constexpr size_t max_elements = 1000, count = 1000000;
        constexpr size_t buffer_size = max_elements * sizeof(Type);

        START_TIME_MEASURE;
        for (size_t i = 0; i < count; ++i) {
#if 0
            char buffer[buffer_size] = {};
			std::pmr::monotonic_buffer_resource pool{ std::data(buffer), std::size(buffer) };
#endif
#if 1
            std::array<std::byte, buffer_size> buffer;
            std::pmr::monotonic_buffer_resource pool{ buffer.data(), buffer.size() };
#endif
            std::pmr::vector<Type> coll{ &pool };
            coll.reserve(max_elements);
            for (int n = 0; n < max_elements; ++n) {
                coll.emplace_back(i);
            }
        }
        STOP_TIME_MEASURE;
    }

    void STD_vs_PMR_Vector_Test() {
        using Type = int;
        // using Type = Utilities::Long;
        // using Type = std::string;

        constexpr size_t max_elements = 1000, count = 1000000;
        constexpr size_t buffer_size = max_elements * sizeof(Type);

        {
            START_TIME_MEASURE;
            for (size_t i = 0; i < count; ++i) {
                std::vector<Type> coll;
                coll.reserve(max_elements);
                for (int n = 0; n < max_elements; ++n) {
                    coll.emplace_back(i);
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (size_t i = 0; i < count; ++i) {
                std::pmr::vector<Type> coll;
                coll.reserve(max_elements);
                for (int n = 0; n < max_elements; ++n) {
                    coll.emplace_back(i);
                }
            }
            STOP_TIME_MEASURE;
        }
    }

    ////////////////////////////////////

    void STD_vs_PMR_Vector_String() {
        constexpr size_t count = 10000;
        constexpr size_t size = 1000;

        /*
        std::array<std::byte, 100'000> buffer1;
        std::pmr::unsynchronized_pool_resource unsynchronized_pool1{ buffer1.data(), buffer1.size() };

        std::array<std::byte, 100'000> buffer2;
        std::pmr::synchronized_pool_resource synchronized_pool2{ buffer2.data(), buffer2.size() };

        std::array<std::byte, 100'000> buffer3;
        std::pmr::monotonic_buffer_resource monotonic_buffer{ buffer3.data(), buffer3.size() };

        std::cout << "PMR Vector + String       : ";

        {
            START_TIME_MEASURE;
            for (int i = 0; i < count; ++i) {
                std::pmr::vector<std::pmr::string> coll{ &unsynchronized_pool1 };
                coll.reserve(size);
                for (int n = 0; n < size; ++n) {
                    coll.emplace_back("just a non-SSO string");
                }
            }
            STOP_TIME_MEASURE;
        }

        std::cout << "PMR Vector + String [Pool]: ";

        {
            START_TIME_MEASURE;
            for (int i = 0; i < count; ++i) {
                std::pmr::vector<std::pmr::string> coll;
                coll.reserve(size);
                for (int n = 0; n < size; ++n) {
                    coll.emplace_back("just a non-SSO string");
                }
            }
            STOP_TIME_MEASURE;
        }

        std::cout << "STD Vector + String       : ";

        {
            START_TIME_MEASURE;
            for (int i = 0; i < count; ++i) {
                std::vector<std::string> coll;
                coll.reserve(size);
                for (int n = 0; n < size; ++n) {
                    coll.emplace_back("just a non-SSO string");
                }
            }
            STOP_TIME_MEASURE;
        }
        */
    }

    ////////////////////////////////////

    void STD_Vector_PMR_String() {
        std::array<std::byte, 200> buffer;
        std::pmr::monotonic_buffer_resource pool{ buffer.data(), buffer.size() };

        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000; ++i) {
                std::pmr::vector<std::pmr::string> coll{ &pool };
                coll.reserve(1000);
                for (int n = 0; n < 1000; ++n) {
                    coll.emplace_back("just a non-SSO string");
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000; ++i) {
                std::vector<std::string> coll;
                coll.reserve(1000);
                for (int n = 0; n < 1000; ++n) {
                    coll.emplace_back("just a non-SSO string");
                }
            }
            STOP_TIME_MEASURE;
        }

    }

}

//----------------------------- BasicLoops: ---------------------------------------------------//
namespace PerformanceTests::BasicLoops {

    struct Small {
        int a;
    };

    struct Medium {
        int a;
        std::string s;
    };

    struct Big {
        int a;
        float f;
        std::string s;
        char buffer[1024];
    };

    enum class HTTPMethod {
        None = 0,
        GET = 1,
        POST = 2,
        HEAD = 3,
        PUT = 4,
        DELETE = 5,
        CONNECT = 6,
        OPTIONS = 7,
        TRACE = 8,
        PATCH = 9
    };

    struct SocketEvent final
    {
        int socket;
        HTTPMethod method;
        std::string data;

        SocketEvent(int s, std::string&& d) :
                socket{ std::move(s) }, data{std::move(d)} 	{
        }


        friend bool operator<(const SocketEvent& event1, const SocketEvent& event2) {
            return event1.socket < event2.socket;
        }

        friend bool operator==(const SocketEvent& event1, const SocketEvent& event2) {
            return event1.socket == event2.socket;
        }

    };

    struct SocketEventHash {
        std::size_t operator()(const SocketEvent& event) const noexcept {
            return std::hash<int>{}(event.socket);
        }
    };


    void For_vs_RangeBased_Loops() {
        std::vector<int> nums = { 1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9 };

        {
            START_TIME_MEASURE;
            for (int i = 0; i < 1000000000; ++i) {
                volatile int sum = 0;
                for (size_t n = 0; n < nums.size(); ++n) {
                    sum += nums[n];
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < 1000000000; ++i) {
                volatile int sum = 0;
                for (int n : nums) {
                    sum += n;
                }
            }
            STOP_TIME_MEASURE;
        }

    }

#pragma optimize( "", off )
    void Array_Vs_Vector() {
        std::vector<int> data_vector(10000);
        std::iota(data_vector.begin(), data_vector.end(), 0);

        constexpr std::array<int, 10000> data_array = [=]() {
            std::array<int, 10000> data{};
            for (int i = 0; i < 10000; i++)
                data[i] = i;
            return data;
        }();

        constexpr unsigned long count = 100000;

        {
            START_TIME_MEASURE;
            for (auto i = 0; i < count; ++i) {
                int sum = 0;
                for (size_t n = 0; n < 10000; ++n) {
                    sum += data_array[n];
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (auto i = 0; i < count; ++i) {
                int sum = 0;
                for (size_t n = 0; n < 10000; ++n) {
                    sum += data_vector[n];
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )

#pragma optimize( "", off )
    void Iterate_Big_vs_Small_Objects()
    {
        constexpr size_t SIZE{10'000};
        constexpr size_t COUNT { 100'000 };

        std::vector<Small> vector_small(SIZE);
        std::vector<Medium> vector_medium(SIZE);
        std::vector<Big> vector_big(SIZE);

        {
            START_TIME_MEASURE;
            int sum{ 0 };
            for (size_t i = 0; i < COUNT; ++i) {
                for (auto& entry : vector_small) {
                    sum += entry.a;
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            int sum{ 0 };
            for (size_t i = 0; i < COUNT; ++i) {
                for (auto& entry : vector_medium) {
                    sum += entry.a;
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            int sum{ 0 };
            for (size_t i = 0; i < COUNT; ++i) {
                for (auto& entry : vector_big) {
                    sum += entry.a;
                }
            }
            STOP_TIME_MEASURE;
        }

    }
#pragma optimize( "", on )


#pragma optimize( "", off )
    void Iterate_Set_vs_Map()
    {
        constexpr size_t SIZE{ 5'000 };
        constexpr size_t COUNT{ 100'000 };

        using _Type = SocketEvent;

        std::unordered_set<int> _set;
        std::unordered_set<_Type, SocketEventHash> _set_events;
        std::unordered_map<int, _Type> _map;

        for (int i = 0; i < SIZE; ++i) {
            _set.insert(i);
            _set_events.insert({ i, {} });
            _map.insert({ i, {i, ""}});
        }

        // assert(_set.size() != _map.size());
        std::cout << _set.size() << " " << _map.size() << std::endl;


        {
            START_TIME_MEASURE;
            // int sum{ 0 };
            for (size_t i = 0; i < COUNT; ++i) {
                for (auto& entry : _set) {
                    // sum += entry.a;
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            // int sum{ 0 };
            for (size_t i = 0; i < COUNT; ++i) {
                for (auto& entry : _set_events) {
                    // sum += entry.a;
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            // int sum{ 0 };
            for (size_t i = 0; i < COUNT; ++i) {
                for (auto& entry : _map) {
                    // sum += entry.a;
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )
}


//----------------------------- Algoritms: ---------------------------------------------------//

namespace PerformanceTests::Algoritms {

    void Algoritm_vs_Foop_ANY_OF() {
        constexpr int find = 75;
        constexpr size_t size = 100;
        std::vector<int> numbers;
        numbers.reserve(size);
        for (int i = 0; i < size; i++)
            numbers.push_back(i);

        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000000; ++i) {
                bool x = false;
                for (int n = 0; n < size; n++) {
                    if (numbers[n] == find) {
                        x = true;
                        break;
                    }
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            for (int i = 0; i < 10000000; ++i) {
                bool x = std::any_of(numbers.begin(), numbers.end(), [&](int v) { return v == find; });
            }
            STOP_TIME_MEASURE;
        }
    }
}



//----------------------------- Algoritms: ---------------------------------------------------//

//#pragma optimize( "", off )

int globalIntVar = 0;

namespace PerformanceTests::GlobalVariables {

    void Access_Global_Vs_Local_Variable() {
        constexpr int max_count = 10000;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < max_count; ++i) {
                globalIntVar = 0;
                for (int n = 0; n < max_count; n++) {
                    globalIntVar = n;
                }
            }
            STOP_TIME_MEASURE;
        }

        int localIntVar = 0;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < max_count; ++i) {
                localIntVar = 0;
                for (int n = 0; n < max_count; ++n) {
                    localIntVar = n;
                }
            }
            STOP_TIME_MEASURE;
        }

        static int staticIntVar = 0;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < max_count; ++i) {
                staticIntVar = 0;
                for (int n = 0; n < max_count; ++n) {
                    staticIntVar = n;
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}
//#pragma optimize( "", on )

//----------------------------- Objects: ---------------------------------------------------//
namespace PerformanceTests::Objects {

    class Object {
    private:
        int a;
        int b;
        int c;
        int d;

    public:
        Object(int a, int b, int c, int) {
            this->a = a;
            this->b = b;
            this->c = c;
            this->d = d;
        }
    };


    class Object1 {
    private:
        int a = 10;
        int b = 20;
        int c = 30;
        int d = 40;

    public:
        Object1() {

        }
    };

    class Object2 {
    private:
        int a;
        int b;
        int c;
        int d;

    public:
        Object2() : a{ 10 }, b{ 20 }, c{ 30 }, d{ 40 } {
        }
    };

    void Object_Variables_InitInConstructor() {
        constexpr int size = 10000;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < size; ++i) {
                for (int n = 0; n < size; ++n) {
                    Object* obj = new Object(10, 20, 30, 40);
                    delete obj;
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < size; ++i) {
                for (int n = 0; n < size; ++n) {
                    Object1* obj = new Object1();
                    delete obj;
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < size; ++i) {
                for (int n = 0; n < size; ++n) {
                    Object2* obj = new Object2();
                    delete obj;
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}

//----------------------------- Attributes: ---------------------------------------------------//
namespace PerformanceTests::Attributes {

    void Test1() {

    }

    [[noreturn]]
    void Test2() {

    }

    void NoReturn_Test() {
        constexpr int size = 10000;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < size; ++i) {
                for (int n = 0; n < size; ++n) {
                    Test1();
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < size; ++i) {
                for (int n = 0; n < size; ++n) {
                    Test2();
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}

//----------------------------- Memory: ---------------------------------------------------//
namespace PerformanceTests::Memory {

    template<size_t N>
    class ObjectExplicit final {
        char buffer[N]{ 0 };

    public:
        ObjectExplicit() {}

        ObjectExplicit(const ObjectExplicit &obj) {}
        ObjectExplicit& operator=(ObjectExplicit& right) {}

        ObjectExplicit(ObjectExplicit && obj) noexcept {}
        ObjectExplicit& operator=(ObjectExplicit&& right) noexcept {}
    };


    template<size_t N>
    class ObjectDefault final {
        char buffer[N]{ 0 };

        /*
        public:
            ObjectDefault() = default;

            ObjectDefault(const ObjectDefault &obj) {}
            ObjectDefault& operator=(ObjectDefault& right) {}

            ObjectDefault(ObjectDefault && obj) noexcept {}
            ObjectDefault& operator=(ObjectDefault&& right) noexcept {}
        */
    };

    using TypeTinyExplicit   = ObjectExplicit<sizeof(int)>;
    using TypeSmallExplicit  = ObjectExplicit<128>;
    using TypeMediumExplicit = ObjectExplicit<1024>;
    using TypeLargeExplicit  = ObjectExplicit<1024 * 64>;

    using TypeTinyDefault   = ObjectDefault<sizeof(int)>;
    using TypeSmallDefault  = ObjectDefault<128>;
    using TypeMediumDefault = ObjectDefault<1024>;
    using TypeLargeDefault  = ObjectDefault<1024 * 64>;

    using TestTypeExplicit = TypeLargeExplicit;
    using TestTypeDefault = TypeLargeDefault;

    void Constructor_Defaut_vs_Explicit() {
        constexpr int size = 1000;
        {
            START_TIME_MEASURE;
            for (int i = 0; i < size; ++i) {
                for (int n = 0; n < size; ++n) {
                    TestTypeExplicit* obj = new TestTypeExplicit();
                    delete obj;
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            for (int i = 0; i < size; ++i) {
                for (int n = 0; n < size; ++n) {
                    TestTypeDefault* obj = new TestTypeDefault();
                    delete obj;
                }
            }
            STOP_TIME_MEASURE;
        }
    }

    /** **/
    constexpr long TEST_REPS_COUNT = 1000;
    //10000000

    /** Buffer size to allocate: **/
    constexpr int BUFFER_SIZE = 10 * 1024 * 1024;

    void AllocateBuffer() {
        START_TIME_MEASURE;

        for (int i = 0; i < TEST_REPS_COUNT; ++i) {
            char* buffer = new char[BUFFER_SIZE];

            //memset(buffer, 0, BUFFER_SIZE);

            /*
            for (auto n = 0; n < BUFFER_SIZE; n++)
                buffer[n] = 0;
            */
            delete[] buffer;
        }

        STOP_TIME_MEASURE;
    }
}

//----------------------------- Polimorphism Types Tests: ---------------------------------------------------//

namespace PerformanceTests::PolimorphismTypes {

    struct ICalculator {
        virtual void fabricMethod() const = 0;
        virtual ~ICalculator() {};
    };

    struct CalcOne : public ICalculator {
        void fabricMethod() const override {
            //std::cout << "CalcOne\n";
        }
    };

    struct CalcTwo : public ICalculator {
        void fabricMethod() const override {
            //std::cout << "CalcTwo\n";
        }
    };

    struct CalcThree : public ICalculator {
        void fabricMethod() const override {
            //std::cout << "CalcThree\n";
        }
    };

    struct CalcFour : public ICalculator {
        void fabricMethod() const override {
            //std::cout << "CalcFour\n";
        }
    };

    struct CalcFive : public ICalculator {
        void fabricMethod() const override {
            //std::cout << "CalcFive\n";
        }
    };

    struct Calculator
    {
    private:
        using AnyCalcPtr = std::function<ICalculator& (std::any&)>;

    public:
        template<typename ConcreteCalculator>
        Calculator(ConcreteCalculator&& calculator) :
                storage{ std::forward<ConcreteCalculator>(calculator) },
                getter{ [](std::any& storage) -> ICalculator& {
                    return std::any_cast<ConcreteCalculator&>(storage);
                } } {
            // Do nothing here.
        }

        ICalculator* operator->() { return &getter(storage); }

    private:
        std::any storage;
        AnyCalcPtr getter;
    };

    /** **/
    constexpr long TEST_REPS_COUNT = 10'000'000;


    void RunTest() {
        {
            START_TIME_MEASURE;
            {
                std::vector<std::unique_ptr<ICalculator>> calcs;
                for (int i = 0; i < 100; i++) {
                    calcs.emplace_back(std::make_unique<CalcOne>());
                    calcs.emplace_back(std::make_unique<CalcTwo>());
                    calcs.emplace_back(std::make_unique<CalcThree>());
                    calcs.emplace_back(std::make_unique<CalcFour>());
                    calcs.emplace_back(std::make_unique<CalcFive>());
                }
                for (int i = 0; i < TEST_REPS_COUNT; ++i) {
                    for (auto& entry : calcs) {
                        entry.get()->fabricMethod();
                    }
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            {
                std::vector<Calculator> calcs;
                for (int i = 0; i < 100; i++) {
                    calcs.emplace_back(CalcOne());
                    calcs.emplace_back(CalcTwo());
                    calcs.emplace_back(CalcThree());
                    calcs.emplace_back(CalcFour());
                    calcs.emplace_back(CalcFive());
                }
                for (int i = 0; i < TEST_REPS_COUNT; ++i) {
                    for (auto& entry : calcs) {
                        entry->fabricMethod();
                    }
                }
            }
            STOP_TIME_MEASURE;
        }
    }

    //---------------------------------------------------------------------------------

    namespace Objects_1 {
        struct Base {
            int a{0};
        };

        struct Derived: Base {
            int b{ 0 };
        };

        struct Big : Derived {
            int c{ 0 };
        };

        struct Grand : Big {
            int d{ 0 };
        };

        struct Complex : Grand {
            int f{ 0 };
        };
    }

    namespace Objects_2 {
        struct AObject {
            int a{ 0 };
        };

        struct BObject {
            int b{ 0 };
        };

        struct CObject {
            int c{ 0 };
        };

        struct DObject {
            int d{ 0 };
        };

        struct Complex {
            AObject a;
            BObject b;
            CObject c;
            DObject d;
            int f{ 0 };
        };
    }

#pragma optimize( "", off )
    void Inheritance_VS_Composition() {
        constexpr long TEST_REPS_COUNT = 100'000'000;
        {
            START_TIME_MEASURE;
            {
                for (int i = 0; i < TEST_REPS_COUNT; ++i) {
                    Objects_1::Complex obj{};
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            {
                for (int i = 0; i < TEST_REPS_COUNT; ++i) {
                    Objects_2::Complex obj{};
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Files {
    using namespace std::string_literals;

    class FileEmulator final {
    private:
        const std::array<std::string, 10> data{
                "Some_Test_line_111111111111111111",
                "Some_Test_line_111111111111111111",
                "Some_Test_line_111111111111111111",
                "Some_Test_line_111111111111111111",
                "Some_Test_line_111111111111111111",
                "Some_Test_line_111111111111111111",
                "Some_Test_line_111111111111111111",
                "Some_Test_line_111111111111111111",
                "Some_Test_line_111111111111111111",
                "Some_Test_line_111111111111111111"
        };

        size_t index{ 0 };
        size_t readCount{ 0 };
        const size_t linesCount{ 0 };

    public:
        FileEmulator(size_t size) : linesCount{ size } {
        }

        bool getline(std::string& buffer) noexcept {
            if (readCount++ >= linesCount)
                return false;

            index = index % data.size();
            buffer.assign(data[index++].data());
            return true;
        }

        void setPos(size_t pos = 0) noexcept {
            readCount = pos;
        }
    };

    void File_Emulator_Tests() {
        FileEmulator file(40);

        std::vector<std::string> lines;
        while (file.getline(lines.emplace_back())) {
            // lines.push_back(line);
        };

        std::for_each(lines.cbegin(), lines.cend(), [](const auto& s) {
            std::cout << s << std::endl;
        });
    }

    void Read_File() {
        std::ifstream file{ R"(D:\Temp\Folder_For_Testing\data\Some_File_Widh_Data.txt)" };

        std::vector<std::string> lines;
        while (std::getline(file, lines.emplace_back())) {
        };

        std::for_each(lines.cbegin(), lines.cend(), [](const auto& s) {
            std::cout << s << std::endl;
        });
    }


    void ReadFile_Emulator_Compare() {
        constexpr long TEST_REPS_COUNT = 1'000;
        FileEmulator file(10);


        std::vector<std::string> lines;
        lines.reserve(TEST_REPS_COUNT + 1);

        {
            START_TIME_MEASURE;
            {
                for (long i = 0; i < TEST_REPS_COUNT; ++i) {
                    for (long n = 0; n < TEST_REPS_COUNT; ++n) {
                        while (file.getline(lines.emplace_back())) {
                        };
                        file.setPos();
                        lines.clear();
                    }
                }
            }
            STOP_TIME_MEASURE;
        }

        {
            START_TIME_MEASURE;
            {
                std::string line;
                for (long i = 0; i < TEST_REPS_COUNT; ++i) {
                    for (long n = 0; n < TEST_REPS_COUNT; ++n) {
                        while (file.getline(line)) {
                            lines.push_back(line);
                        };
                        file.setPos();
                        lines.clear();
                    }
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}


namespace NetworkModules_Tests {

    namespace Network
    {
        using Socket = int;
        constexpr int INVALID_SOCKET{ -1 };

        /** Client request class. **/
        //  Class that implements a request from a client. Containing the socket handle of
        //  the specific client  TCP connection and the data itself (in string representation).
        //  Used to pass it from the network request selector thread to the client request
        //  handler thread
        template<typename T = std::string>
        class Request {
        public:
            /** Client connection socket handle. **/
            Socket socket = INVALID_SOCKET;

            /** Client connection socket handle. **/
            T request{};

        public:
            Request() = default;
            // Request(int s, T r): socket(s), request(std::move(r)) {}

            /** Request constructor. **/
            template<typename... Types>
            Request(int s, Types&&... params) :
                    socket(s), request(std::forward<Types>(params)...) {
            }

            // Disable the copy construcor.
            Request(const Request& req) = delete;

            // Disable the copy assignment operator.
            Request& operator=(const Request& req) = delete;

            // Request(Request&& req) noexcept = delete;
            // Request& operator=(Request&& req) noexcept = delete;

            Request(Request&& req) noexcept :
                    socket(std::exchange(req.socket, INVALID_SOCKET)),
                    request(std::move(req.request)) {
            }

            Request& operator=(Request&& req) noexcept {
                if (this == &req) {
                    return *this;
                }
                this->socket = std::exchange(req.socket, INVALID_SOCKET);
                this->request = std::move(req.request);
                return *this;
            }
        };
    }

    using ClientRequest = Network::Request<std::string>;

    void DEBUG() {
        std::vector<ClientRequest> requests;

        char buffer[] = "qwerty_12345";

        std::string text{ buffer , 6 };
        std::cout << text << std::endl;

        requests.emplace_back(1, buffer, 6);
    }


    void Emplate_Perf_Test() {
        /** **/
        constexpr long TEST_REPS_COUNT = 1000;

        constexpr char data[] = "11111111111111111111111111111111111111111111111111111111111111111111111111";
        constexpr size_t _size = 32;

        std::vector<ClientRequest> requests;

        {
            START_TIME_MEASURE;
            {
                for (long i = 0; i < TEST_REPS_COUNT; ++i) {
                    for (long n = 0; n < TEST_REPS_COUNT; ++n) {
                        //std::string buffer(data, 32);
                        requests.emplace_back(n, std::string(data, 32));
                    }
                    requests.clear();
                }
            }
            STOP_TIME_MEASURE;
        }

        requests.clear();

        {
            START_TIME_MEASURE;
            {
                for (long i = 0; i < TEST_REPS_COUNT; ++i) {
                    for (long n = 0; n < TEST_REPS_COUNT; ++n) {
                        requests.emplace_back(n, data, _size);
                    }
                    requests.clear();
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}

#if 0
namespace Caching_Performance {

    // constexpr size_t CACHE_LINE_SIZE = std::hardware_destructive_interference_size;
    constexpr size_t CACHE_LINE_SIZE = sizeof(int);

    constexpr size_t SIZE = CACHE_LINE_SIZE / sizeof(int) + 1;
    constexpr size_t COUNT = 1'000'000'000;

    void Test()
    {
        srand((unsigned int)time(NULL));

        std::cout << "SIZE = " << SIZE << std::endl;

        int* p = new int[SIZE];

        auto proc = [](int* data) {
            for (int i = 0; i < COUNT; ++i)
                *data = *data + rand();
        };

        START_TIME_MEASURE;

        std::thread t1(proc, &p[0]);
        std::thread t2(proc, &p[SIZE - 1]);

        t1.join();
        t2.join();

        STOP_TIME_MEASURE;
    }


#pragma optimize( "", off )
    void Matrix_Walkthrough_Order() {

        constexpr size_t TESTS_COUNT{ 100'000 };
        constexpr size_t SIZE{ 100 };
        int matrix[SIZE][SIZE]{};

        {
            START_TIME_MEASURE;
            {
                for (size_t n = 0; n < TESTS_COUNT; ++n) {
                    int x = 0;
                    for (size_t column = 0; column < SIZE; ++column) {
                        for (size_t row = 0; row < SIZE; ++row) {
                            x += matrix[row][column];
                        }
                    }
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            {
                for (size_t n = 0; n < TESTS_COUNT; ++n) {
                    int x = 0;
                    for (size_t row = 0; row < SIZE; ++row) {
                        for (size_t column = 0; column < SIZE; ++column) {
                            x += matrix[row][column];
                        }
                    }
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )
}
#endif


namespace Parralel_Algoritms {

    constexpr unsigned long long COUNT = 100'000'000;

    void Test() {
        auto seed = std::random_device{}();

        std::vector<int> data(COUNT);

        {
            START_TIME_MEASURE;
            {
                generate(data.begin(), data.end(), std::mt19937{ seed });
                sort(data.begin(), data.end());
                is_sorted(data.begin(), data.end());
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            {
                std::generate(std::execution::par_unseq, data.begin(), data.end(), std::mt19937{ seed });
                std::sort(std::execution::par_unseq, data.begin(), data.end());
                std::is_sorted(std::execution::par_unseq, data.begin(), data.end());
            }
            STOP_TIME_MEASURE;
        }
    }

    void Test_SortOnly() {
        auto seed = std::random_device{}();

        std::vector<int> data1(COUNT), data2(COUNT), data3(COUNT);
        std::generate(std::execution::par_unseq, data1.begin(), data1.end(), std::mt19937{ seed });
        std::generate(std::execution::par_unseq, data2.begin(), data2.end(), std::mt19937{ seed });
        std::generate(std::execution::par_unseq, data3.begin(), data3.end(), std::mt19937{ seed });

        {
            START_TIME_MEASURE;
            {
                sort(data1.begin(), data1.end());
                is_sorted(data1.begin(), data1.end());
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            {
                std::sort(std::execution::par_unseq, data2.begin(), data2.end());
                //std::is_sorted(std::execution::par_unseq, data2.begin(), data2.end());
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            {
                std::sort(std::execution::par_unseq, data3.begin(), data3.end());
                //std::is_sorted(std::execution::par_unseq, data2.begin(), data2.end());
            }
            STOP_TIME_MEASURE;
        }
    }

    void CPU_Usage_Tests() {
        auto seed = std::random_device{}();
        std::vector<int> data(COUNT);

        for (int i = 0; i < 1000; ++i) {
            std::generate(std::execution::par_unseq, data.begin(), data.end(), std::mt19937{ seed });
            std::sort(std::execution::par_unseq, data.begin(), data.end());
        }
    }
}



namespace Search {

    int BinarySearch(const std::vector<int>& numbers, const int X) {
        int start = 0, end = numbers.size() - 1;
        int mid = (start + end) / 2;
        while (end > start) {
            if (numbers[mid] == X)
                return mid;
            else if (numbers[mid] > X)
                end = mid;
            else if (numbers[mid] < X)
                start = mid;
            mid = (start + end) / 2;
        }
        return -1;
    }

    int SequentialSearch(const std::vector<int>& numbers, const int X) {
        const auto size = numbers.size();
        for (int i = 0; i < size && X >= numbers[i]; ++i)
            if (X == numbers[i])
                return i;
        return -1;
    }

    std::vector<int> buildVector(const size_t size,
                                 int start,
                                 int end) noexcept {
        std::random_device rd{};
        auto mtgen = std::mt19937{ rd() };
        auto distribution = std::uniform_int_distribution<>{ start, end };

        std::vector<int> vector;
        vector.reserve(size);
        for (auto i = 0; i < size; ++i)
            vector.push_back(distribution(mtgen));

        std::sort(vector.begin(), vector.end());
        return vector;
    }

    int getRandomInRange(int start,int end) noexcept {
        std::random_device rd{};
        auto mtgen = std::mt19937{ rd() };
        auto distribution = std::uniform_int_distribution<>{ start, end };
        return distribution(mtgen);
    }

    void BinarySearch_vs_SequentialSearch() {
        constexpr size_t SIZE{ 10'000};
        constexpr size_t TESTS_COUNT{ 5'000'000 };
        const std::vector<int> data = buildVector(SIZE, 1, 100000);

        const int index = getRandomInRange(0, SIZE - 1);
        const int value = data[index];

        {
            START_TIME_MEASURE;
            {
                for (int i = 0; i < TESTS_COUNT; ++i) {
                    SequentialSearch(data, value);
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            {
                for (int i = 0; i < TESTS_COUNT; ++i) {
                    BinarySearch(data, value);
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}

namespace Vector
{
    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vect) {
        for (const T& v : vect)
            stream << v << ' ';
        return stream;
    }

    template<typename T>
    std::vector<T> operator*(const std::vector<T>& vect, T value) {
        const typename std::vector<T>::size_type size{ vect.size() };
        std::vector<T> result;
        result.reserve(size);
        for (size_t i = 0; i < size; ++i)
            result[i] *= value;
        return result;
    }

    template<typename T>
    std::vector<T> operator/(const std::vector<T>& vect, T value) {
        const typename std::vector<T>::size_type size{ vect.size() };
        std::vector<T> result;
        result.reserve(size);
        for (size_t i = 0; i < size; ++i)
            result[i] /= value;
        return result;
    }

    template<typename T>
    std::vector<T> Multiply(const std::vector<T>& vect, T value) noexcept {
        std::vector<T> result{ vect };
        const typename std::vector<T>::size_type size{ vect.size() };
        for (size_t i = 0; i < size; ++i)
            result[i] *= value;
        return result;
    }

    // This works faster than Multiply() !!!!!!
    template<typename T>
    std::vector<T> Multiply2(const std::vector<T>& vect, T value) noexcept {
        const typename std::vector<T>::size_type size{ vect.size() };
        std::vector<T> result;
        result.reserve(size);
        for (size_t i = 0; i < size; ++i)
            result[i] *= value;
        return result;
    }

    template<typename T>
    void MultiplyInOut(const std::vector<T>& vect,
                       T value,
                       std::vector<T>& out) noexcept {
        const typename std::vector<T>::size_type size { vect.size() };
        out.reserve(size);
        for (size_t i = 0; i < size; ++i)
            out[i] *= value;
    }

    void DebugTest() {
        std::vector<int> v{ 1,2,3,4,5,6,7,8,9 };
        std::cout << v * 3 << std::endl;
        std::cout << Multiply(v, 3) << std::endl;
    }

    void MultiplyTest() {
        constexpr size_t SIZE { 10000 };
        constexpr size_t TESTS_COUNT{ 1'000'000};
        const std::vector<int> data = [&]() {
            std::vector<int> v;
            for (size_t i = 0; i < SIZE; ++i)
                v.push_back(i);
            return v;
        }();

        {
            START_TIME_MEASURE;
            {
                for (int i = 0; i < TESTS_COUNT; ++i) {
                    {
                        auto x = data * 3;
                    }
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            {
                for (int i = 0; i < TESTS_COUNT; ++i) {
                    {
                        auto x = Multiply(data, 3);
                    }
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            {
                for (int i = 0; i < TESTS_COUNT; ++i) {
                    {
                        auto x = Multiply2(data, 3);
                    }
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            {
                for (int i = 0; i < TESTS_COUNT; ++i) {
                    {
                        std::vector<int> x;
                        MultiplyInOut(data, 3, x);
                    }
                }
            }
            STOP_TIME_MEASURE;
        }
    }
}


#if 0
namespace Performance {

    int getRandomInt() {
        std::random_device rd{};
        auto mtgen = std::mt19937{ rd() };
        auto ud = std::uniform_int_distribution<>{ 10000, 1000000 };
        return ud(mtgen);
    }


#pragma optimize( "", off )
    void CPU_UsageTest() {

        auto loader = []() {
            const int I = getRandomInt();
            const int N = getRandomInt();
            const int M = getRandomInt();
            size_t sum = 0;
            for (int i = 0; i < I; ++i) {
                sum += i;
                for (int n = 0; n < N; ++n) {
                    sum += n;
                    for (int m = 0; m < M; ++m) {
                        sum += m;
                    }
                }
            }
            std::cout << "Done" << std::endl;
        };

        std::vector<std::future<void>> workers;
        for (int i = 0; i < 24; ++i)
            workers.emplace_back(std::async(loader));

        std::for_each(workers.cbegin(), workers.cend(), [](const auto& F) { F.wait(); });
    }
#pragma optimize( "", on )
}
#endif

namespace DataLayout
{
    struct Object
    {
        std::string name;
        uint32_t value {0};
    };

    struct ObjectList
    {
        std::vector<std::string> names;
        std::vector<uint32_t> values;
    };


#pragma optimize( "", off )
    void benchmark()
    {
        constexpr size_t objCount {100'000};
        constexpr size_t TESTS_COUNT {100'000'000};

        std::vector<Object> objs (objCount);
        ObjectList objectList;
        objectList.names.resize(objCount);
        objectList.values.resize(objCount);

        {
            START_TIME_MEASURE;
            {
                uint32_t sum = 0;
                for (size_t i = 0; i < TESTS_COUNT; ++i) {
                    for (const Object& obj: objs) {
                        sum += obj.value;
                        sum += obj.name.size();
                    }
                }
            }
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            {
                uint32_t sum = 0;
                for (size_t i = 0; i < TESTS_COUNT; ++i) {
                    for (size_t n = 0; n < objCount; ++n) {
                        sum += objs[n].name.size();
                        sum += objs[n].value;
                    }
                }
            }
            STOP_TIME_MEASURE;
        }
    }
#pragma optimize( "", on )
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);
    using namespace PerformanceTests;

    // Collections::TestAll();
    VirtualCall_vs_TypeErasure::benchmark();


    // Caching_Performance::Test();
    // Caching_Performance::Matrix_Walkthrough_Order();

    // Buffers::ArrayOfBytes_vs_CharBuffer();

    // Span::Pass_Vector_As_Param();

    // Strings::PassStringToFunction();

    // Pass_ConstStringRef_AsParameter::Construct_ForwardTest();
    // Pass_ConstStringRef_AsParameter::Emplace_StringParameter_ToVector();

    // StringBasedObject::RunTest();
    // StringBasedObject::RunTest_LocalString();

    // Strings::StringView_String_PassToFunction_InPlace();
    // Strings::StringView_String_PassToFunction_FromString();
    // Strings::StringView_String_PassToFunction_StringPart();

    // Strings::Substring_Search_StringView_vs_Array();

    // Strings::ReassignementTest();
    // Strings::Append_Performace_Tests();
    // Strings::RVO_For_Default_Parameters();
    // Strings::Split_String_Tests();
    // Strings::Format();
    // Strings::Create_From_TwoString();

    // Strings_HTTP::Compare_HTTP_Method_Parsers();





    // Ranges::Filter_Test();

    // Files::ReadFile_Emulator_Compare();

    // Attributes::NoReturn_Test();
    // Objects::Object_Variables_InitInConstructor();

    // BasicLoops::For_vs_RangeBased_Loops();
    // BasicLoops::Array_Vs_Vector();
    // BasicLoops::Iterate_Big_vs_Small_Objects();
    // BasicLoops::Iterate_Set_vs_Map();

    // MoveOperatios::Construct_String_Object();
    // Algoritms::Algoritm_vs_Foop_ANY_OF();
    // CollectionsTests::Vector::PushBack_vs_Emplace_String();
    // Noexcept_Move_Performance::Test();

    // MemoryPoolsTests::Test();
    // Memory::Constructor_Defaut_vs_Explicit();
    // Memory::AllocateBuffer();

    // SharedPtr_PassTests::Pass_ByValue_vs_PassRef();
    // SharedPtr_PassTests::Pass_ByValue_vs_PassRef_WEAK();
    // SharedPtr_PassTests::PassByValue_ToBaseClass();
    // SharedPtr_PassTests::PassByValue_ToDerivedClass();
    // SharedPtr_PassTests::Put_SharedPtr_To_Vector();
    // SharedPtr_PassTests::Create_Shared_vs_New();

    // UniquedPtr_PassTests::PassByRawPointer();
    // UniquedPtr_PassTests::PassByRefTest();
    // UniquedPtr_PassTests::PassByValueTest();
    // UniquedPtr_PassTests::MakeUniquw_vs_NEW();


    // AccessVariable_Map_vs_ArrayOnStack::AccessValueFromStdArray();
    // AccessVariable_Map_vs_ArrayOnStack::AccessValueFromMap();

    // GlobalVariables::Access_Global_Vs_Local_Variable();

    // Const_vs_NonConst_Variables::ConstLocalVariable();
    // Const_vs_NonConst_Variables::Find_in_ConstexprArray();

    // FinalMethods::TestNotFinalMethod();
    // FinalMethods::TestFinalMethod();

    // Move_Experiments::Move_vs_Copy_VectorToMap();

    // Search::BinarySearch_vs_SequentialSearch();


    // PolymorphicMemoryResources::PMR_Vector_Test();
    // PolymorphicMemoryResources::STD_vs_PMR_Vector_Test();
    // PolymorphicMemoryResources::STD_vs_PMR_Vector_String();
    // PolymorphicMemoryResources::STD_Vector_PMR_String();

    // PolimorphismTypes::RunTest();
    // PolimorphismTypes::Inheritance_VS_Composition();


    // Parralel_Algoritms::Test();
    // Parralel_Algoritms::Test_SortOnly();
    // Parralel_Algoritms::CPU_Usage_Tests();

    // Vector::DebugTest();
    // Vector::MultiplyTest();

    // Performance::CPU_UsageTest();

    // Exceptions::Tests();

    // DataLayout::benchmark();

    return EXIT_SUCCESS;
}

