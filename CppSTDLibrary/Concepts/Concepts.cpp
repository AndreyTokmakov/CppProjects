//============================================================================
// Name        : Concepts.h
// Created on  : 04.11.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Concepts C++20 library src class
//============================================================================

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>


#include <array>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <concepts>
#include <memory>
#include <utility>
#include <functional>

#include "Concepts.h"
#include "../Helpers/Integer.h"


namespace Concepts
{
    struct BaseObject { };
    struct Object { };


    struct AddableObject
    {
        AddableObject operator+(const AddableObject&) {
            return *this;
        }
    };
}

namespace Concepts
{
    struct Object_NotMovable
    {
        Object_NotMovable(Object_NotMovable&& obj) noexcept = delete;
        Object_NotMovable& operator=(Object_NotMovable&& obj) noexcept = delete;
    };

    template<std::movable T>
    class MovableVector {
    private:
        std::vector<T> vector;

    public:
        // MovableVector() {}
        template<class... Args>
        void emplace_back(Args&&... params) {
            vector.emplace_back(std::forward<Args>(params)...);
        }
    };

    void MovableTest()
    {
        MovableVector<Object> v;
        // MovableVector<Concepts::Object_NotMovable> v;
    }
};


namespace Concepts {
    struct NonHashableClass { };
    struct HashableClass { };
}

namespace std {
    template <>
    struct hash<Concepts::HashableClass> {
        inline size_t operator()(const Concepts::HashableClass& x) const {
            return  31;
        }
    };
}


namespace Concepts::STDConcepts
{
    // copyable

    struct NonMovable {
        NonMovable() = default;
        ~NonMovable() = default;

        NonMovable(const NonMovable&) = default;
        NonMovable& operator=(const NonMovable&) = default;

        NonMovable(NonMovable&&) = delete;
        NonMovable& operator=(NonMovable&&) = delete;
    };

    struct NonCopyableType {
        NonCopyableType() = default;
        ~NonCopyableType() = default;

        NonCopyableType(const NonCopyableType&) = default;
        NonCopyableType& operator=(const NonCopyableType&) = default;

        NonCopyableType(NonCopyableType&&) = delete;
        NonCopyableType& operator=(NonCopyableType&&) = delete;
    };

    struct CopyableType {
        CopyableType() = default;
        ~CopyableType() = default;

        CopyableType(const CopyableType&) = default;
        CopyableType& operator=(const CopyableType&) = default;

        CopyableType(CopyableType&&) = default;
        CopyableType& operator=(CopyableType&&) = default;
    };


    template <typename T>
    void fun(T t) requires std::copyable<T> {
        std::cout << typeid(t).name() << " is copyable\n";
    }

    void Copyable()
    {
        // fun(NonMovable{});         // ERROR
        // fun(NonCopyableType{});    // ERROR
        fun(CopyableType{});
    }
}

namespace Concepts::Custom_Concepts
{

    template <class T>
    concept Integral = std::is_integral<T>::value;

    template <class T>
    concept SignedIntegral = Integral<T> && std::is_signed<T>::value;

    template <class T>
    concept UnsignedIntegral = Integral<T> && !SignedIntegral<T>;

    template <class T>
    concept IsNotDoubleType = !std::is_same_v<T, double>;

    template<SignedIntegral T>
    void handleSignedValue(T val) {
    }

    template<UnsignedIntegral T>
    void handleUnsignedValue(T val) {
    }

    void It_Not_Void_Test()
    {
        auto foo = [] <IsNotDoubleType T>(T v) {};
        double v = 1'23;

        // foo(v);    WILL NOT COMPIE
    }

    void Signed_Tests()
    {
        {
            int a{ 0 };
            handleSignedValue(a);
        }
        {
            unsigned int a{ 0 };

            // ERROR !!!!!!!
            // handleSignedValue(a);
        }
    }

    void Unsigned_Tests()
    {
        {
            int a{ 0 };

            // ERROR HERE
            // handleUnsignedValue(a);
        }
        {
            unsigned int a{ 0 };
            handleUnsignedValue(a);
        }
    }

    template<typename T>
    concept Addable = requires(T a, T b) {
        a + b;
        b + a;
        { a + b } -> std::same_as<T>;
        { b + a } -> std::same_as<T>;
    };

    template<typename T>
    concept Subtractable = requires(T a, T b) {
        a - b;
        b - a;
        { a - b } -> std::same_as<T>;
        { b - a } -> std::same_as<T>;
    };

    template<typename T> requires Addable<T>
    auto func1(T a, T b) -> decltype(a - b)
    requires Subtractable<T> // Addable<T> && Subtractable<T>
    {
        return a - b;
    }

    template<Addable T>
    T add(T a, T b)
    {
        return a + b;
    }

    void Addable_Test()
    {
        add(1, 2);
        /// add("", "");
        add(std::string{""}, std::string{""});
        /// add(BaseObject{}, BaseObject{});
        add(AddableObject{}, AddableObject{});

        // AddableObject a, b;
        // auto c = a + b;
    }

    void Lambda_Concept_Tests() {

        std::cout << func1(10, 3) << std::endl;

        auto func2 = []<typename T> requires Addable<T>(T a, T b) -> decltype(a - b)
        requires Subtractable<T> {
            return a - b;
        };

        std::cout << func2(10, 3) << std::endl;
    }

    namespace Simple
    {

        template<typename Type>
        concept CheckCustomSize = requires(Type val) {
            { val++ } -> std::same_as<int>; // Type ++ shall return BOOL type
        };

        template<typename Type>
        concept StringCheck = requires(Type val) {
            { val.size() } -> std::same_as<size_t>; // Type ++ shall return BOOL type
            // { typeid(val) } -> std::same_as<std::string>;
        };

        template<typename Type>
        concept IsString = std::is_same_v<std::string, Type>;

        template<typename Type>
        concept IsStringOrFloat = std::is_same_v<std::string, Type> ||
                                  std::is_same_v<float, Type>;

        template<typename Type>
        concept Str_Float_Bool = requires(Type v) {
            //{ Type{} }  -> std::same_as<int>
            { Type{} }  -> std::same_as<std::string>;
            // { Type{} }  -> std::same_as<int>;
        };






        template <CheckCustomSize T>
        void func(T var) {
            std::cout << typeid(T).name() << " = " << sizeof(T) << std::endl;
        }

        template <StringCheck T>
        void func2(T var) {
            std::cout << typeid(T).name() << " = " << sizeof(T) << std::endl;
        }

        template <IsString T>
        void take_string(T var) {
            std::cout << typeid(T).name() << " = " << sizeof(T) << std::endl;
        }

        template <IsStringOrFloat T>
        void take_string_or_float(T var) {
            std::cout << typeid(T).name() << " = " << sizeof(T) << std::endl;
        }

        /*
        template <Str_Float_Bool T>
        void take_str_float_bool(T var) {
            std::cout << typeid(T).name() << " = " << sizeof(T) << std::endl;
        }
        requires
        */
    }

    void SimpleTest() {
        int var = 0;

        Simple::func(var);

        std::string text = "";
        Simple::func2(text);


        std::vector<int> v;
        Simple::func2(v);


        Simple::take_string(text);


        Simple::take_string_or_float(3.0f);

        // Simple::take_str_float_bool(text);
    }

    //================================================================================

    template<typename T>
    concept is_pointer = requires(T p) {
        *p;                                   // operator * has to be valid
        {p < p} -> std::convertible_to<bool>; // < yields bool
        p == nullptr;                         // can compare with nullptr
    };

    void IsPointer()
    {
        auto testFunc = [](is_pointer auto a) { };
        int v = 12;
        int* vp = &v;

        // testFunc(v); // INFO: Shall not compile
        testFunc(vp);
    }

    //=================================================================================

