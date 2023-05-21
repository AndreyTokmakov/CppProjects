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
#include <functional>
#include <filesystem>
#include <tuple>
#include <ranges>

#include <exception>
#include <thread>
#include <future>
#include <mutex>
#include <syncstream>
#include <utility>

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
#include <span>
#include <cmath>
#include <stack>
#include <variant>
#include <chrono>
#include <random>

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
#include "DataStructures/MaxStack.h"
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
#include "Memory/Memory.h"
#include "BinaryAnalyzer/BinaryAnalyzer.h"

// C++ 23:
#include <expected>
#include <format>
#include <stacktrace>

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

namespace OOP
{
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
        // std::cout << "This wil call once... Even without the object" << std::endl;
        std::operator<<(std::cout, "This wil call once... Even without the object\n");
        return 1;
    }();

public:
    StaticInitObject() {
        std::cout << "StaticInitObject::ObjectX()!!" << std::endl;
    }
};





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


namespace Memory
{
    struct Base {
        virtual void info() const noexcept {
            std::cout << "Base::info()\n";
        }

        virtual ~Base() = default;
    };

    struct Derived : Base {
        void info() const noexcept override {
            std::cout << "Derived::info()\n";
        }
    };

    struct Parent
    {
        virtual std::unique_ptr<Base> make() {
            return std::make_unique<Base>();
        }
    };

    struct Child : Parent
    {
        std::unique_ptr<Base> make() override {
            return std::make_unique<Derived>();
        }
    };

    void test()
    {
        Parent{}.make()->info();
        Child{}.make()->info();
    }


    struct ARPHeader
    {
        uint16_t htype {0};
        uint16_t ptype {0};
        uint8_t  hlen {};
        uint8_t  plen {};
        uint16_t opcode {0};
        uint8_t  sender_mac[6]{};
        uint32_t sender_ip {};
        uint8_t  target_mac[6]{};
        uint32_t target_ip {};

    public:
        [[nodiscard]]
        bool SetSenderMACAddress([[maybe_unused]] std::string_view mac) const {
            return htype != 0;
        }

    } __attribute__((packed, aligned(1))) ;


    void initMemset(ARPHeader* arpHeader)
    {
        memset(arpHeader, 0, sizeof(ARPHeader));
    }

    void initAssignment(ARPHeader* arpHeader)
    {
        *arpHeader = {};
    }

    void Memset_vs_Assignment()
    {
        std::unique_ptr<ARPHeader> apr { std::make_unique<ARPHeader>()};

        apr->target_ip = 12345;
        std::cout << apr->target_ip << std::endl;

        // initMemset(apr.get());
        initAssignment(apr.get());

        std::cout << apr->target_ip << std::endl;
    }

