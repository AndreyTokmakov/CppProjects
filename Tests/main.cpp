/**============================================================================
Name        : Tests.cpp
Created on  : 15.09.2021
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Tests C++ project
============================================================================**/


#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>


#include <array>
#include <vector>
#include <list>
#include <forward_list>
#include <deque>
#include <queue>
#include <map>
#include <span>
#include <stack>


#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <memory>


#include <concepts>
#include <version>
#include <expected>
#include <print>
#include <format>
#include <ostream>
#include <queue>
#include <stdfloat>

#include <experimental/socket>
#include <experimental/scope>

// #include <experimental/meta>
// #include <experimental/compiler>

//#include <experimental/contracts>
//#include <ind>


#include "BitFlags/BitFlags.h"
#include "BinManipulation/BinManipulation.h"
#include "Geometry/PointsAndLines.h"
#include "Concepts//Concepts.h"
#include "Encoding/Unicode.h"
#include "Encoding/Punycode.h"
#include "Encoding/StringUtils.h"
#include "Encoding/Convertaion_UTF8_UTF32.h"
#include "Templates/ExpressionTemplates.h"
#include "Templates/Templates.h"
#include "Logging/LowLatencyLogger.h"
#include "Crow/Crow.h"
#include "DebugLogger/DebugLogger.h"
#include "Collections/CollectionsTests.h"
#include "ObjectOrientedExperimetns/OOP_Experiments.h"
#include "ObjectOrientedExperimetns/VirtualTables.h"
#include "ConstexprMap/ConstexprMap.h"
#include "Multithreading/Multithreading.h"
#include "DesignPatterns/DesignPatterns.h"
#include "Files_Filesystem/Files.h"
#include "Helpers/Helpers.h"
#include "Strings/Strings.h"
#include "CopyElision_RVO/CopyElision_RVO.h"
#include "Performance/Performance.h"
#include "Memory/Memory.h"
#include "Memory/ObjectMemoryPool.h"
#include "BinaryAnalyzer/BinaryAnalyzer.h"
#include "TestAssignments/TestAssignments.h"
#include "TableFormatter/TableFormatter.h"
#include "Coroutines/Coroutines.h"
#include "Cpp_NEW_Features/Cpp_NEW_Features.h"
#include "Cpp_NEW_Features/Execution.h"
#include "Heap/Heap.h"
#include "FindMinMaxValues/FindMinMaxValues.h"
#include "FindMinMaxValues/FindMinMaxValues.h"
#include "FunctionCall_LookUp/FunctionCall_LookUp.h"

#define PRINT_LINE   std::cout.width(128); std::cout.fill('='); std::cout << '\n';

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


inline Helpers::Integer make() {
    return Helpers::Integer {};
}

template<typename T = int>
struct Path: std::vector<T> {
    T sum {};

    void push_back(T&& v) {
        std::vector<T>::push_back(v);
    }
};





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


namespace LockFreeQueue
{
    template<typename T>
    struct lock_free_queue
    {
        struct node
        {
            std::shared_ptr<T> data;
            std::atomic<node*> next;
            node() : next(nullptr) {}  //  initialize the node
        };

        std::atomic<node*> head;
        std::atomic<node*> tail;

        void enqueue(T value)
        {
            std::shared_ptr<T> new_data = std::make_shared<T>(value);
            node* new_node = new node();
            new_node->data = new_data;

            //  do an infinite loop to change the tail
            while (true)
            {
                node* current_tail = this->tail.load(std::memory_order_acquire);
                node* tail_next = current_tail->next;

                //  everything is correct so far, attempt the swap
                if (current_tail->next.compare_exchange_strong( tail_next, new_node, std::memory_order_release)) {
                    this->tail = new_node;
                    break;
                }
            }
        }

        std::shared_ptr<T> dequeue()
        {
            std::shared_ptr<T> return_value = nullptr;

            //  do an infinite loop the change the head
            while (true)
            {
                node* current_head = this->head.load(std::memory_order_acquire);
                node* next_node = current_head->next;

                if (this->head.compare_exchange_strong(current_head, next_node, std::memory_order_release)) {
                    return_value.swap(next_node->data);
                    delete current_head;
                    break;
                }
            }
            return return_value;
        }
    };

    void Test()
    {
        lock_free_queue<int> queue;

        queue.enqueue(1);
    }

}


