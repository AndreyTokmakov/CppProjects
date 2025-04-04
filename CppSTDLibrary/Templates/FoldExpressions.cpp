/**============================================================================
Name        : FoldExpressions.cpp
Created on  : 27.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FoldExpressions
============================================================================**/

#include "FoldExpressions.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace
{
    template<typename T>
    std::ostream& operator<<(std::ostream &stream,
                             const std::vector<T> & collections) {
        for (const T& i: collections)
            stream << " " << i;
        return stream;
    }
}

namespace FoldExpressions::Basic_Operations
{
    template<typename... Args>
    consteval size_t count(Args&&...) {
        return sizeof...(Args);
    }

    void Count_Number_Entries()
    {
        std::cout << count() << std::endl;
        std::cout << count(1) << std::endl;
        std::cout << count(1,2,3,4,5) << std::endl;
    }
}

namespace FoldExpressions::Basic_Operations
{
    template<typename Predicate, typename... Types>
    consteval size_t count_if(Predicate&& predicate, Types&&... args) {
        return (0 + ... + predicate(std::forward<Types>(args)));
    }

    void Count_If()
    {
        auto is_even = [](auto v){ return !(v & 1); };

        std::cout << count_if(is_even, 2) << std::endl;
        std::cout << count_if(is_even, 1,2,3,4,5) << std::endl;
    }
}

namespace FoldExpressions::Basic_Operations
{
    template<template <typename T> typename Predicate, typename... Args>
    consteval size_t count_if_types() {
        return (0 + ... + Predicate<Args>{}());
    }

    template<typename T>
    struct is_signed_type {
        consteval bool operator ()() const {
            return std::is_signed_v<T>;
        }
    };

    void Count_If_Types()
    {
        std::cout << count_if_types<is_signed_type, int, unsigned int, double>() << std::endl;
    }
}

namespace FoldExpressions
{
    template<typename ...Args>
    auto sum(Args ...args) {
        return (args + ... + 0);
    }

    template<typename ...Args>
    auto sum2(Args ...args) {
        return (args + ...);
    }

    template<typename ...Args>
    void FoldPrintAndSize(Args&&... args) {
        std::cout << " Count = " << sizeof ... (args) << std::endl;
        (std::cout << ... << std::forward<Args>(args)) << std::endl;
    }

    template<typename ...Args>
    void FoldPrint(Args&&... args) {
        (std::cout << ... << std::forward<Args>(args)) << std::endl;
    }

    template<typename ...Args>
    void FoldPrintEx(Args&&... args) {
        auto add_space = []<typename T>(const T& arg) {
            std::cout << ' ';
            return arg;
        };
        (std::cout << ... << add_space(std::forward<Args>(args))) << std::endl;
    }

    template<typename T, typename ...Args>
    void FoldPrint1(const T& var, Args&&... args) {
        std::cout << var;
        (std::cout << ... << std::forward<Args>(args)) << std::endl;
    }

    template<typename T, typename... Args>
    void push_back_vec(std::vector<T>& vector, Args&&... args) {
        (vector.push_back(args), ...);
    }

    template<typename T, typename... A>
    auto make_vector(A&&... params) -> decltype(auto)
    {
        constexpr size_t size = sizeof...(params);
        std::vector<T> v;
        v.reserve(size);
        (v.emplace_back(params), ...);
        return v;
    }

    template<typename FunPtr, typename ...Args>
    void invoke(FunPtr funcPtr, Args&& ...args) {
        (funcPtr(args), ...);
    }

    template<class ...Args>
    bool invokeFuncs(Args ... args) {
        return (args() && ...);
    }

    class Operators {
    public:
        template<typename ...Args>
        static auto unary_left(Args ...args) {
            //(std::cout << ... << std::forward<Args>(args));
            //std::cout << std::endl;
            return (... + args);
        }

        template<typename ...Args>
        static auto unary_right(Args ...args)
        {
            //(std::cout << ... << std::forward<Args>(args));
            //std::cout << std::endl;
            return (args + ...);
        }

