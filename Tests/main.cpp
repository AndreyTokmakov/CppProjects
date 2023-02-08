//============================================================================
// Name        : Tests.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Tests C++ project
//============================================================================

#include <optional>
#include <iostream>
#include <string>
#include <string_view>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>

#include <exception>
#include <thread>
#include <future>
#include <mutex>
#include <syncstream>

#include <numeric>

#include <utility>
#include <vector>
#include <any>
#include <list>
#include <forward_list>
#include <deque>
#include <map>
#include <algorithm>
#include <array>
#include <version>
#include <concepts>

#include "Algorithms/Algorithms.h"
#include "Geometry/PointsAndLines.h"
#include "Templates_Metaprogramming/Templates.h"
#include "Encoding/Unicode.h"
#include "Encoding/Punycode.h"
#include "Encoding/StringUtils.h"
#include "Encoding/Convertaion_UTF8_UTF32.h"
#include "ExpressionTemplates/ExpressionTemplates.h"
#include "DataStructures/LRUCache.h"
#include "DataStructures/EventLoop.h"
#include "DebugLogger/DebugLogger.h"
#include "Collections/CollectionsTests.h"
#include "ObjectOrientedExperimetns/RAIIWrapper.h"
#include "ConstexprMap/ConstexprMap.h"
#include "Multithreading/Multithreading.h"
#include "DesignPatterns/DesignPatterns.h"
#include "Iterators/Iterators.h"
#include "Files_Filesystem/Files.h"
#include "Helpers/Utilities.h"
#include "Strings/Strings.h"
#include "CopyElision_RVO/CopyElision_RVO.h"
#include "Performance/Performance.h"



namespace CurveCalcData {

    // TODO: OffSet as enum Upper, Lower?
    template<typename T, size_t OffSet = 0, size_t Capacity = 16>
    class TeethArray: public std::array<T, Capacity> {
    private:
        using value_type = T;
        using reference = value_type&;
        using size_type = std::size_t;

        // Add static_asserts for type

        static inline constexpr size_type _offset { 11 };

        [[nodiscard]]
        inline constexpr size_type _toothId2ArrayIndex(size_type id) const noexcept {
            if ((id + OffSet) > 20)
                id -= 2;
            return id - OffSet - _offset;
        }

    public:
        reference operator[](size_type index) noexcept {
            return std::array<value_type, Capacity>::operator[](_toothId2ArrayIndex(index));
        }

        const reference operator[](size_type index) const noexcept {
            return std::array<value_type, Capacity>::operator[](_toothId2ArrayIndex(index));
        }

        reference get(size_type index) noexcept {
            return std::array<value_type, Capacity>::operator[](_toothId2ArrayIndex(index));
        }

        const reference get(size_type index) const noexcept {
            return std::array<value_type, Capacity>::operator[](_toothId2ArrayIndex(index));
        }
    };

    void TeethArrayTest() {
        TeethArray<int> teeth {};

        teeth[11] = 11;
        teeth[18] = 18;
        teeth[21] = 21;
        teeth[28] = 28;

        for (auto x: teeth)
            std::cout << x << " ";
    }
}

// TODO: Add UNICODE src



namespace Conv
{

}

template<auto ...P>
struct Printer {
    inline static std::tuple data = std::tuple(P...);

    template<int idx>
    static auto get() {
        return std::get<idx>(data);
    }
};

void Funny_Tuple_Test () {
    static char str1[] = "answer1";
    static char str2[] = "answer2";
    static char str3[] = {'A', 'B', 0};

    auto x = Printer<str1, str2, str3>();

    std::cout << x.get<0>() << std::endl;
    std::cout << x.get<1>() << std::endl;
    std::cout << x.get<2>() << std::endl;

}



namespace FriendTests {

    class A {
        int v {0};

        friend class B;
    };

    class B {
    public:
        void foo(A& a) {
            a.v = 10;
        }
    };
}