namespace Maga_Super_IF_Else_Switch_Hack
{
    using namespace std::string_view_literals;

    void processProd(std::string_view) {}
    void processRC(std::string_view) {}
    void processBeta(std::string_view) {}

    void processThisString1(std::string_view input)
    {
        if (input == "production"sv) {
            processProd(input);
        } else if (input == "RC"sv) {
            processRC(input);
        } else if (input == "beta"sv)
            processBeta(input);
    }

    void processThisString2(std::string_view input)
    {
        constexpr uint32_t i { 0 };
        switch (input[i]) {
            case "production"[i]: processProd(input); break;
            case "RC"[i]: processRC(input); break;
            case "beta"[i]: processBeta(input); break;
        }
    }
}

namespace PermissionsTest
{
    enum class Permission : uint8_t {
        Read = 1,
        Write,
        Execute,
    };

    template<typename T>
    requires(std::is_enum_v<T>and requires(T e) {
        // look for enable_bitmask_operator_or to enable this operator
        enable_bitmask_operator_or(e);
    })
    constexpr auto operator|(const T lhs, const T rhs)
    {
        using underlying = std::underlying_type_t<T>;
        return static_cast<T>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    }

    void enable_bitmask_operator_or(Permission permission)
    {

    }

    void Test()
    {
        Permission mask = Permission::Read | Permission::Write;
        enable_bitmask_operator_or(mask);
    }
}


namespace UBBook
{
    struct User
    {
        std::string name;
        std::vector<int> tokens;
    };

    User get_user()
    {
        return {
                "Hello world",
                {1,2,3,4,5}
        };
    }

    void Test()
    {
        std::string&& name = get_user().name;
        auto& v = *(std::vector<int>*)( (char*)(&name) + sizeof(std::string) );
        for (int x : v) {
            std::cout << x;
        }
    }
}


namespace Move_Copy_Tests
{
    using Integer = Helpers::Integer;

    void passByConstRef(const Integer& integer)
    {
        std::cout << integer.value << std::endl;
    }

    void passByRRef(Integer&& integer)
    {
        std::cout << integer.value << std::endl;
    }


    void passByValue(Integer integer)
    {
        std::cout << integer.value << std::endl;
    }


    void test()
    {
        {
            passByConstRef(Integer{1});
        }
        {
            passByRRef(Integer{2});
        }
        {
            passByValue(Integer{3});
        }
    }
}





namespace StringTest_SSO
{
    template<typename T>
    struct BasicString
    {
        using data_type = T;
        using pointer = data_type*;
        using size_type = size_t*;

        struct sized_data {
            pointer ptr ;
            size_type length;
            size_type capacity;
        };

        union {
            sized_data onHeap;
            data_type onStack[sizeof(sized_data)];
        } storage;
    };

    class String
    {
        size_t capacity;

        struct heapbuf
        {
            char *ptr;
            size_t size;
        };

        union
        {
            heapbuf heap;
            char stack[sizeof(heapbuf)];
        };
    };


    void Tests()
    {
        BasicString<char> str {};
        String str2 {};

        std::cout << sizeof(str) << std::endl;
        std::cout << sizeof(str2) << std::endl;
    }
}

namespace PipelineOperator
{
    std::string FuncOne(std::string &&s) {
        s += " [ONE] ";
        std::cout << "FuncOne  : " << s << std::endl;
        return s;
    }

    std::string FuncTwo(std::string &&s) {
        s += " [TWO] ";
        std::cout << "FuncTwo  : " << s << std::endl;
        return s;
    }

    std::string FuncThree(std::string &&s) {
        s += " [THREE] ";
        std::cout << "FuncThree: " << s << std::endl;
        return s;
    }

    template <typename T, typename Function>
        requires (std::invocable<Function, T>)
    constexpr std::invoke_result_t<Function, T> operator | (T &&t, Function &&f) {
        return std::invoke(std::forward<Function>(f), std::forward<T>(t));
    }

    /*
    template <typename T>
    concept is_expected = requires(T t) {
        typename T::value_type;
        typename T::error_type;
        requires std::is_constructible_v<bool, T>;
        requires std::same_as<std::remove_cvref<decltype(*t)>, typename T::value_type>;
        requires std::constructible_from<T, std::unexpected<typename T::error_type>>;
    };

    using namespace std;
    template <typename T, typename E, typename Function>
    requires invocable<Function, T> &&
             is_expected<typename std::invoke_result_t<Function, T>>
    constexpr std::invoke_result_t<Function, T> operator | (std::expected<T, E> &&ex, Function &&f)
    {
        return ex ?
               invoke(forward<Function>(f), *forward<expected<T, E>>(ex)) :
        ex;
    }
    */