    template<typename T>
    concept Hashable = requires(T a) {
        { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
    };

    template<typename Type>
    concept Incrementable = requires(Type var) {
        var++; ++var;
    };

    template<typename Type>
    concept Decreamentable = requires(Type var) {
        var--;
        --var;
    };

    //--------------------------------------------------------------------------------------//

    template<Hashable T>
    void printHash(T) {  // constrained C++20 function template
        //
    }



    void Hashable_Test() {
        // NonHashableClass a;
        HashableClass a;

        printHash(a);
    }

    //---------------------------------------------------------------------------------//

    template<typename T>
    concept HasSizeAndSwapable = requires(T & a, T & b) {
        { a.swap(b) } noexcept;
        { a.size()  } -> std::convertible_to<std::size_t>;
        // ......
    };


    void Has_Size_and_Swappable() {
        // HasSizeAndSwapable - src???
    }

    //---------------------------------------------------------------------------------//

    template<Incrementable T>
    void Increment(T value) {
        std::cout << "Before: " << value << std::endl;
        value++;
        std::cout << "After: " << value << std::endl;
    }

    template<typename T>
    void Increment2(T value) requires Incrementable<T> {
        std::cout << "Before: " << value << std::endl;
        value++;
        std::cout << "After: " << value << std::endl;
    }

    template<typename T> requires Incrementable<T>
    void Increment3(T value) {
        std::cout << "Before: " << value << std::endl;
        value++;
        std::cout << "After: " << value << std::endl;
    }

    void Incrementable_Test()
    {
        auto Increment4 = [](Incrementable auto var) {
            std::cout << "Before: " << var << std::endl;
            var++;
            std::cout << "After: " << var << std::endl;
        };


        Increment(1);
        Increment2(1);
        Increment3(1);
        Increment4(1);
#if 0
        Increment(std::string("s"));
            Increment2(std::string("s"))
#endif
    }

    //---------------------------------------------------------------------------------//

    template <class T>
    concept TestObjecttBased = std::is_base_of<BaseObject, T>::value;

    template <class T, class U>
    concept Derived = std::is_base_of_v<U, T>;


    template<TestObjecttBased T>
    class SomeClass {
        T v;
    };

    template<Derived<BaseObject> T>
    class SomeClass1 {
        T v;
    };

    void Custom_Concept_IsBaseOf() {
        [[maybe_unused]]
        SomeClass<BaseObject> a;

        [[maybe_unused]]
        SomeClass1<BaseObject> a1;

#if 0
        // Will not compile since TestObject is not Base of 'std::string':
            SomeClass<std::string> b;
            SomeClass1<std::string> b1;
#endif

        std::cout << "OK. Done!" << std::endl;
    }

    //===========================================================================//

    class BaseX {
        friend bool operator<(const BaseX& a, const BaseX& b) noexcept;
        //friend bool operator>(const BaseX& a, const BaseX& b) noexcept;
    };

    bool operator<(const BaseX& a, const BaseX& b) noexcept {
        return true;
    }

    /*
    bool operator>(const BaseX& a, const BaseX& b) noexcept {
        return true;
    }
    */

    template<typename T>
    concept Comparable = requires(T const& a, T const& b) {
        { a < b } -> std::same_as<bool>;
        { a > b } -> std::same_as<bool>;
        { a == b };
        { a == b } -> std::convertible_to<bool>;
        { a == b } noexcept -> std::convertible_to<bool>;
    };
    // NOTE: -> std::same_as<bool> can be skiped
    // the expression {a > b} must be valid  AND std::same_as<decltype((a > b)), bool>
    // must be satisfied i.e., (a > b) must return 'bool'

    template<Comparable T>
    void compare(T a, T b) {
    }

    void Comparable_Test() {
        {
            std::string a{ "aaa" }, b{ "aaa" };
            std::cout << (a < b) << std::endl;
            compare(a, b);
        }
#if 0
        {
                BaseObject a;
                BaseObject b;
                compare(a, b); // ERROR: (a < b) - not implemented
            }
#endif
#if 0
        {
                BaseX a;
                BaseX b;
                compare(a, b); // ERROR: (a < b) - OK, {a > b} - NOT
            }
#endif
    }

    //===========================================================================//

    template<typename T, typename... Args>
    concept Constructible = requires(Args&&... params) {
        { T(std::forward<Args>(params)...) };
    };



    template<typename T, typename... Args>
    requires Constructible<T, Args...>
    void foo(T a, Args... params) {
    }


    struct SS {
        SS() {
        }

        SS(int a, std::string&& str, int b) {
        }

        SS(int a, std::string&& str, int b, int c) {
        }
    };


    void Multi_Arguments_Tests() {
        {
#if 0
            foo(SS{}, 1);   // ERROR: no SS(int a) constructor
#endif

#if 0
            foo(SS{}, 1, ""); // ERROR: no SS(int a) constructor
#endif

            foo(SS{}, 1, "", 2);
            foo(SS{}, 1, "", 2, 3);
        }
    }

    // ---------------------------------------------------------------

    template<typename Coll>
    concept HasPushBack = requires (Coll c, typename Coll::value_type v) {
        c.push_back(v);
    };

    template<typename Coll>
    concept SupportsPushBack = requires(Coll coll)
    {
        // type requirement: `typename Coll`, Coll type must be a valid type
        typename Coll::value_type;

        coll.push_back(std::declval<typename Coll::value_type>());

        requires requires(Coll::value_type&& var) {
            coll.push_back(std::move(var));
        };
    };

    void PushBackToSomeVector_V1(HasPushBack auto& collection,
                                 const auto& v) {
        collection.push_back(v);
    }

    void PushBackToSomeVector_V1_2(SupportsPushBack auto& collection,
                                   const auto& v) {
        collection.push_back(v);
    }

    void PushBackToSomeVector_V2(auto& collection,
                                 const auto& v) {
        if constexpr (requires { collection.push_back(v); }) {
            collection.push_back(v);
        }
        else {
            collection.insert(v);
        }
    }


    void CheckCollection_HasPushBack_Method()
    {
        {
#if 0       // Will not compile because of HasPushBack
            std::set<int> v;
                PushBackToSomeVector_V1(v, 1);
#endif
        }

        {
            std::vector<int> v;
            PushBackToSomeVector_V1(v, 1);
            PushBackToSomeVector_V1_2(v, 1);
        }
    }

    void CheckCollection_HasPushBack_Method_2()
    {
        {
            std::set<int> v;
            PushBackToSomeVector_V2(v, 1);
        }

        {
            std::vector<int> v;
            // PushBackToSomeVector_V2(v, 1);
        }

    }

    //-------------------------------------------------------------

    struct Money
    {
        double value {};
    };

    template<typename T>
    concept PricedItem = requires(T item) {
        { item.price() } -> std::same_as<Money>;
    };

    template<int taxRate, PricedItem Item>
    class Taxed : private Item {
    public:
        template<typename... Args>
        explicit Taxed(Args&& ... args): Item {std::forward<Args>(args)...} {
            /** .... **/
        }

        [[nodiscard]]
        Money price() const {
            return Item::price() * (1.0 + (taxRate / 100));
        }
    };

    struct Ticket {
        [[nodiscard]] Money price() const {
            return Money {10.0f};
        }
    };

    struct SomethingPriceless {
        [[nodiscard]] std::string name() const {
            return {"sdsdsd"};
        }
    };

    void Check_BaseType_ContainsMethod_ReturnValue()
    {
        Taxed<10, Ticket> ticket {};

        // Taxed<10, SomethingPriceless> bad {};
    }
};


namespace Concepts::NestedConcepts
{
    struct Droid {
        Droid clone(){
            return Droid{};
        }
    };

    struct DroidV2 {
        Droid clones(){
            return Droid{};
        }
    };

    template<typename C>
    concept Clonable = requires (C clonable) {
        clonable.clone();
        requires std::same_as<C, decltype(clonable.clone())>;
    };

    template<typename C>
    concept Clonable2 = requires (C clonable) {
        { clonable.clone() } -> std::same_as<C>;
    };


    void CheckMethodReturnType()
    {
        Clonable auto c = Droid{};
        Clonable2 auto c2 = Droid{};

        // NOTE: nested requirement 'same_as<C, decltype (clonable.clone())>' is not satisfied
        // Clonable auto c3 = DroidV2{};
        // Clonable2 auto c4 = DroidV2{};
    }
}

namespace Concepts::Concepts_With_Auto
{
    class Base {
    };

    class ComparableBase {
        friend bool operator<(const ComparableBase& a, const ComparableBase& b) noexcept;
        friend bool operator>(const ComparableBase& a, const ComparableBase& b) noexcept;
    };

    bool operator<(const ComparableBase& a, const ComparableBase& b) noexcept {
        return true;
    }

    bool operator>(const ComparableBase& a, const ComparableBase& b) noexcept {
        return true;
    }

    /*****************************************/

    template<typename T>
    concept Comparable = requires(T const& a, T const& b) {
        {a < b} -> std::same_as<bool>;
        {a > b} -> std::same_as<bool>;
    };


    void handle(const Comparable auto& a, const Comparable auto& b) {
    }


    /*****************************************/

    void Test() {
#if 0
        {
            Base a, b;
            handle(a, b);
        }
#endif

        {
            ComparableBase a, b;
            handle(a, b);
        }
    }

    //======================================================================

    void print(std::unsigned_integral auto u) {
        std::cout << "Unsigned integral: " << u << '\n';
    }


    void Print_Tests() {
        //  print(1); // ERROR
        print(1u);
    }
}



namespace Concepts::Requires {