std::vector<std::string> split(std::string_view input,
                               std::string_view delims = " ") {
    std::vector<std::string> output;
    for (size_t first = 0; first < input.size(); ) {
        const auto second = input.find_first_of(delims, first);
        if (first != second)
            output.emplace_back(input.substr(first, second - first));
        if (second == std::string_view::npos)
            break;
        first = second + 1;
    }
    return output;
}


struct AnyBase {
    virtual const std::type_info& type() = 0;
    virtual void copy_to(std::any&) = 0;
    virtual void move_to(std::any&) = 0;
    virtual ~AnyBase() = default;
};

std::string bin2hex(const std::string& input)
{
    std::string res;
    const char hex[] = "0123456789ABCDEF";
    for(auto sc : input)
    {
        unsigned char c = static_cast<unsigned char>(sc);
        res += hex[c >> 4];
        res += hex[c & 0xf];
    }

    return res;
}


class Graph
{
public:
    std::map<int, bool> visited;
    std::map<int, std::vector<int>> nodes;
    std::vector<int> path;

    void addEdge(int v, int w) {
        nodes[v].push_back(w);
    }



    int calcFriends () const noexcept{
        int result = std::numeric_limits<int>::max();
        for (const auto& [id, friends]: nodes)
        {
            if (friends.size() > 1)
            {
                const std::vector<int>& fr = nodes.at(id);
                int current  = fr.size() - 1;
                for (int i: fr)
                    current += nodes.at(i).size() - 1;

                result = std::min(result, current);

            }
        }
        return 0;
    }
};



namespace Chrono {

    using namespace std::literals;

    void Test1() {

        std::chrono::year_month_day ymd{2021y/2/2d};

        // std::cout << first << std::endl;

    }
}




inline const Utilities::Object make() {
    return Utilities::Object {};
}

template<typename T = int>
struct Path: std::vector<T> {
    T sum {};

    void push_back(T&& v) {
        std::vector<T>::push_back(v);
    }
};

std::string FormatString(std::string s) {
    std::string result;
    result.reserve(s.size());

    size_t  block = 0;
    for (char c: s) {
        if (' ' != c && '-' != c) {
            result.push_back(c);
            ++block;
        }

        if (block >= 3) {
            result.push_back(' ');
            block = 0;
        }
    }

    if (result.size() > 3 && ' ' == result[result.size() -2])
        std::swap(result[result.size() -2], result[result.size() -3]);

    //std::cout << "[" << result[result.size() -2] << "]" << std::endl;

    return result;
}

namespace OOP {



    void Test() {
        Helpers::Long l1 {1}, l2 { 2 };

        l1 = l2;
    }

    //-----------------------------------------------------------------------------------------

    struct A {
        virtual void a() = 0;
    };

    struct B {
        virtual void b() = 0;
    };

    struct C_A: A {
        void a() override {
            std::cout << "C::a()\n";
        }
    };

    struct C: A, B {
        void a() override {
            std::cout << "C::a()\n";
        }

        void b() override {
            std::cout << "C::b()\n";
        }
    };

    void VirtualMethodTests()
    {
        /*
        std::unique_ptr<A> ptr = std::make_unique<C>();

        ptr->a();
        dynamic_cast<B*>(ptr.get())->b();
        */

        C c;
        C_A c_a;

        std::cout << sizeof(c_a) << std::endl;
        std::cout << sizeof(c) << std::endl;
    }

    //----------------------------------------------------------------

    void MoveTest() {
        Helpers::Long l1 {111};
        // Long l2 = std::move(l1);

    }


    struct Base {
        Base() {
            std::cout << "Base::Base()" << std::endl;
        }

        operator short() const {
            std::cout << "operator Base::short()" << std::endl;
            return 1;
        }
    };

    struct Derived: Base {
        Derived() {
            std::cout << "Derived::Derived()" << std::endl;
        }
    };

    void count(int) {

    }

    void TestClassConversationOperatorCall()
    {
        const Derived object;
        count(object);
    }
}




namespace Tuples {

    template <size_t Index, typename Tuple, typename Functor>
    auto tuple_at(const Tuple& tpl, const Functor& func) -> void {
        const auto& v = std::get<Index>(tpl);
        func(v);
    }