    void SimplePipeTest()
    {
        std::string start_str("Start string ");
        std::string&& result = std::move(start_str) | FuncOne | FuncTwo | FuncThree;

        std::cout << "\nResult: " << result << std::endl;
    }
}

namespace Designated_Initialization
{
    struct Data
    {
        int a;
        int b;
        int c;
    };

    void Test()
    {
        [[maybe_unused]]
        Data data {.a = 10, .c = 13};
    }
}


namespace LookUpTests
{
    namespace Utils
    {
        std::string foo() {
            return {"A::foo()"};
        }

        template<class>
        struct Hash;

        template<class>
        struct HashEx;
    }

    std::string foo() {
        return {"foo() | free function"};
    }

    template<>
    struct Utils::Hash<int>
    {
        std::string callFoo() {
            return foo();
        }
    };

    template<>
    struct Utils::HashEx<int>
    {
        std::string callFoo() {
            return LookUpTests::foo();
        }
    };

    void Unexpected_Method_Call_Resolution()
    {
        std::cout << Utils::Hash<int>{}.callFoo()   << std::endl; // ===>   A::foo()
        std::cout << Utils::HashEx<int>{}.callFoo() << std::endl; // ===>   foo() | free function
    }


    namespace my
    {
        struct Book
        {
            struct Hash
            {
                size_t operator()(const Book& b) const {
                    return 1;
                }
            };
        };

        /*
        template<>
        struct std::hash<Book>: my::Book::Hash {

        };
        */
    }


    namespace A { int x = 0; }
    namespace B { int x = 0; }


};

namespace Types_Experiments
{
    struct TypeExplorer
    {
        void operator()(this auto&& self)
        {
            using SelfType = decltype(self);
            using UnrefSelfType = std::remove_reference_t<SelfType>;
            if constexpr (std::is_lvalue_reference_v<SelfType>)
            {
                if constexpr (std::is_const_v<UnrefSelfType>)
                    std::cout << "const lvalue\n";
                else
                    std::cout << "mutable lvalue\n";
            }
            else
            {
                if constexpr (std::is_const_v<UnrefSelfType>)
                    std::cout << "const rvalue\n";
                else
                    std::cout << "mutable rvalue\n";
            }
        }
    };



    struct TypeIdCnt
    {
        template<typename>
        static uint32_t GetUniqueId()
        {
            static const uint32_t TypeId = NewTypeId();
            return TypeId;
        }

    private:

        static uint32_t NewTypeId()
        {
            static std::atomic<uint32_t> CurrentId = 0;
            return CurrentId++;
        }
    };

    template<typename T>
    static uint32_t GetTypeId()
    {
        return TypeIdCnt::GetUniqueId<T>();
    }

    struct A {};
    struct B {};
    struct C: A {};

    void Test_Get_TypeID()
    {
        std::cout << "int   = " << GetTypeId<int>() << std::endl;
        std::cout << "short = " << GetTypeId<short>() << std::endl;
        std::cout << "char  = " << GetTypeId<char>() << std::endl;
        std::cout << "int   = " << GetTypeId<int>() << std::endl;
        std::cout << "A     = " << GetTypeId<A>() << std::endl;
        std::cout << "B     = " << GetTypeId<B>() << std::endl;
        std::cout << "C     = " << GetTypeId<C>() << std::endl;
        std::cout << "A     = " << GetTypeId<A>() << std::endl;
    }
};


#if 0
/** Reflections **/
// #include <experimental/meta>

namespace Reflections
{
    enum class Color {
        Red,
        Green,
        Blue
    };

    template<typename E> requires std::is_enum_v<E>
    constexpr std::string enum_to_string_1(E value)
    {
        std::string result = "<unnamed>";

        [:expand(std::meta::enumerators_of(^E)):] >> [&]<auto e>{
            if (value == [:e:]) {
                result = std::meta::identifier_of(e);
            }
        };
        return result;
    }

