/**============================================================================
Name        : Templates.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Templates src
============================================================================**/

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <algorithm>
#include <concepts>
#include <utility>
#include <algorithm>
#include <cstdint>
#include <functional>
// #include <format>

#include "../Integer/Integer.h"
#include "Templates.h"

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

namespace Templates {

    class A {
    public:
        virtual void info() const noexcept {
            std::cout << __FUNCTION__ << std::endl;
        }
    };

    class B : public A {
    public:
        virtual void info() const noexcept override {
            std::cout << __FUNCTION__ << std::endl;
        }
    };

    class C {
    };


    /////////////////////////////////

    void Rank() {
        std::cout << std::rank<int[1][2][3]>::value << std::endl;
        std::cout << std::rank<int[][2][3][4]>::value << std::endl;
        std::cout << std::rank<int>::value << std::endl;
    }

    void Is_Base_Of__Test() {
        std::cout << std::boolalpha;
        std::cout << "a2b: " << std::is_base_of<A, B>::value << '\n';
        std::cout << "b2a: " << std::is_base_of<B, A>::value << '\n';
        std::cout << "c2b: " << std::is_base_of<C, B>::value << '\n';
        std::cout << "Same types: " << std::is_base_of<C, C>::value << '\n';
    }

    void Is_Same() {
        std::cout << std::boolalpha;

        std::cout << "is_same<int, int32_t> = " << std::is_same<int, int32_t>::value << std::endl;
        std::cout << "is_same<int, int64_t> = " << std::is_same<int, int64_t>::value << std::endl;
        std::cout << "is_same<float, int32_t> = " << std::is_same<float, int32_t>::value << std::endl;

        std::cout << "----------------" << std::endl;

        std::cout << "is_same<int, int> = " << std::is_same<int, int>::value << std::endl;
        std::cout << "is_same<int, unsigned int> = " << std::is_same<int, unsigned int>::value << std::endl;
        std::cout << "is_same<int, signed int> = " << std::is_same<int, signed int>::value << std::endl;

        std::cout << "----------------" << std::endl;

        std::cout << "is_same<char, char> = " << std::is_same<char, char>::value << std::endl;
        std::cout << "is_same<char, unsigned char> = " << std::is_same<char, unsigned char>::value << std::endl;
        std::cout << "is_same<char, signed char> = " << std::is_same<char, signed char>::value << std::endl;

        std::cout << "----------------" << std::endl;

        std::cout << "is_same<long, long> = " << std::is_same<long, long>() << std::endl;
        std::cout << "is_same<long, int> = " << std::is_same<char, int>() << std::endl;

        std::cout << "----------------" << std::endl;

        std::cout << "is_same<A, A> = " << std::is_same<A, A>() << std::endl;
        std::cout << "is_same<A, B> = " << std::is_same<A, B>() << std::endl;
    }

    template<typename T = int>
    class Foo {
    private:
        T val;

    public:
        Foo(T v) : val(v) {
        }

        void info() {
            std::cout << typeid(this->val).name() << std::endl;
        }

    };

    void DefaultTest() {
        Foo f(1);
        f.info();


        Foo<std::string> f1("Test");
        f1.info();
    }
}

namespace Templates::FunctionSpecialization
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

namespace Templates::ClassSpecialization
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


namespace Templates::ClassSpecialization_RawArrays
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
        extern int x[];

        foo(a, a, a, x, x, x, x);
    }
}

namespace Templates::PartialSpecialization {

    template<typename T1, typename T2>
    class Object {
    public:
        Object() {
            std::cout << "Object()" << std::endl;
        }
    };

    template<typename T>
    class Object<T, T> {
    public:
        Object() {
            std::cout << "Object<T, T>" << std::endl;
        }
    };

    // partial specialization: second type is int
    template<typename T>
    class Object<T, int> {
    public:
        Object() {
            std::cout << "Object<T, int>" << std::endl;
        }
    };

    // partial specialization: both template parameters are pointer types
    template<typename T1, typename T2>
    class Object<T1*, T2*> {
    public:
        Object() {
            std::cout << "Object<T1*, T2*>" << std::endl;
        }
    };


    template <typename T1, typename T2>     // Common case
    struct Pair {
        T1 first;
        T2 second;

        void print_first() {
            std::cout << "Pair<" << typeid(T1).name() << ","
                      << typeid(T2).name() << ">::print_first()" << std::endl;
        }
    };

    template <typename T>    // Partial specialization on first argument as int
    struct Pair<int, T> {
        void print() {
            std::cout << "Pair<" << typeid(int).name() << ","
                      << typeid(T).name() << ">::print()" << std::endl;
        }
    };


