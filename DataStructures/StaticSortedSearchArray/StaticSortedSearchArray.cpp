/**============================================================================
Name        : StaticSortedSearchArray.cpp
Created on  : 21.11.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StaticSortedSearchArray.cpp
============================================================================**/

#include "StaticSortedSearchArray.hpp"
#include "PerfUtilities.hpp"

#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <random>


#include <set>
#include <flat_set>

namespace static_sorted_search_array
{
    enum class SortOrder
    {
        Ascending,
        Descending
    };

    template<typename Ty, SortOrder ordering = SortOrder::Ascending>
    struct SortedArray
    {
        using size_type      = uint32_t;
        using value_type     = Ty;
        using pointer        = value_type*;
        using const_pointer  = const pointer;
        using array_type     = value_type[];

        size_type size { 0 };
        size_type capacity { 0 };
        std::unique_ptr<array_type> elements { nullptr };

        explicit SortedArray(const size_type capacity) :
                size { 0 }, capacity { capacity }, elements { std::make_unique<array_type>(capacity) }  {
        }

        SortedArray(const SortedArray & other):
                size { other.size },
                capacity { other.capacity },
                elements { std::make_unique_for_overwrite<array_type>(capacity) }
        {
            std::copy_n(other.elements.get(), size, elements.get());
        }

        SortedArray & operator=(const SortedArray & other)
        {
            auto newElements = std::make_unique<value_type[]>(other.capacity);
            std::copy_n(other.elements.get(), other.size, newElements.get());

            elements = std::move(newElements);
            size = other.size;
            capacity = other.capacity;

            return *this;
        }

        SortedArray(SortedArray && other) noexcept:
                size { std::exchange(other.size, 0) },
                capacity { std::exchange(other.capacity, 0) },
                elements { std::move(other.elements) }
        {
        }

        SortedArray & operator=(SortedArray && other) noexcept
        {
            size = std::exchange(other.size, 0);
            capacity = std::exchange(other.capacity, 0);
            elements = std::move(other.elements);

            return *this;
        }

        [[nodiscard]]
        size_type lowerBound(const value_type& item) const noexcept
        {
            size_type left = 0;
            size_type right = size;

            while (left < right)
            {
                const size_type mid = left + (right - left) / 2;

                if (better(elements[mid], item))
                    left = mid + 1;
                else
                    right = mid;
            }

            return left;
        }


        [[nodiscard]]
        bool contains(const value_type& item) const noexcept
        {
            const size_type index = lowerBound(item);
            return index < size && elements[index] == item;
        }


        bool push(const value_type& item)
        {
            const size_type insertIndex = lowerBound(item);
            if (insertIndex < size && elements[insertIndex] == item)
                return false;

            if (size == capacity) {
                if (insertIndex == size)
                    return false;
            } else{
                ++size;
            }

            for (size_type index = size - 1; index > insertIndex; --index)
                elements[index] = elements[index - 1];

            elements[insertIndex] = item;
            return true;
        }

        [[nodiscard]]
        pointer data() noexcept {
            return elements.get();
        }

        [[nodiscard]]
        const_pointer data() const noexcept {
            return elements.get();
        }

        [[nodiscard]]
        size_type getSize() const noexcept {
            return size;
        }

        [[nodiscard]]
        size_type getCapacity() const noexcept {
            return capacity;
        }

        static constexpr bool better(const value_type& a, const value_type& b) noexcept
        {
            if constexpr (ordering == SortOrder::Descending)
                return a > b;

            return a < b;
        }
    };

    template<typename Ty, typename Collection>
    class base_iterator
    {
        using size_type = Collection::size_type;

        size_type index { 0 };
        Collection& collection;

    public:
        base_iterator(Collection& collection, const size_type index) noexcept :
            index { index },
            collection { collection }
        {
        }

        [[nodiscard]]
        bool operator==(const base_iterator& other) const noexcept
        {
            return index == other.index;
        }

        [[nodiscard]]
        bool operator!=(const base_iterator& other) const noexcept
        {
            return !(*this == other);
        }

        [[nodiscard]]
        const Ty& operator*() const noexcept
        {
            return collection.elements[index];
        }

        base_iterator& operator++() noexcept
        {
            ++index;
            return *this;
        }
    };

    template <typename T>
    using array_iterator = base_iterator<T, SortedArray<T>>;

    template <typename T>
    using array_const_iterator = base_iterator<T, const SortedArray<T>>;

    template <typename T>
    array_iterator<T> begin(SortedArray<T>& collection) {
        return array_iterator<T>(collection, 0);
    }

    template <typename T>
    array_iterator<T> end(SortedArray<T>& collection) {
        return array_iterator<T>(collection, collection.getSize());
    }

    template <typename T>
    array_const_iterator<T> begin(const SortedArray<T>& collection) {
        return array_const_iterator<T>(collection, 0);
    }

    template <typename T>
    array_const_iterator<T> end(const SortedArray<T>& collection) {
        return array_const_iterator<T>(collection, collection.getSize());
    }
}

namespace static_sorted_search_array::testing
{
    std::random_device rd{};
    std::mt19937 generator = std::mt19937 {rd()};

    int getRandomInRange(const int32_t start, const int32_t end) noexcept
    {
        auto distribution = std::uniform_int_distribution<>{ start, end };
        return distribution(generator);
    }