    template <typename E>
    requires std::is_enum_v<E>
    constexpr std::optional<E> string_to_enum_2(std::string_view name)
    {
        template for (constexpr auto e : std::meta::enumerators_of(^E)) {
            if (name == std::meta::identifier_of(e)) {
                return [:e:];
            }
        }
        return std::nullopt;
    }

    void EnumToString()
    {
        std::cout << "enum_to_string(Color::red): " << enum_to_string_1(Color::Red) << '\n';
        std::cout << "enum_to_string(Color::red): " << enum_to_string_2(Color::Red) << '\n';
    }
}

#endif


namespace ADL_LookUp::Function_Templates
{
    template<typename T>
    void funk(T v)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void funk(int v)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }


    void funk(int* v)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void Template_vs_Int_Parameter()
    {
        int v { 123 };
        funk(v);
        funk(&v);

        funk("");
    }
}

namespace ADL_LookUp::Implicit_Bool_From_SV
{
    using namespace std::string_view_literals;

    auto stringify(bool b) -> std::string_view {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return b ? "true" : "false";
    }

    std::string_view stringify(std::string_view s)  {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return s;
    }

    void From_StringView()
    {
        std::cout << stringify(true) << "\n";

        // const char[7] --> const char* --> к bool
        // All built-in conversions have higher priority ove user-defined conversion to std::string_view via its constructor.
        std::cout << stringify("string") << "\n";

        std::cout << stringify("string"sv) << "\n";
    }
}

namespace BitwiseOperations
{
    enum class Weekdays
    {
        Monday = 0x01,
        Tuesday = 0x02,
        Wednesday = 0x03,
        Thursday = 0x08,
        Friday = 0x19,
        Saturday = 0x20,
        Sunday = 0x40
    };

    bool isWorkday(Weekdays day)
    {
        if (static_cast<int>(day) & (static_cast<int>(Weekdays::Monday) |
                                     static_cast<int>(Weekdays::Tuesday) |
                                     static_cast<int>(Weekdays::Wednesday) |
                                     static_cast<int>(Weekdays::Thursday) |
                                     static_cast<int>(Weekdays::Friday)))
        {
            return true;
        }
        return false;
    }

    void test()
    {
        for (Weekdays day: { Weekdays::Monday, Weekdays::Tuesday, Weekdays::Wednesday, Weekdays::Thursday,
                             Weekdays::Friday, Weekdays::Saturday, Weekdays::Sunday })
        {
            std::cout << "isWorkday: " << std::boolalpha << isWorkday(day) << std::endl;
        }
    }
}


namespace LockFreeQueueTest
{
    template<typename T>
    class LockFreeStackPush
    {
        struct Node
        {
            T data;
            Node* next;
            explicit Node(T d): data(d), next(nullptr) {}
        };

        std::atomic<Node*> head;

    public:

        LockFreeStackPush() = default;
        LockFreeStackPush(const LockFreeStackPush&) = delete;
        LockFreeStackPush& operator= (const LockFreeStackPush&) = delete;

        void push(T val)
        {
            Node* const newNode = new Node(val);
            newNode->next = head.load();
            while( !head.compare_exchange_strong(newNode->next, newNode) );
        }
    };

    void Test()
    {
        LockFreeStackPush<int> stack;
    }
}

namespace VirtualFunctionTests
{
    struct Base
    {
        virtual void info()
        {
            std::cout << "Base::Base::info" << "\n";
        }

        virtual ~Base() = default;
    };

    struct Derived: Base
    {
        void info() override
        {
            std::cout << "Base::Base::info" << "\n";
        }
    };

    void demo()
    {
        Base* b = new Base();
        Derived* d = new Derived();
        Derived* d1 = new Derived();
        Base* bd = d;

        // using lPtr = uint64_t*;

        [[maybe_unused]] const uint64_t *** mVtableB = reinterpret_cast<const uint64_t***>(&b);
        [[maybe_unused]] const uint64_t *** mVtableD = reinterpret_cast<const uint64_t***>(&d);
        [[maybe_unused]] const uint64_t *** mVtableD1 = reinterpret_cast<const uint64_t***>(&d1);
        [[maybe_unused]] const uint64_t *** mVtableBD = reinterpret_cast<const uint64_t***>(&bd);

        // std::cout << **mVtableB << std::endl;
        // std::cout << **mVtableD << std::endl;
        // std::cout << **mVtableD1 << std::endl;
        // std::cout << **mVtableBD << std::endl;

        std::cout << (void*) mVtableD[0][0][0] << std::endl;
        std::cout << (void*) mVtableD[0][0][1] << std::endl;
        std::cout << (void*) mVtableD[0][0][2] << std::endl;
        std::cout << (void*) mVtableD[0][0][3] << std::endl;
        std::cout << (void*) d << std::endl;
    }
}