        template<typename ...Args>
        static auto binary_left(Args ...args) {
            //(std::cout << ... << std::forward<Args>(args));
            //std::cout << std::endl;
            return (3 + ... + args);
        }

        template<typename ...Args>
        static auto binary_right(Args ...args) {
            //(std::cout << ... << std::forward<Args>(args));
            //std::cout << std::endl;
            return (args + ... + 11);
        }
    };

    template<class ...Args>
    bool containsZero(Args const& ... args) {
        return ((0 == args) || ...);
    }
    template<typename ...Args>
    void printer_ex(Args&&... args) {
        std::cout << "[ ";
        (std::cout << ... << std::forward<Args>(args)) << " ]" << std::endl;
    }

    template<typename T>
    void printer_two_params_only(T a, T b) {
        std::cout << "[ " << a << "  " << b << " ]" << std::endl;
    }

    template<typename ...Args>
    void invoke2(Args&&... args)
    {
        printer_ex(std::forward<Args>(args) ...);
        // printer_two_params_only(std::forward<Args>(args) ...);
    }


    void Test()
    {
        std::cout << sum(1, 2, 3, 4) << std::endl;
        std::cout << sum2(1, 2, 3, 4) << std::endl;
    }

    void Print_Size_Test() {
        {
            FoldPrintAndSize(" 1 ", " 2 ", " 3 ");
        }

        std::cout << "----------------------------------Test2-----------------------------\n";

        {
            auto print_lambda = []<typename ...Args> (Args&&... args) {
                std::cout << " Count = " << sizeof ... (args) << std::endl;
                (std::cout << ... << std::forward<Args>(args)) << std::endl;
            };

            print_lambda(" 1 ", " 2 ", " 3 ");
        }
    }

    void PrintTest() {
        FoldPrint("hello", ", ", 10, ", ", 90.0);
    }

    void PrintTest_Spaces() {
        FoldPrintEx("hello", ", ", 10, ", ", 90.0);
    }

    void PrintTest1() {
        FoldPrint1("Params: ", "hello", " | ", 10, " | ", 90.0);
    }

    void FillVector() {
        std::vector<std::string> strings;
        push_back_vec(strings, "One", "Two", "Three");
        std::for_each(strings.begin(), strings.end(), [](const std::string& v)->void { std::cout << v << std::endl;  });
    }

    void FillVector2() {
        auto for_each_arg = []<typename Function, typename... Values> (Function function, Values&&... values) -> auto {
            return (function(std::forward<Values>(values)), ...);
        };

        std::vector<std::string> strings;

        for_each_arg([&strings](auto&& value) { strings.push_back(value); }, "One", "Two", "Three");
        std::for_each(strings.begin(), strings.end(), [](const std::string& v)->void { std::cout << v << std::endl;  });
    }

    void Make_Vector() {
        auto v = make_vector<std::string>("One", "Two", "Three");
        std::cout << typeid(v).name() << std::endl;
    }



    void CallFunction() {
        invoke([](auto v) { std::cout << v << std::endl;  }, 1, 2, 3, "Four", "Five");
    }

    void PassParamsToFunction() {
        // auto func = [] (auto v) { std::cout << "Lambda: " << v << std::endl; };
        // invoke(func, 1, 2, 3, "Four", "Five");
        invoke2(1,2,3,4, "33333");
    }

    void CallFunctionList() {
        auto f1 = [] { std::cout << "f1() called" << std::endl; return true; };
        auto f2 = [] { std::cout << "f2() called" << std::endl; return true; };
        auto f3 = [] { std::cout << "f3() called" << std::endl; return true; };
        auto f4 = [] { std::cout << "f3() called" << std::endl; return false; };

        std::cout << std::boolalpha << invokeFuncs(f1, f1, f3) << std::endl;
        std::cout << std::boolalpha << invokeFuncs(f1, f4, f3) << std::endl;
    }

