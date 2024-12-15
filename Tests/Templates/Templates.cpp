//============================================================================
// Name        : Templates.h
// Created on  : 02.02.2022
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Templates
//============================================================================

#include "Templates.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <functional>
#include <utility>
#include <cstdint>
#include <list>

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

namespace Templates::FoldExpressions
{

    template <typename R, typename ... Ts>
    constexpr auto matches(const R& range, Ts ... ts) -> decltype(auto) {
        return (std::count(std::begin(range), std::end(range), ts) + ...);
    }

    void MatchingTests()
    {
        static_assert(3 == matches("abcdefg", 'a', 'd', 'f'));
#if 0
        static_assert(3 == matches("abcdefg", 'a', 'd', 'F'));
#endif
    }


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

        [[maybe_unused]]
        auto result = logged_add(2, 3);
    }
}

namespace Templates::FoldExpressions
{
    // When there is only one parameter left
    template<typename T>
    double sum(const T &last) {
        std::cout << "sum(" << last << ")\n";
        return last;
    }

    // When there are 2 or more parameter
    template<typename T, typename ...Ts>
    double sum(const T &first, const Ts &... args) {
        return first + sum(args...);
    }

    void Recursive_Expansion() {
        sum(1, 2, 3, 4, 5);
    }


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

namespace Templates::FoldExpressions
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



namespace Templates::NTTP
{
    struct Config
    {
        int v{12};
    };

    template<Config config>
    struct Task
    {
        void submit() {
            std::cout << config.v << std::endl;
        }
    };

    template<auto Func>
    struct PersonalBudget {
        double compute(std::uint32_t amt) {
            return Func(amt);
        }
    };



    void testConfig() {
        Task<Config{}>().submit();
    }

    void testPersonalBudget ()
    {
        auto savings = [](int amt) -> decltype(auto) {
            return static_cast<double>(0.75*amt);
        };

        PersonalBudget<savings> savingsBudget{};

        auto saveResult = savingsBudget.compute(2300);
        std::cout << "Estimated Savings: " << saveResult << std::endl;
    }
}


namespace Templates
{
    /*
    template<typename T, typename ... Args>
    T (*Func)(Args ...) = [](Args ... args) {
        return (args + ... + 0);
    };
    */

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

    void Test2()
    {
        using CustomerOP = Overloader<CustomerHash,CustomerEq>;

        const Customer c1 {"one"}, c2 { "two"};

        CustomerOP{}(c1);
        CustomerOP{}(c1, c2);
    }
}


namespace Templates::Specialization
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

namespace Templates::Specialization
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

namespace Traits
{
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


    void test()
    {
        static_assert(is_same_v<int, int>);
        static_assert(not is_same_v<char, int>);
        static_assert(is_same_v<int&, int&>);
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

namespace Templates::TemplatedSpecialisation
{
    template <typename T>
    struct MyClass
    {
        using type = T;

        MyClass() {
            std::cout << "MyClass<T>" << std::endl;
        }
    };

    template <typename T>
    struct MyClass<T*>
    {
        using type = T*;

        explicit MyClass(type p) : ptr(p) {
            std::cout << "MyClass<T*>" << std::endl;
        }

        type getPtr() {
            return ptr;
        }

    private:
        type ptr;
    };

    void Test()
    {
        MyClass<int> generic;
        MyClass<int*> ci(nullptr);
        MyClass<long*> cl(nullptr);
        MyClass<std::pair<char, float>*> cp(0);

        /**
           MyClass<T>
           MyClass<T*>
           MyClass<T*>
           MyClass<T*>
         */
    }
}

namespace CompressedPair
{
    // First
    template<typename T,
             typename Deleter,
             bool hasEmptyBase = std::is_empty_v<Deleter> and not std::is_final_v<Deleter>>
    struct compressed_pair
    {
        T* data {};
        Deleter* deleter {};

        compressed_pair() {   std::cout << __PRETTY_FUNCTION__  << " First "<< std::endl;};
        compressed_pair(T* ptr, Deleter* del): data {ptr}, deleter{del} {
        }

        T*   first() { return data; }
        Deleter& second() { return *deleter; }
    };

    // Second
    template<typename T, typename Deleter>
    struct compressed_pair<T, Deleter, true> : public Deleter
    {
        T* data {};

        compressed_pair() {   std::cout << __PRETTY_FUNCTION__  << " Second "<< std::endl;};
        explicit compressed_pair(T* ptr): data { ptr } {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        T* first() { return data; }
        Deleter& second() { return *this; }
    };

    struct DeleterSimple {};
    struct DeleterSimpleFinal final {};

    template<typename T>
    struct DeleterSimpleComplex final
    {
        int counter = 0;

        void operator()(const T* ptr) {
            delete ptr;
        }
    };

    void Tests()
    {
        {
            compressed_pair<int, DeleterSimple>      pair;  // Second specialization
            std::cout << sizeof(pair) << std::endl;
        }
        {
            compressed_pair<int, DeleterSimpleFinal> pair;  // First
            std::cout << sizeof(pair) << std::endl;
        }
        {
            compressed_pair<int, DeleterSimpleComplex<int>> pair;  // First
            std::cout << sizeof(pair) << std::endl;
        }
    }
}


/** Pack Indexing **/
namespace pack_indexing
{

#if 0
    template <typename... T>
    constexpr auto first_plus_last(T... values) -> T...[0]
    {
        return T...[0](values...[0] + values...[sizeof...(values)-1]);
    }
#endif

    void get_parameters_pack_element()
    {
        using namespace std::string_literals;

#if 0
        std::string hello = first_plus_last("Hello"s, "world"s, "goodbye"s, "World"s);
        std::cout << "hello: " << hello << '\n';

        constexpr int sum = first_plus_last(1, 2, 10);
        std::cout << "sum: " << sum << "\n\n";
#endif


        std::cout << __cplusplus << std::endl;


        // Check compiler version
#if defined(__GNUC__)
        std::cout << "Compiler: GCC" << std::endl;
        std::cout << "Version: " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << std::endl;
#elif defined(_MSC_VER)
        std::cout << "Compiler: MSVC" << std::endl;
      std::cout << "Version: " << _MSC_VER << std::endl;
  #elif defined(__clang__)
        std::cout << "Compiler: Clang" << std::endl;
      std::cout << "Version: " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__ << std::endl;
  #else
        std::cout << "Compiler: Unknown" << std::endl;
#endif

    }
}

namespace Templates::FoldExpressions
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



void Templates::TestAll()
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;


    // FoldExpressions::MatchingTests();
    // FoldExpressions::PassingFunction_to_ClassTemplateArgument();
    // FoldExpressions::Recursive_Expansion();
    // FoldExpressions::Recursive_Expansion_Two();
    // FoldExpressions::GetFirstElementType_CreateVector();

    FoldExpressions::Concatenate_Arrays();

    // NTTP::testConfig();
    // NTTP::testPersonalBudget();

    // Specialization::Test();
    // Specialization::PrintOperator_TemplateSpecialisation_Ostream();

    // Templated_Templates::Container_WithTemplated_Types();

    // TemplatedSpecialisation::Test();

    // CompressedPair::Tests();


    // pack_indexing::get_parameters_pack_element();
}