void variableArgFunc([[maybe_unused]] std::initializer_list<int> args) {}
class VariableArgCtor {
public:
    VariableArgCtor(std::initializer_list<int> args) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    VariableArgCtor(int a, int b, int c) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};

namespace Int_to_UInt_Tests
{

    constexpr auto add_unsigned_numbers_auto(uint8_t a, uint8_t b) {
        return a + b;
    }

    constexpr uint8_t add_unsigned_numbers_explicit(uint8_t a, uint8_t b) {
        return a + b;
    }


    void Tests()
    {
        static_assert(256 == add_unsigned_numbers_auto(255, 1));
        static_assert(0 == add_unsigned_numbers_explicit(255, 1));
    }
}


namespace EnumBasedStrongTypes
{
    enum class Integer : int32_t {
    };

    void call(Integer val) {
        std::cout << static_cast<std::underlying_type_t<Integer>>(val) << std::endl;

    }

    void Tests()
    {
        Integer value = Integer { 10 };
        call(value);
        // call(123);
    }
}






namespace OrderBook_TableDispatch
{
    using Ticker = std::string;

    enum class Exchange
    {
        Binance,
        ByBit,
        Deribit,
        GateIO,
        OKX,
    };

    struct Event
    {
        // TODO: Ticker ??? INT ?? how to optimize
        Ticker ticker;
    };

    std::ostream& operator<<(std::ostream& stream, const Event& event) {
        stream << std::format("Event({})", event.ticker);
        return stream;
    }

    struct OrderBook
    {
        Ticker ticker;

        void processEvent(const Event& event) const
        {
            std::cout << "OrderBook '" << ticker << "' handling the event" << event << std::endl;
        }
    };

    struct BookKeeper
    {

        std::map<Ticker, std::unique_ptr<OrderBook>> orderBooksByTicker;
        // std::vector<std::unique_ptr<OrderBook>> orderBooksByTicker;

        BookKeeper()
        {
            for (const auto& ticker: { "APPL", "TEST"}){
                orderBooksByTicker.emplace(ticker, std::make_unique<OrderBook>(ticker));
            }
        }

        void processEvent(const Event& event)
        {
            // TODO: Get TICKER here ??
            const auto& book = orderBooksByTicker[event.ticker];
            book->processEvent(event);
        }
    };

    struct PricingEngine
    {
        BookKeeper binanceBookKeeper;
        BookKeeper byBitBookKeeper;
        std::array<BookKeeper*, 2> books { &binanceBookKeeper, &byBitBookKeeper };

        void push(Exchange exchange, const Event& event) const
        {
            BookKeeper* book_keeper = books[static_cast<uint32_t>(exchange)];
            book_keeper->processEvent(event);
        }
    };

    void Tests()
    {
        PricingEngine engine;
        engine.push(Exchange::Binance, Event { "APPL"});
        engine.push(Exchange::Binance, Event { "TEST"});
    }
}

namespace Store_Pointer_In_Collection
{
    using Helpers::Integer;
    std::vector<Integer*> intPtrs;

    struct Wrapper {
        Integer* ptr;
    };

    void init()
    {
        intPtrs.push_back(new Integer(1));
        intPtrs.push_back(new Integer(2));
        intPtrs.push_back(new Integer(3));
    }

    void clean()
    {
        for (Integer* ptr: intPtrs)
            delete ptr;
    }

    void set(Wrapper& data, uint32_t pos)
    {
        data.ptr = intPtrs[pos];
    }

    void demo()
    {
        init();
        Wrapper wrapper{};

        set(wrapper, 2);

        std::cout << *wrapper.ptr << std::endl;

        clean();
    }
}


namespace OOP_FoldExpr_Inheritance
{
    struct IWritable
    {
        virtual void write(const std::string& data) = 0;
        virtual ~IWritable() = default;
    };

    struct IReadable
    {
        virtual std::string read() = 0;
        virtual ~IReadable() = default;
    };

    template<typename ... Types>
    struct Base: public Types ...
    {
        virtual ~Base() = default;

    protected:

        std::string resource;
    };

