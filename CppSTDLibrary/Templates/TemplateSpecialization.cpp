/**============================================================================
Name        : PartialSpecialization.cpp
Created on  : 09.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PartialSpecialization.cpp
============================================================================**/

#include "TemplateSpecialization.h"

#include <iostream>

namespace TemplateSpecialization::Specialization
{
    template<typename T>
    struct Node
    {
        T value {};
    };

    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const Node<T>& node)
    {
        stream << node.value;
        return stream;
    }

    template<>
    std::ostream& operator<<(std::ostream& stream, const Node<int>& node)
    {
        stream << "Integer value: " << node.value;
        return stream;
    }

    void PrintOperator_TemplateSpecialisation_Ostream()
    {
        {
            Node<int> node{123};
            std::cout << node << std::endl;
        }
        {
            Node<std::string> node{"123"};
            std::cout << node << std::endl;
        }
    }
}

namespace TemplateSpecialization::Specialization
{
    template<typename T1, typename T2>
    struct Object {
        Object() { std::cout << "Object<T1, T2>()" << std::endl; }
    };

    template<typename T>
    struct Object<T, T> {
        Object() { std::cout << "Object<T, T>()" << std::endl; }
    };


    template<typename T>
    struct Object<T, int> {
        Object() { std::cout << "Object<T, int>()" << std::endl; }
    };

    template<typename T1, typename T2>
    struct Object<T1 *, T2 *> {
        Object() { std::cout << "Object<T1*, T2*>()" << std::endl; }
    };

    void Test() {
        Object<int, float> mif;    // uses Object<T1, T2>()
        Object<float, float> mff;  // uses Object<T , T>()
        Object<float, int> mfi;    // uses Object<T , int>()
        Object<int *, float *> mp; // uses Object<T1*,T2*>()
    }
}


namespace TemplateSpecialization::ClassSpecialization
{

    class Object {
    private:
        std::string value;

    public:
        Object(std::string str) : value(std::move(str)) { }
        friend std::ostream& operator<<(std::ostream& os, const Object& obj);
    };


    std::ostream& operator<<(std::ostream& os, const Object& obj) {
        os << obj.value;
        return os;
    }


    template <class T>
    class TestClass {
    public:
        TestClass() {
            // Initialization of data members
            std::cout << "General template object" << std::endl;
        }
    };

    template <>
    class TestClass <int> {
    public:
        TestClass() {
            // Initialization of data members
            std::cout << "Specialized template object" << std::endl;
        }
    };

    template <>
    class TestClass <Object> {
    public:
        TestClass(const Object& o) {
            // Initialization of data members
            std::cout << "Specialized template object for " <<typeid(o).name() << std::endl;
        }
    };

    void Test() {
        TestClass<int> a;
        TestClass<char> b;
        TestClass<float> c;

        TestClass<Object> o(Object("TETET"));
    }

    template <typename T>       // Common case
    struct Vector {
        void print() {
            std::cout << "Vector<" << typeid(T).name() << ">::print()" << std::endl;
        }
    };

    template <>                 // Special case
    struct Vector<bool> {
        void print_bool() {
            std::cout << "Vector<" << typeid(bool).name() << ">::print_bool()" << std::endl;
        }
    };

    void Test2()
    {
        Vector<int> v1;
        // v1.print_bool();  // ERROR
        v1.print();          // OK

        Vector<bool> v2;
        v2.print_bool();     // OK
        // v2.print();       // ERROR

    }
};

namespace TemplateSpecialization::PartialSpecialization
{
    template<typename T1, typename T2>
    struct Object
    {
        Object() {
            std::cout << "Object()" << std::endl;
        }
    };

    template<typename T>
    struct Object<T, T>
    {
        Object() {
            std::cout << "Object<T, T>" << std::endl;
        }
    };

    // partial specialization: second type is int
    template<typename T>
    struct Object<T, int>
    {
        Object() {
            std::cout << "Object<T, int>" << std::endl;
        }
    };

    // partial specialization: both template parameters are pointer types
    template<typename T1, typename T2>
    struct Object<T1*, T2*>
    {
        Object() {
            std::cout << "Object<T1*, T2*>" << std::endl;
        }
    };

    template <typename T1, typename T2>     // Common case
    struct Pair
    {
        T1 first;
        T2 second;

        void print_first() {
            std::cout << "Pair<" << typeid(T1).name() << ","
                      << typeid(T2).name() << ">::print_first()" << std::endl;
        }
    };

    template <typename T>    // Partial specialization on first argument as int
    struct Pair<int, T>
    {
        void print() {
            std::cout << "Pair<" << typeid(int).name() << ","
                      << typeid(T).name() << ">::print()" << std::endl;
        }
    };


    void Test()
    {
        Object<int, float> mif; // uses Object<T1,T2>
        Object<float, float> mff; // uses Object<T,T>
        Object<float, int> mfi; // uses Object<T,int>
        Object<int*, float*> mp; // uses Object<T1*,T2*>

        // If more than one partial specialization matches equally well, the declaration is ambiguous :
        // Object<int, int> m;  // ERROR: matches Object<T,T>
        // Object<int*, int*> m; // ERROR: matches Object<T,T>  and Object<T1*,T2*>
    }

    void Test2()
    {
        Pair<char, float> p1 {}; // Chose common case
        p1.print_first();        // OK
        // p1.print();           // Not OK: p1 is common case & it doesn't have print() method

        Pair<int, float> p2;     // Chose special case
        p2.print();              // OK
        // p2.print_first();     // Not OK: p2 is special case & it does not have print_first()

        // Use case 3 ----------------------------------------------------------
        // Pair<int> p3;         // Not OK: Number of argument should be same as Primary template

    }


