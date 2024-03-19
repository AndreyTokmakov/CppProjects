/**============================================================================
Name        : CollectionsTests.h
Created on  : 11.06.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : CollectionsTests
============================================================================**/

#include "CollectionsTests.h"

#include <iostream>
#include <utility>
#include <numeric>

#include <list>
#include <vector>
#include <array>

#include <memory>
#include <chrono>
#include <random>
#include <unordered_set>
#include <unordered_map>

#include "../Helpers/Wrapper.h"

namespace CollectionsTests::Utils
{
    std::string randomString(size_t size = 16)
    {
        std::random_device rd{};
        std::mt19937 generator = std::mt19937 {rd()};
        auto ud = std::uniform_int_distribution<> {(int)'a', (int)'z'};

        std::string str;
        str.reserve(size);
        while (size-- > 0)
            str.push_back(static_cast<char>(ud(generator)));
        return str;
    }
}

namespace CollectionsTests::CustomArrayTest {

    template<typename T = int>
    class Array {
    private:
        using value_type = T;
        using size_type  = size_t;

        static_assert(!std::is_same_v<value_type, void>,
                      "Type of the value shall not be void");

        value_type* data { nullptr };
        size_type size { 0 };

    public:
        explicit Array(size_type s): data {new value_type[s]}, size {s} {
        }

        value_type& operator[](size_type idx) {
            return data[idx];
        }

        const value_type& operator[](size_type idx) const {
            return data[idx];
        }

        inline value_type* Data() noexcept {
            return data;
        }

        [[nodiscard]]
        inline size_type Size() const noexcept {
            return size;
        }

        Array(const Array& other):
                data { new value_type[other.size] }, size { other.size } {
            std::copy_n(other.data, size, data);
        }

        Array(Array&& other) noexcept:
                data { std::exchange(other.data, nullptr) },
                size { std::exchange(other.size, 0) } {
        }

        Array& operator=(const Array& other) {
            Array localCopy(other);
            // swap(localCopy);
            Array::swap(localCopy, *this);
            return *this;
        }

        Array& operator=(Array&& other) noexcept {
            if (this != &other) {
                delete[] data;
                data = std::exchange(other.data, nullptr);
                size = std::exchange(other.size, 0);
            }
            return *this;
        }

        /*
        [[nodiscard]]
        inline T begin() const noexcept {
            return data[0];
        }

        [[nodiscard]]
        inline T end() const noexcept {
            return data[size - 1];
        }
        */

        void swap(Array& other) noexcept {
            std::swap(this->data, other.data);
            std::swap(this->size, other.size);
        }

        static void swap(Array& first, Array& second) noexcept {
            std::swap(first.data, second.data);
            std::swap(first.size, second.size);
        }

        ~Array() {
            std::cout << "Array::~Array()\n";
            delete[] data;
            data = nullptr;
            size = 0;
        }
    };


    template <typename T, typename Coll>
    class IteratorBase {
    private:
        using value_type = T;
        using size_type  = size_t;
        using collection_type  = Coll;

        size_type index {0};
        collection_type& collection;

    public:
        IteratorBase(collection_type& collection, const size_type index) :
                index { index }, collection { collection } {
        }

        inline bool operator!=(const IteratorBase& other) const noexcept {
            return index != other.index;
        }

        inline const value_type& operator*() const noexcept {
            return collection[index];
        }

        const IteratorBase& operator++() noexcept {
            ++index;
            return *this;
        }
    };

    template <typename T>
    using ArrayIterator = IteratorBase<T, Array<T>>;

    template <typename T>
    using ArrayConstIterator = IteratorBase<T, const Array<T>>;


    template <typename T>
    inline ArrayIterator<T> begin(Array<T>& collection) {
        return ArrayIterator<T> (collection, 0);
    }

    template <typename T>
    inline ArrayIterator<T> end(Array<T>& collection) {
        return ArrayIterator<T> (collection, collection.Size());
    }