    void OperatorsTests() {
        std::cout << Operators::unary_left(2, 3) << std::endl;
        std::cout << Operators::unary_right(2, 3) << std::endl;
        std::cout << Operators::binary_left(2, 3) << std::endl;
        std::cout << Operators::binary_right(2, 3) << std::endl;
    }

    void IsContainsZero() {
        std::cout << "Contains zero: " << std::boolalpha << containsZero(1,2,3,4,5) << std::endl;
        std::cout << "Contains zero: " << std::boolalpha << containsZero(1, 0, 3, 4, 5) << std::endl;
    }
};

namespace FoldExpressions
{
    template<typename ...Ts>
    struct GetFirst{};

    template<typename First, typename ...Rest>
    struct GetFirst<First, Rest...>{
        using Type = First;
    };

    template<typename... Ts>
    auto make_vector(Ts&&... params) -> decltype(auto)
    {
        constexpr size_t size = sizeof...(params);
        using _Ty = typename GetFirst<Ts...>::Type;
        std::vector<_Ty> vec {};
        vec.reserve(size);
        (vec.emplace_back(std::forward<Ts>(params)), ...);
        return vec;
    }

    void GetFirstElementType_CreateVector()
    {
        {
            const auto collection = make_vector(1, 2, 3);
            std::cout << collection << std::endl;
        }
        {
            const auto collection = make_vector(
                    std::string {"I"}, std::string {"II"}, std::string {"III"}
            );
            std::cout << collection << std::endl;
        }
    }
}



namespace FoldExpressions
{
    template<uint32_t N, class ... Nums>
    consteval uint32_t factorial(Nums ... values)
    {
        if constexpr (sizeof ... (values) == N)
        {
            return (values * ... * 1);
        }
        else
        {
            return factorial<N>(sizeof ... (values) + 1, values...);
        }
    }

    void Factorial_Test()
    {
        static_assert(1 == factorial<0>());
        static_assert(1 == factorial<1>());
        static_assert(6 == factorial<3>());
        static_assert(720 == factorial<6>());
    }
}

namespace FoldExpressions
{

    class Blob {
    protected:
        std::vector<std::string> strings;

    public:
        template<typename... Args>
        Blob(Args&&... args) : strings(std::forward<Args>(args)...)
        {
        }

        void Display() {
            for (const auto& str : this->strings) {
                std::cout << str << std::endl;
            }
        }
    };

    /*
    class BlobEx {
    protected:
        std::vector<std::string> strings;
        std::list<int> numbers;

    public:
        template<typename... Args>
        BlobEx(Args&&... args): strings(? ? ? ) numbers(? ? ? )
        {
        }
    };
    */

    /////////////////////////////////////////////////////////

    void Init_Vector_Class()
    {
        std::cout << "---------------------------------------------------- Test1: ----------------------------------\n" << std::endl;
        {
            Blob blob(5, "C++ Truths");
            blob.Display();
        }
        std::cout << "\n---------------------------------------------------- Test2: ----------------------------------\n" << std::endl;
        {
            const std::string param[] = { "Circle", "Triangle", "Square" };
            Blob blob(param, param + 3);
            blob.Display();
        }
    }
};


namespace FoldExpressions
{

    template<typename... Types>
    auto _average(Types const&... values)
    {
        constexpr auto numberOfValues = double{ sizeof...(values) };
        static_assert(numberOfValues > 0);
        return (... + values) / numberOfValues;
    }

    auto Average()
    {
        std::cout << _average(1, 2, 3, 4, 5, 6) << std::endl;
    }

    //----------------------------------------------------------------------

    template<typename Function, typename... Values>
    auto for_each_arg(Function function, Values&&... values)
    {
        return (function(std::forward<Values>(values)), ...);
    }

    auto For_Each()
    {
        auto callable = [](auto x) {
            std::cout << "(" << typeid(x).name() << ") [" << x << "]\n";
        };
        for_each_arg(callable, 1, 2, std::string("Three"));
    }