    template <size_t Base, size_t N>
    struct pow {
        static constexpr long long val = Base * pow<Base, N - 1>::val;
    };

    template <size_t Base>
    struct pow<Base, 0> { // Partial specialized class
        static constexpr long long val = 1;
    };

    void Pow_Test_Static()
    {
        static_assert(16 == pow<2, 4>::val);
        static_assert(27 == pow<3, 3>::val);
    }
}


namespace TemplateSpecialization::PartialSpecialization_WithConcepts
{
    template<typename T>
    concept BasicAddable = requires(T a, T b) {
        a + b;
    };

    template<typename T>
    concept Addable = BasicAddable<T> && requires(T a, T b) {
        { a + b } -> std::convertible_to<T>;
    };


    template<typename T>
    void sink(T v) {
        std::cout << "Default\n";
    }

    template<typename T>
    void sink(T v) requires BasicAddable<T>{
        std::cout << "BasicAddable\n";
    }

    template<typename T>
    void sink(T v) requires Addable<T>{
        std::cout << "Addable\n";
    }

    void Test() {
        int v = 10;
        sink(v);
    }

    //---------------------------------------

    template<typename ... Args>
    void handleMultipleValues(Args... params) {
        std::cout << "Default\n";
    }

    template<typename ... Args>
    void handleMultipleValues(Args... params) requires (sizeof ... (Args) == 1) {
        std::cout << "For single element\n";
    }

    template<typename ... Args>
    void handleMultipleValues(Args... params) requires (sizeof ... (Args) % 2 == 0) {
        std::cout << "For even number of elements\n";
    }

    void DependingOf_NumberOfParameters() {
        handleMultipleValues(1,2,3);
        handleMultipleValues(1);
        handleMultipleValues(1, 2);
    }
}

namespace TemplateSpecialization::ClassSpecialization_RawArrays
{
    template<typename T>
    struct ArrayKeeper;

    /// primary template
    template<typename T, std::size_t SZ>
    struct ArrayKeeper<T[SZ]> /// partial specialization for arrays of known bounds
    {
        static void print() {
            std::cout << "print() for T[" << SZ << "]\n";
        }
    };


    template<typename T, std::size_t SZ>
    struct ArrayKeeper<T(&)[SZ]> /// partial spec. for references to arrays of known bounds
    {
        static void print() {
            std::cout << "print() for T(&)[" << SZ << "]\n";
        }
    };


    template<typename T>
    struct ArrayKeeper<T[]> /// partial specialization for arrays of unknown bounds
    {
        static void print() {
            std::cout << "print() for T[]\n";
        }
    };


    template<typename T>
    struct ArrayKeeper<T(&)[]> /// partial spec. for references to arrays of unknown bounds
    {
        static void print() {
            std::cout << "print() for T(&)[]\n";
        }
    };


    template<typename T>
    struct ArrayKeeper<T*> /// partial specialization for pointers
    {
        static void print() {
            std::cout << "print() for T*\n";
        }
    };

    template<typename T1, typename T2, typename T3>
    void foo(int a1[7],
             int a2[],
             int (&a3)[42],
             int (&x0)[],
             T1 x1,
             T2& x2, T3&& x3)
    {
        ArrayKeeper<decltype(a1)>::print(); /// uses MyClass<T*>
        ArrayKeeper<decltype(a2)>::print(); /// uses MyClass<T*>
        ArrayKeeper<decltype(a3)>::print(); /// uses MyClass<T(&)[SZ]>
        ArrayKeeper<decltype(x0)>::print(); /// uses MyClass<T(&)[]>
        ArrayKeeper<decltype(x1)>::print(); /// uses MyClass<T*>
        ArrayKeeper<decltype(x2)>::print(); /// uses MyClass<T(&)[]>
        ArrayKeeper<decltype(x3)>::print(); /// uses MyClass<T(&)[]>
    }

    void Specialize_ForRawArrays()
    {
        int a[42];
        int x[1];

        foo(a, a, a, x, x, x, x);
    }
}

namespace TemplateSpecialization::FunctionSpecialization
{
    template<typename T>
    T sqrt(T t) {
        std::cout << "Some generic implementation" << std::endl;
        return {};
    }

    template<>
    int sqrt<int>(int i) {
        std::cout << "Highly optimized integer implementation" << std::endl;
        return {};
    }

    void Test1() {
        sqrt("16");
        sqrt(16);
    }

    template <typename T, typename U>
    void foo(T t, U u) {
        std::cout << "Common case" << std::endl;
    }


    template <>
    void foo<int, int>(int a1, int a2) {
        std::cout << "Fully specialized case" << std::endl;
    }

#if 0 // Compilation error: partial function specialization is not allowed.
    template <typename U>
    void foo<std::string, U>(std::string t, U u) {
        std::cout << "Partial specialized case" << std::endl;
    }
#endif

    void PartialSpecialization()
    {
        foo(1, 2.1); // Common case
        foo(1, 2); // Fully specialized case
    }

}


void TemplateSpecialization::TestAll()
{
    // FunctionSpecialization::Test1();
    // FunctionSpecialization::PartialSpecialization();

    // ClassSpecialization::Test();
    // ClassSpecialization::Test2();

    // Specialization::Test();
    // Specialization::PrintOperator_TemplateSpecialisation_Ostream();

    // ClassSpecialization_RawArrays::Specialize_ForRawArrays();


    PartialSpecialization::Test();
    // PartialSpecialization::Test2();
    // PartialSpecialization::Pow_Test_Static();

    // PartialSpecialization_WithConcepts::Test();
    // PartialSpecialization_WithConcepts::DependingOf_NumberOfParameters();
}