    void Test() {
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


namespace Templates::PartialSpecialization_WithConcepts
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

namespace Templates::EnableIf {

    template<class T>
    typename std::enable_if<std::is_floating_point<T>::value, T>::type return_and_print_value(T t) {
        std::cout << "Returning <float> value " << t << std::endl;
        return t;
    }

    template<class T>
    typename std::enable_if<std::is_integral<T>::value, T>::type return_and_print_value(T t) {
        std::cout << "Returning <integer> value " << t << std::endl;
        return t;
    }

    template<class T>
    typename std::enable_if<std::is_same<T, std::string>::value, T>::type return_and_print_value(T t) {
        std::cout << "Returning <std::string> value " << t << std::endl;
        return t;
    }

    template <class T>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    is_odd(T i) {
        return bool(i % 2);
    }

    template <class T, class T2 = typename std::enable_if<std::is_integral<T>::value>::type>
    bool is_even(T i) {
        return !bool(i % 2);
    }

    template<class T>
    void print_not_pointer(T var, typename std::enable_if<std::is_integral<T>::value >::type* = 0) {
        std::cout << var << std::endl;
    }

    template <class Type>
    typename std::enable_if<std::is_base_of<A, Type>::value, void>::type PrintInfo(const Type& var) {
        var.info();
    }


    template<typename T>
    typename std::enable_if<std::is_pointer<T>::value, std::remove_pointer_t<T>>::type RemovePointerAndPrint(T var) {
        return *var;
    }

    template<typename T>
    typename std::enable_if<!std::is_pointer<T>::value, T>::type RemovePointerAndPrint(T var) {
        return var;
    }


    //////////////////////////////////////////////////////////////////////////////////////////

    // 1. the return type (bool) is only valid if T is an integral type:
    template <class T>
    typename std::enable_if<std::is_integral<T>::value, void>::type IsOdd_PrintInfo(T i) {
        if (0 == i % 2) {
            std::cout << i << " is Even" << std::endl;
        } else {
            std::cout << i << " is Odd" << std::endl;
        }
    }

    template <class T,class T2 = typename std::enable_if<std::is_integral<T>::value>::type>
    void IsOdd_PrintInfo_2(T i) {
        if (typeid(int) == typeid(T2))
            std::cout << "Type2 = INT" << std::endl;
        if (0 == i % 2) {
            std::cout << i << " is Even" << std::endl;
        } else {
            std::cout << i << " is Odd" << std::endl;
        }
    }

    //////////////////////////////////  HAS ITERATOR /////////////////////////////////////////

    template <typename T>
    class has_iterator {
    public:
        template <typename U>
        static char test(typename U::iterator* x);

        template <typename U>
        static long test(U* x);

        static const bool value = sizeof(test<T>(0)) == 1;
    };

    template <typename T>
    typename std::enable_if<!has_iterator<T>::value, void>::type show(const T& x) {
        std::cout << x << std::endl;
    }

    template <typename T>
    typename std::enable_if<has_iterator<T>::value, void>::type show(const T& x) {
        for (auto& i : x)
            std::cout << i << std::endl;
    }

    ////////////////////////////////////////////////////////////////////////////////

    void SimpleTest() {
        {
            int value = 123;
            std::cout << return_and_print_value(value) << std::endl;
        }
        {
            float value = 123.123f;
            std::cout << return_and_print_value(value) << std::endl;
        }
        {
            std::string value = "Str_123";
            std::cout << return_and_print_value(value) << std::endl;
        }
    }

    void Is_Odd_Even() {
        short int i = 1;    // code does not compile if type of i is not integral
        std::cout << std::boolalpha;
        std::cout << "i is odd:  " << is_odd(i) << std::endl;
        std::cout << "i is even: " << is_even(i) << std::endl;


        std::cout << "i is even: " << is_even<int, int>(i) << std::endl;
        std::cout << "i is even: " << is_even<int, long>(i) << std::endl;

        print_not_pointer(7);
        //print_not_pointer(&i);
    }

    //---------------------------------------------------------------------------------------//


    //---------------------------------------------------------------------------------------//

    void Print_Class_Info() {
        A a;
        PrintInfo(a);

        B b;
        PrintInfo(b);

        // This will not compile:
#if 0
        C c;
		PrintInfo(c);
#endif
    }

    void Enable_If_CheckPointer() {
        int v = 10;
        int *int_ptr = &v;

        std::cout << RemovePointerAndPrint(v) << std::endl; // 10
        std::cout << RemovePointerAndPrint(int_ptr) << std::endl; // 10
    }

    //-----------------------------------------------------------------------------//

    void Basic_Test() {

        std::cout << "------------------- Enable_if_t: ---------------------------\n\n";

        auto T  = typeid(std::enable_if_t<std::is_integral_v<int>, std::string>).name();; // string
        std::cout << T << std::endl;

        using Test_Type = int;
        auto T_1 = typeid(std::enable_if_t<std::is_integral_v<Test_Type> && std::is_signed_v<Test_Type>,
                short>).name(); // short
        std::cout << T_1 << std::endl;

        auto T1 = typeid(std::enable_if_t<std::is_integral_v<int>>).name(); // void
        std::cout << T1 << std::endl;

        int var = 10;
        using T_2 = std::enable_if_t<std::is_integral_v<int>, int>;
        std::cout << "T_2 = " << typeid(T_2).name() << std::endl;


        std::cout << "\n------------------- Conditional: ---------------------------\n\n";

        auto T2 = typeid(std::conditional_t<std::is_integral_v<int>, double, float>).name(); // double
        std::cout << T2 << std::endl;

        auto T3 = typeid(std::conditional_t<!std::is_integral_v<int>, double, float>).name(); // float
        std::cout << T3 << std::endl;

        auto T4 = typeid(std::conditional_t<std::is_integral_v<std::string>, double, float>).name(); // float
        std::cout << T4 << std::endl;
    }

    void Test1() {
        IsOdd_PrintInfo(123);
        IsOdd_PrintInfo(10);

        // ERROR
        // IsOdd_PrintInfo(12.34);
    }

    void Test2() {
        IsOdd_PrintInfo_2<int ,int>(123);
        IsOdd_PrintInfo_2<int>(10);
    }

    void Test_HasIterator() {
        int a = 128;
        std::vector<std::string> s = { "today", "is", "Saturday" };

        show(a);
        show(s);
    }

    //------------------------------------------------------------------------------//


    template<typename T>
    typename std::enable_if<(sizeof(T) > 4), T>::type foo(T value) {
        std::cout << "Size of <T> " << sizeof(T) << std::endl;
        return value;
    }

    void SizeOf(){
        double i = 123.456;
        std::cout << foo(i) << std::endl;
    }

    //------------------------------------------------------------------------------------//

    template<typename T>
    using EnableIfString = std::enable_if_t<std::is_convertible_v<T, std::string>>;

    class Person
    {
    private:
        std::string name;
    public:
        // generic constructor for passed initial name:
        template<typename STR, typename = EnableIfString<STR>>
        explicit Person(STR&& n) : name(std::forward<STR>(n)) {
            std::cout << "TMPL-CONSTR for '" << name << "'\n";
        }
        // copy and move constructor:
        Person(Person const& p) : name(p.name) {
            std::cout << "COPY-CONSTR Person '" << name << "'\n";
        }
        Person(Person&& p) : name(std::move(p.name)) {
            std::cout << "MOVE-CONSTR Person '" << name << "'\n";
        }
    };

    void EnableIf_Typedef()
    {
        std::string s = "sname";
        Person p1(s); // init with string object => calls TMPL-CONSTR
        Person p2("tmp"); // init with string literal => calls TMPL-CONSTR
        Person p3(p1); // OK => calls COPY-CONSTR
        Person p4(std::move(p1)); // OK => calls MOVE-CONST
    }

    class MyString : public std::string {
    public:
        template<typename STR>
        MyString(STR&& str) : std::string(std::forward<STR>(str)) {
        }

        template<typename... T>
        MyString(T&&... params) : std::string(std::forward<T>(params)...) {
        }
    };

    class SomeClass {
    protected:
        int value {};

    public:
        SomeClass(int i): value(i) {
        }

        // If we make 'operator std::string()' private we will no be able to 'Person' class
        // with object of this class 'SomeClass'
        operator std::string() {
            return std::to_string(this->value);
        }

        friend std::ostream& operator<<(std::ostream&, const SomeClass&);
    };

    std::ostream& operator<<(std::ostream& os, const SomeClass& obj) {
        os << obj.value;
        return os;
    }

    void EnableIf_Typedef_2()
    {
        MyString str("2345");
        Person p1(str);

        SomeClass stri(12345);
        Person p2(stri);
    }


    //------------------------------------------------------------------------------------//

    // Here we check requirenment that _Type can be converted to std::string
    template <class _Type, class T2 = std::enable_if_t<std::is_convertible_v<_Type, std::string>>>
    class StringVector {
    protected:
        std::vector<_Type> strings;

    public:
        using const_iterator = typename std::vector<_Type>::const_iterator;

    public:
        template <class... T>
        StringVector(T&&... params) : strings(std::forward<T>(params)...) {
        }

        inline size_t size() const noexcept {
            return this->strings.size();
        }

        template <class... _Valty>
        decltype(auto) emplace_back(_Valty&& ... params) {
            return this->strings.emplace_back(std::forward<_Valty>(params)... );
        }

        template <class... _Valty>
        decltype(auto) emplace(const_iterator _Where, _Valty&& ... params) {
            return this->strings.emplace(_Where, std::forward<_Valty>(params)... );
        }

        void print() {
            std::cout << "Size = " << this->strings.size() << std::endl;
            for (const auto& e : this->strings)
                std::cout << e << std::endl;
        }
    };


    void TemplateClassParams_Requirenments ()
    {
        {
            StringVector<MyString> strings(std::initializer_list<MyString>({ "one", "two", "three" }));
            strings.emplace_back("four");
            strings.emplace_back("five");
            strings.print();
        }
        std::cout << "------------------------------------- Test2 --------------------------------------------" << std::endl;
        {
            StringVector<SomeClass> strings(std::initializer_list<SomeClass>({1,2,3}));
            strings.emplace_back(4);
            strings.print();
        }
    }


    //------------------------------------------------------------------------------------------------------//

    template<typename T>
    using CanbeCastToString = std::enable_if_t<std::is_convertible_v<T, std::string>>;

    template<typename T, typename = CanbeCastToString<T>>
    void Print_StringLike_Object(T const& param) {
        std::cout << "Size: " << param.size() << std::endl;
        std::cout << param << std::endl;
    }

    void Method_Works_OnlyWith_StringsLikeObject() {
        Print_StringLike_Object(std::string("SomeStringLikeObject"));
        Print_StringLike_Object<std::string>("SomeStringLikeObject2");
    }

    //==========================================================================================================//

    struct Base {
    };

    struct Derived : public Base {
    };

    struct NotDerived {
    };

    template<typename T,
            typename R = std::enable_if_t<std::is_base_of_v<Base, T>, Base*>,
            typename ... Args>
    R build(Args&& ... params) {
        return new T(std::forward< Args>(params)...);
    }


    void CreateObject_With_Enable_IF() {
        auto x = build<Derived>();
        std::cout << typeid(x).name() << std::endl;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::EnableIf_Classes_Applications {

    namespace TypeGuardTest {

        template<typename T, size_t Size>
        class Array {
        private:
            T data[Size]{};

            static_assert(std::is_integral_v<T>, "This function is only designed for integral types.");
            static_assert(sizeof(T) >= 4, "Size > 4 is reqired");

        public:

        };
    }

    void Static_Assert_Check() {
        TypeGuardTest::Array<int, 10> array;

#if 0
        // TypeGuardTest::Array<std::string, 10> array;
		// TypeGuardTest::Array<bool, 10> array;
#endif
    }

    //========================================================================


    namespace TypeGuardTest1 {

        template<typename T,
                size_t Size>
        class Array1 {
        private:
            using Type = std::enable_if_t<std::is_integral_v<T> && sizeof(T) >= 4, T>;
            Type data[Size]{};

        public:

        };

        template<typename T,size_t Size,
                typename Type = std::enable_if_t<std::is_integral_v<T> && sizeof(T) >= 4, T>>
        class Array2 {
        private:
            Type data[Size]{};
        public:

        };
    }

    void Type_Check() {
        TypeGuardTest1::Array1<int, 10> array1;
        TypeGuardTest1::Array2<int, 10> array2;

#if 0
        TypeGuardTest1::Array1<std::string, 10> array;
		TypeGuardTest1::Array1<bool, 10> array;

		TypeGuardTest1::Array2<std::string, 10> array;
		TypeGuardTest1::Array2<bool, 10> array;
#endif
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::Methods {

    template<typename T>
    T* BuildArray(size_t size) {
        static_assert(std::is_integral<T>::value, "This function is only designed for integral types.");
        T* d = new T[size];
        for (T i = 0; i < size; i++)
            d[i] = i;
        return d;
    }

    template<typename T = int>
    T* BuildArrayEx(size_t size) {
        static_assert(std::is_integral<T>::value, "This function is only designed for integral types.");
        T* d = new T[size];
        for (T i = 0; i < size; i++)
            d[i] = i;
        return d;
    }

    void SimpleTemplate() {
        int* data = BuildArray<int>(10);
        for (int i = 0; i< 10; i++)
            std::cout << data[i] << std::endl;
    }

    void SimpleTemplate_DefaultType() {
        int* data = BuildArrayEx(10);
        for (int i = 0; i < 10; i++)
            std::cout << data[i] << std::endl;
    }


    //------------------------------------------------------------//

    template<typename T>
    void foo_bad() {
        // x has undefined value if T is built-in type
        [[maybe_unused]]
        T x;
        // std::cout << x << std::endl; // ERROR
    }

    template<typename T>
    void foo() {
        // x is zero (or false) if T is a built-in type
        T x{};
        std::cout << x << std::endl;
    }

    void ZeroInitialization() {
        foo_bad<int>();
        foo<int>();
    }

    //---------------------------------------------------------------------//

    template<typename T, int N, int M>
    bool less(const T(&a)[N], const T(&b)[M])
    {
        std::cout << "N = " << N << ". M = " << M << std::endl;
        for (int i = 0; i < N && i < M; ++i) {
            if (a[i] < b[i])
                return true;
            if (b[i] < a[i])
                return false;
        }
        return false;
    }

    void Compare_Raw_Arrays() {
        int x[] = { 1, 2, 3 }, y[] = { 1, 2, 3, 4, 5 };
        std::cout << std::boolalpha << less(x, y) << std::endl;
    }

    //-------------------------------------------------------------------------------//

    template<int N, int M>
    bool less_str(char const(&a)[N], char const(&b)[M]) {
        std::cout << "N = " << N << ". M = " << M << std::endl;
        for (int i = 0; i < N && i < M; ++i) {
            if (a[i] < b[i])
                return true;
            if (b[i] < a[i])
                return false;
        }
        return N < M;
    }

    void Compare_String_Literals() {
        std::cout << std::boolalpha << less_str("ab", "abc") << std::endl;
        std::cout << std::boolalpha << less_str("az", "abc") << std::endl;
    }

    //------------------------------------------------------------------------------------//

    template<typename T, std::size_t L1, std::size_t L2>
    void _handle_arrays(T (&arg1)[L1], T (&arg2)[L2])
    {
        T* pa = arg1; // decay arg1
        T* pb = arg2; // decay arg2

        for (size_t i = 0; i < L1; i++)
            std::cout << arg1[i] << " ";
        std::cout << std::endl;

        for (size_t i = 0; i < L1; i++)
            std::cout << arg2[i] << " ";
        std::cout << std::endl;

        for (size_t i = 0; i < L1; i++)
            std::cout << pa[i] << " ";
        std::cout << std::endl;

        for (size_t i = 0; i < L1; i++)
            std::cout << pb[i] << " ";
        std::cout << std::endl;
    }

    template<typename T>
    void _handle_arrays_2(T& a, T& b)
    {

    }

    void Template_Params_Only_ForArrays() {
        int a[] = {1,2,3,4,5};
        int b[] = {11, 12, 13, 14, 15};
        _handle_arrays(a,b);
        _handle_arrays_2(a, b);
    }
}





namespace Templates::FoldExpression
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
        static auto unary_right(Args ...args) {
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

    ////////////////////////////////////////////

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
    void invoke2(Args&&... args) {
        printer_ex(std::forward<Args>(args) ...);
        // printer_two_params_only(std::forward<Args>(args) ...);
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////

    void Test() {
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

namespace Templates::FoldExpression
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


namespace Templates::VariadicTemplates {

    template<typename... Args>
    auto adder(Args... args) {
        return (... + args);
        // or (args + ...)
    }

    void Sum_Multiple_Variables() {
        long sum = adder(1, 2, 3, 8, 7);
        std::cout << sum << std::endl;

        std::string s1 = "x", s2 = "aa", s3 = "bb", s4 = "yy";
        std::string ssum = adder(s1, s2, s3, s4);

        std::cout << ssum << std::endl;
    }

    //////////////////////////////////////////////////////

    void print() {
        std::cout << "* * * Will be called at the very end * * * " << std::endl;
    }

    template <typename T, typename... Types>
    void print(T var1, Types... var2) {
        std::cout << var1 << std::endl;
        print(var2 ...);
    }

    void Print_Multiple_Variables_RecursiveHack() {

        print(1, 2, 3.14, "Pass me any number of arguments", "I will print\n");
    }


    ///////////////////////////////////////////////////////////////////////

    template<typename T, typename... Types>
    void print_sizes(T firstArg, Types... args) {
        std::cout << sizeof...(Types) << '\n'; // print number of remaining types
        std::cout << sizeof...(args) << '\n'; // print number of remaining args
    }

    void Variadic_Sizeof() {
        print_sizes("qwerty", 1.4, 1u, false);
    }

    ///////////////////////////////////////////////////////////////////////////

    template<typename T1, typename... TN>
    constexpr bool isHomogeneous(T1, TN...) {
        return (std::is_same<T1, TN>::value && ...); // since C++17
    }

    void Check_IfTypes_AreSame() {
        std::cout << std::boolalpha << isHomogeneous<int, double, int>(1, 1, 1) << std::endl;
        std::cout << std::boolalpha << isHomogeneous<int, int, int>(1, 1, 1) << std::endl;
    }


    //-------------------------------------------------------------------------

    template<typename T>
    T adder(T v) {
        return v;
    }

    template<typename T, typename... Args>
    T adder(T first, Args... args) {
        return first + adder(args...);
    }

    void Sum_Values() {

        auto result = adder(1, 2, 3, 4, 5);
        std::cout << result << std::endl;
    }

    //-------------------------------------------------------------------------

    template<typename T>
    bool pair_comparer(T a, T b) {
        // In real-world code, we wouldn't compare floating point values like
        // this. It would make sense to specialize this function for floating
        // point types to use approximate comparison.
        return a == b;
    }

    template<typename T, typename... Args>
    bool pair_comparer(T a, T b, Args... args) {
        return a == b && pair_comparer(args...);
    }


    void Compare_Pairs() {

        auto result = pair_comparer(1.5, 1.5, 2, 2, 6, 6);
        std::cout << std::boolalpha << result << std::endl;

        result = pair_comparer(1, 1, 1, 2);
        std::cout << std::boolalpha << result << std::endl;
    }

    //-------------------------------------------------------------------------

    template<typename T>
    bool all_equals(T a, T b) {
        return a == b;
    }

    template<typename T, typename... Args>
    bool all_equals(T a, Args... args) {
        return a == all_equals(args...);
    }

    void All_Equals() {
        auto result = all_equals(1, 1, 1, 1);
        std::cout << std::boolalpha << result << std::endl;

        result = all_equals(1, 2, 1, 1);
        std::cout << std::boolalpha << result << std::endl;
    }

    //----------------------------------------------------------------------------

    template <int a>
    int sum_Non_Template_Params() {
        return a;
    }

    template <int a, int b, int... c>
    int sum_Non_Template_Params() {
        return sum_Non_Template_Params<b, c...>() + a;
    }

    void Sum_NonTemplate_Params() {
        auto x = sum_Non_Template_Params<1, 0, 3, 0>();
        std::cout << x << std::endl;

        x = sum_Non_Template_Params<1, 4, 3, 11>();
        std::cout << x << std::endl;
    }

    //========================================================================================================

    // When there is only two parameter left
    template<typename T>
    double sum_two(const T &a, const T &b) {
        std::cout << "sum(" << a << ',' << b << ")\n";
        return a + b;
    }

    template<typename T, typename ...Ts>
    double sum_two(const T &first, const Ts &... args) {
        return first + sum_two(args...);
    }

    void Recursive_Expansion_Two() {
        sum_two(1, 2, 3, 4, 5);
    }

    //========================================================================================================

    template<typename... Ts>
    class Overload: Ts... {
    public:
        using Ts::operator()...;
    };

    template<typename... Ts>
    Overload(Ts...)->Overload<Ts...>;



    template<typename T>
    class Base {
        T value {};
    public:
        Base() { }
        Base(T v) : value{ v } {}
    };


    template<typename... Types>
    class Multi : private Base<Types>...
    {
    public:
        // derive all constructors:
        using Base<Types>::Base...;
    };


    void ExtendedUsingDeclarations() {
        using MultiISB = Multi<int, std::string, bool>;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                FoldExpressionExtended                                                                          //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::FoldExpressionExtended {

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

    void Init_Vector_Class() {
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace Templates::FoldExpression_CallClassMethod {

    template<typename... Bases>
    class MultiBase : private Bases... {
    public:
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

    void FoldClassMethod() {
        MultiBase<A, B, C> mb;
        mb.print();
        // MultiBase<A, B, C>().print();
    }

    //==============================================================================================

    template<typename T>
    class Base {
    private:
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

    struct Node {
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace Templates::FoldExpression_Examples {

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::Decltype {

    using namespace std::string_literals;

    template <class T, class U>
    auto mix(T Lhs, U Rhs) -> decltype(Lhs + Rhs) {
        return Lhs + Rhs;
    }

    //////////////////////////////////////////////////////////

    int& foo(int& i) { return i; }
    float& foo(float& f) { return f; }

    template <class T>
    auto transparent_forwarder(T& t) -> decltype(foo(t)) {
        return foo(t);
    }

    void Test() {
        std::cout << typeid(mix(""s, "")).name() << std::endl;
        std::cout << typeid(mix(1, 3)).name() << std::endl;
        std::cout << typeid(mix(1, 3.3)).name() << std::endl;
        std::cout << typeid(mix(1, 3.3f)).name() << std::endl;
    }

    void Test2() {
        int i = 1;
        std::cout << typeid(transparent_forwarder(foo(i))).name() << std::endl;

        float f = 1;
        std::cout << typeid(transparent_forwarder(foo(f))).name() << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::Forwarding {


    template<class T>
    void __print_no_forward(T&& var) {
        var.printInfo();
    }

    template<class T>
    void print_no_forward(T&& var) {
        __print_no_forward(var);
    }




    template<class T>
    void __print(T&& var) {
        var.printInfo();
    }

    template<class T>
    void print(T&& var) {
        __print(std::forward<T>(var));
    }




    template<class... Args>
    void __print_integers(Args&&... args) {
        (args.printInfo(), ...);
        (args.increment(), ...);
    }

    template<class... Args>
    void print_integers(Args&&... args) {
        __print_integers(std::forward<Args>(args) ... );
    }

    /////////////////////////////////////////////////////////////

    void Test_No_Forward() {
        Integer integer(111);
        print_no_forward(integer);
    }

    void Test_Integer() {
        Integer integer(123);
        print(integer);
    }

    void Test_IntegerList() {
        Integer integer1(11);
        Integer integer2(22);

        print_integers(integer1, integer2);
    }

    //-------------------------------------------------------------------------------------//

    class Str {
    private:
        std::string value;

    public:

        template<typename ... Args>
        Str(Args&& ... params) : value(std::forward<Args>(params)...) {
            std::cout << "[Object_ToMove constructor 2] (" << this->value << ")" << std::endl;
        }


        Str(const Str &obj) {
            std::cout << "[Copy constructor] (" << this->value << ")" << std::endl;
            this->value = obj.value;
        }

        Str& operator=(const Str& right) {
            std::cout << "[Copy assignment operator] (" << this->value << " -> " << right.value << ")" << std::endl;
            if (this != &right)
                this->value = right.value;
            return *this;
        }

        Str(Str&& obj) noexcept : value(std::move(obj.value)) {
            std::cout << "[Move constructor] (" << this->value << ")" << std::endl;
        }

        Str& operator=(Str&& right) noexcept {
            std::cout << "[Move assignment operator] (" << this->value << " => " << right.value << ")" << std::endl;
            if (this != &right)
                this->value = std::move(right.value);
            return *this;
        }

        virtual ~Str() {
            std::cout << "[Destructor] (" << this->value << ")" << std::endl;
        }

        inline virtual std::string getValue() const noexcept {
            return this->value;
        }

        virtual void printInfo() const noexcept {
            std::cout << "Object_ToMove = " << this->value << std::endl;
        }
    };

    void Construct_Object() {
        {
            Str str("Some_Test_String");
            str.printInfo();
            Str str1 = std::move(str);
        }
        {
            const Str str("Some_Test_String");
            str.printInfo();
#if 0
            // ERROR HERE. Cant consturct string using std::move
			Str str1 = std::move(str);
#endif
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::Auto_Template_Params {

    template<auto val>
    void func() {
        std::cout << val << std::endl;
    }

    template<auto val, typename T>
    void func1(const T& t) {
        std::cout << t << val << std::endl;
    }

    void test() {
        func<10>();
        func1<10>("Input value: ");
    }

    ///------------------------------------------------------------///

    template<auto val>
    class Constant {
    private:
        static constexpr auto value = val;

    public:
        decltype(val) getValue() {
            return this->value;
        }
    };

    template<auto v>
    struct constant
    {
        static constexpr auto value = v;
    };

    void ConstClass() {
        using intCons = Constant<'A'>;
        using i = constant<42>;
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::CheckIsPointer {

    template<typename T>
    struct is_pointer {
        static constexpr bool value = false;
    };

    template<typename T>
    struct is_pointer<T*> {
        static constexpr bool value = true;
    };

    void IsPointerTest() {
        int a = 10;
        int *b = &a;

        std::cout << "is 'a' pointer: " << std::boolalpha << is_pointer<decltype(a)>::value << std::endl;
        std::cout << "is 'b' pointer: " << std::boolalpha << is_pointer<decltype(b)>::value << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::DefaultArguments {

    template <typename T, size_t N>
    class Array {
    public:
        T data[N];
    };


    template <typename T = int>
    class Value {
    private:
        T value;

    public:
        Value(const T& val) : value(val) {
        }

        void PrintInfo() {
            std::cout << "Value type is:  " << typeid(T).name() << std::endl;
            std::cout << "Value: " << value << std::endl;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////

    void CreateArray() {

        Array<int, 10> intArray;

        std::cout << sizeof(intArray) << std::endl;
        std::cout << sizeof(Array<int, 10>::data) << std::endl;
    }

    void DefaultTypes() {
        Value val(123);
        val.PrintInfo();

        Value<float> fVal(1.123f);
        fVal.PrintInfo();

        Value<std::string> strVal("Some_String_Value");
        strVal.PrintInfo();
    }

    ////////////////////////////////////////////////////////////////////////////////////

    template<int n> struct funStruct {
        enum { val = 2 * funStruct<n - 1>::val };
    };

    template<> struct funStruct<0> {
        enum { val = 1 };
    };

    void Metaprogramming() {
        std::cout << funStruct<8>::val << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::StaticVariables {

    template <typename T>
    void fun(const T& x) {
        static int i = 10;
        std::cout << ++i << std::endl;
    }

    void FunctionStaticVariales() {
        fun<int>(1);  // prints 11
        fun<int>(2);  // prints 12
        fun<double>(1.1); // prints 11
    }

    ///////////////////////////////////////////

    template <class T>
    class Test {
        T val;

    public:
        static int count;
        Test() {
            count++;
        }
    };

    template<class T>
    int Test<T>::count = 10;

    void ClassStaticVariales()
    {
        Test<int> a;     // value of count for Test<int> is 11 now
        Test<int> b;     // value of count for Test<int> is 12 now
        Test<double> c;  // value of count for Test<double> is 11 now

        std::cout << Test<int>::count    << std::endl;  // prints 12
        std::cout << Test<double>::count << std::endl;  // prints 11
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Templates::Compile_Time_IF {

    template <typename T>
    auto GetValue(T t) {
        if constexpr (std::is_pointer<T>::value) {
            return *t;
        }
        else {
            return t;
        }
    }

    void GetValueFromPointer() {
        int value = 123;
        int* prtVal = &value;

        std::cout << GetValue(value) << std::endl;
        std::cout << GetValue(prtVal) << std::endl;
    }

    //----------------------------------------------------------------------//

    template<typename T>
    typename std::enable_if<std::is_pointer<T>::value, std::remove_pointer_t<T>>::type GetValue2(T t) {
        return *t;
    }

    template<typename T>
    typename std::enable_if<!std::is_pointer<T>::value, T>::type GetValue2(T t) {
        return t;
    }

    void GetValueFromPointer2() {
        int value = 123;
        int* prtVal = &value;

        std::cout << GetValue2(value) << std::endl;
        std::cout << GetValue2(prtVal) << std::endl;
    }

    //----------------------------------------------------------------------//

    template<int N>
    constexpr bool is_negative() {
        if constexpr (N >= 0)
            return false;
        else
            return true;
    }

    void IsNegative() {
        constexpr bool negative = is_negative<1>();
        std::cout << std::boolalpha << negative << std::endl;

        constexpr bool negative1 = is_negative<-1>();
        std::cout << std::boolalpha << negative1 << std::endl;

        static_assert(is_negative<-1>(), "OK");
        // static_assert(is_negative<1>(), "NOT NEGATIVE");
    }

    //----------------------------------------------------------------------//

    template <typename T>
    std::string toString(const T& x) {
        if constexpr (std::is_same_v<T, std::string>) {
            return x; // statement invalid, if no conversion to string
        }
        else if constexpr (std::is_arithmetic_v<T>) {
            return std::to_string(x); // statement invalid, if x is not numeric
        }
        else {
            return std::string(x); // statement invalid, if no conversion to string
        }
    }

    void ToStringTest() {
        {
            std::string value = "SomeText";
            std::string result = toString(value);
            std::cout << value << " -> " << result << std::endl;
        }
        {
            int value = 12345;
            std::string result = toString(value);
            std::cout << value << " -> " << result << std::endl;
        }
        {
            char value[] = "Some pain text";
            std::string result = toString(value);
            std::cout << value << " -> " << result << std::endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::Conditional
{
    void SimpleTest ()
    {
        typedef std::conditional<true, int, double>::type Type1;
        typedef std::conditional<false, int, double>::type Type2;
        typedef std::conditional<sizeof(int) >= sizeof(double), int, double>::type Type3;

        std::cout << typeid(Type1).name() << std::endl;
        std::cout << typeid(Type2).name() << std::endl;
        std::cout << typeid(Type3).name() << std::endl;

        short int i = 1;    // code does not compile if type of i is not integral

        typedef std::conditional<true,  int, float>::type A;                     // int
        typedef std::conditional<false, int, float>::type B;                     // float
        typedef std::conditional<std::is_integral<A>::value, long, int>::type C; // long
        typedef std::conditional<std::is_integral<B>::value, long, int>::type D; // int

        typedef std::conditional_t<false, int, float> B1;                      // int
        typedef std::conditional_t<std::is_integral<B>::value, long, int> D1; // int

        std::cout << std::boolalpha;
        std::cout << "\n ------------------- Types:" << std::endl;
        std::cout << "A: " << typeid(A).name() << std::endl;
        std::cout << "B: " << typeid(B).name() << std::endl;
        std::cout << "C: " << typeid(C).name() << std::endl;
        std::cout << "D: " << typeid(D).name() << std::endl;

        std::cout << std::endl;

        std::cout << "B1: " << typeid(B1).name() << std::endl;
        std::cout << "D1: " << typeid(D1).name() << std::endl;
    }


    template<typename T>
    void foo(T& val, std::conditional_t<sizeof(T) <= 8,
            std::decay_t<T>,
            std::add_const_t<std::add_lvalue_reference_t<T>>> x)
    {
        // val += x;
    }


    void Condition_Ref_vs_NonRef() {
        {
            int a = 1, b = 2;
            foo(a, b);
        }

        {
            std::string a = "1", b = "2";
            foo(a, b);
        }
    }


};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::NontypeFunctionTemplates {

    template<int Val, typename T>
    constexpr T addValue(T x) {
        return x + Val;
    }

    void AddValue() {
        constexpr auto result = addValue<10>(5);
        std::cout << "5 + 10 = " << result << std::endl;
    }

    void TransformArray() {
        std::array<int, 10> data {1,2,3,4,5,6,7,8,9,10};
        std::transform(data.begin(), data.end(), data.begin(), addValue<10, int>);
        std::for_each(data.begin(), data.end(), [](int i) {std::cout << i << std::endl; });
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::Requires {
    using namespace std::string_view_literals;

    template<typename STR>
    requires std::is_convertible_v<STR, std::string>
    void func(STR&& n) {

    }


    void Test() {
        std::string str{"str"};
        func(str);

#if 0
        std::string_view str_view = "dsdsds"sv;
		func(str_view);
#endif
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::PackExpansions {

    template<typename... Types>
    class Mixin : public Types... { // base class pack expansion
        int x {0};
        int y {0};

    public:

        Mixin() : Types()... {
            // base class initializer pack expansion
        }

        Mixin(int v) : Types(v)... {
            // base class initializer pack expansion
        }


        template<typename ... Args>
        Mixin(Args&& ... params) : Types(std::forward<Args>(params)...)... {
            // base class initializer pack expansion
        }
    };

    struct Type1 {
        Type1() { std::cout << "Type1::Type1()" << std::endl; }
        Type1(int value) { std::cout << "Type1::Type1(" << value << ")" << std::endl; }
    };

    struct Type2 {
        Type2() { std::cout << "Type2::Type2()" << std::endl; }
        Type2(int value) { std::cout << "Type2::Type2(" << value << ")" << std::endl; }
    };

    struct Type3 {
        Type3() { std::cout << "Type3::Type3()" << std::endl; }
        Type3(int value) { std::cout << "Type3::Type3(" << value << ")" << std::endl;}
    };

    void Create_Mixin_Class () {
        Mixin<Type1, Type2, Type3> p;
        Mixin<Type1, Type2, Type3> p1(1);
    }

    //-------------------------------------------------------------------------//

    template<typename... Types>
    struct Mixin2: public Types... { // base class pack expansion

        void test() {
            (..., Types::test());
        }

        template<typename ... Ty>
        void print(Ty&& ... params) {
            (Types::print(std::forward<Ty>(params)...), ...);
        }


        using Types::operator()...;
    };

    //template <class ...Ts>
    //Mixin2(Ts&&...)->Mixin2<std::remove_reference_t<Ts>...>;

    struct TestBase
    {
        template<typename T>
        auto add_space(const T& arg) -> decltype(arg) {
            std::cout << ' ';
            return arg;
        };
    };

    struct Ty1: TestBase {
        void test() {
            std::cout << "Ty1::test()\n";
        }

        void operator()() {
            std::cout << "Ty1::operator()\n";
        }

        template<typename ... Types>
        void print(Types&& ... params)
        {
            std::cout << "Ty1::print(";
            (std::cout << ... << add_space(std::forward<Types>(params)));
            std::cout << ")\n";
        }
    };

    struct Ty2: TestBase {
        void test() {
            std::cout << "Ty2::test()\n";
        }

        void operator()() {
            std::cout << "Ty2::operator()\n";
        }

        template<typename ... Types>
        void print(Types&& ... params) {
            std::cout << "Ty2::print(";
            (std::cout << ... << TestBase::add_space(std::forward<Types>(params)));
            std::cout << ")\n";
        }
    };



    void Mixin_CallFunc() {
        Mixin2<Ty1, Ty2> M;

        M.test();
        //M();
        M.print(1, 2, 3);
    }

    //---------------------------------------------------------------------------------

    template<typename T1, typename... TN>
    struct IsHomogeneous {
        static constexpr bool value = (std::is_same<T1,TN>::value && ...);
    };

    void IsHomogeneous_Test() {
        std::cout << std::boolalpha << IsHomogeneous<double, double, decltype(42)>::value << std::endl;
        std::cout << std::boolalpha << IsHomogeneous<int, short, decltype(42)>::value << std::endl;
        std::cout << std::boolalpha << IsHomogeneous<int, int, decltype(42)>::value << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::StaticMembersInTemplates {

    template<typename T>
    class Base {
    private:
        inline static size_t counter{ 0 };

    public:

        ~Base() {
            std::cout << "Base<" << typeid(T).name() << ">::~Base()" << std::endl;
        }

        inline size_t getCounter() const noexcept {
            return counter;
        }

        inline void printInfo() const noexcept {
            std::cout << counter << std::endl;
        }

        inline void setCounter(size_t new_value) noexcept {
            counter = new_value;
        }

        inline void incrementCounter() noexcept {
            counter++;
        }
    };


    void Test() {

        {
            Base<int> X;
            X.printInfo();
            X.incrementCounter();
            X.incrementCounter();
            X.printInfo();
        }

        {
            Base<std::string> X;
            std::cout << "Counter = " << X.getCounter() << std::endl;
            X.setCounter(12);
        }

        {
            Base<int> A;
            A.printInfo();
            A.incrementCounter();
            A.incrementCounter();
            A.printInfo();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::Friends {

    template<typename T>
    class Keeper;

    template<typename T>
    class Object {
    private:
        T value;

        friend class Keeper<T>;
    };

    template<typename T>
    class Keeper {
    private:
        Object<T> data;

    public:
        // friend class Object<T>;

        void Test() {
            std::cout << data.value << std::endl;
        }
    };

    void Access_Private_Field() {
        Keeper<std::string> data;
        data.Test();

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::Applications_And_Examples
{

    // Need partial specialization for this to work
    template <typename T>
    struct Logger;

    // Return type and argument list
    template <typename R, typename... Args>
    struct Logger<R(Args...)>
    {
        std::function<R(Args...)>    m_func;
        std::string                  m_name;

        Logger(std::function<R(Args...)> f, std::string n) :
                m_func { std::move(f) }, m_name { std::move(n) } {
        }

        R operator()(Args... args)
        {
            std::cout << "Entering " << m_name << std::endl;
            R result = m_func(args...);
            std::cout << "Exiting " << m_name << std::endl;
            return result;
        }
    };

    template <typename R, typename... Args>
    auto make_logger(R (*func)(Args...), const std::string &name)
    {
        return Logger<R(Args...)>(std::function<R(Args...)>(func), name);
    }

    double add(double a, double b)
    {
        return a + b;
    }

    void PassingFunction_to_ClassTemplateArgument()
    {
        auto logged_add = make_logger(add, "Add");
        auto result = logged_add(2, 3);
    }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::TypeTraits {

    template<typename T>
    struct is_void {
        static constexpr bool value { false };
    };

    template<>
    struct is_void<void> {
        static constexpr bool value { true }; //only true for void type
    };


    void IsVoid_Test() {
        std::cout << std::boolalpha << is_void<int>::value << std::endl;
        std::cout << std::boolalpha << is_void<std::string>::value << std::endl;
        std::cout << std::boolalpha << is_void<void>::value << std::endl;
    }


    template<typename T>
    struct is_pointer {
        static constexpr bool value { false };
    };

    template<typename T>
    struct is_pointer<T*>{
        static constexpr bool value { true };//set value as true
    };

    void IsPointer_Test() {
        std::cout << std::boolalpha << is_pointer<int*>::value << std::endl;
        std::cout << std::boolalpha << is_pointer<int>::value << std::endl;
    }
}

namespace Templates::OperatorOverload
{
    class Customer
    {
    private:
        std::string name;
    public:
        explicit Customer(std::string n) : name(std::move(n)) {
        }

        [[nodiscard]]
        std::string getName() const { return name; }
    };

    struct CustomerEq {
        bool operator() (Customer const& c1, Customer const& c2) const {
            std::cout << "CustomerEq() called" << std::endl;
            return c1.getName() == c2.getName();
        }
    };

    struct CustomerHash {
        std::size_t operator() (Customer const& c) const {
            std::cout << "CustomerHash() called" << std::endl;
            return std::hash<std::string>()(c.getName());
        }
    };

    template<typename... Bases>
    struct Overloader : Bases...
    {
        using Bases::operator()...; // OK since C++17
    };

    void FoldExpressionOverload()
    {
        using CustomerOP = Overloader<CustomerHash,CustomerEq>;

        const Customer c1 {"one"}, c2 { "two"};

        CustomerOP{}(c1);
        CustomerOP{}(c1, c2);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Templates::TemplateTypename
{

    template<template <typename Con> typename ResultType>
    auto getData()
    {
        ResultType<int> result;

        if constexpr (requires { result.reserver(1);}) {
            result.reserver(3);
        }

        result.push_back(1);
        result.push_back(2);
        result.push_back(3);

        return result;
    }

    void GetDataTest()
    {
        auto data = getData<std::vector>();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Templates::TestAll()
{
    // Is_Base_Of__Test();
    // Is_Same();
    // Rank();

    // NontypeFunctionTemplates::AddValue();
    // NontypeFunctionTemplates::TransformArray();

    // Compile_Time_IF::ToStringTest();
    // Compile_Time_IF::IsNegative();
    // Compile_Time_IF::GetValueFromPointer2();

    // DefaultTest();
    // DefaultArguments::CreateArray();
    // DefaultArguments::DefaultTypes();
    // DefaultArguments::Metaprogramming();

    // EnableIf::Basic_Test();
    // EnableIf::Test1();
    // EnableIf::Test2();
    // EnableIf::Test_HasIterator();
    // EnableIf::SimpleTest();
    // EnableIf::Is_Odd_Even();
    // EnableIf::Only_For_Arrays();
    // EnableIf::Print_Class_Info();
    // EnableIf::Enable_If_CheckPointer();
    // EnableIf::SizeOf();
    // EnableIf::EnableIf_Typedef();
    // EnableIf::EnableIf_Typedef_1();
    // EnableIf::EnableIf_Typedef_2();
    // EnableIf::TemplateClassParams_Requirenments();
    // EnableIf::Method_Works_OnlyWith_StringsLikeObject();
    // EnableIf::CreateObject_With_Enable_IF();

    // EnableIf_Classes_Applications::Static_Assert_Check();
    // EnableIf_Classes_Applications::Type_Check();


    // Conditional::SimpleTest();
    // Conditional::Condition_Ref_vs_NonRef();


    // Methods::SimpleTemplate();
    // Methods::SimpleTemplate_DefaultType();
    // Methods::ZeroInitialization();
    // Methods::Compare_Raw_Arrays();
    // Methods::Compare_String_Literals();
    // Methods::Template_Params_Only_ForArrays();


    // ---------------------------------------- VARIADIC TEMPLATES ----------------------------------------


    // VariadicTemplates::Sum_Multiple_Variables();
    VariadicTemplates::Print_Multiple_Variables_RecursiveHack();
    // VariadicTemplates::Recursive_Expansion_Two();

    // VariadicTemplates::Variadic_Sizeof();
    // VariadicTemplates::Check_IfTypes_AreSame();
    // VariadicTemplates::ExtendedUsingDeclarations();
    // VariadicTemplates::Compare_Pairs();
    // VariadicTemplates::Sum_Values();
    // VariadicTemplates::All_Equals();
    // VariadicTemplates::Sum_NonTemplate_Params();

    // ---------------------------------------- FOLD EXPRESSION ----------------------------------------

    // FoldExpression::Test();
    // FoldExpression::PrintTest();
    // FoldExpression::PrintTest_Spaces();
    // FoldExpression::Print_Size_Test();
    // FoldExpression::PrintTest1();
    // FoldExpression::FillVector();
    // FoldExpression::FillVector2();
    // FoldExpression::Make_Vector();

    // FoldExpression::GetFirstElementType_CreateVector();

    // FoldExpression::CallFunction();
    // FoldExpression::CallFunctionList();
    // FoldExpression::PassParamsToFunction();

    // FoldExpression::OperatorsTests();
    // FoldExpression::IsContainsZero();
    // FoldExpressionExtended::Init_Vector_Class();

    // FoldExpression_CallClassMethod::FoldClassMethod();
    // FoldExpression_CallClassMethod::FoldedPathTraversals();
    // FoldExpression_CallClassMethod::FoldedMultiClassConstructors();

    // FoldExpression_Examples::Average();
    // FoldExpression_Examples::For_Each();
    // FoldExpression_Examples::CountMatches_Compiletime();
    // FoldExpression_Examples::SumElements_Default();
    // FoldExpression_Examples::InvokeFunc_WithFolds();

    // ---------------------------------------- MIXINS & PACKS ----------------------------------------

    // PackExpansions::Create_Mixin_Class();
    // PackExpansions::Mixin_CallFunc();
    // PackExpansions::IsHomogeneous_Test(); /** Class to check All types are same **/

    // --------------------------------- TYPE_TRAITS -------------------------------------------------

    // TypeTraits::IsVoid_Test();
    // TypeTraits::IsPointer_Test();

    // ------------------------------- Templates <Typename T> ------------------------------------------

    // TemplateTypename::GetDataTest();

    // ------------------------------- Templates Specialization --------------------------------------

    // FunctionSpecialization::Test1();
    // FunctionSpecialization::PartialSpecialization();

    // ClassSpecialization::Test();
    // ClassSpecialization::Test2();

    // PartialSpecialization::Test();
    // PartialSpecialization::Test2();
    // PartialSpecialization::Pow_Test_Static();

    // PartialSpecialization_WithConcepts::Test();
    // PartialSpecialization_WithConcepts::DependingOf_NumberOfParameters();

    // ClassSpecialization_RawArrays::Specialize_ForRawArrays();

    // -------------------------------------------------------------------------------------------------

    // Applications_And_Examples::PassingFunction_to_ClassTemplateArgument();

    // StaticMembersInTemplates::Test();

    // OperatorOverload::FoldExpressionOverload();

    // Forwarding::Test_No_Forward();
    // Forwarding::Test_Integer();
    // Forwarding::Test_IntegerList();
    // Forwarding::Construct_Object();

    // Auto_Template_Params::test();

    // Decltype::Test();
    // Decltype::Test2();

    // CheckIsPointer::IsPointerTest();

    // StaticVariables::FunctionStaticVariales();
    // StaticVariables::ClassStaticVariales();

    // Requires::Test();


    // Applications_And_Examples::Fibonachi_Old_Test();

    // Friends::Access_Private_Field();

}