    template<typename Tuple, typename Functor, size_t Index = 0>
    auto tuple_for_each(const Tuple &tpl, const Functor &f) -> void {
        constexpr auto tuple_size = std::tuple_size_v<Tuple>;
        if constexpr(Index < tuple_size) {
            tuple_at<Index>(tpl, f);
            tuple_for_each<Tuple, Functor, Index + 1>(tpl, f);
        }
    }

    void IterateTest()
    {
        auto tpl = std::make_tuple(1, true, std::string{"Jedi"});
        tuple_for_each(tpl, [](const auto& v) {
            std::cout << v << " ";
        });
    }
}

template<typename T>
struct Optional {
    T v {};

    /*
    template<class Type>
    void set(Type&& newVal) {
        v.~T();
        v = std::forward<Type>(newVal);
    }
    */

    template<class Type>
    void set(Type&& newVal) {
        //v.~T();
        std::exchange(v, std::forward<Type>(newVal));
    }
};


namespace RecursiveLambda {

    template<class Function>
    class y_combinator_result {
        Function func;

    public:
        template<class T>
        explicit y_combinator_result(T &&fun): func(std::forward<T>(fun)) {
        }

        template<class ...Args>
        decltype(auto) operator()(Args &&...args) {
            return func(std::ref(*this), std::forward<Args>(args)...);
        }
    };

    template<class Function>
    decltype(auto) y_combinator(Function && func) {
        return y_combinator_result<std::decay_t<Function>>(std::forward<Function>(func));
    }


    void DemoTest() {
        auto gcd = y_combinator([](auto gcd, int a, int b) -> int {
            return b == 0 ? a : gcd(b, a % b);
        });
        std::cout << gcd(20, 30) << std::endl;
    }
}





class StaticInitObject
{
    static inline const int x = []{
        std::cout << "This wil call once... Even without the object" << std::endl;
        return 1;
    }();

public:
    StaticInitObject() {
        std::cout << "StaticInitObject::ObjectX()!!" << std::endl;
    }
};



namespace Memory
{
    struct Object
    {
        Object() { std::cout << "Object()" << std::endl; }
        ~Object() { std::cout << "~Object()" << std::endl; }

        Object(const Object&) { std::cout << "Object(const Object& obj)" << std::endl; }
        Object(Object&&) noexcept { std::cout << "Object(Object&& obj) noexcept" << std::endl; }

        Object& operator==(const Object&) {
            std::cout << "Object& operator==(const Object&)" << std::endl;
            return *this;
        }

        Object& operator==(Object&&) noexcept {
            std::cout << "Object& operator==(Object&&) noexcept" << std::endl;
            return *this;
        }
    };

    class BadClass
    {
    private:
        Memory::Object* obj = new Object();
        // std::unique_ptr<Memory::Object> obj { std::make_unique<Memory::Object>() };

    public:

        BadClass() {
            throw 1;
        }

        ~BadClass() {
            delete obj;
        }
    };