    template<typename T>
    requires std::integral<T>
    auto gcd(T a, T b) {
        if( b == 0 ) return a;
        else return gcd(b, a % b);
    }

    template<typename T>
    auto gcd1(T a, T b) requires std::integral<T> {
        if( b == 0 ) return a;
        else return gcd1(b, a % b);
    }

    template<std::integral T>
    auto gcd2(T a, T b) {
        if( b == 0 ) return a;
        else return gcd2(b, a % b);
    }

    auto gcd3(std::integral auto a, // (4)
              std::integral auto b) {
        if( b == 0 ) return a;
        else return gcd3(b, a % b);
    }

    void FunctionDefinition_Examples() {
        std::cout << "gcd(100, 10)= "  <<  gcd(100, 10)  << '\n';
        std::cout << "gcd1(100, 10)= " <<  gcd1(100, 10)  << '\n';
        std::cout << "gcd2(100, 10)= " <<  gcd2(100, 10)  << '\n';
        std::cout << "gcd3(100, 10)= " <<  gcd3(100, 10)  << '\n';
    }

    //------------------------------------------------------------


    namespace Simple {

        template <typename T> requires (sizeof(T) > 4)
        void func(T var) {
            std::cout << typeid(T).name() << " = " << sizeof(T) << std::endl;
        }

        template <typename T> requires (sizeof(T) > 4 && sizeof(T) < 16)
        void func2(T var) {
            std::cout << typeid(T).name() << " = " << sizeof(T) << std::endl;
        }
    }

    void SimpleTest() {
#if 0
        {
            long a{ 0 };
            Simple::func(a); // Will not compile (sizeof(int) > 4) == false
        }
#endif
        {
            double d{ 1.2 };
            Simple::func(d); // Will not compile (sizeof(int) > 4) == false
        }
#if 0
        {
            std::string var;
            Simple::func2(var); // Will not compile (sizeof(int) > 4) == true BUT [sizeof(T) < 16] = false
        }
#endif
    }

    //------------------------------------------------------------------------------------

    template <unsigned int i>
    requires (i <= 20)             // (1)
    int sum(int j) {
        return i + j;
    }

    void Constrain_Template_Param_Value()
    {
        std::cout << "sum<20>(2000): " << sum<20>(2000) << std::endl;
        // std::cout << "sum<23>(2000): " << sum<23>(2000) << std::endl; // ERROR

    }

    //====================================================================================



    class Object {
    };

    class Base : public Object {
    };

    class Derived : public Base {
    };

    class Shape {
    };

    //--------------------------------------------------------------------------//

    template <class A, class B>
    concept DerivedFrom = std::is_base_of<A, B>::value;

    template <typename T>
    concept ObjectBased = DerivedFrom<Object, T>;

    template <typename T>
    concept ObjectBasedEx = requires(T var) {
        { true == std::is_same_v<Object, T> };
    };


    template <typename T>
    void foo(T param) requires ObjectBased<T> {
    }

    template <typename T>
    void fooEx(T param) requires ObjectBasedEx<T> {
    }

    //--------------------------------------------------------------//


    void Test1() {
        Base b;

        foo(b);
        fooEx(b);

        Derived d;
        foo(d);
        fooEx(d);

#if 1
        Shape s;
        //foo(s);
        fooEx(s);
#endif

    }

    //--------------------------------------------------------------//

    template<typename T>
    requires std::is_same<T, std::string>::value
    void print_string(T text) {
        std::cout << text << std::endl;
    }

    void Test2() {

        print_string(std::string("some_text"));

#if 0
        print_string("some_text");
#endif

#if 0
        print_string(123);
#endif
    }

    //--------------------------------------------------------------//

    template<typename Container>
    concept Sizable = requires(const Container & c) {
        { c.size() } -> std::same_as<size_t>;
    };

    void Assert_With_Requires() {
        using T = std::vector<int>;
        static_assert(Sizable<T>); // Fail
    }

    //----------------------------------------------------------------------//

    namespace DestructorClasses {

        class A {

        };

        class B {
            ~B() = default;
        };

    }


    template <typename T>
    concept NoExceptDestructible = requires (T & value) {
        { value.~T() } noexcept;
    };

    void Test_Destructor_NoExcept() {
        static_assert(NoExceptDestructible<std::string>);
        static_assert(NoExceptDestructible<int>);
        static_assert(NoExceptDestructible<DestructorClasses::A>);
#if 0
        static_assert(NoExceptDestructible<DestructorClasses::B>);
#endif
    }

    //----------------------------------------------------------------------------//

    template <typename C>
    concept Character = std::same_as<std::remove_cv_t<C>, char>
                        || std::same_as<std::remove_cv_t<C>, char8_t>
                        || std::same_as<std::remove_cv_t<C>, char16_t>
                        || std::same_as<std::remove_cv_t<C>, char32_t>
                        || std::same_as<std::remove_cv_t<C>, wchar_t>;

    void _accept_char_only(Character auto c) {

    }

    void  Test_is_Character() {
        // _accept_char_only(1);
        _accept_char_only('c');
    }

    //-----------------------------------------------------------------------------------------//

    struct Money {
        uint64_t value{};
    };


    template<typename T> requires std::is_arithmetic_v<T>
    [[nodiscard]]
    Money operator*(const Money& money, T factor) {
        return Money {static_cast<uint64_t>( money.value * factor )};
    }

    void CheckOperatorArgument_IsArithmetic()
    {
        Money val {13};
        auto res = val * 10;
    }
}



namespace Concepts::Requires_With_Constexpr {

    template<typename T>
    struct has_capacity {
        static constexpr bool value{ false };
    };

    template<typename T>
    struct has_capacity<std::vector<T>> {
        static constexpr bool value{ true };
    };

    template <class T>
    concept HasCapacity1 = requires(T a) {
        { a.capacity() } -> std::convertible_to<std::size_t>;
    };

    template <class T>
    concept HasCapacity2 = std::is_same_v<T, std::vector<int>>;

    //-------------------------------------------------------------//

    template<HasCapacity1 T>
    size_t getCapacity1(const T& container) {
        if constexpr (has_capacity<T>::value)
            return container.capacity();
        return 0;
    }

    template<HasCapacity2 T>
    size_t getCapacity2(const T& container) {
        return container.capacity();
    }

    template<typename T>
    size_t getCapacity3(const T& container) {
        if constexpr (has_capacity<T>::value)
            return container.capacity();
        return 0;
    }

    template<typename T>
    size_t getCapacity4(const T container) {
        // if constexpr (requires std::integral<T>::value == true ) { /// DO NOT COMPILE
        if constexpr (false) {
            return container.capacity();
        }
        else {
            return container.size();
        }
    }

    //-------------------------------------------------------------//


    void Vector_vs_Array() {
        std::vector<int> numbers(4);
        std::array<int, 4> data{ 1,2,3,4 };

        std::cout << "Capacity (" << typeid(numbers).name() << ") = " << getCapacity1(numbers) << std::endl;
        // std::cout << "Capacity (" << typeid(data).name() << ") = " << getCapacity1(data) << std::endl;      // SHALL NOT COMPILE

        std::cout << std::endl;

        std::cout << "Capacity (" << typeid(numbers).name() << ") = " << getCapacity2(numbers) << std::endl;
        // std::cout << "Capacity (" << typeid(data).name() << ") = " << getCapacity2(data) << std::endl;      // SHALL NOT COMPILE

        std::cout << std::endl;

        std::cout << "Capacity (" << typeid(numbers).name() << ") = " << getCapacity3(numbers) << std::endl;
        std::cout << "Capacity (" << typeid(data).name() << ") = " << getCapacity3(data) << std::endl;

        std::cout << std::endl;

        std::cout << "Capacity (" << typeid(numbers).name() << ") = " << getCapacity4(numbers) << std::endl;
        std::cout << "Capacity (" << typeid(data).name() << ") = " << getCapacity4(data) << std::endl;
    }

    //----------------------------------------------------------------

    struct First {
        int length() const {
            return 1;
        }

        int size() const {
            return 2;
        }
    };

    template <typename T>
    int getNumberOfElements(T& val)
    {
        if constexpr (requires(T v){ { v.length() } -> std::convertible_to<int>; }) {
            return val.length();
        }
        else if constexpr (requires(T v){ {v.size() } -> std::convertible_to<int>; }) {
            return val.size();
        }
        else return -1;
    }

    void Constexpr_Check_Method()
    {
        First first;
        std::cout << "getNumberOfElements(first): "  << getNumberOfElements(first) << '\n';
    }



    template<typename Coll>
    concept HasPushBack = requires (Coll coll, typename Coll::value_type v) {
        coll.push_back(v);
    };