    struct Json: public Base<IReadable, IWritable>
    {
        void write(const std::string& data) override {
            resource = R"({"data":")" + data + "\"}";
        }

        std::string read() override {
            return resource;
        }
    };

    struct Xml: public Base<IReadable, IWritable>
    {
        void write(const std::string& data) override {
            resource = "<data>" + data + "</data>";
        }

        std::string read() override {
            return resource;
        }
    };

    void demo()
    {
        Json json;
        Xml xml;

        std::vector<IWritable*> toWrite { &json, &xml };
        std::vector<IReadable*> toRead { &json, &xml };

        for (IWritable* wr: toWrite) {
            wr->write("Hello world");
        }
        for (IReadable* wr: toRead) {
            std::cout << wr->read() << std::endl;
        }
    }
}


namespace TTT
{
    class  Obj
    {
        int* a;
        int* b;

        Obj() {
            std::cout << "Obj::Obj()\n";
        }

    public:

        Obj(int* aPtr, int* bPtr): a (aPtr), b (bPtr) {
            std::cout << "Obj::Obj()\n";
        }

        ~Obj() {
            std::cout << "Obj::~Obj("<< *a << ", " << *b <<")\n";
        }
    };

    void demo()
    {
        Obj obj { new int(10), new int(20) };
    }
}


namespace ExcDtor
{
    struct BadObject
    {
        ~BadObject() noexcept(false)
        {
            throw std::runtime_error("BadObject");
        }
    };

    void demo()
    {
        try
        {
            BadObject bad;
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }
}


namespace ASM_Usage
{
    int64_t int32_to_64_garbage(int32_t i32)
    {
        int64_t i64;
        __asm__("" :        // do nothing
                "=r"(i64) : // produces result in register
                "0"(i32));  // from this input
        return i64;
    }

    /// Count the number of CPU cycles that passed during this period.
    uint64_t get_tsc()
    {
        uint32_t l;
        uint64_t h;
        __asm__("rdtsc" : "=a"(l), "=d"(h));
        return l | (h << 32);
    }


    void measureElapsedTime()
    {
        const auto start = get_tsc();

        std::this_thread::sleep_for(std::chrono::seconds(1U));

        const auto end = get_tsc();
        std::cout << end - start << std::endl;
    }
}


namespace FibonacciSequence_Lambda
{
    void generate()
    {
        auto gen = [current = 0, next = 1]() mutable {
            return current = std::exchange(next, current + next);
        };

        std::vector<int> fib(10);
        std::generate(fib.begin(), fib.end(), gen);
        for (int i : fib) {
            std::cout << i << " ";
        }
        std::cout << std::endl;

        // 1 1 2 3 5 8 13 21 34 55
    }
}


namespace resource_registry
{
    using TypeID = uint32_t;

    [[nodiscard]]
    constexpr TypeID generateUniqueTypeId()
    {
        static std::atomic<TypeID> currentId { 1 };
        return currentId++;
    }

    template<typename Ty>
    struct TypeId
    {
        [[nodiscard]]
        TypeID getTypeId() const noexcept
        {
            static const TypeID id = generateUniqueTypeId();
            return id;
        }
    };

    struct Resource
    {
        virtual ~Resource() = default;

    private:

        virtual std::unique_ptr<Resource> create() const noexcept = 0;
    };


    struct Board: TypeId<Board> {};
    struct Sensor: TypeId<Sensor> {};

    template<typename Ty>
    concept HasTypeIdGetter = requires(const Ty& obj) {
        { obj.getTypeId() } noexcept -> std::convertible_to<TypeID>;
    };

    template<HasTypeIdGetter Ty>
    void check(const Ty& obj)
    {
        std::cout << "Id: " <<  obj.getTypeId() << std::endl;
    }

    void test()
    {
        Board b1, b2;
        Sensor s1, s2;

        check(b1);
        check(b2);
        check(s1);
        check(s2);

    }
}

namespace array_to_store_crtp_objects
{
    struct ParserOne
    {
        static void parse() {
            std::cout << "ParserOne::parse()" << std::endl;
        }
    };

    struct ParserTwo
    {
        static void parse() {
            std::cout << "ParserTwo::parse()" << std::endl;
        }
    };

    // TODO: Concepts
    template<typename Derived>
    struct Interface
    {
        [[nodiscard]]
        const Derived &self() const noexcept {
            return *static_cast<const Derived *const>(this);
        }

