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
#include <utility>
#include <vector>
#include <list>
#include <array>
#include <algorithm>
#include <cstdint>
#include <functional>

#include "Templates.hpp"

namespace Templates
{
    struct  A
    {
        virtual ~A() = default;

        virtual void info() const noexcept {
            std::cout << __FUNCTION__ << std::endl;
        }
    };

    struct B : A
    {
        void info() const noexcept override {
            std::cout << __FUNCTION__ << std::endl;
        }
    };

    class C {
    };


    /////////////////////////////////

    void Rank()
    {
        std::cout << std::rank_v<int[1][2][3]> << std::endl;
        std::cout << std::rank_v<int[][2][3][4]> << std::endl;
        std::cout << std::rank_v<int> << std::endl;
    }

    void Is_Base_Of__Test() {
        std::cout << std::boolalpha;
        std::cout << "a2b: " << std::is_base_of_v<A, B> << '\n';
        std::cout << "b2a: " << std::is_base_of_v<B, A> << '\n';
        std::cout << "c2b: " << std::is_base_of_v<C, B> << '\n';
        std::cout << "Same types: " << std::is_base_of_v<C, C> << '\n';
    }

    void Is_Same() {
        std::cout << std::boolalpha;

        std::cout << "is_same<int, int32_t> = " << std::is_same_v<int, int32_t> << std::endl;
        std::cout << "is_same<int, int64_t> = " << std::is_same_v<int, int64_t> << std::endl;
        std::cout << "is_same<float, int32_t> = " << std::is_same_v<float, int32_t> << std::endl;

        std::cout << "----------------" << std::endl;

        std::cout << "is_same<int, int> = " << std::is_same_v<int, int> << std::endl;
        std::cout << "is_same<int, unsigned int> = " << std::is_same_v<int, unsigned int> << std::endl;
        std::cout << "is_same<int, signed int> = " << std::is_same_v<int, signed int> << std::endl;

        std::cout << "----------------" << std::endl;

        std::cout << "is_same<char, char> = " << std::is_same_v<char, char> << std::endl;
        std::cout << "is_same<char, unsigned char> = " << std::is_same_v<char, unsigned char> << std::endl;
        std::cout << "is_same<char, signed char> = " << std::is_same_v<char, signed char> << std::endl;

        std::cout << "----------------" << std::endl;

        std::cout << "is_same<long, long> = " << std::is_same<long, long>() << std::endl;
        std::cout << "is_same<long, int> = " << std::is_same<char, int>() << std::endl;

        std::cout << "----------------" << std::endl;

        std::cout << "is_same<A, A> = " << std::is_same<A, A>() << std::endl;
        std::cout << "is_same<A, B> = " << std::is_same<A, B>() << std::endl;
    }

    template<typename T = int>
    class Foo
    {
        T val;