    template<typename Coll>
    concept SupportsPushBack = requires(Coll coll)
    {
        // type requirement: `typename Coll`, Coll type must be a valid type
        typename Coll::value_type;

        coll.push_back(std::declval<typename Coll::value_type>());

        requires requires(typename Coll::value_type&& var) {
            coll.push_back(std::move(var));
        };
    };


    void PushBackToSomeCollection(auto& collection,
                                  const auto& v) {
        if constexpr (requires { collection.push_back(v); }) {
            collection.push_back(v);
        }
        else {
            collection.insert(v);
        }
    }

    void PushBackToSomeCollection_CheckPushback_vs_Insert()
    {
        std::set<int> set;
        std::vector<int> vector;
        std::list<int> list;

        PushBackToSomeCollection(set, 1);
        PushBackToSomeCollection(vector, 1);
        PushBackToSomeCollection(list, 1);
    }
}

namespace Concepts::Requires
{

    template<typename T>
    concept Addable = requires(T a, T b) {
        a + b;
    };

    template<typename T>
    concept Dividable = requires(T a, T b) {
        a / b;
    };

    template<typename T>
    concept SomeComplexConcept = requires (T x) // optional set of fictional parameter(s)
    {
        // simple requirement: expression must be valid
        // expression must be valid
        x++;

        // type requirement: `typename T`, T type must be a valid type
        typename T::value_type;
        // typename S<T>;

        // compound requirement: {expression}[noexcept][-> Concept];
        // {expression} -> Concept<A1, A2, ...> is equivalent to
        // requires Concept<decltype((expression)), A1, A2, ...>
        {*x};  // dereference must be valid
        {*x} noexcept;  // dereference must be noexcept


        // dereference must  return T::value_type
        {*x} noexcept -> std::same_as<typename T::value_type>;

        // nested requirement: requires ConceptName<...>;
        // constraint Addable<T> must be satisfied
        requires Addable<T>;

        // nested requirement: requires ConceptName<...>;
        // constraint Dividable<T> must be satisfied
        requires Dividable<T>;


        { x.~T() } noexcept;
    };


    template<typename T>
    requires SomeComplexConcept<T>
    void foo(T value) {

    }


    void Complex_Concepts_Tests() {
#if 0
        foo(1);
#endif
    }
}

namespace Concepts::RequiresSequence {

    // Example #1
    template<typename Type>
    concept ITestable1 = requires (Type val, int a, std::string str) {
        ++val;
        val + a;
        str + val;
        str < val;
    };


    // Example #2
    template<typename Type>
    concept ITestable2 = requires (Type val, int a, std::string str) {
        typename Type::pointer;
        typename Type::reference;
        typename std::vector<Type>;
    };


    // Example #3
    // Compound
    template<typename Type>
    concept CompoundConcept = requires (Type val, int v) {
        // Type has noexcept dtor()
        { val.~Type() } noexcept;

        { val + v} -> std::same_as<decltype(v)>;
    };



    //====================================================================//

    template<typename T> requires CompoundConcept<T>
    void func1(T v) {
    }

    void Test1() {

        Integer v{ 1 };
        func1(v);

    }
}

namespace Concepts::SFINAE {

    void foo(auto param) {
        std::cout << "Func 1 called\n";
    }

    void foo(auto param) requires (sizeof(param) < 2) {
        std::cout << "Func 2 called\n";
    }

    void ChooseOverloadedFunc_WithRequire() {
        std::cout << "------------------------------ Test1 -----------------------------\n";
        foo(23);


        std::cout << "\n------------------------------ Test2 -----------------------------\n";
        foo('c');
    }
}


namespace Concepts::Concepts_With_Lambdas {

    void Params_Concepts() {

        auto sum1 = []<std::integral T> (T a, T b) -> T { return a + b; };
        // auto sum2 = [](T a, T b)  requires std::integral<T> { return a + b; };

        auto a = sum1(1, 2);
        std::cout << a << std::endl;

#if 0
        auto a = sum1(1.3, 2);
#endif
    };


    void ReturnValue_Concepts() {

        auto sum = []<typename T> (const T & a, const T & b) constexpr noexcept
                -> auto requires std::integral<T>
        {
            return a + b;
        };

        {
            auto result = sum(1, 2);
            std::cout << result << std::endl;
        }

#if 0
        {
            // ERROR: result is std::integral<>
            auto result = sum(1.1, 2.2);
        }
#endif
    };
}


namespace Concepts::Tests
{
    template<typename _Ty>
    class Vector : public std::vector<_Ty> {

    public:
        // private:
        size_t size() const {
            return 0;
        }
    };

    template<typename Container>
    concept Sizable = requires(const Container & c) {
        {c.size()} -> std::same_as<size_t>;
    };

    void StaticAssert_Conects() {
        using T = std::vector<int>;
        using T1 = Vector<int>;

        static_assert(Sizable<T>);
        static_assert(Sizable<T1>);

        auto s = T1().size();
        std::cout << s << std::endl;
        std::cout << std::boolalpha << (typeid(size_t) == typeid(s)) << std::endl;
    }

    //====================================================================================

    template<typename T>
    struct S {
        explicit(!std::is_convertible_v<T, int>) S(T) {}
    };

    void f() {
        S<char> sc = 'x';           // OK

#if 0
        S<std::string> ss1 = "_x";   // Error, constructor is explicit
#endif

        S<std::string> ss2{ "" };  // OK
    }


    void LAMBDA_CONCEPT() {

        auto sum1 = []<std::integral T> (T a, T b) -> T { return a + b; };
        // auto sum2 = [](T a, T b)  requires std::integral<T> { return a + b; };

        auto a = sum1(1, 2);
        std::cout << a << std::endl;

#if 0
        auto a = sum1(1.3, 2);
#endif

    }

    //-----------------------------------------------------------------

    struct A {
    };

    struct B {
    };

    std::ostream& operator<<(std::ostream& stream, const B& b) {
        stream << "B()";
        return stream;
    }

    template<typename T>
    concept Printable = requires(std::ostream& os, const T& msg)
    {
        {os << msg};
    };

    //now impose the constraint define above on print method
    template <Printable T>
    void print(const T& msg){
        std::cout << msg;
    }

    void Printable_Test()
    {
        print(1);
        // print(A{});
        print(B{});
    }
}

namespace Concepts::Function_Constrains
{

    template<typename Func>
    concept FuncReturnsInt = requires(Func f) {
        { f() } -> std::same_as<int>;
    };

    template<FuncReturnsInt Func>
    int foo(Func func) {
        return func();
    }

    void Test_Function_Return_Int()
    {
        auto f1 = []()-> int { return 1; };
        auto f2 = []() -> std::string { return {}; };

        auto a1 = foo(f1);
        std::cout << typeid(a1).name() << " = " << a1 << std::endl;
#if 0
        auto a2 = foo(f2);
#endif
    }
}


namespace Concepts::Function_Constrains
{
    template<typename Type>
    concept IsFunctor = requires (Type obj) {
            { obj(0) } -> std::same_as<int>;
    };

    void client(const IsFunctor auto& obj)
    {
        std::cout << obj(10) << std::endl;
    }

    struct FunctorOne
    {
        int operator()(int x) const {
            return x * 10;
        }
    };

    struct FunctorTwo
    {
        int operator()() const {
            return 10;
        }
    };

    void CheckTypeIsFunctor()
    {
        client(FunctorOne{});
        // client(FunctorTwo{});
    }
}


namespace Concepts::Callables
{
    template<std::invocable Func>
    void call(Func func) {
        func();
    }

    template<typename Func>
    void call_2(Func func) requires std::invocable<Func> {
        func();
    }

    void Test_Invocable()
    {
        auto F = [] { std::cout << "Ok" << std::endl; };

        call(F);
        call_2(F);

        std::cout << "----------------- Classic invocable -------------\n";

        std::invoke(F);
        F();
    }
}

namespace Concepts::Callables
{
    template<std::regular_invocable Func>
    void call_regular(Func func) {
        func();
    }

    template<typename Func>
    void call_regular_2(Func func) requires std::regular_invocable<Func> {
        func();
    }

    void Test_Invocable_Regular()
    {
        auto F = [] { std::cout << "Ok" << std::endl; };

        call_regular(F);
        call_regular_2(F);

        std::cout << "----------------- Classic invocable -------------\n";

        std::invoke(F);
        F();
    }
}

namespace Concepts::Callables
{
    template<std::predicate Func>
    void check_1(Func func) {
        func();
    }

    template<typename Func>
    void check_2(Func func) requires std::predicate<Func> {
        func();
    }