    //----------------------------------------------------------------------

    template <typename H, typename ... T>
    auto _sum(H head, T... tail) -> decltype(auto) {
        return (head + ... + tail); // expands to: head + tail[0] + tail[1] + ...
    }


    void SumElements_Default()
    {
        {
            int a = _sum(10);
            std::cout << a << std::endl;
        }
        {
            int a = _sum(10, 1);
            std::cout << a << std::endl;
        }
        {
            int a = _sum(10, 1, 2 ,3 ,4, 5);
            std::cout << a << std::endl;
        }
    }

    //---------------------------------------------------------------------------------

    template <typename R, typename ... Ts>
    constexpr auto matches(const R& range, Ts ... ts) -> decltype(auto) {
        return (std::count(std::begin(range), std::end(range), ts) + ...);
    }

    void CountMatches_Compiletime() {
        static_assert(3 == matches("abcdefg", 'a', 'd', 'f'));
#if 0
        static_assert(1 == matches("abcdefg", 'a', 'd', 'f'));
#endif
    }

    //---------------------------------------------------------------------------------

    void _funk(int v) {
        // FIXME: std::format
        // std::cout << std::format("_funk({})\n", v);
    }

    void InvokeFunc_WithFolds() {
        {
            const std::vector ts{ 1,2,3,4,5 };
            for (auto elem : ts)
                _funk(elem);
        }
        {
            // TODO: Can we make it work?
            // (_funk(ts), ...);
        }
    }
}

namespace FoldExpressions::Classes
{
    template<typename... Bases>
    struct MultiBase : private Bases...
    {
        void print() {
            // call print() of all base classes:
            (..., Bases::print());
        }
    };

    struct A {
        void print() { std::cout << "A::print()\n"; }
    };

    struct B {
        void print() { std::cout << "B::print()\n"; }
    };

    struct C {
        void print() { std::cout << "C::print()\n"; }
    };

    void FoldClassMethod()
    {
        MultiBase<A, B, C> mb;
        mb.print();
        // MultiBase<A, B, C>().print();
    }
}

namespace FoldExpressions::Classes
{
    struct One {
        One() { std::cout << "One::One()\n"; }
    };

    struct Two {
        Two() { std::cout << "Two::Two()\n"; }
    };

    struct Three {
        Three() { std::cout << "Three::Three()\n"; }
    };

    template<typename ...Base>
    struct MyStruct :  Base... {
        MyStruct() : Base()... {}
    };


    void Call_All_Base_Class_Constructor()
    {
        MyStruct<One, Two, Three> s{};
    }
}

namespace FoldExpressions
{

    template<typename T>
    class Base
    {
        T value{};

    public:
        Base() = default;
        Base(T v) : value{v} { }

        virtual void print() {
            if constexpr (std::is_same_v<T, bool>)
                std::cout << std::boolalpha <<  value << std::endl;
            else
                std::cout << value << std::endl;
        }
    };

    template<typename... Types>
    class Multi : private Base<Types>... {
    public:
        /** derive all constructors: **/
        using Base<Types>::Base...;

        void print() override {
            (..., Base<Types>::Base::print());
        }
    };

    void FoldedMultiClassConstructors()
    {
        using MultiISB = Multi<int, std::string, bool>;

        MultiISB m1 = 42;
        MultiISB m2 = std::string("hello");
        MultiISB m3 = true;

        // MultiISB m4 = 42.0;

        m1.print();
        m2.print();
        m3.print();
    }

    //==============================================================================================

    struct Node
    {
        int value;
        Node* left;
        Node* right;
        Node(int i = 0) : value(i), left(nullptr), right(nullptr) {}
    };

    auto left = &Node::left;
    auto right = &Node::right;

    // traverse tree, using fold expression:
    template<typename T, typename... TP>
    Node* traverse(T np, TP... paths) {
        return (np ->* ... ->*paths); // np ->* paths1 ->* paths2 ...
    }

