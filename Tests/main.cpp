/**============================================================================
Name        : Tests.cpp
Created on  : 15.09.2021
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Tests C++ project
============================================================================**/

#include <optional>
#include <iostream>
#include <string>
#include <string_view>
#include <cstring>
#include <fstream>
#include <functional>
#include <filesystem>
#include <tuple>
#include <ranges>
#include <cassert>

#include <exception>
#include <thread>
#include <future>
#include <mutex>
#include <syncstream>
#include <utility>

#include <numeric>
#include <queue>

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
#include <span>
#include <cmath>
#include <stack>
#include <variant>
#include <chrono>
#include <random>
#include <format>
#include <iomanip>

#include <experimental/socket>

#include "Algorithms/Algorithms.h"
#include "Geometry/PointsAndLines.h"
#include "Encoding/Unicode.h"
#include "Encoding/Punycode.h"
#include "Encoding/StringUtils.h"
#include "Encoding/Convertaion_UTF8_UTF32.h"
#include "ExpressionTemplates/ExpressionTemplates.h"
#include "Templates_Metaprogramming/Templates.h"
#include "DataStructures/LRUCache.h"
#include "DataStructures/EventLoop.h"
#include "DataStructures/MaxStack.h"
#include "DataStructures/MinStack.h"
#include "DataStructures/DVector.h"
#include "DataStructures/RateLimiter.h"
#include "DataStructures/BitReader.h"
#include "DebugLogger/DebugLogger.h"
#include "Date_Time_Chrono/Date_Time_Chrono.h"
#include "Collections/CollectionsTests.h"
#include "ObjectOrientedExperimetns/RAIIWrapper.h"
#include "ObjectOrientedExperimetns/OOP_Experiments.h"
#include "ObjectOrientedExperimetns/VirtualTables.h"
#include "Optional/Optional.h"
#include "ConstexprMap/ConstexprMap.h"
#include "Multithreading/Multithreading.h"
#include "DesignPatterns/DesignPatterns.h"
#include "Iterators/Iterators.h"
#include "Math/Math.h"
#include "Files_Filesystem/Files.h"
#include "Helpers/Utilities.h"
#include "Strings/Strings.h"
#include "CopyElision_RVO/CopyElision_RVO.h"
#include "Performance/Performance.h"
#include "Memory/Memory.h"
#include "BinaryAnalyzer/BinaryAnalyzer.h"
#include "ThinkCell/ThinkCell.h"
#include "TableFormatter/TableFormatter.h"
#include "Coroutines/Coroutines.h"
#include "Cpp23_Features/Cpp23_Features.h"
#include "Auto/AutoTests.h"
#include "Heap/Heap.h"
#include "Comparators/Comparators.h"
#include "FindMinMaxValues/FindMinMaxValues.h"
#include "FindMinMaxValues/FindMinMaxValues.h"

struct AnyBase {
    virtual const std::type_info& type() = 0;
    virtual void copy_to(std::any&) = 0;
    virtual void move_to(std::any&) = 0;
    virtual ~AnyBase() = default;
};

namespace
{
    template<typename T>
    std::ostream& operator<<(std::ostream & stream,
                             const std::vector<T>& collection)
    {
        for (const T& v: collection)
            stream << v << ' ';
        return stream;
    }

    template<typename T, size_t _Size>
    std::ostream& operator<<(std::ostream & stream,
                             const std::array<T, _Size>& collection)
    {
        for (const T& v: collection)
            stream << v << ' ';
        return stream;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream & stream,
                             const std::priority_queue<T>& queue)
    {
        std::priority_queue<T> queueCopy = queue;
        while (false == queueCopy.empty()) {
            stream << queueCopy.top()  << ' ';
            queueCopy.pop();
        }
        return stream;
    }
}

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


inline Utilities::Object make() {
    return Utilities::Object {};
}

template<typename T = int>
struct Path: std::vector<T> {
    T sum {};

    void push_back(T&& v) {
        std::vector<T>::push_back(v);
    }
};

std::string FormatString(const std::string& s) {
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

#if 0

class StaticInitObject
{
    static inline const int x = []{
        // std::cout << "This wil call once... Even without the object" << std::endl;
        std::operator<<(std::cout, "This wil call once... Even without the object\n");
        return 1;
    }();

public:
    StaticInitObject() {
        std::cout << "StaticInitObject::ObjectX()!!" << std::endl;
    }
};

#endif


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
        void getInfo(int) {
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

        [[maybe_unused]]
        auto obj = data.getSelf();

        // data.invokeInfo();
        // BuilderBase<Data>{}.When(&Data::getInfo, 6);
    }
}