    void Test_Predicate() {
        auto F = []()-> bool {
            std::cout << "Ok" << std::endl;
            return true;
        };

        check_1(F);
        check_2(F);

        std::cout << "----------------- Cassic invokation -------------\n";

        std::invoke(F);
        F();
    }

    //----------------------------------------------------------------------------

    template<std::predicate<int> Func>
    void check_with_params_1(Func func, int param)
    {
        func(param);
    }

    template<typename Func>
    void check_with_params_2(Func func, int param) requires std::predicate<Func, int>
            {
        func(param);
    }

    void Test_Predicate_WithParams() {
        auto func = [](int x)-> int {
            std::cout << "Ok" << std::endl;
            return true;
        };

        check_with_params_1(func, 1);
        check_with_params_2(func, 1);

        std::cout << "----------------- Cassic invokation -------------\n";

        std::invoke(func, 123);
        func(123);
    }

    //----------------------------------------------------------------------------

    template<std::predicate<int>... Func>
    void check_with_params_varidic_1(Func... func) {
        (func(123), ...);
    }

    template<typename... Func>
    void check_with_params_varidic_2(Func... func)
    requires (std::predicate<Func, int> && ...)
    {
        (func(321), ...);
    }

    void Test_Predicate_WithParams_Variadic()
    {
        auto func1 = [](int x)-> bool {
            std::cout << "Ok 1: " << x << std::endl;
            return true;
        };

        auto func2 = [](int x)-> bool {
            std::cout << "Ok 2: " << x << std::endl;
            return true;
        };

        check_with_params_varidic_1(func1, func2);
        check_with_params_varidic_2(func1, func2);
    }

    //----------------------------------------------------------------------------

    void PrintVecIf(const std::vector<int>& vec,
                    std::predicate<int> auto func)
    {
        for (auto& elem : vec)
            if (func(elem))
                std::cout << elem << ' ';
        std::cout << std::endl;
    }

    void Test_Predicate_PrintVector() {
        std::vector<int> numbers(10);
        std::iota(numbers.begin(), numbers.end(), 0);

        PrintVecIf(numbers, [](int v) { return v % 2 == 0; });
    }
}


namespace Conversations {

    void handle_string(const std::convertible_to<std::string> auto& val) {

    }

    struct MyString : std::string {
    };

    struct MyStringNOT {
    };

    void String_Test() {

        // handle_string(1);
        handle_string("");
        handle_string(std::string{ "12345" });

        MyString s;
        handle_string(s);

#if 0
        MyStringNOT s1;
        handle_string(s1);
#endif
    }

    void func_handle_strings(const auto& x)
    requires std::convertible_to<decltype(x), std::string>
    {
        const std::string s = x;
    }

    template<typename T>
    requires std::convertible_to<T, std::string>
    void func_handle_strings_template(const T& x)
    {
        const std::string s = x;
    }

    void Convertible_To_Tests() {
        func_handle_strings("12345" );
        func_handle_strings(MyString{} );
        // func_handle_strings(MyStringNOT{} );

        func_handle_strings_template("12345" );
        func_handle_strings_template(MyString{} );
        // func_handle_strings_template(MyStringNOT{} );

    }
}



namespace ValidatTypeContains_Members_or_Types {

    struct TestTypeGood {
        using ElementType = int;
    };

    struct TestTypeBad {
    };

    template <typename T>
    concept type_test = requires {
        typename T::ElementType; // ElementType member type must exist
    };

    void __handle_TestType(const type_test auto& val) {
    }



    template <std::integral T>
    struct S {
        T v;
    };

    template <typename T>
    concept ValidForTemplate = requires {
        typename S<T>; // checks whether S<T>  is a valid template substitution
    };

    void __handle_Template_Substitution(const ValidForTemplate auto& val) {
    }



    void ContainTypeDef() {
        TestTypeGood s;
        __handle_TestType(s);

#if 0
        TestTypeBad b;
        __handle_TestType(b);
#endif
    }

    void Valid_Template_Substitution() {

        __handle_Template_Substitution(123);

#if 0
        __handle_Template_Substitution(123f);
        __handle_Template_Substitution("");
#endif
    }
}

namespace Concepts::IntegerConcepts
{
    template<typename T>
    concept Arithmetic = std::is_arithmetic<T>::value;

    Arithmetic auto sum(Arithmetic auto t,
                        Arithmetic auto t2) {
        return t + t2;
    }

    void IntegerSum()
    {
        std::cout << typeid(sum(5.5, 5.5)).name() << '\n';     // double
        std::cout << typeid(sum(5.5, true)).name() << '\n';    // double
        std::cout << typeid(sum(true, 5.5)).name() << '\n';    // double
        std::cout << typeid(sum(true, false)).name() << '\n';  // int
    }
}


namespace Concepts::Noexcept
{
    class A {
    };

    class B {
        ~B() = default;
    };



    template <typename T>
    concept NoExceptDestructible = requires (T & value) {
        { value.~T() } noexcept;
    };

    void Test_Destructor_NoExcept() {
        static_assert(NoExceptDestructible<std::string>);
        static_assert(NoExceptDestructible<int>);
        static_assert(NoExceptDestructible<A>);
#if 0
        static_assert(NoExceptDestructible<B>);
#endif
    }

    //======================================================================

    template<typename Func>
    concept NoexceptFunc = requires (Func f) {
        // requires Func not to throw any exception
        { f() } noexcept;

        // requires Func type to be callable:
        requires std::invocable<Func>;
    };

    template<NoexceptFunc Func>
    auto invoke_noexcept_funcs_only(Func func) -> decltype(auto) {
        return func();
    }

    template<NoexceptFunc Func>
    void sink([[maybe_unused]] Func func) {

    }

    void Tests() {
        auto f1_noexcept = []() noexcept -> std::string { return { "string_noexcept" }; };
        auto f2_throwing = []() -> std::string { return { "string_noexcept" }; };

        auto a2 = invoke_noexcept_funcs_only(f1_noexcept);
#if 0
        auto a3 = invoke_noexcept_funcs_only(f2_throwing);
#endif

        sink(f1_noexcept);

    }

    //-------------------------------------------------------------------------

    template <typename T>
    concept assignment_cant_throw = requires (T a, T b) {
        { a = b } noexcept;
    };

    struct X {
        X& operator = (const X& lhs) noexcept { return *this; }
    };
    struct Y {
        Y& operator = (const Y& lhs) { return *this; }
    };

    template <assignment_cant_throw T>
    struct Test {
    };


    void ChecK_Assignment_Cant_Throw()
    {
        [[maybe_unused]]
        Test<X> a; // OK
#if 0
        Test<Y> b; // Fails
#endif
    }
}

namespace Concepts::Noexcept
{
    struct UnsafeToMoveType
    {
        UnsafeToMoveType() = default;
        UnsafeToMoveType(UnsafeToMoveType&&) {}
        UnsafeToMoveType& operator=(UnsafeToMoveType&&) { return *this; }
    };

    struct SafeToMoveType
    {
        SafeToMoveType() = default;
        SafeToMoveType(SafeToMoveType&&) noexcept {}
        SafeToMoveType& operator=(SafeToMoveType&&) noexcept { return *this; }
    };

    template <typename T>
    void unsafe_swap(T& left, T& right)
    {
        auto tmp = std::move(left);
        left = std::move(right); // What happens if this move throws? left was moved from, and moving it back might throw again
        right = std::move(tmp);
    }

    template<typename T>
    concept NoexceptMove = requires (T& a, T& b) {
        { a = std::move(b) } noexcept;
    };

    template <NoexceptMove T>
    void safe_swap(T& left, T& right)
    {
        auto tmp = std::move(left);
        left = std::move(right);
        right = std::move(tmp);
    }

    void Check_If_Type_CanBe_Noexcept_Swapped()
    {
        SafeToMoveType a, b;
        safe_swap(a, b); // OK

        UnsafeToMoveType x, y;

        /** Wouldn't compile **/
        // safe_swap(x, y);      // ====> UnsafeType doesn't satisfy the noexcept requirement
        unsafe_swap(x, y); // UnsafeType doesn't satisfy the noexcept requirement
    }
}



namespace Concepts::FoldExpression
{

    template<typename ... Types>
    concept TestConcept = requires(Types ... params) {
        (... + params);
        // requires std::sam
        // requires (sizeof ... (params)) > 1;
        // { (... + params) } noexcept -> std::same_as<first_arg_t<Types ...>>;
    };


    template<TestConcept ...Args>
    void PrintConstrained(Args&&... args) {
        std::cout << " Count = " << sizeof ... (args) << std::endl;
        (std::cout << ... << std::forward<Args>(args)) << std::endl;
    }


    void Test() {
        PrintConstrained(1, 2.0f);
    }