    void Test() {
        Array<uint32_t> numbers {3};

        numbers[2] = 4;

        for (uint32_t i = 0; i < numbers.Size(); ++i) {
            std::cout << numbers[i] << std::endl;
        }

        std::cout << "--------------------------------------------\n";

        Array<uint32_t> numbers1 (numbers);
        for (uint32_t i = 0; i < numbers1.Size(); ++i) {
            std::cout << numbers1[i] << std::endl;
        }

        std::cout << "--------------------------------------------\n";

        Array<uint32_t> numbers2(3);
        numbers2 = numbers1;
        for (uint32_t i = 0; i < numbers2.Size(); ++i) {
            std::cout << numbers2[i] << std::endl;
        }

        std::cout << "--------------------------------------------\n";

        numbers2 = numbers2;
        for (uint32_t i = 0; i < numbers2.Size(); ++i) {
            std::cout << numbers2[i] << std::endl;
        }
    }

    void MoveConstructor_Test() {
        Array<int> numbers {5};

        std::iota(numbers.Data(), numbers.Data() + numbers.Size(), 1);
        for (size_t i = 0; i < numbers.Size(); ++i)
            std::cout << numbers[i] << std::endl;

        std::cout << "--------------------------------------------\n";

        Array<int> numbers2 = std::move(numbers);
        for (size_t i = 0; i < numbers2.Size(); ++i)
            std::cout << numbers2[i] << std::endl;

        std::cout << "--------------------------------------------\n";

        std::cout << numbers.Size() << " " << numbers2.Size() << std::endl;
    }

    void MoveAssignment_Test() {
        Array<int> numbers1 {5}, numbers2 {5};

        std::iota(numbers1.Data(), numbers1.Data() + numbers1.Size(), 1);
        std::iota(numbers2.Data(), numbers2.Data() + numbers2.Size(), 5);

        auto printArrays = [](const Array<int>& array1, const Array<int>& array2) {
            const auto maxSize = std::max(array1.Size(), array2.Size());
            for (size_t i = 0; i < maxSize; ++i) {
                if (array1.Size() > i) std::cout << array1[i];
                std::cout << "     ";
                if (array2.Size() > i) std::cout << array2[i];
                std::cout << std::endl;
            }
        };

        printArrays(numbers1, numbers2);

        std::cout << "---------------- move test ----------------------------\n";

        numbers1 = std::move(numbers2);
        printArrays(numbers1, numbers2);

        std::cout << "--------------- self assignment -----------------------------\n";

        numbers1 = std::move(numbers1);
        printArrays(numbers1, numbers2);

        std::cout << "--------------------------------------------\n";
    }

    void IteratorsTest() {
        Array<int> numbers {10};
        std::iota(numbers.Data(), numbers.Data() + numbers.Size(), 1);

        for (auto && e : numbers) {
            std::cout << e << std::endl;
        }
    }
}


namespace CollectionsTests::MoveConstructor_Noexcept_Vector
{
    struct Widget {
        std::list<int> data_;
        explicit Widget(int n) : data_(n) {}
    };

    struct Gadget {
        std::list<int> data_;
        explicit Gadget(int n) : data_(n) {}

        Gadget(Gadget&&) noexcept = default;
        Gadget(const Gadget&) = default;
        Gadget& operator=(Gadget&&) = default;
        Gadget& operator=(const Gadget&) = default;
    };

    template<class T>
    void time_it(const char *message)
    {
        std::vector<T> v(10000, T(1000));
        // assert(v.capacity() == 10000);
        auto start = std::chrono::system_clock::now();

        v.push_back(T(0));

        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - start
        );
        printf("%s: %zums\n", message, size_t(elapsed_ms.count()));
    }

    void test()
    {
        time_it<Widget>("reallocation with Widgets");
        time_it<Gadget>("reallocation with Gadgets");
    }
}

namespace CollectionsTests::Arrays
{
    template<typename T, size_t Size>
    void printArray(const std::array<T, Size>& data)
    {
        for (const auto& v: data)
            std::cout << v << ' ';
        std::cout << std::endl;
    }

    template<typename T>
    void printArray2(const T& data)
    {
        for (const auto& v: data)
            std::cout << v << ' ';
        std::cout << std::endl;
    }

    void PrintArrayTest()
    {
        std::array<int, 10> numbers {};
        std::iota(numbers.begin(), numbers.end(), 1);

        printArray(numbers);
        printArray2(numbers);
    }
}