namespace ReturnTypeCast
{
    struct Value
    {
        std::string value{"123.456"};

        explicit operator int() {
            std::cout << "Value::operator int()\n";
            return 123;
        }

        // private:
        operator double() {
            std::cout << "Value::operator double()\n";
            return 123.456f;
        }
    };


    void tests()
    {
        Value val;
        {
            const int v = val;
            std::cout << v << std::endl;
        }
        {
            const double v = val;
            std::cout << v << std::endl;
        }
    }
}

namespace OperatorCall_ExplicitTypeSpecialization
{
    struct Data1 {
    };

    template<typename T>
    struct StorageOptions
    {
        void ignore_missing([[maybe_unused]] bool ignore) {
            // Some logic
        }
    };

    struct StorageLoader
    {
        template<typename DataType>
        DataType operator()(const StorageOptions<DataType>*)
        {
            return DataType{};
        }
    };

    void Test()
    {
        StorageLoader storageLoader;

        {
            StorageOptions<Data1> dataOptions;
            dataOptions.ignore_missing(true);

            auto data = storageLoader(&dataOptions);
            std::cout << typeid(data).name() << std::endl;
        }

        {
            auto data = storageLoader.operator()<Data1>(nullptr);
            std::cout << typeid(data).name() << std::endl;
        }

        {
            Data1 obj;
            auto data = storageLoader.operator()<decltype(obj)>(nullptr);
            std::cout << typeid(data).name() << std::endl;
        }
    }
}

namespace CallFunctionByName
{
    using namespace std::string_view_literals;

    struct my_class
    {
        void function_a() const { std::cout << "my_class::function_a\n"; }
        void function_b() const { std::cout << "my_class::function_b\n"; }
        void function_c() const { std::cout << "my_class::function_c\n"; }

        void function_d(int v) const { std::cout << "my_class::function_d: params: " << v << "\n"; }
    };

    class my_class_functions_collection {
    private:
        using my_class_func_t = void(my_class::*)() const;

    public:
        explicit my_class_functions_collection(my_class *my_class_ptr) : mc_ptr(my_class_ptr) {
        }

        void call_function(std::string_view func_name) {
            // (mc_ptr->*(functions_collection.at(func_name)) )();
            std::invoke(funcMapping.at(func_name) , mc_ptr);
        }

        template<typename... Args>
        void call_function_ex(std::string &&func_name, Args&&... params)
        {
            std::invoke(funcMapping.at(func_name) , mc_ptr, std::forward<Args>(params)...);
        }

    private:
        const std::unordered_map<std::string_view, my_class_func_t> funcMapping {
                {"function_a"sv, &my_class::function_a},
                {"function_b"sv, &my_class::function_b},
                {"function_c"sv, &my_class::function_c},
        };
        my_class *mc_ptr;
    };

    void Test()
    {
        my_class mc;
        my_class_functions_collection mc_functions(&mc);

        mc_functions.call_function("function_a");
        mc_functions.call_function("function_b");
        mc_functions.call_function("function_c");
        mc_functions.call_function_ex("function_c");
    }
}

void parseInputParams(const char** argv, const size_t size)
{
    const std::vector<std::string_view> args(argv, argv + size);

    for (const std::string_view sv: args)
        std::cout << sv << " ";
    std::cout << std::endl;

    const std::span<const char*> argsSpan(argv,  size);

    for (const std::string_view sv: argsSpan)
        std::cout << sv << " ";
    std::cout << std::endl;
}

namespace ConceptsTests
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

namespace Conversation
{
    struct B {};

    struct A {
        A() = default;
        explicit A(const B&)  {}
        operator B() const {  return {}; }
    };

    void Fun([[maybe_unused]] A a) {}

    void test2()
    {
        Fun(A{});
        // Fun(B{}); // #B Will not compile due to explicit ctor
    }
}

namespace PrintTemplateType
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


    void test()
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

namespace ObjectOrientedExperiments
{
    class Base {
    public:
        virtual void show() { std::cout << "In Base" << std::endl; }
    };

    class Derived : public Base {
    public:
        virtual void show() { std::cout << "In Derived" << std::endl; }
    };