    template<typename Ty>
    std::ostream& operator<<(std::ostream& os, const std::vector<Ty>& values)
    {
        for (const auto& value : values)
            os << value << " ";
        return os;
    }

    [[nodiscard]]
    std::vector<int32_t> getTestData(const size_t size = 10'000'000)
    {
        std::vector<int32_t> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = getRandomInRange(0, size);
        }
        return data;
    }

    void benchmark()
    {
        constexpr uint32_t collectionSize { 1'000 }, testDataSize = 100'000'000;
        const std::vector<int32_t> data = getTestData(testDataSize);

        utilities::perf::ScopedTimer timer { "SortedArray"};
        SortedArray<int> array (collectionSize);
        for (uint32_t idx = 0; idx < testDataSize; ++idx)
        {
            const auto key = data[idx];
            array.push(key);
        }
        std::cout << array.getSize() << std::endl;
    }
}

namespace static_sorted_search_array::perf_tests
{
    using Value = std::uint64_t;
    using Clock = std::chrono::steady_clock;

    constexpr std::size_t OperationCount = 5'000'000;
    constexpr std::size_t WarmupCount = 100'000;

    constexpr std::uint64_t RandomSeed = 0x123456789abcdef0ULL;


    volatile std::uint64_t benchmarkSink = 0;
    template<typename Container>
    [[nodiscard]]
    std::uint64_t checksum(const Container& container) noexcept
    {
        std::uint64_t result = 0;
        for (const auto& value : container) {
            result ^= value + 0x9e3779b97f4a7c15ULL + (result << 6) + (result >> 2);
        }

        return result;
    }

    template<typename Container>
    void consume(const Container& container) noexcept
    {
        benchmarkSink ^= checksum(container);
    }

    [[nodiscard]]
    std::vector<Value> generateValues()
    {
        std::mt19937_64 generator { RandomSeed };
        std::uniform_int_distribution<Value> distribution { 0, std::numeric_limits<Value>::max()};

        std::vector<Value> values(OperationCount);
        for (std::size_t index = 0; index < OperationCount; ++index)
            values[index] = distribution(generator);

        return values;
    }

    template<typename Function>
    [[nodiscard]]
    double measure(const std::vector<Value>& values, Function&& function)
    {
        for (std::size_t index = 0; index < WarmupCount; ++index)
            function(values[index]);

        const auto start = Clock::now();
        for (const Value value : values)
            function(value);

        const auto finish = Clock::now();
        return std::chrono::duration<double, std::nano>(finish - start).count();
    }

    template<typename Set>
    bool pushLimited(Set& set, const Value value, const std::size_t capacity)
    {
        if (set.size() < capacity)
            return set.insert(value).second;
        if (value > *set.rbegin())
            return false;

        const auto [iterator, inserted] = set.insert(value);
        if (!inserted)
            return false;

        set.erase(std::prev(set.end()));
        return true;
    }

    template<std::size_t Capacity>
    void benchmarkSortedArray(const std::vector<Value>& values)
    {
        SortedArray<Value, SortOrder::Ascending> array { Capacity };
        const double elapsed = measure(values, [&array](const Value value){
            array.push(value);
        });

        consume(array);
        std::cout << "SortedArray" << "   capacity=" << Capacity << "   time=" << elapsed / 1'000'000.0 << " ms"
            << "   ns/op=" << elapsed / static_cast<double>(OperationCount) << '\n';
    }

    template<std::size_t Capacity>
    void benchmarkSet(const std::vector<Value>& values)
    {
        std::set<Value> set;
        const double elapsed = measure(values, [&set](const Value value) {
            pushLimited(set, value, Capacity);
        });

        consume(set);
        std::cout << "std::set" << "       capacity=" << Capacity<< "   time=" << elapsed / 1'000'000.0 << " ms"
            << "   ns/op=" << elapsed / static_cast<double>(OperationCount)
            << '\n';
    }

    template<std::size_t Capacity>
    void benchmarkFlatSet(const std::vector<Value>& values)
    {
        std::flat_set<Value> set;
        const double elapsed = measure(values, [&set](const Value value){
            pushLimited(set, value, Capacity);
        });

        consume(set);
        std::cout << "std::flat_set" << "   capacity=" << Capacity << "   time=" << elapsed / 1'000'000.0 << " ms"
            << "   ns/op=" << elapsed / static_cast<double>(OperationCount) << '\n';
    }

    template<std::size_t Capacity>
    void runBenchmark(const std::vector<Value>& values)
    {
        std::cout << '\n';
        std::cout << "Capacity: " << Capacity << '\n';
        std::cout << "----------------------------------------\n";

        benchmarkSortedArray<Capacity>(values);
        benchmarkSet<Capacity>(values);
        benchmarkFlatSet<Capacity>(values);
    }

    void run()
    {
        const auto values = generateValues();

        runBenchmark<16>(values);
        runBenchmark<64>(values);
        runBenchmark<256>(values);
        runBenchmark<1024>(values);

        std::cout << '\n';
        std::cout << "Benchmark sink: " << benchmarkSink << '\n';
    }
}

void static_sorted_search_array::TestAll()
{
    // testing::benchmark();
    perf_tests::run();
}