    //--------------------------------------------------------------------------

    struct A {
        A(int, int, int) {}
    };

    struct B {
        B(int, std::string) {}
    };

    template <typename Concrete, typename... Ts>
    requires std::is_constructible_v<Concrete, Ts...>
    std::unique_ptr<Concrete> constructArgsTwo(Ts&&... params) {
        return std::make_unique<Concrete>(std::forward<Ts>(params)...);
    }

    void Test_Construct_With_Arguments()
    {
        auto aObj = constructArgsTwo<A>(1, 2 ,3);
        auto bObj = constructArgsTwo<B>(1, "");
    }


    //--------------------------------------------------------------------------

    template<typename T, typename ... Ts>
    constexpr bool are_same_v = std::conjunction_v<std::is_same<T, Ts> ... >;

    template <typename... Args>
    requires are_same_v<Args...>
    auto Add(Args&& ... params) {
        return (... + params);
    }

    void Test_All_Params_are_SameType() {
        auto x = Add(1,2,3);

        // auto b = Add(1, 2, 3.4);

    }

    //-----------------------------------------------------------------

    template<typename ... Types>
    concept SumHasIntegerType = requires(Types ... params) {
        // { (... + params) } noexcept -> std::same_as<int>;
        (... + params); // Can be sum-ed
        requires sizeof...(params) > 1 ;
    };

    template <SumHasIntegerType... Args>
    auto sumToInt(Args&& ... params) {
        return (... + params);
    }

    /*
    void Sum_Has_Integer_Type() {
        auto x = sumToInt(1,3);
    }*/

    //--------------------------------------------------------------------------

    template<typename... Types>
    void print_sizes(Types&&... args) {
        std::cout << sizeof...(Types) << " | " << sizeof...(args) << "\n\n";
    }

    template<typename T, typename... Types>
    void print_sizes_remaining(T firstArg, Types... args) {
        std::cout << sizeof...(Types) << " | " << sizeof...(args) << "\n\n"; // print number of remaining args
    }

    template<typename T>
    concept ComparableBase = requires(T const& a, T const& b) {
        { a < b } -> std::same_as<bool>;
        { a > b } -> std::same_as<bool>;
        { a == b };
        { a == b } -> std::convertible_to<bool>;
        { a == b } noexcept -> std::convertible_to<bool>;
    };

    template<typename ... Types>
    concept Comparable = requires(Types ... params)
    {
        true;
        // requires sizeof ... (Types) >= 2;

        // { (... > params) } -> std::same_as<bool>;

        // { (... + params) } noexcept -> std::same_as<first_arg_t<Types ...>>;
    };

    template <Comparable... Args>
    auto tryToCompare(Args&& ... params) {
        return true;
    }

    void Elements_Shall_be_Comparable()
    {
        tryToCompare(1, 2);


        /*
        print_sizes(1,2 ,3);
        print_sizes_remaining(1,2,3);

        print_sizes("qwerty", 1.4, 1u, false);
        print_sizes_remaining("qwerty", 1.4, 1u, false);
        */
    }
}

namespace Concepts::Is_Constructible
{
    class Investment {
    public:
        virtual ~Investment() { }
        virtual void calcRisk() = 0;
    };

    class Stock : public Investment {
    public:
        explicit Stock(const std::string& s) { std::cout << "Stock(" << s << ")\n"; }
        void calcRisk() override { }
    };

    class Bond : public Investment {
    public:
        explicit Bond(const std::string& s1, const std::string& s2, int v) {
            std::cout << "Bond(" << s1 << ", " << s2  << ", " << v << ")\n";
        }
        void calcRisk() override { }
    };

    class RealEstate : public Investment {
    public:
        explicit RealEstate(const std::string& s1, double a, int b) {
            std::cout << "RealEstate(" << s1 << ", " << a << ", " << b << ")\n";
        }
        void calcRisk() override { }
    };


    template <typename Concrete, typename... Ts>
    std::unique_ptr<Concrete> constructArgsOne(Ts&&... params)
    {
        std::cout << __func__ << ": ";
        // fold expression:
        ((std::cout << params << ", "), ...);
        std::cout << "\n";

        if constexpr (std::is_constructible_v<Concrete, Ts...>)
            return std::make_unique<Concrete>(std::forward<Ts>(params)...);
        else
            return nullptr;
    }



    template <typename Concrete, typename... Ts>
    requires std::is_constructible_v<Concrete, Ts...>
    std::unique_ptr<Concrete> constructArgsTwo(Ts&&... params) {
        return std::make_unique<Concrete>(std::forward<Ts>(params)...);
    }

    template <typename Concrete, typename... Ts>
    std::unique_ptr<Concrete> constructArgsTwo(...) {
        return nullptr;
    }

    //========================================================================================

    // No Concepts src
    void ConstructTest1() {

        {
            std::cout << "-------------------------------------- Stock test1 -------------------------------\n";
            auto obj = constructArgsOne<Stock>("Val");
            if (nullptr == obj)
                std::cout << "OBJ == NULL\n";
        }

        {
            std::cout << "-------------------------------------- Bond test1 -------------------------------\n";
            auto obj = constructArgsOne<Bond>("Val");
            if (nullptr == obj)
                std::cout << "OBJ == NULL\n";
        }


        {
            std::cout << "-------------------------------------- Bond test2 -------------------------------\n";
            auto obj = constructArgsOne<Bond>("Val", "Val2", 3);
            if (nullptr == obj)
                std::cout << "OBJ == NULL\n";
        }

        {
            std::cout << "-------------------------------------- RealEstate test1 -------------------------------\n";
            auto obj = constructArgsOne<RealEstate>("Val", "Val2", 3);
            if (nullptr == obj)
                std::cout << "OBJ == NULL\n";
        }

        {
            std::cout << "-------------------------------------- RealEstate test2 -------------------------------\n";
            auto obj = constructArgsOne<RealEstate>("Val", 3.3f, 23);
            if (nullptr == obj)
                std::cout << "OBJ == NULL\n";
        }
    }

    void ConstructTest2() {

        {
            std::cout << "-------------------------------------- Stock test1 -------------------------------\n";
            auto obj = constructArgsTwo<Stock>("Val");
            if (nullptr == obj)
                std::cout << "OBJ == NULL\n";
        }

        {
            std::cout << "-------------------------------------- Bond test1 -------------------------------\n";
            auto obj = constructArgsTwo<Bond>("Val");
            if (nullptr == obj)
                std::cout << "OBJ == NULL\n";
        }


        {
            std::cout << "-------------------------------------- Bond test2 -------------------------------\n";
            auto obj = constructArgsTwo<Bond>("Val", "Val2", 3);
            if (nullptr == obj)
                std::cout << "OBJ == NULL\n";
        }

        {
            std::cout << "-------------------------------------- RealEstate test1 -------------------------------\n";
            auto obj = constructArgsTwo<RealEstate>("Val", "Val2", 3);
            if (nullptr == obj)
                std::cout << "OBJ == NULL\n";
        }

        {
            std::cout << "-------------------------------------- RealEstate test2 -------------------------------\n";
            auto obj = constructArgsTwo<RealEstate>("Val", 3.3f, 23);
            if (nullptr == obj)
                std::cout << "OBJ == NULL\n";
        }
    }

}


namespace Concepts::Containers {

    template<typename T>
    concept Container = requires(T t)
    {
        typename T::value_type;
        typename T::size_type;
        typename T::allocator_type;
        typename T::iterator;
        typename T::const_iterator;

        t.size();
        t.begin();
        t.end();
        t.cbegin();
        t.cend();
    };

    struct A {};

    void Test() {
        A a;
        std::vector<int> b;

        static_assert(not Container<decltype(a)>);
        static_assert(Container<decltype(b)>);
    }
}


namespace Concepts::Iterators {

    template<typename It>
    concept RandomAccessIterator = requires(It i,
                                            typename std::incrementable_traits<It>::difference_type n)
    {
        { i - i } -> std::same_as<decltype(n)>;
        { i + n } -> std::same_as<It>;
        { i - n } -> std::same_as<It>;
    };

    template<typename It>
    concept LessThanComparable = requires(It a, It b)
    {
        typename It::value_type;
        { *a < *b } -> std::same_as<bool>;
    };




    struct MyStruct {
        int value;
        MyStruct(int v) : value(v) {}
        auto operator<=>(const MyStruct&) const = default;
    };

    template<typename T>
    requires RandomAccessIterator<T>&& LessThanComparable<T>
    void mysort(T begin, T end) {
        std::sort(begin, end);
    }


    void SortCollections_RandomAccessIterator()
    {
        std::vector<MyStruct> vec{ 1, 3, 5 , 2, 2 };

        mysort(vec.begin(), vec.end());
    }
}


namespace Concepts::ClassMethods
{