namespace CollectionsTests::Trie
{
    class Trie
    {
        struct TrieNode {
            std::array<std::unique_ptr<TrieNode>, 26> children{};
            bool isEndOfWord{false};
        };

        std::unique_ptr<TrieNode> root{};


    public:
        Trie() : root{std::make_unique<TrieNode>()} {
        }

        void insert(std::string_view word)
        {
            TrieNode *currNode = root.get();
            for (const char c: word)
            {
                const int index = c - 'a';
                if (nullptr == currNode->children[index]) {
                    currNode->children[index] = std::make_unique<TrieNode>();
                }
                currNode = currNode->children[index].get();
            }
            currNode->isEndOfWord = true;
        }

        [[nodiscard]]
        bool search(const std::string& word) const
        {
            TrieNode *currNode = root.get();
            for (const char c: word)
            {
                const int index = c - 'a';
                if (nullptr == currNode->children[index])
                    return false;
                currNode = currNode->children[index].get();
            }
            return currNode->isEndOfWord;
        }

        /*
        [[nodiscard]]
        bool search(std::string_view word) const
        {
            TrieNode *currNode = root.get();
            for (const char c: word)
            {
                const int index = c - 'a';
                if (nullptr == currNode->children[index])
                    return false;
                currNode = currNode->children[index].get();
            }
            return currNode->isEndOfWord;
        }
         */
    };

    void test()
    {
        Trie t;

        const std::string text { "12345" };

        std::cout << t.search(text) << std::endl;
        t.insert(text);
        std::cout << t.search(text) << std::endl;
    }

