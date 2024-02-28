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
#include "Concepts//Concepts.h"
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
#include "FunctionCall_LookUp/FunctionCall_LookUp.h"

#define PRINT_LINE   std::cout.width(128); std::cout.fill('='); std::cout << '\n';

struct AnyBase
{
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



namespace RecursiveLambda
{

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


namespace InvokeTest
{
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

        operator short() {
            std::cout << "Value::operator short()\n";
            return 123;
        }

        operator unsigned int() {
            std::cout << "Value::operator unsigned int()\n";
            return 123;
        }

        explicit operator int() {
            std::cout << "Value::operator int()\n";
            return 123;
        }

        operator double() {
            std::cout << "Value::operator double()\n";
            return 123.456f;
        }
    };


    void tests()
    {
        Value val;


        {
            [[maybe_unused]] short v = val;
            // std::cout << v << std::endl;
        }
        {
            [[maybe_unused]] unsigned int v = val;
            // std::cout << v << std::endl;
        }
        {
            [[maybe_unused]] int v = val;
            // std::cout << v << std::endl;
        }
        {
            // [[maybe_unused]] float v = val;
            // std::cout << v << std::endl;
        }
        {
            [[maybe_unused]] double v = val;
            // std::cout << v << std::endl;
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

namespace MoveExperiments
{
    using Helpers::Integer;

    template<typename _Ty>
    struct CountingAllocator: std::allocator<_Ty>
    {
        static inline size_t allocated = 0;

        _Ty* allocate(size_t size)
        {
            allocated += size;
            // return new _Ty[size];
            return std::allocator<_Ty>::allocate(size);
        }

        void deallocate(_Ty* ptr, size_t size)
        {
            //delete[] ptr;
            std::allocator<_Ty>::deallocate(ptr, size);
        }
    };

    // std::vector<std::string> logs {};
    std::vector<Integer> storage {};

    template<typename T> requires std::convertible_to<T, Integer>
    void store_new(T&& v)
    {
        storage.push_back(std::forward<T>(v));
    }

    void store(const Integer& obj)
    {
        storage.push_back(obj);
    }

    void store(Integer&& obj)
    {
        storage.push_back(std::move(obj));
    }

    void test_overload()
    {
        {
            Integer l{1};
            store(l);
        }

        PRINT_LINE

        {
            store(Integer{2});
        }

        PRINT_LINE
    }

    void test_perfect_forwarding()
    {
        {
            Integer l{1};
            store_new(l);
        }

        PRINT_LINE

        {
            store_new(Integer{2});
        }

        PRINT_LINE
    }

    void MoveStringToArray_Segfault()
    {
        constexpr size_t capacity {10};

        {
            std::string text {"123456789"};
            std::cout << std::quoted(text) << std::endl;

            std::string* data = new std::string[capacity];

            std::cout << std::quoted(data[5]) << " --> ";
            data[5] = std::move(text);
            std::cout << std::quoted(data[5]) << std::endl;

            delete[] data;
        }

        std::cout << std::endl;

        {
            std::string text {"123456789"};
            std::cout << std::quoted(text) << std::endl;

            auto allocator = CountingAllocator<std::string>{};
            std::string* data = allocator.allocate(capacity);


            std::cout << data << std::endl;

            std::cout << std::quoted(data[5]) << " --> ";
            data[5] = std::move(text);
            std::cout << std::quoted(data[5]) << std::endl;

            allocator.deallocate(data, capacity);
        }
    }
}

namespace ReturnClass_MemberRef_CopyCTor
{
    struct Holder
    {
        Helpers::Integer value { 123 };

        Helpers::Integer& getAsRef() noexcept
        {
            return value;
        }

        Helpers::Integer getAsVal() noexcept
        {
            return value;
        }
    };

    void tests()
    {
        Holder holder;

        std::cout << "-----------------------------------------------------------------\n";

        {
            Helpers::Integer & valRef = holder.getAsRef();
            std::cout << valRef.value << std::endl;
        }

        std::cout << "-----------------------------------------------------------------\n";

        {
            Helpers::Integer val = holder.getAsRef();
            std::cout << val.value << std::endl;
        }

        std::cout << "-----------------------------------------------------------------\n";

        {
            const Helpers::Integer & valRef = holder.getAsVal();
            std::cout << valRef.value << std::endl;
        }

        std::cout << "-----------------------------------------------------------------\n";

        {
            Helpers::Integer val = holder.getAsVal();
            std::cout << val.value << std::endl;
        }

        std::cout << "-----------------------------------------------------------------\n";
    }
}

namespace OOP
{

    class Base {
    protected:
        int a {0};
    };

    class Derived: protected Base
    {
    public:
        void test_derived()
        {
            a = 10;
        }
    };

    class DerivedEx: protected Derived
    {
    public:
        void test_derived_ex()
        {
            a = 10;
        }
    };

    void foo()
    {
        Derived d;
        d.test_derived();

        DerivedEx de;
        de.test_derived_ex();
    }
}


namespace WrapperTests
{
    void create()
    {
        Helpers::Integer obj {1};
    }

    void increment()
    {
        Helpers::Integer obj {1};

        ++obj;
        std::cout << obj << std::endl;

        obj++;
        std::cout << obj << std::endl;
    }

    void decrement()
    {
        Helpers::Integer obj {3};

        --obj;
        std::cout << obj << std::endl;

        obj--;
        std::cout << obj << std::endl;
    }

    void Test()
    {
        // create();
        // increment();
        decrement();
    }
}


namespace StaticCounter
{
    template<typename T>
    struct TableEntry
    {
        static inline size_t uniqueEntryId { 0 };
        std::string name;
        size_t id {};

        explicit TableEntry(std::string name): name {std::move(name)} {
            // First entry shall have ID = 1 not zero
            id = ++uniqueEntryId;
        }
    };

    struct Movie: TableEntry<Movie>
    {
        explicit Movie(std::string name): TableEntry {std::move(name)} {
        }
    };
    struct Theater: TableEntry<Theater>
    {
        explicit Theater(std::string name): TableEntry {std::move(name)} {
        }
    };

    void Test()
    {
        Movie a("A"),b("B"),c("C");
        Theater t1("T1"), t2("T2");

        std::cout << a.id << " " << b.id << " " << c.id << std::endl;
        std::cout << t1.id << " " << t2.id << std::endl;
    }
}

namespace TestAssignment
{
    struct Result
    {
        int output1[100];
        int output2{};
        int output3{};

        Result() : output1() {
        }
    };

    struct account_balance
    {
        int account_id;
        int balance;
    };

    struct transfer
    {
        int from;
        int to;
        int amount;
    };


    std::ostream& operator<<(std::ostream& stream, const account_balance& acct)
    {
        stream << "Acct(" << acct.account_id << ", " << acct.balance << ")";
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const std::vector<account_balance>& balances)
    {
        std::cout << "Balances:\n";
        for (const auto& x: balances)
            stream << "\tAcct(" << x.account_id << ", " << x.balance << ")\n";
        return stream;
    }

    using transaction = std::vector<transfer>;

    struct Database
    {
        using TransactionEntry = std::pair<transaction, size_t>;

        std::unordered_map<int, int> accounts;
        std::vector<TransactionEntry> unsettledTransaction;
        std::vector<size_t> appliedTransactions;
        size_t transactionId { 0 };

        explicit Database(const std::vector<account_balance>& acctBalances)
        {
            accounts.reserve(acctBalances.size());
            for (const account_balance& accInfo: acctBalances) {
                accounts[accInfo.account_id] = accInfo.balance;
            }
        }

        // push a transaction to the database
        void push_transaction(const transaction& transaction)
        {
            unsettledTransaction.push_back({transaction, transactionId++});
            for (const transfer& transfer: transaction)
            {
                accounts[transfer.from] -= transfer.amount;
                accounts[transfer.to] += transfer.amount;
                std::cout << "Handing Transfer(from: " << transfer.from
                    << ", to: " << transfer.to << ", amount: " << transfer.amount << ")\n";
            }
        }

        [[nodiscard]]
        bool hasOverdrawn() const
        {
            for (const auto& [acctId, balance]: accounts)
                if (0 > balance)
                    return true;
            return false;
        }

        // settle the database such that invariant is maintained and best state of the database is present
        void settle()
        {
            int idx = unsettledTransaction.size() - 1;
            while (hasOverdrawn()) {
                const TransactionEntry &transaction = unsettledTransaction[idx--];
                for (const auto &transfer: transaction.first) {
                    accounts[transfer.from] += transfer.amount;
                    accounts[transfer.to] -= transfer.amount;

                    std::cout << "\tRollback(from: " << transfer.from << ", to: "
                              << transfer.to << ", amount: " << transfer.amount << ") ID = " << transaction.second << "\n";
                }
            }

            unsettledTransaction.resize(idx + 1);
            for (const TransactionEntry& transactionEntry: unsettledTransaction)
                appliedTransactions.push_back(transactionEntry.second);

            unsettledTransaction.clear();
        }

        // return a listing of all balances in any order
        [[nodiscard]]
        std::vector<account_balance> get_balances() const
        {
            std::vector<account_balance> balances;
            balances.reserve(accounts.size());

            for (const auto& [acctId, balance]: accounts)
                balances.push_back({acctId, balance});

            return balances;
        }

        // Given the logical sequence of transactions constructed by the ordered sequence of calls
        // to push_transaction AND having called settle()
        //
        // return the 0-based indices of the surviving transactions in that sequence which, when
        // applied to the initial balances produce a state where the invariant is maintained
        //

        [[nodiscard]]
        std::vector<size_t> get_applied_transactions() const
        {
            return appliedTransactions;
        }

        void stats()
        {
            std::cout << "Balances:\n";
            for (const auto& [acctId, balance]: accounts)
                std::cout << "\tAcct(" << acctId << ", " << balance << ")\n";


            std::cout << "Applied:\n";
            for (size_t idx: appliedTransactions)
                std::cout << idx << ", ";
            std::cout << std::endl;

            /*
            for (const TransactionEntry& transaction: unsettledTransaction)
            {
                for (const transfer& transfer: transaction.first)
                {
                    std::cout << "\tfrom: " << transfer.from<< ", to: "
                        << transfer.to << ", amount: " << transfer.amount << ") ID = " << transaction.second << "\n";
                }
            }
             */
        }
    };

    Database create_database(const std::vector<account_balance> &initial_balance) {
        return Database{initial_balance};
    }

    void Test()
    {
        const std::vector<account_balance> initialBalances {
            {1, 10},
            {2, 5}
        };

        const transaction& transaction {
                {1, 2, 5}
        };


        Database db = create_database(initialBalances);
        db.push_transaction(transaction);

        std::cout << db.get_balances() << std::endl;

        db.settle();

        std::cout << db.get_balances() << std::endl;

        db.stats();
    }
}

namespace TestAssignment2
{
    struct Result
    {
        int output1[100];
        int output2{};
        int output3{};

        Result() : output1() {
        }
    };

    struct account_balance
    {
        int account_id;
        int balance;
    };

    struct transfer
    {
        int from;
        int to;
        int amount;
    };


    std::ostream& operator<<(std::ostream& stream, const account_balance& acct)
    {
        stream << "Acct(" << acct.account_id << ", " << acct.balance << ")";
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const std::vector<account_balance>& balances)
    {
        std::cout << "Balances:\n";
        for (const auto& x: balances)
            stream << "\tAcct(" << x.account_id << ", " << x.balance << ")\n";
        return stream;
    }

    using transaction = std::vector<transfer>;

    struct Database
    {
        using TransactionEntry = std::pair<transaction, size_t>;

        std::unordered_map<int, int> accounts;
        std::vector<TransactionEntry> unsettledTransaction;
        std::vector<size_t> appliedTransactions;
        size_t transactionId { 0 };

        explicit Database(const std::vector<account_balance>& acctBalances)
        {
            accounts.reserve(acctBalances.size());
            for (const account_balance& accInfo: acctBalances) {
                accounts[accInfo.account_id] = accInfo.balance;
            }
        }

        // push a transaction to the database
        void push_transaction(const transaction& transaction)
        {
            const size_t transId = transactionId++;
            for (const transfer& transfer: transaction) {
                accounts[transfer.from] -= transfer.amount;
                accounts[transfer.to] += transfer.amount;
            }

            if (hasSucceeded()) {
                unsettledTransaction.clear();
                appliedTransactions.push_back(transId);
            }
            else {
                unsettledTransaction.emplace_back(transaction, transId);
            }
        }

        [[nodiscard]]
        bool hasSucceeded() const
        {
            return std::all_of(accounts.cbegin(), accounts.cend(), [](const auto& acct) {
                return acct.second >= 0;
            });
        }

        // settle the database such that invariant is maintained and best state of the database is present
        void settle()
        {
            for (const auto& transaction: unsettledTransaction){
                for (const auto &transfer: transaction.first) {
                    accounts[transfer.from] += transfer.amount;
                    accounts[transfer.to] -= transfer.amount;
                }
            }

            unsettledTransaction.clear();
        }

        // return a listing of all balances in any order
        [[nodiscard]]
        std::vector<account_balance> get_balances() const
        {
            std::vector<account_balance> balances;
            balances.reserve(accounts.size());

            for (const auto& [acctId, balance]: accounts)
                balances.push_back({acctId, balance});

            return balances;
        }

        // Given the logical sequence of transactions constructed by the ordered sequence of calls
        // to push_transaction AND having called settle()
        //
        // return the 0-based indices of the surviving transactions in that sequence which, when
        // applied to the initial balances produce a state where the invariant is maintained
        //

        [[nodiscard]]
        std::vector<size_t> get_applied_transactions() const
        {
            return appliedTransactions;
        }

        void stats()
        {
            std::cout << "Balances:\n";
            for (const auto& [acctId, balance]: accounts)
                std::cout << "\tAcct(" << acctId << ", " << balance << ")\n";


            std::cout << "Applied:\n";
            for (size_t idx: appliedTransactions)
                std::cout << idx << ", ";
            std::cout << std::endl;

            /*
            for (const TransactionEntry& transaction: unsettledTransaction)
            {
                for (const transfer& transfer: transaction.first)
                {
                    std::cout << "\tfrom: " << transfer.from<< ", to: "
                        << transfer.to << ", amount: " << transfer.amount << ") ID = " << transaction.second << "\n";
                }
            }
             */
        }
    };

    Database create_database(const std::vector<account_balance> &initial_balance) {
        return Database{initial_balance};
    }

    void Test()
    {
        const std::vector<account_balance> initialBalances {
                {1, 10},
                {2, 5}
        };

        const transaction& transaction {
                {1, 2, 3}
        };


        Database db = create_database(initialBalances);
        db.push_transaction(transaction);
        db.push_transaction(transaction);

        db.settle();
        db.stats();

        db.push_transaction(transaction);
        db.push_transaction(transaction);

        std::cout << "--------------\n";

        db.stats();

        db.settle();
        db.stats();
    }
}

namespace TestAssignment3
{
    struct Result
    {
        int output1[100];
        int output2{};
        int output3{};

        Result() : output1() {
        }
    };

    struct account_balance
    {
        int account_id;
        int balance;
    };

    struct transfer
    {
        int from;
        int to;
        int amount;
    };


    std::ostream& operator<<(std::ostream& stream, const account_balance& acct)
    {
        stream << "Acct(" << acct.account_id << ", " << acct.balance << ")";
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const std::vector<account_balance>& balances)
    {
        std::cout << "Balances:\n";
        for (const auto& x: balances)
            stream << "\tAcct(" << x.account_id << ", " << x.balance << ")\n";
        return stream;
    }

    using transaction = std::vector<transfer>;

    struct Database
    {
        using TransactionEntry = std::pair<transaction, size_t>;

        std::unordered_map<int, int> accounts;
        std::vector<TransactionEntry> unsettledTransaction;
        std::vector<size_t> appliedTransactions;
        size_t transactionId { 0 };

        explicit Database(const std::vector<account_balance>& acctBalances)
        {
            accounts.reserve(acctBalances.size());
            for (const account_balance& accInfo: acctBalances) {
                accounts[accInfo.account_id] = accInfo.balance;
            }
        }

        // push a transaction to the database
        void push_transaction(const transaction& transaction)
        {
            unsettledTransaction.emplace_back(transaction, transactionId++);
            for (const transfer& transfer: transaction)
            {
                accounts[transfer.from] -= transfer.amount;
                accounts[transfer.to] += transfer.amount;
            }
        }

        [[nodiscard]]
        bool hasOverdrawn() const
        {
            return std::any_of(accounts.cbegin(), accounts.cend(), [](const auto& acct) {
                return 0 > acct.second;
            });
        }

        // settle the database such that invariant is maintained and best state of the database is present
        void settle()
        {
            // We process all transactions in reverse order until we find the most recent one with a positive balance
            int idx = static_cast<int>(unsettledTransaction.size()) - 1;
            while (hasOverdrawn()) {
                const TransactionEntry &transaction = unsettledTransaction[idx--];
                for (const auto &transfer: transaction.first) {
                    accounts[transfer.from] += transfer.amount;
                    accounts[transfer.to] -= transfer.amount;
                }
            }

            unsettledTransaction.resize(idx + 1);
            for (const TransactionEntry& transactionEntry: unsettledTransaction)
                appliedTransactions.push_back(transactionEntry.second);

            unsettledTransaction.clear();
        }

        // return a listing of all balances in any order
        [[nodiscard]]
        std::vector<account_balance> get_balances() const
        {
            std::vector<account_balance> balances;
            balances.reserve(accounts.size());

            for (const auto& [acctId, balance]: accounts)
                balances.push_back({acctId, balance});

            return balances;
        }

        // Given the logical sequence of transactions constructed by the ordered sequence of calls
        // to push_transaction AND having called settle()
        //
        // return the 0-based indices of the surviving transactions in that sequence which, when
        // applied to the initial balances produce a state where the invariant is maintained
        //

        [[nodiscard]]
        std::vector<size_t> get_applied_transactions() const
        {
            return appliedTransactions;
        }

        void stats()
        {
            std::cout << "Balances:\n";
            for (const auto& [acctId, balance]: accounts)
                std::cout << "\tAcct(" << acctId << ", " << balance << ")\n";


            std::cout << "Applied:\n";
            for (size_t idx: appliedTransactions)
                std::cout << idx << ", ";
            std::cout << std::endl;

            /*
            for (const TransactionEntry& transaction: unsettledTransaction)
            {
                for (const transfer& transfer: transaction.first)
                {
                    std::cout << "\tfrom: " << transfer.from<< ", to: "
                        << transfer.to << ", amount: " << transfer.amount << ") ID = " << transaction.second << "\n";
                }
            }
             */
        }
    };

    Database create_database(const std::vector<account_balance> &initial_balance) {
        return Database{initial_balance};
    }

    void Test()
    {
        const std::vector<account_balance> initialBalances {
                {1, 10},
                {2, 5}
        };

        const transaction& transaction {
                {1, 2, 3}
        };


        Database db = create_database(initialBalances);
        db.push_transaction(transaction);
        db.push_transaction(transaction);

        db.settle();
        db.stats();

        db.push_transaction(transaction);
        db.push_transaction(transaction);

        std::cout << "--------------\n";

        db.stats();

        db.settle();
        db.stats();
    }
}



int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);
    // WrapperTests::Test();
    // StaticCounter::Test();

    // MoveExperiments::MoveStringToArray_Segfault();
    // MoveExperiments::test_perfect_forwarding();
    // MoveExperiments::test_overload();

    // Experiments::Test({20, 40, 60});
    // FindMinMaxValues::TestAll();


    // TestAssignment3::Test();

    std::vector<int> v;
    for (int i = 0; i < 100; ++i)
        v.push_back(i);

    v.clear();
    std::cout << v.capacity() << std::endl;











    /** * * * * *  Move to lib * * * * * **/
    // OperatorCall_ExplicitTypeSpecialization::Test();
    // ReturnTypeCast::tests();
    // CallFunctionByName::Test();
    // ReturnClass_MemberRef_CopyCTor::tests();
    // Algorithms::TestAll();
    // AutoTests::TestAll();
    // BinaryAnalyzer::TestAll();
    // Cpp23_Features::TestAll();
    // Concepts::TestAll();
    // Comparators::TestAll();
    // CollectionsTests::TestAll();
    // Coroutines::TestAll();
    // CopyElision_RVO::TestAll();
    // ConstexprMap::TestAll();
    // DebugLogger::TestAll();
    // DVector::TestAll();
    // DesignPatterns::TestAll();
    // Date_Time_Chrono::TestAll();
    // Heap::TestAll();
    // Iterators::TestAll();
    // Files::TestAll();
    // FunctionCall_LookUp::TestAll();
    // RateLimiter::TestAll();
    // LRUCache::TestAll();
    // Multithreading::TestAll();
    // Memory::TestAll();
    // MaxStack::TestAll();
    // MinStack::TestAll();
    // Math::TestAll();
    // UniquePtr_Size::SizeTest();
    // ExpressionTemplates::TestAll();
    // ObjectOrientedExperiments::RAIIWrapper::TestAll();
    // ObjectOrientedExperiments::OOP_Experiments::TestAll();
    // ObjectOrientedExperiments::VirtualTables::TestAll();
    // Optional::TestAll();
    // PointsAndLines::TestAll();           // Geometry
    // EventLoop::TestAll();
    // Iterators::TestAll();
    // Strings::TestAll();
    // Performance::TestAll();
    // Templates::TestAll();
    // ThinkCell::IntervalMapTest();
    // TableFormatter::TestAll();

    // Convertaion_UTF8_UTF32::TestAll();    // Encoding
    // Unicode::TestAll();                   // Encoding
    // StringUtils::TestAll();               // Encoding

    // InvokeTest::Test();

    // StaticInitObject a, b;

    // OrderBook::TestAll();
    // OrderBook2::TestAll();
    // MatchingOrderBook::TestAll();
    // MatchingOrderBookEx::TestAll();


    return EXIT_SUCCESS;
}