    void Call_BaseClass_Func_Hack()
    {
        Base *ptr = new Derived;
        ptr->Base::show();  // "In Base" will be printed
        ptr->show();        // "In Base" will be printed
    }
}


namespace MoveExperiments
{
    using Helpers::Long;

    std::vector<std::string> logs {};
    std::vector<Long> storage {};

    template<typename T> requires std::convertible_to<T, Long>
    void store_new(T&& v)
    {
        storage.push_back(std::forward<T>(v));
    }

    void store(const Long & str)
    {
        storage.push_back(str);
    }

    void store(Long && str)
    {
        storage.push_back(std::move(str));
    }


    void test_overload()
    {
        {
            Long l{1};
            store(l);
        }
        std::cout << std::endl;

        {
            store(Long{1});
        }
        std::cout << std::endl;
    }

    void test_perfect_forwarding()
    {
        {
            Long l{1};
            store_new(l);
        }
        std::cout << std::endl;

        {
            store_new(Long{1});
        }
        std::cout << std::endl;
    }
}

template<typename _Ty>
struct Allocator: std::allocator<_Ty>
{
    _Ty* allocate(size_t size)
    {
        return new _Ty[size];
    }

    void deallocate(_Ty* ptr)
    {
        delete[] ptr;
    }
};

void MoveStringToArray()
{
    constexpr size_t capacity {10};

    std::string text {"12345"};

    auto allocator = Allocator<std::string>{};

    std::string* data = allocator.allocate(capacity);
    // std::string* data = new std::string("qwerty");
    // std::string* data = new std::string[capacity];

    std::cout << std::quoted(data[5]) << std::endl;

    // *data = std::move(text);
    data[5] = std::move(text);

    std::cout << std::quoted(data[5]) << std::endl;
}




int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);
    // parseInputParams(std::vector {"one", "two", "three", "four", "five"}.data(), 5);



    // MoveStringToArray();

    // PrintTemplateType::test();

    // ObjectOrientedExperiments::Call_BaseClass_Func_Hack();

    // ConceptsTests::If_Constexpr_Concepts();

    // Experiments::Test({20, 40, 60});

    // Coroutines::TestAll();

    // OperatorCall_ExplicitTypeSpecialization::Test();
    // CallFunctionByName::Test();

    // ReturnTypeCast::tests();

    // FindMinMaxValues::TestAll();

    // Heap::TestAll();

    // Comparators::TestAll();

    // Cpp23_Features::TestAll();
    // AutoTests::TestAll();
    // Algorithms::TestAll();
    // Multithreading::TestAll();
    // Memory::TestAll();
    // Iterators::TestAll();
    // Files::TestAll();
    // ConstexprMap::TestAll()
    // DesignPatterns::TestAll();
    // Date_Time_Chrono::TestAll();
    // MaxStack::TestAll();
    // MinStack::TestAll();
    // RateLimiter::TestAll();
    // DebugLogger::TestAll();
    // UniquePtr_Size::SizeTest();
    // CollectionsTests::TestAll();
    // Templates::TestAll();
    // ExpressionTemplates::TestAll();
    // CopyElision_RVO::TestAll();
    // ObjectOrientedExperiments::RAIIWrapper::TestAll();
    // ObjectOrientedExperiments::OOP_Experiments::TestAll();
    // ObjectOrientedExperiments::VirtualTables::TestAll();
    // Optional::TestAll();
    // Math::TestAll();
    // LRUCache::TestAll();
    // EventLoop::TestAll();
    // DVector::TestAll();
    // Iterators::TestAll();
    // Convertaion_UTF8_UTF32::TestAll();    // Encoding
    // Unicode::TestAll();                   // Encoding
    // StringUtils::TestAll();               // Encoding
    // Strings::TestAll();
    Performance::TestAll();
    // BinaryAnalyzer::TestAll();
    // ThinkCell::IntervalMapTest();
    // TableFormatter::TestAll();

    // OOP::TestClassConversationOperatorCall();

    // InvokeTest::Test();

    // StaticInitObject a, b;
    // Concepts_Experiments::TestConcepts();

    // OrderBook::TestAll();
    // OrderBook2::TestAll();
    // MatchingOrderBook::TestAll();
    // MatchingOrderBookEx::TestAll();

    // TestDataGenerator::GenerateData();

    // CacheLineTests::Test();
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