    void Memset_vs_Assignment_Perf()
    {
        std::unique_ptr<ARPHeader> apr { std::make_unique<ARPHeader>()};
        constexpr size_t iterCount {1'000'00};

        {
            auto start = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < iterCount; ++i)
            {
                for (int n = 0; n < iterCount; n++)
                {
                    initAssignment(apr.get());
                    // initMemset(apr.get());
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "Result: " << duration << " microseconds" << std::endl;
        }

        {
            auto start = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < iterCount; ++i)
            {
                for (int n = 0; n < iterCount; n++)
                {
                    // initAssignment(apr.get());
                    initMemset(apr.get());
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "Result: " << duration << " microseconds" << std::endl;
        }
    }

    void Double_Delete_Nullptr()
    {
        int* iPtr = new int(100500);

        std::cout << *iPtr << " at " << iPtr << std::endl;

        {
            delete iPtr;
            iPtr = nullptr;
        }

        delete iPtr;
    }
}


namespace Decorator
{
    struct Money {
        uint64_t value{};
    };


    template<typename T> requires std::is_arithmetic_v<T>
    [[nodiscard]]
    Money operator*(const Money& money, T factor) {
        return Money {static_cast<uint64_t>( money.value * factor )};
    }

    [[nodiscard]]
    constexpr Money operator+(const Money& lhs, const Money& rhs) noexcept {
        return Money{lhs.value + rhs.value};
    }

    std::ostream &operator<<(std::ostream &stream, const Money &money) {
        stream << money.value;
        return stream;
    }



    template<typename T>
    concept PricedItem = requires(T item) {
        { item.price() } -> std::same_as<Money>;
    };

    template<int taxRate, PricedItem Item>
    class Taxed : private Item {
    public:
        template<typename... Args>
        explicit Taxed(Args&& ... args): Item {std::forward<Args>(args)...} {
            // ....
        }

        [[nodiscard]]
        Money price() const {
            return Item::price() * (1.0 + (taxRate / 100));
        }
    };


    template<int discount, PricedItem Item>
    class Discounted {
    public:
        template<typename... Args>
        explicit Discounted(Args&& ... args): item{std::forward<Args>(args)...} {
            // ....
        }

        [[nodiscard]]
        Money price() const {
            return item.price() * (1.0 - (discount / 100));
        }

    private:
        Item item;
    };


    struct Ticket
    {
        Ticket(std::string name, Money price ): name_{ std::move(name) } , price_{ price } {
            // ....
        }

        [[nodiscard]]
        const std::string& name() const {
            return name_;
        }

        [[nodiscard]]
        Money price() const {
            return price_;
        }

    private:
        std::string name_;
        Money price_;
    };


    struct Book
    {
        Book(std::string name, Money price ): name_{ std::move(name) }, price_{ price }
        {}

        [[nodiscard]]
        std::string const& name() const {
            return name_;
        }

        [[nodiscard]] Money price() const {
            return price_;
        }

    private:
        std::string name_;
        Money price_;
    };


    void test()
    {
        Taxed<15, Discounted<20, Ticket>> item1 { "Core C++", Money{499} };
        Taxed<16, Discounted<21, Ticket>> item2 { "Core C++", Money{499} };
        Taxed<17, Discounted<22, Book>> item3 { "Core C++", Money{499} };

        const Money totalPrice1 = item1.price();  // Results in 459.08
        const Money totalPrice2 = item2.price();
        const Money totalPrice3 = item3.price();
    }
}

namespace Date_Time
{
    using namespace std::chrono;

    std::ostream& operator<<(std::ostream& stream,
                             const std::chrono::year_month_day& ymd)
    {
        stream << static_cast<int>(ymd.year()) << " / "
               << static_cast<unsigned>(ymd.month()) << " / "
               << static_cast<unsigned>(ymd.day()) ;

        return stream;
    }

    void ChronoTests()
    {
        using namespace std::chrono;
        using namespace std::chrono_literals;

        std::chrono::year_month_day startDay = std::chrono::day {1} / 2 / 2023;
        std::cout << startDay << std::endl;


        std::chrono::year_month_day d2 { year {2023}, month {3}, day{14}};
        std::cout << d2 << std::endl;

        /*
        for (auto d = startDay; d.month() == startDay.month(); d += std::chrono::months{1}) {
            std::cout << d << '\n';
        }*/
    }

    void Test2()
    {
        std::cout << "USA switching to summer time on "
                  << year_month_day{2023y/March/Sunday[2]} << "\n";
        std::cout << "Europe switching to summer time on "
                  << year_month_day{2023y/March/Sunday[last]} << "\n\n";

    }
}


namespace NTTP
{
    template<auto Func>
    struct PersonalBudget {
        double compute(std::uint32_t amt) {
            return Func(amt);
        }
    };

    void test()
    {

        auto Savings1 = [](int amt) -> decltype(auto) {
            return static_cast<double>(0.75*amt);
        };

        PersonalBudget<Savings1> Savingsbudget{};

        auto savings = Savingsbudget.compute(2300);
        std::cout << "Estimated Savings: " << savings << std::endl;

    }
}


namespace OOP_Test
{
    struct Element {
        std::string name {};
    };

    std::vector<Element> getElements() {
        return {
            Element{"ElementOne"},
            Element{"ElementTwo"},
            Element{"ElementThree"},
            Element{"ElementFour"},
            Element{"ElementFive"}
        };
    }

    namespace ExampleTwo
    {
        struct HandlerBase
        {
            void handle(std::span<Element> elements) {
                for (Element el: elements)
                    handle(std::move(el));
            }

            virtual void handle(Element element) = 0;
            virtual ~HandlerBase() = default;
        };

        struct HandlerA: HandlerBase {
            void handle(Element element) override {
                std::cout << "HandlerA: " << element.name << std::endl;
            }
        };

        struct HandlerB: HandlerBase {
            void handle(Element element) override {
                std::cout << "HandlerA: " << element.name << std::endl;
            }
        };

        std::unique_ptr<HandlerBase> getHandler()
        {
            return std::make_unique<HandlerA>();
        }

        void test()
        {
            const auto handler = getHandler();
            std::vector<Element> elements = getElements();
            handler->handle(elements);
        }
    }

    namespace ExampleThree
    {
        struct HandlerBase
        {
            virtual void handle(std::span<Element> elements) = 0;
            virtual ~HandlerBase() = default;
        };

        template<class Derived>
        struct Handler: HandlerBase {
            void handle(std::span<Element> elements) {
                for (Element el: elements)
                    d.handle(std::move(el));
            }

            Derived d {};
        };

        struct HandlerA {
            void handle(Element element) {
                std::cout << "HandlerA: " << element.name << std::endl;
            }
        };

        struct HandlerB {
            void handle(Element element) {
                std::cout << "HandlerA: " << element.name << std::endl;
            }
        };

        std::unique_ptr<HandlerBase> getHandler()
        {
            return std::make_unique<Handler<HandlerA>>();
        }

        void test()
        {
            const auto handler = getHandler();
            std::vector<Element> elements = getElements();
            handler->handle(elements);
        }
    }
}

namespace Math
{
    int calc(int val)
    {
        int count = 0;
        while (val > 1) {
            val /= 2;
            ++count;
        }
        return count;
    }

    void Log2Test()
    {
        calc(59218);
    }
}



namespace StackTrace
{

}

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);


    const char* const ptr = new char('s');

    std::cout << *ptr << std::endl;
    std::cout << sizeof(ptr) << std::endl;

    // NTTP::test();

    // Math::Log2Test();

    // OOP_Test::ExampleTwo::test();
    // OOP_Test::ExampleThree::test();

    // Experiments::Test({20, 40, 60});
    // Multithreading::TestAll();
    // Memory::TestAll();
    // Strings::TestAll();
    // Iterators::TestAll();
    // Algorithms::TestAll();
    // Files::TestAll();
    // ConstexprMap::TestAll()
    // DesignPatterns::TestAll();
    // MaxStack::TestAll();
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
    // BinaryAnalyzer::TestAll();


    // Memory::test();
    // Memory::Memset_vs_Assignment();
    // Memory::Memset_vs_Assignment_Perf();
    // Memory::Double_Delete_Nullptr();


    // OOP::MoveTest();
    // OOP::TestClassConversationOperatorCall();


    // InvokeTest::Test();
    // Templates::Test();
    // Templates::Test2();


    // Date_Time::ChronoTests();
    // Date_Time::Test2();


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