    template<typename T, bool enable = true>
    class Sample
    {
    public:
        int DisableThisMethodOnRequest() requires(enable) {
            return 42;
        }
    };


    void DisableClassMethods()
    {

        Sample<int, false> s;
        // auto x = s.DisableThisMethodOnRequest();

        Sample<int, true> s2;
        s2.DisableThisMethodOnRequest();

    }
}

namespace Concepts::Static_Asserts
{
    struct Fir {
        int count() const {
            return 2020;
        }
    };

    struct Sec {
        int size() const {
            return 2021;
        }
    };

    void TestClassMethods()
    {
        Fir fir;
        static_assert(requires(Fir fir){ { fir.count() } -> std::convertible_to<int>; });     // OK

        Sec sec;
        // static_assert(requires(Sec sec){ { sec.count() } -> std::convertible_to<int>; });  // ERROR

        int third;
        // static_assert(requires(int third){ { third.count() } -> std::convertible_to<int>; }); // ERROR
    }

    //--------------------------------------------------------------

    template<typename Ty>
    class Vector : public std::vector<Ty> {

    public:
        // private:
        size_t size() const {
            return 0;
        }
    };

    template<typename Container>
    concept Sizable = requires(const Container & c) {
        {c.size()} -> std::same_as<size_t>;
    };

    void StaticAssert_Conects() {
        using T = std::vector<int>;
        using T1 = Vector<int>;

        static_assert(Sizable<T>);
        static_assert(Sizable<T1>); // ERROR: if Vector::size() ---> private

        auto s = T1().size();
        std::cout << s << std::endl;
        std::cout << std::boolalpha << (typeid(size_t) == typeid(s)) << std::endl;
    }
}

namespace Concepts::CheckFunctionOverloadExists
{
    void Compute(int in, int& out) {
        std::cout << "Compute(int in, int& out) called\n";
    }

    void Compute(std::string in, std::string& out) {
        std::cout << "Compute(std::string in, std::string& out) called\n";
    }



    template<typename T>
    concept is_function_available = requires(T v, T& out) {
        Compute(v, out);
    };

    template <typename T>
    void TryComputeOne(T val)
    {
        if constexpr (is_function_available<T>)
        {
            T out { };
            Compute(val, out);
        }
        else {
            std:: cout << "fallback...\n";
        }
    }

    template <typename T>
    void TryComputeTwo(T val)
    {
        if constexpr (requires(T v, T& out) { Compute(v, out);})
        {
            T out { };
            Compute(val, out);
        }
        else {
            std:: cout << "fallback...\n";
        }
    }

    void TryCallFunction()
    {
        TryComputeOne(1);
        TryComputeTwo(1);

        std::cout << std::endl;

        TryComputeOne(1.0f);
        TryComputeTwo(1.0f);

        std::cout << std::endl;

        TryComputeOne(std::string {});
        TryComputeTwo(std::string {});
    }
}

namespace Concepts::CheckCallHaveFunction_IfConstexpr
{
    template<typename T>
    concept SupportsValidation = requires(T t)
    {
        t.validate();
    };

    template<typename T>
    void Send(const T& data)
    {
        if constexpr(SupportsValidation<T>) {
            data.validate();
        }
        else {
            std::cout << "Can not be validated\n";
        }
    }

    struct EmptyObject { };

    struct Validator
    {
        void validate() const {
            std::cout << "ComplexType::validate()" << std::endl;
        }
    };

    void If_Constexpr_Concepts()
    {
        EmptyObject obj1;
        Validator obj2;

        Send(obj1);
        Send(obj2);

        static_assert(SupportsValidation<Validator>);
        static_assert(not SupportsValidation<EmptyObject>);
    }
}

namespace Concepts::Regular
{
    // regular
    // std:::semiregular

    struct Long
    {
        long value {0};

        Long() = default;
        ~Long() = default;

        Long(const Long& l) : value { l.value } {}
        Long(Long&& l) noexcept : value { std::exchange(l.value, 0) } {}

        Long& operator=(const Long& right) {
            if (&right != this)
                value = right.value;
            return *this;
        }

        Long& operator=(Long&& right) noexcept {
            if (this != &right)
                this->value = std::exchange(right.value, 0);
            return *this;
        }
    };


    struct LongComparable
    {
        long value {0};

        LongComparable() = default;
        ~LongComparable() = default;

        LongComparable(const LongComparable& l) : value { l.value } {}
        LongComparable(LongComparable&& l) noexcept : value { std::exchange(l.value, 0) } {}

        LongComparable& operator=(const LongComparable& right) {
            if (&right != this)
                value = right.value;
            return *this;
        }

        LongComparable& operator=(LongComparable&& right) noexcept {
            if (this != &right)
                this->value = std::exchange(right.value, 0);
            return *this;
        }

        friend bool operator==(const LongComparable& left, const LongComparable& right) noexcept;
        friend bool operator!=(const LongComparable& left, const LongComparable& right) noexcept;
    };

    bool operator==(const LongComparable& left, const LongComparable& right) noexcept {
        return left.value == right.value;
    }

    bool operator!=(const LongComparable& left, const LongComparable& right) noexcept {
        return not (left == right);
    }

    void IsSemirRegular()
    {
        static_assert(std::semiregular<Long> == true);
        // static_assert(std::regular<Long> == true);
    }

    void IsRegular()
    {
        static_assert(std::semiregular<LongComparable> == true);
        static_assert(std::regular<LongComparable> == true);
    }
}


namespace Concepts::ConceptsAsInterface
{
    template<typename Type>
    concept HasInfoMethod = requires (Type val) {
        { val.info() } noexcept;
    };

    struct ClassA {
        void info() noexcept {
            std::cout << "ClassA() info" << std::endl;
        }
    };

    struct ClassB {
        int info() noexcept {
            std::cout << "ClassB() info" << std::endl;
            return 0;
        }
    };

    void printInfo(HasInfoMethod auto& obj)
    {
        obj.info();
    }

    void passClassObjAsInterface()
    {
        ClassA aObj;
        ClassB bObj;

        printInfo(aObj);
        printInfo(bObj);
    }
}

namespace Concepts::CheckAllTypesAreSame
{
    template<typename T, typename ... Ts>
    constexpr bool are_same_v = std::conjunction_v<std::is_same<T, Ts> ... >;

    template <typename... Args>
    requires are_same_v<Args...>
    auto _add(Args&& ... params) {
        return (... + params);
    }

    template <typename... T>
    bool validate_integrals(T ...)
    requires (std::integral<T> && ... )
    // requires std::conjunction_v<std::is_integral<T> ... >
    {
        return true;
    }

    void Check_with_Concepts()
    {
        auto x = _add(1,2,3);
        // auto b = Add(1, 2, 3.4);
    }

    void Check_with_StaticAssert()
    {
        static_assert(are_same_v<int, int, int>);
        static_assert(not are_same_v<int, int&, int>);
    }

    void Check_ALL_Integral()
    {
        std::cout << std::boolalpha << validate_integrals(1) << std::endl;
        std::cout << std::boolalpha << validate_integrals(1, 2) << std::endl;
        // std::cout << std::boolalpha << validate_integrals(1, 2, 2.2) << std::endl;
    }
}


namespace Concepts::CheckTypes
{
    template<typename T, typename ... Types>
    concept SameAsAnyOf = (... or std::same_as<T, Types>);

    template<typename ... Ts>
    struct Keeper
    {
        constexpr explicit Keeper(SameAsAnyOf<Ts ...  > auto obj) {
        }

        constexpr void setValue(SameAsAnyOf<Ts ...  > auto obj) {
        }
    };

    struct A {};
    struct B {};
    struct C {};

    void CheckThatTypeSameAs()
    {
        Keeper<int, double> keeper(1);
        Keeper<int, std::string> keeper1(std::string {"text"});

        Keeper<A, B> keeper3 { A{}};

        // Keeper<A, B> keeper4 { C{}};  // Shall not compile
    }
}

namespace Concepts::STD
{
    struct A {};
    struct B: A {};
    struct C: A {};
    struct D: B {};

}

namespace Concepts::STD::Common_With
{
    struct A {};
    struct B: A {};
    struct C: A {};
    struct D: B {};

    void HaveCommonClass()
    {
        static_assert(std::common_with<B, D>);     // True
        static_assert(not std::common_with<B, C>); // False
        static_assert(std::common_with<A, B>);     // True
    }
}


namespace Concepts::STD::Same_AS
{
    using AA = A;