    void PerformanceTests()
    {
        constexpr size_t samplesCount { 1'000'000 }, strLen { 64 }, testsCount { 1 };
        std::vector<std::string> samples;
        samples.reserve(samplesCount);

        for (size_t idx = 0; idx < samplesCount; ++idx) {
            samples.push_back(Utils::randomString(strLen));
        }

        Trie trie;
        std::unordered_set<std::string> set;
        for (const std::string& str: samples){
            set.insert(str);
            trie.insert(str);
        }

        {
            auto start = std::chrono::high_resolution_clock::now();

            for (size_t idx = 0; idx < testsCount; ++idx) {
                for (const std::string& str: samples)
                {
                    if (set.find(str) == set.end())
                    {
                        std::cout << "ERROR\n";
                    }
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			std::cout << "Result: " << duration << " microseconds" << std::endl;
        }

        {
            auto start = std::chrono::high_resolution_clock::now();

            for (size_t idx = 0; idx < testsCount; ++idx) {
                for (const std::string& str: samples) {
                    if (!trie.search(str))
                    {
                        std::cout << "ERROR\n";
                    }
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "Result: " << duration << " microseconds" << std::endl;
        }
    }
}


namespace CollectionsTests::UnorderedMap
{
    void DeletedNotExisting()
    {
        std::unordered_map<int, std::string> numbers {
            {1, "One"}, {2, "Two"}, {3, "Three"}, {4, "Four"}, {5, "Five"}
        };

        auto res = numbers.erase(1);
        std::cout << res << std::endl;

        res = numbers.erase(1);
        std::cout << res << std::endl;
    }

    enum class State
    {
        Open,
        Closed
    };

    struct Session
    {
        std::string buffer;
        State state { State::Closed };

        Session(State state = State::Closed): state {state} {
            std::cout << "Session created\n";
        }
    };

    void Try_Emplace_Existing()
    {
        std::unordered_map<int, Session> sessions;

        {
            auto [iter, ok] = sessions.try_emplace(1);
            std::cout << std::boolalpha << ok << std::endl;
        }

        {
            auto [iter, ok] = sessions.try_emplace(1);
            std::cout << std::boolalpha << ok << std::endl;
        }
    }
}

namespace CollectionsTests::Vector
{
    using Helpers::Integer;

    void GetRefToBack_And_DeleteLastElement()
    {
        std::vector<Integer> numbers { Integer{1}, Integer {2}, Integer{3}, Integer{4}, Integer{5}};

        std::cout << "----------------------------------------------------------\n";

        Integer& last = numbers.back();
        numbers.pop_back();

        std::cout << last.getValue() << std::endl;

        std::cout << "----------------------------------------------------------\n";

    }
}

namespace CollectionsTests::HeterogeneousLookup
{
    struct Wrapped
    {
        int64_t value {0};

        static Wrapped create(int64_t v) {
            return Wrapped{v};
        }

    private:
        explicit Wrapped(int64_t v) : value(v) {
            std::cout << "Wrapped(" << v << ")\n";
        };
    };

    struct Hasher
    {
        using is_transparent = void;

        size_t operator()(int64_t value) const {
            return std::hash<int64_t>{}(value);
        }

        size_t operator()(const Wrapped& wrapped) const {
            return std::hash<int64_t>{}(wrapped.value);
        }
    };

    struct Comparator
    {
        using is_transparent = void;

        bool operator()(int64_t left, const Wrapped& right) const {
            return left == right.value;
        }

        bool operator()(const Wrapped& left, const Wrapped& right) const {
            return left.value == right.value;
        }
    };

    void Test()
    {
        std::unordered_map<Wrapped, std::string, Hasher, Comparator> data;

        data.insert_or_assign(Wrapped::create(10), std::string {"Hello World!"});
        data.insert_or_assign(Wrapped::create(5), std::string {"Goodbye!"});

        std::cout << "\tbefore find(5)" <<  std::endl;

        auto j = data.find(5z);
        // j->first == Wrapped{5}, j->second == "Goodbye!"

        std::cout << "j->first.value == " << j->first.value << ", j->second == " << j->second << "\n";
        // j->first.value == 5, j->second == Goodbye!

    }
}


namespace CollectionsTests::HeterogeneousLookup2
{

    using Helpers::Integer;


    Integer createInt(Integer::value_type value)
    {
        return Integer {value};
    }

    struct Hasher
    {
        using is_transparent = void;

        size_t operator()(int32_t value) const {
            return std::hash<int32_t>{}(value);
        }

        size_t operator()(const Integer& integer) const {
            return std::hash<int32_t>{}(integer.value);
        }
    };

    struct IntegerHash {
        std::size_t operator()(const Integer& s) const noexcept {
            return std::hash<int>{}(s.getValue());
        }
    };

    struct Comparator
    {
        using is_transparent = void;

        bool operator()(int64_t left, const Integer& right) const {
            return left == right.value;
        }

        bool operator()(const Integer& left, const Integer& right) const {
            return left.value == right.value;
        }
    };


    void Test_Bad()
    {
        std::unordered_map<Integer, std::string, IntegerHash> data;

        data.emplace(10, std::string {"Hello World!"});
        data.emplace(5, std::string {"Goodbye!"});

        std::cout << " ---------------------- before find(5) -----------------------" <<  std::endl;
        auto j = data.find(5);
        std::cout << " ---------------------- after -----------------------" <<  std::endl;


        std::cout << "j->first.value == " << j->first.value << ", j->second == " << j->second << "\n";
        // Output:j->first.value == 5, j->second == Goodbye!
    }

    void Test_OK()
    {
        std::unordered_map<Integer, std::string, Hasher, Comparator> data;

        data.emplace(10, std::string {"Hello World!"});
        data.emplace(5, std::string {"Goodbye!"});

        std::cout << " ---------------------- before find(5) -----------------------" <<  std::endl;
        auto j = data.find(5z);
        std::cout << " ---------------------- after -----------------------" <<  std::endl;

        std::cout << "j->first.value == " << j->first.value << ", j->second == " << j->second << "\n";
        // Output:j->first.value == 5, j->second == Goodbye!
    }
}


void CollectionsTests::TestAll()
{
    // CustomArrayTest::Test();
    // CustomArrayTest::MoveConstructor_Test();
    // CustomArrayTest::MoveAssignment_Test();
    // CustomArrayTest::IteratorsTest();

    // MoveConstructor_Noexcept_Vector::test();

    // Arrays::PrintArrayTest();

    // Trie::test();
    // Trie::PerformanceTests();

    // UnorderedMap::DeletedNotExisting();
    // UnorderedMap::Try_Emplace_Existing();

    // Vector::GetRefToBack_And_DeleteLastElement();

    // HeterogeneousLookup::Test();
    // HeterogeneousLookup2::Test_Bad();
    HeterogeneousLookup2::Test_OK();
};