    public:
        explicit Foo(T v) : val(std::move(v)) {
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



namespace Templates::Methods
{
    template<typename T>
    T* BuildArray(const size_t size)
    {
        static_assert(std::is_integral_v<T>, "This function is only designed for integral types.");
        T* d = new T[size];
        for (int32_t i = 0; i < static_cast<T>(size); ++i)
            d[i] = i;
        return d;
    }

    template<typename T = int>
    T* BuildArrayEx(size_t size)
    {
        static_assert(std::is_integral_v<T>, "This function is only designed for integral types.");
        T* d = new T[size];
        for (int32_t i = 0; i < static_cast<T>(size); ++i) {
            d[i] = i;
        }
        return d;
    }

    void SimpleTemplate() {
        const int* data = BuildArray<int>(10);
        for (int i = 0; i< 10; i++)
            std::cout << data[i] << std::endl;
    }

    void SimpleTemplate_DefaultType() {
        const int* data = BuildArrayEx(10);
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
        constexpr int x[] = { 1, 2, 3 };
        const int y[] = { 1, 2, 3, 4, 5 };
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

namespace Templates::VariadicTemplates
{
    template<typename... Args>
    auto adder(Args... args) {
        return (... + args);
        // or (args + ...)
    }

    void Sum_Multiple_Variables()
    {
        long sum = adder(1, 2, 3, 8, 7);
        std::cout << sum << std::endl;

        std::string s1 = "x", s2 = "aa", s3 = "bb", s4 = "yy";
        std::string ssum = adder(s1, s2, s3, s4);

        std::cout << ssum << std::endl;
    }
}

namespace Templates::VariadicTemplates
{
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
}

namespace Templates::VariadicTemplates
{
    template<typename T, typename... Types>
    void print_sizes(T firstArg, Types... args) {
        std::cout << sizeof...(Types) << '\n'; // print number of remaining types
        std::cout << sizeof...(args) << '\n'; // print number of remaining args
    }

    void Variadic_Sizeof() {
        print_sizes("qwerty", 1.4, 1u, false);
    }
}

namespace Templates::VariadicTemplates
{
    template<typename T1, typename... TN>
    constexpr bool isHomogeneous(T1, TN...) {
        return (std::is_same<T1, TN>::value && ...); // since C++17
    }

    void Check_IfTypes_AreSame() {
        std::cout << std::boolalpha << isHomogeneous<int, double, int>(1, 1, 1) << std::endl;
        std::cout << std::boolalpha << isHomogeneous<int, int, int>(1, 1, 1) << std::endl;
    }
}

namespace Templates::VariadicTemplates
{
    template<typename T>
    T adder(T v) {
        return v;
    }

    template<typename T, typename... Args>
    T adder(T first, Args... args) {
        return first + adder(args...);
    }

    void Sum_Values()
    {
        const int result = adder(1, 2, 3, 4, 5);
        std::cout << result << std::endl;
    }
}

namespace Templates::VariadicTemplates
{
    template<typename T>
    bool pair_comparer(T a, T b)
    {
        // In real-world code, we wouldn't compare floating point values like
        // this. It would make sense to specialize this function for floating
        // point types to use approximate comparison.
        return a == b;
    }

    template<typename T, typename... Args>
    bool pair_comparer(T a, T b, Args... args) {
        return a == b && pair_comparer(args...);
    }

    void Compare_Pairs()
    {
        auto result = pair_comparer(1.5, 1.5, 2, 2, 6, 6);
        std::cout << std::boolalpha << result << std::endl;

        result = pair_comparer(1, 1, 1, 2);
        std::cout << std::boolalpha << result << std::endl;
    }
}

namespace Templates::VariadicTemplates
{
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
}

namespace Templates::VariadicTemplates
{
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
}

namespace Templates::VariadicTemplates
{
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
}

namespace Templates::VariadicTemplates
{
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
        Base() = default;
        explicit Base(T v) : value{ v } {}
    };

    template<typename... Types>
    class Multi : Base<Types>...
    {
    public:
        // derive all constructors:
        using Base<Types>::Base...;
    };

    void ExtendedUsingDeclarations()
    {
        using MultiISB = Multi<int, std::string, bool>;
    }
}

namespace Templates::Decltype
{
    using namespace std::string_literals;

    template <class T, class U>
    auto mix(T Lhs, U Rhs) -> decltype(Lhs + Rhs) {
        return Lhs + Rhs;
    }

    int& foo(int& i) { return i; }
    float& foo(float& f) { return f; }

    template <class T>
    auto transparent_forwarder(T& t) -> decltype(foo(t)) {
        return foo(t);
    }

    void Test()
    {
        std::cout << typeid(mix(""s, "")).name() << std::endl;
        std::cout << typeid(mix(1, 3)).name() << std::endl;
        std::cout << typeid(mix(1, 3.3)).name() << std::endl;
        std::cout << typeid(mix(1, 3.3f)).name() << std::endl;
    }

    void Test2()
    {
        int i = 1;
        std::cout << typeid(transparent_forwarder(foo(i))).name() << std::endl;

        float f = 1;
        std::cout << typeid(transparent_forwarder(foo(f))).name() << std::endl;
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

    void ConstClass()
    {
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
            const std::string value = "SomeText";
            const std::string result = toString(value);
            std::cout << value << " -> " << result << std::endl;
        }
        {
            constexpr int value = 12345;
            const std::string result = toString(value);
            std::cout << value << " -> " << result << std::endl;
        }
        {
            constexpr char value[] = "Some pain text";
            const std::string result = toString(value);
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

        [[maybe_unused]]
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

        explicit Mixin(int v) : Types(v)... {
            // base class initializer pack expansion
        }


        template<typename ... Args>
        Mixin(Args&& ... params) : Types(std::forward<Args>(params)...)... {
            // base class initializer pack expansion
        }
    };

    struct Type1 {
        Type1() { std::cout << "Type1::Type1()" << std::endl; }
        explicit Type1(const int value) { std::cout << "Type1::Type1(" << value << ")" << std::endl; }
    };

    struct Type2 {
        Type2() { std::cout << "Type2::Type2()" << std::endl; }
        explicit Type2(const int value) { std::cout << "Type2::Type2(" << value << ")" << std::endl; }
    };

    struct Type3 {
        Type3() { std::cout << "Type3::Type3()" << std::endl; }
        explicit Type3(int value) { std::cout << "Type3::Type3(" << value << ")" << std::endl;}
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

        void operator()() const
        {
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

        void operator()() const
        {
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
        static constexpr bool value = (std::is_same_v<T1,TN> && ...);
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
    class Base
    {
        inline static size_t counter{ 0 };

    public:

        ~Base() {
            std::cout << "Base<" << typeid(T).name() << ">::~Base()" << std::endl;
        }

        [[nodiscard]] size_t getCounter() const noexcept {
            return counter;
        }

        void printInfo() const noexcept {
            std::cout << counter << std::endl;
        }

        void setCounter(const size_t new_value) noexcept {
            counter = new_value;
        }

        void incrementCounter() noexcept {
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
    class Object
    {
        T value;

        friend class Keeper<T>;
    };

    template<typename T>
    class Keeper
    {
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

    double add(const double a, const double b)
    {
        return a + b;
    }

    void PassingFunction_to_ClassTemplateArgument()
    {
        auto logged_add = make_logger(add, "Add");

        [[maybe_unused]]
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

    template<typename T1, typename T2>
    struct is_same
    {
        static constexpr bool value { false };
    };

    template<typename T>
    struct is_same<T, T>
    {
        static constexpr bool value {true };
    };

    template<typename T1, typename T2>
    static constexpr bool is_same_v = is_same<T1, T2>::value;


    void IsSame_Test()
    {
        static_assert(is_same_v<int, int>);
        static_assert(not is_same_v<char, int>);
        static_assert(is_same_v<int&, int&>);
    }
}

namespace Templates::OperatorOverload
{
    class Customer
    {
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

namespace Templates::Templated_Templates
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

namespace Templates::Templated_Templates
{
    template <typename T,
            template <typename, typename> typename Container>
    class MyClass
    {
        Container<T, std::allocator<T>> container;

    public:
        void push(const T& value)
        {
            container.push_back(value);
        }

        T pop()
        {
            T value = container.back();
            container.pop_back();
            return value;
        }
    };

    void Container_WithTemplated_Types()
    {
        MyClass<int, std::vector> with_vector;
        with_vector.push(1);
        with_vector.push(2);
        with_vector.push(3);

        MyClass<int, std::list> with_list;
        with_list.push(4);
        with_list.push(5);
        with_list.push(6);
    }
}



namespace Templates::TemplateClass_StaticMethodCall
{
    template<typename T>
    struct Other
    {
        template<typename U>
        static void staticMethod() {
            std::cout << "Other::staticMethod()" << std::endl;
        }
    };


    template<typename T>
    void Test()
    {
        Other<T>::template staticMethod<T>();
    }

    void StaticMethodTest()
    {
        Test<int>();
        Test<std::string>();
    }
}

void Templates::TestAll()
{
    // TypeListChecker::TestAll();
    // FoldExpressions::TestAll();
    // NTTP::TestAll();
    // SFINAE::TestAll();
    // TemplateSpecialization::TestAll();
    // PackIndexing::TestAll();
    // IndexSequence::TestAll();
    // Metaprogramming::TestAll();
    // Expression_Templates::TestAll();
    // perfect_forwarding::TestAll();

    // Is_Base_Of__Test();
    // Is_Same();
    // Rank();


    // Compile_Time_IF::ToStringTest();
    // Compile_Time_IF::IsNegative();
    // Compile_Time_IF::GetValueFromPointer2();

    // DefaultTest();
    // DefaultArguments::CreateArray();
    // DefaultArguments::DefaultTypes();
    // DefaultArguments::Metaprogramming();


    // Conditional::SimpleTest();
    // Conditional::Condition_Ref_vs_NonRef();

    // TemplateClass_StaticMethodCall::StaticMethodTest();

    // Methods::SimpleTemplate();
    // Methods::SimpleTemplate_DefaultType();
    // Methods::ZeroInitialization();
    // Methods::Compare_Raw_Arrays();
    // Methods::Compare_String_Literals();
    // Methods::Template_Params_Only_ForArrays();

    // ---------------------------------------- VARIADIC TEMPLATES ----------------------------------------

    // VariadicTemplates::Sum_Multiple_Variables();
    // VariadicTemplates::Print_Multiple_Variables_RecursiveHack();
    // VariadicTemplates::Recursive_Expansion_Two();

    // VariadicTemplates::Variadic_Sizeof();
    // VariadicTemplates::Check_IfTypes_AreSame();
    // VariadicTemplates::ExtendedUsingDeclarations();
    // VariadicTemplates::Compare_Pairs();
    // VariadicTemplates::Sum_Values();
    // VariadicTemplates::All_Equals();
    // VariadicTemplates::Sum_NonTemplate_Params();

    // ---------------------------------------- MIXINS & PACKS ----------------------------------------

    // PackExpansions::Create_Mixin_Class();
    // PackExpansions::Mixin_CallFunc();
    // PackExpansions::IsHomogeneous_Test(); /** Class to check All types are same **/

    // --------------------------------- TYPE_TRAITS -------------------------------------------------

    // TypeTraits::IsVoid_Test();
    // TypeTraits::IsPointer_Test();
    // TypeTraits::IsSame_Test();

    // ------------------------------- Templates <Typename T> ------------------------------------------

    // Templated_Templates::GetDataTest();
    // Templated_Templates::Container_WithTemplated_Types();

    // -------------------------------------------------------------------------------------------------

    // Applications_And_Examples::PassingFunction_to_ClassTemplateArgument();

    // StaticMembersInTemplates::Test();

    // OperatorOverload::FoldExpressionOverload();

    // Auto_Template_Params::test();

    // Decltype::Test();
    // Decltype::Test2();

    // CheckIsPointer::IsPointerTest();

    // StaticVariables::FunctionStaticVariales();
    // StaticVariables::ClassStaticVariales();

    // Requires::Test();


    // Applications_And_Examples::Fibonachi_Old_Test();

    // Friends::Access_Private_Field();


    // ConditionalExplicit::TestAll();

}