    void CheckTypeAreSame()
    {
        auto a = 10, b = 20;

        static_assert(std::same_as<A, AA>);                     // True
        static_assert(std::same_as<decltype(a), decltype(b)>);  // True
    }
}


namespace Concepts::STD::Derived_From
{
    struct A { };
    struct B : A { };
    struct C {};

    template <typename Ty>
    concept DerivedFromA = std::derived_from<Ty, A>;

    template<DerivedFromA T>
    void func(T v) {
    }

    void SimpleExample()
    {
        static_assert(std::derived_from<A, A>);
        static_assert(std::derived_from<B, A>);
        static_assert(not std::derived_from<C, A>); //
    }

    void ComplexTest()
    {
        A objA;
        B objB;
        [[maybe_unused]] C objC;


        func(objA);
        func(objB);
        // func(objC);   // Compile error
    }
}


namespace Concepts::CRPT_Replace
{
    struct AnimalTag {};

    template<typename T>
    concept Animal = requires(T animal) { animal.make_sound();} &&
        std::derived_from<T, AnimalTag>;

    void print(const Animal auto& animal) {
        animal.make_sound();
    }

    struct Sheep: AnimalTag {
        void make_sound() const { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    };

    struct Cow: AnimalTag {
        void make_sound() const { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    };

    struct Dog: AnimalTag {
        void make_sound() const { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    };

    void Concepts_Instead_CRTP()
    {
        constexpr Cow cow;
        print(cow);

        constexpr Sheep sheep;
        print(sheep);

        constexpr Dog dog;
        print(dog);
    }
}

namespace Concepts::Constraints_On_Member_Function
{
    using namespace std::string_view_literals;

    template<typename T>
    struct TypeWrapper
    {
        T value {};

        constexpr TypeWrapper() = default;
        explicit constexpr TypeWrapper(T v): value { v } {
        }

        void print() const noexcept {
            std::cout << value << std::endl;
        }

        [[nodiscard]]
        constexpr bool isZero() const noexcept requires std::integral<T> || std::floating_point<T>
        {
            return 0 == value;
        }

        [[nodiscard]]
        constexpr bool isEmpty() const noexcept requires std::same_as<T, std::string_view>
        {
            return value.empty();
        }
    };

    void Check_If_Function_Available()
    {
        {
            constexpr TypeWrapper<int> wrapper {10};
            static_assert(wrapper.isZero() == false);
            // static_assert(wrapper.isEmpty() == false);  <--- Can not compile
        }

        {
            constexpr TypeWrapper<std::string_view> wrapper { "Hellow World"sv };
            // static_assert(wrapper.isZero() == false);  <--- Can not compile
            static_assert(wrapper.isEmpty() == false);
        }
    }
}

namespace Concepts::Requires_Clause_vs_Expression
{
    template<typename T>
    bool test_type() {
        return requires (const T obj) { ++obj; };
    }

    constexpr bool test_can_increment() {
        return requires (int i) { ++i; };
    }

    constexpr bool test_can_NOT_increment() {
        // will not compile with CONST
        // return requires (const int i) { ++i; };
        return true;
    }

    constexpr bool test_can_increment_func() {
        return test_type<int>();
    }

    void Requires_Expression()
    {
        test_can_increment();
        test_can_NOT_increment();

        test_can_increment_func();
    }


    template<typename T>
    void func_ok(T v) requires true {};

    template<typename T>
    void func_not_ok(T v) requires false {};

    template<bool Result>
    constexpr bool get_bool() { return Result ;};

    template<typename T>
    void based_on_function_call_ok(T v) requires (get_bool<true>()) {};

    template<typename T>
    void based_on_function_call_not_ok(T v) requires (get_bool<false>()) {};


    void Requires_Clause()
    {
        func_ok(1);
        // func_not_ok(1);  /// ERROR

        based_on_function_call_ok(1);
        // based_on_function_call_not_ok(1); /// ERROR
    }
}




// https://www.youtube.com/watch?v=jzwqTi7n-rg | Back to Basics: Concepts in C++ - Nicolai Josuttis - CppCon 2024

void Concepts::TestAll()
{
    // MovableTest();
    // Lambda_Concept_Tests();
    // STDConcepts::Copyable();


    Requires_Clause_vs_Expression::Requires_Expression();
    Requires_Clause_vs_Expression::Requires_Clause();

    // Custom_Concepts::SimpleTest();
    // Custom_Concepts::Addable_Test();
    // Custom_Concepts::IsPointer();
    // Custom_Concepts::Hashable_Test();
    // Custom_Concepts::Incrementable_Test();
    // Custom_Concepts::Has_Size_and_Swappable();
    // Custom_Concepts::Comparable_Test();
    // Custom_Concepts::Multi_Arguments_Tests();
    // Custom_Concepts::Custom_Concept_IsBaseOf();
    // Custom_Concepts::CheckCollection_HasPushBack_Method();
    // Custom_Concepts::CheckCollection_HasPushBack_Method_2(); // not working
    // Custom_Concepts::Check_BaseType_ContainsMethod_ReturnValue();
    // Custom_Concepts::It_Not_Void_Test();
    // Custom_Concepts::Lambda_Concept_Tests();
    // Custom_Concepts::Signed_Tests();
    // Custom_Concepts::Unsigned_Tests();


    // Requires::FunctionDefinition_Examples();
    // Requires::SimpleTest();
    // Requires::Test1();
    // Requires::Test2();
    // Requires::Constrain_Template_Param_Value();
    // Requires::Assert_With_Requires();
    // Requires::Test_Destructor_NoExcept();
    // Requires::Complex_Concepts_Tests();
    // Requires::Test_is_Character();
    // Requires::CheckOperatorArgument_IsArithmetic();

    // Requires_With_Constexpr::Vector_vs_Array();
    // Requires_With_Constexpr::Constexpr_Check_Method();
    // Requires_With_Constexpr::PushBackToSomeCollection_CheckPushback_vs_Insert();

    // RequiresSequence::Test1();

    // Callables::Test_Invocable();
    // Callables::Test_Invocable_Regular();
    // Callables::Test_Predicate();
    // Callables::Test_Predicate_WithParams();
    // Callables::Test_Predicate_WithParams_Variadic();
    // Callables::Test_Predicate_PrintVector();

    // Noexcept::Tests();
    // Noexcept::ChecK_Assignment_Cant_Throw();
    // Noexcept::Check_If_Type_CanBe_Noexcept_Swapped();


    // STD::Common_With::HaveCommonClass();
    // STD::Same_AS::CheckTypeAreSame();
    // STD::Derived_From::SimpleExample();
    // STD::Derived_From::ComplexTest();


    // Concepts_With_Auto::Test();
    // Concepts_With_Auto::Print_Tests();

    // Concepts_With_Lambdas::Params_Concepts();


    // CRPT_Replace::Concepts_Instead_CRTP();
    // ConceptsAsInterface::passClassObjAsInterface();
    // NestedConcepts::CheckMethodReturnType();
    // SFINAE::ChooseOverloadedFunc_WithRequire();

    // Function_Constrains::Test_Function_Return_Int();
    Function_Constrains::CheckTypeIsFunctor();
    // Constraints_On_Member_Function::Check_If_Function_Available();

    // Conversations::String_Test();
    // Conversations::Convertible_To_Tests();


    // Is_Constructible::ConstructTest1();      /** Fabric test **/
    // Is_Constructible::ConstructTest2();      /** Fabric test + Concepts **/

    // ValidatTypeContains_Members_or_Types::ContainTypeDef();
    // ValidatTypeContains_Members_or_Types::Valid_Template_Substitution();

    // CheckFunctionOverloadExists::TryCallFunction();
    // CheckCallHaveFunction_IfConstexpr::If_Constexpr_Concepts();

    // Regular::IsSemirRegular();
    // Regular::IsRegular();

    // Containers::Test();

    // FoldExpression::Test();
    // FoldExpression::Test_Construct_With_Arguments();
    // FoldExpression::Test_All_Params_are_SameType();
    // FoldExpression::Elements_Shall_be_Comparable();

    /*
    CheckAllTypesAreSame::Check_with_Concepts();
    CheckAllTypesAreSame::Check_with_StaticAssert();
    CheckAllTypesAreSame::Check_ALL_Integral();
    CheckTypes::CheckThatTypeSameAs();   // <--- Allow construction using one of the base class only
    */

    // IntegerConcepts::IntegerSum();

    // Iterators::SortCollections_RandomAccessIterator();

    // ClassMethods::DisableClassMethods();

    // Tests::StaticAssert_Conects();
    // Tests::LAMBDA_CONCEPT();
    // Tests::Printable_Test();

    // Static_Asserts::TestClassMethods();
    // Static_Asserts::StaticAssert_Conects();
};