    void Test()
    {
        try {
            BadClass b;
        }
        catch (...) {
            std::cout << "Ops" << std::endl;
        }
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

    //------------------------------------------------------------------------------

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

    void Test()
    {
        Task<Config{}>().submit();
    }

    //------------------------------------------------------------------------------

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


namespace Memory
{
    using session = int;

    auto session_factory(int id) {
        return std::make_shared<session>(id);
    }

    // https://ibob.bg/blog/2023/01/01/tracking-shared-ptr-leaks/
    void SharedPtrLeak()
    {
        std::shared_ptr<session> leak;

        std::vector<std::weak_ptr<session>> registry;

        constexpr int N = 20;
        srand(unsigned(std::time(nullptr)));
        auto i_to_leak = rand() % (2 * N);

        // std::cout << "i_to_leak = " << i_to_leak << std::endl;

        for (int i = 0; i < N; ++i) {
            auto sptr = session_factory(i);
            registry.push_back(sptr);
            if (i == i_to_leak) {
                leak = sptr;
                // std::cout << "Expect " << leak << " to leak\n";
            }
        }

        for (auto& w : registry) {
            if (w.use_count()) {
                std::cout << "found a leak in " << w.lock() << "\n";
            }
        }

        std::cout << "Done\n";
    }
}

namespace InvokeTest {

    template<typename F>
    concept FunctionPointer = std::is_member_function_pointer_v<F>;

    template<typename Derived>
    struct BuilderBase
    {
        /*
        template<typename... T>
        auto &When(FunctionPointer auto func, T &&... params) {
            auto &self = static_cast<This&>(*this);
            std::invoke(func, self, std::forward<T>(params)...);
            return actualThis;
        }

        auto &When(auto action) {
            auto &self = static_cast<This &>(*this);
            action(self);
            return actualThis;
        }
        */

        auto getSelf()
        {
            auto &self = static_cast<Derived&>(*this);
            return self;
        }

    };


    struct DataBuilder: BuilderBase<DataBuilder>
    {
        void getInfo(int v) {
            std::cout << "Data::getInfo()" << std::endl;
        }

        void invokeInfo() {

            std::invoke(&DataBuilder::getInfo, this, 2);
        }
    };


    /*
    template <typename Derived>
    struct Base {
        void info() {
            auto &self = static_cast<Derived&>(*this);
            self.info_impl();
        }

    protected:
        virtual void info_impl() = 0;
    };

    struct Triangle : public Base<Triangle> {
        void info_impl() { std::cout << "Triangle::info_impl()\n"; }
    };
    */

    void Test()
    {
        DataBuilder data;
        auto obj = data.getSelf();


        // data.invokeInfo();
        // BuilderBase<Data>{}.When(&Data::getInfo, 6);



    }
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Experiments::Test({20, 40, 60});
    // DesignPatterns::TestAll();
    Multithreading::TestAll();
    // Memory::Test();
    // Strings::TestAll();
    // Iterators::TestAll();
    // Algorithms::TestAll();
    // Files::TestAll();
    // ConstexprMap::TestAll()
    // DebugLogger::TestAll();
    // UniquePtr_Size::SizeTest();
    // CollectionsTests::TestAll();
    // Templates::TestAll();
    // ExpressionTemplates::TestAll();
    // CopyElision_RVO::TestAll();
    // ObjectOrientedExperiments::RAIIWrapper::TestAll();
    // LRUCache::TestAll();
    // EventLoop::TestAll();
    // Iterators::TestAll();
    // Unicode::TestAll();                   // Encoding
    // StringUtils::TestAll();               // Encoding
    // Convertaion_UTF8_UTF32::TestAll();    // Encoding
    // Performance::TestAll();

    // OOP::Test();
    // OOP::VirtualMethodTests();
    // OOP::MoveTest();
    // OOP::TestClassConversationOperatorCall();


    // InvokeTest::Test();


    // Memory::SharedPtrLeak();

    // Templates::Test();
    // Templates::Test2();

    // StaticInitObject a, b;
    // Concepts_Experiments::TestConcepts();

    // OrderBook::TestAll();
    // OrderBook2::TestAll();

    // MatchingOrderBook::TestAll();
    // MatchingOrderBookEx::TestAll();

    // TestDataGenerator::GenerateData();

    // CacheLineTests::Test();

    // Optional<Utilities::Long> opt;
    // opt.set(Utilities::Long {2});

    // Funny_Tuple_Test();


    // Tuples::IterateTest();

    // CurveCalcData::TeethArrayTest();

    // Templates::TestAll();
    // PointsAndLines::TestAll();

#if 0
    {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        for (long i = 0; i < 100'000'000; ++i) {
            std::copy_n(input.cbegin(), input.size(), buffer);
        }
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "It took me " << time_span.count() << " seconds.\n";
    }

    {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        for (long i = 0; i < 100'000'000; ++i) {
            std::uninitialized_move_n(input.begin(), input.size(), buffer);
        }

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "It took me " << time_span.count() << " seconds.\n";
    }
#endif

    return EXIT_SUCCESS;
}
