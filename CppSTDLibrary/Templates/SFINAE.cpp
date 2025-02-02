/**============================================================================
Name        : SFINAE.cpp
Created on  : 02.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SFINAE.cpp
============================================================================**/

#include "SFINAE.h"

#include <iostream>
#include <type_traits>
#include <vector>

namespace
{
    struct A
    {
        virtual void info() const noexcept {
            std::cout << __FUNCTION__ << std::endl;
        }

        virtual ~A() = default;
    };

    struct B : public A
    {
        void info() const noexcept override {
            std::cout << __FUNCTION__ << std::endl;
        }
    };

    struct C {
    };
}



namespace SFINAE::EnableIf
{

    template<class T>
    typename std::enable_if<std::is_floating_point<T>::value, T>::type
    return_and_print_value(T t)
    {
        std::cout << "Returning <float> value " << t << std::endl;
        return t;
    }

    template<class T>
    typename std::enable_if<std::is_integral<T>::value, T>::type
    return_and_print_value(T t) {
        std::cout << "Returning <integer> value " << t << std::endl;
        return t;
    }

    template<class T>
    typename std::enable_if<std::is_same<T, std::string>::value, T>::type
    return_and_print_value(T t)
    {
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
    void print_not_pointer(T var, typename std::enable_if<std::is_integral<T>::value >::type* = 0)
    {
        std::cout << var << std::endl;
    }

    template <class Type>
    typename std::enable_if<std::is_base_of<A, Type>::value, void>::type
    PrintInfo(const Type& var)
    {
        var.info();
    }

    template<typename T>
    typename std::enable_if<std::is_pointer<T>::value, std::remove_pointer_t<T>>::type
    RemovePointerAndPrint(T var)
    {
        return *var;
    }

    template<typename T>
    typename std::enable_if<!std::is_pointer<T>::value, T>::type RemovePointerAndPrint(T var)
    {
        return var;
    }

    // 1. the return type (bool) is only valid if T is an integral type:
    template <class T>
    typename std::enable_if<std::is_integral<T>::value, void>::type
    IsOdd_PrintInfo(T i)
    {
        if (0 == i % 2) {
            std::cout << i << " is Even" << std::endl;
        } else {
            std::cout << i << " is Odd" << std::endl;
        }
    }

    template <class T,class T2 = typename std::enable_if<std::is_integral<T>::value>::type>
    void IsOdd_PrintInfo_2(T i)
    {
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

    void SimpleTest()
    {
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

    void Is_Odd_Even()
    {
        short int i = 1;    // code does not compile if type of i is not integral
        std::cout << std::boolalpha;
        std::cout << "i is odd:  " << is_odd(i) << std::endl;
        std::cout << "i is even: " << is_even(i) << std::endl;


        std::cout << "i is even: " << is_even<int, int>(i) << std::endl;
        std::cout << "i is even: " << is_even<int, long>(i) << std::endl;

        print_not_pointer(7);
        //print_not_pointer(&i);
    }


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


    void Basic_Test()
    {

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

    void Test1()
    {
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

namespace Templates::EnableIf_Classes_Applications
{

    namespace TypeGuardTest
    {
        template<typename T, size_t Size>
        struct Array
        {
            T data[Size]{};

            static_assert(std::is_integral_v<T>, "This function is only designed for integral types.");
            static_assert(sizeof(T) >= 4, "Size > 4 is required");
        };
    }


    void Static_Assert_Check()
    {
        TypeGuardTest::Array<int, 10> array;

#if 0
        // TypeGuardTest::Array<std::string, 10> array;
		// TypeGuardTest::Array<bool, 10> array;
#endif
    }



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

    void Type_Check()
    {
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



void SFINAE::TestAll()
{
    // EnableIf::Basic_Test();
    EnableIf::Test1();
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

}