    void FoldedPathTraversals() {
        Node* root = new Node{ 0 };
        root->left = new Node{ 1 };
        root->left->right = new Node{ 2 };
        Node* node = traverse(root, left, right);
    }
}

namespace FoldExpressions
{
    template<typename T, size_t... sizes>
    auto concat(const std::array<T, sizes>&... arrays)
    {
        std::array<T, (sizes + ...)> resultArray;
        size_t idx {};
        ((std::copy_n(arrays.begin(), sizes, resultArray.begin() + idx), idx += sizes), ...);
        return resultArray;
    }

    void Concatenate_Arrays()
    {
        std::array<int, 3> values1 {1, 2, 3};
        std::array<int, 1> values2 {4};
        std::array<int, 4> values3 {5,6,7,8};

        {
            const auto result = concat(values1, values2);
            for (const auto &v: result) {
                std::cout << v << ' ';
            }
            std::cout << std::endl;
        }

        {
            const auto result = concat(values1, values2, values3);
            for (const auto &v: result) {
                std::cout << v << ' ';
            }
            std::cout << std::endl;
        }
    }
}


namespace FoldExpressions::Variadic_Friends
{
    template<class Derived, class Message>
    struct Receiver
    {
        Receiver() {
            std::cout << "Receiver::Receiver(): " << typeid(Message).name()  << std::endl;
        }

        Derived& self() noexcept {
            return *static_cast<Derived*>(this);
        }

        void receive(Message msg)
        {
            std::cout << typeid(self()).name() << " ==> Handling: " << typeid(msg).name()  << std::endl;
            self().counter += 1;
        }
    };

    struct MsgOne {
        // MsgOne() { std::cout << "MsgOne::MsgOne()" << std::endl; }
    };

    struct MsgTwo {
        // MsgTwo() { std::cout << "MsgOne::MsgTwo()" << std::endl; }
    };

    template<class... Message>
    struct Dispatcher : public Receiver<Dispatcher<Message...>, Message>...
    {
        using Receiver<Dispatcher, Message>::Receiver...;

        /* Dispatcher() : Receiver<Dispatcher<Message...>, Message>()... {
            std::cout << "Dispatcher::Dispatcher()" << std::endl;
        } */

        void receiveAll(Message... msg)
        {
            (..., Receiver<Dispatcher, Message>::Receiver::receive(msg));
        }

        friend Receiver<Dispatcher, MsgOne>;
        friend Receiver<Dispatcher, MsgTwo>;

        /** Error pre-C++26, accepted from C++26 **/
        // friend Receiver<Dispatcher, Message>...;

    private:
        int counter { 0 };
    };


    void demo()
    {
        Dispatcher<MsgOne, MsgTwo>  dispatcher;
        dispatcher.receiveAll(MsgOne{}, MsgTwo{});
    }
}

void FoldExpressions::TestAll()
{
    // Basic_Operations::Count_Number_Entries();
    // Basic_Operations::Count_If();
    Basic_Operations::Count_If_Types();




    // Test();
    // PrintTest();
    // PrintTest_Spaces();
    // Print_Size_Test();
    // PrintTest1();
    // FillVector();
    // FillVector2();
    // Make_Vector();

    // GetFirstElementType_CreateVector();

    // CallFunction();
    // CallFunctionList();
    // PassParamsToFunction();

    // OperatorsTests();
    // IsContainsZero();
    // Init_Vector_Class();

    // Classes::FoldClassMethod();
    // Classes::Call_All_Base_Class_Constructor();

    // Concatenate_Arrays();

    // FoldedPathTraversals();
    // FoldedMultiClassConstructors();

    // Variadic_Friends::demo();

    // Average();
    // For_Each();
    // CountMatches_Compiletime();
    // SumElements_Default();
    // InvokeFunc_WithFolds();

    // FoldExpressions::Factorial_Test();
}