        void doSomething() const
        {
            self().parse();
        }
    };

    struct WrapperOne: Interface<WrapperOne>, ParserOne { };
    struct WrapperTwo: Interface<WrapperTwo>, ParserTwo { };

    void test()
    {
        constexpr WrapperOne wrapperOne;
        wrapperOne.doSomething();

        constexpr WrapperTwo wrapperTwo;
        wrapperTwo.doSomething();
    }
}

namespace array_to_store_deducing_this_objects
{
    struct ParserOne
    {
        static void parse() {
            std::cout << "ParserOne::parse()" << std::endl;
        }
    };

    struct ParserTwo
    {
        static void parse() {
            std::cout << "ParserTwo::parse()" << std::endl;
        }
    };

    struct Interface
    {
        void doSomething(this auto& self) {
            self.parse();
        }
    };

    struct WrapperOne: Interface, ParserOne { };
    struct WrapperTwo: Interface, ParserTwo { };

    void test()
    {
        constexpr WrapperOne wrapperOne;
        wrapperOne.doSomething();

        constexpr WrapperTwo wrapperTwo;
        wrapperTwo.doSomething();
    }
}




int main([[maybe_unused]] const int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);


    // resource_registry::test();
    // array_to_store_crtp_objects::test();
    // array_to_store_deducing_this_objects::test();
    // Cpp_NEW_Features::TestAll();
    // Execution::TestAll();
    // Coroutines::TestAll();
    // ASM_Usage::measureElapsedTime();
    // FibonacciSequence_Lambda::generate();
    // VirtualFunctionTests::demo();
    // Int_to_UInt_Tests::Tests();
    // EnumBasedStrongTypes::Tests();
    // OrderBook_TableDispatch::Tests();
    // Store_Pointer_In_Collection::demo();
    // OOP_FoldExpr_Inheritance::demo();
    // WrapperTests::Test();
    // StaticCounter::Test();
    // LookUpTests::Unexpected_Method_Call_Resolution();
    // Types_Experiments::Test_Get_TypeID();
    // ADL_LookUp::Function_Templates::Template_vs_Int_Parameter();
    // ADL_LookUp::Implicit_Bool_From_SV::From_StringView();
    // MoveExperiments::MoveStringToArray_Segfault();
    // MoveExperiments::test_perfect_forwarding();
    // MoveExperiments::test_overload();
    // FindMinMaxValues::TestAll();
    // LockFreeQueue::Test();
    // UBBook::Test();
    // PipelineOperator::SimplePipeTest();
    // StringTest_SSO::Tests();
    // BitwiseOperations::test();
    // BinaryAnalyzer::TestAll();
    // BitFlags::TestAll();
    // BinManipulation::TestAll();
    // Concepts::TestAll();
    // LockFreeQueueTest::Test();
    // CompileTime_Programming::Factorial();
    // OperatorCall_ExplicitTypeSpecialization::Test();
    // CallFunctionByName::Test();
    // ReturnClass_MemberRef_CopyCTor::tests();
    // Crow::TestAll();
    // CollectionsTests::TestAll();
    // CopyElision_RVO::TestAll();
    // ConstexprMap::TestAll();
    // DebugLogger::TestAll();
    // DesignPatterns::TestAll();
    // Heap::TestAll();
    // Files::TestAll();
    // FunctionCall_LookUp::TestAll();
    // LowLatencyLogger::TestAll();
    // Multithreading::TestAll();
    // Memory::TestAll();
    // MaxStack::TestAll();
    // UniquePtr_Size::SizeTest();
    // ObjectOrientedExperiments::OOP_Experiments::TestAll();
    // ObjectOrientedExperiments::VirtualTables::TestAll();
    // PointsAndLines::TestAll();           // Geometry
    // Strings::TestAll();
    // Performance::TestAll();
    // Templates::TestAll();
    // ExpressionTemplates::TestAll();
    // TestAssignments::TestAll();
    // TableFormatter::TestAll();
    // Convertaion_UTF8_UTF32::TestAll();    // Encoding
    // Unicode::TestAll();                   // Encoding
    // StringUtils::TestAll();               // Encoding
    // OrderBook::TestAll();
    // OrderBook2::TestAll();
    // MatchingOrderBook::TestAll();
    // MatchingOrderBookEx::TestAll();

    return EXIT_SUCCESS;
}
