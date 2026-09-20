/**============================================================================
Name        : SortedPriceLevels.cpp
Created on  : 20.09.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SortedPriceLevels.cpp
============================================================================**/

#include "SortedPriceLevels.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <flat_map>
#include <iostream>
#include <random>
#include <vector>

namespace
{
      enum class SortOrder
    {
        Ascending,
        Descending
    };

    struct PriceLevel
    {
        std::uint64_t price { 0 };
        std::uint64_t quantity { 0 };
    };

    template<SortOrder ordering = SortOrder::Ascending>
    class SortedPriceLevels
    {
    public:
        using size_type = std::uint32_t;
        using value_type = PriceLevel;
        using pointer = value_type*;
        using const_pointer = const value_type*;

        explicit SortedPriceLevels(const size_type capacity) :
            capacity { capacity },
            levels { std::make_unique<value_type[]>(capacity) }
        {
        }

        SortedPriceLevels(const SortedPriceLevels& other) :
            size { other.size },
            capacity { other.capacity },
            levels { std::make_unique<value_type[]>(other.capacity) }
        {
            std::copy_n(other.levels.get(), size, levels.get());
        }

        SortedPriceLevels& operator=(const SortedPriceLevels& other)
        {
            if (this == &other)
                return *this;

            auto newLevels = std::make_unique<value_type[]>(other.capacity);
            std::copy_n(other.levels.get(), other.size, newLevels.get());

            levels = std::move(newLevels);
            size = other.size;
            capacity = other.capacity;

            return *this;
        }

        SortedPriceLevels(SortedPriceLevels&& other) noexcept :
            size { std::exchange(other.size, 0) },
            capacity { std::exchange(other.capacity, 0) },
            levels { std::move(other.levels) }
        {
        }

        SortedPriceLevels& operator=(SortedPriceLevels&& other) noexcept
        {
            if (this == &other)
                return *this;

            size = std::exchange(other.size, 0);
            capacity = std::exchange(other.capacity, 0);
            levels = std::move(other.levels);

            return *this;
        }

        [[nodiscard]]
        size_type lowerBound(const std::uint64_t price) const noexcept
        {
            size_type left = 0;
            size_type right = size;

            while (left < right)
            {
                const size_type mid = left + (right - left) / 2;
                if (better(levels[mid].price, price))
                    left = mid + 1;
                else
                    right = mid;
            }

            return left;
        }

        [[nodiscard]]
        PriceLevel* find(const std::uint64_t price) noexcept
        {
            const size_type index = lowerBound(price);
            if (index == size || levels[index].price != price)
                return nullptr;

            return &levels[index];
        }

        [[nodiscard]]
        const PriceLevel* find(const std::uint64_t price) const noexcept
        {
            const size_type index = lowerBound(price);
            if (index == size || levels[index].price != price)
                return nullptr;

            return &levels[index];
        }

        [[nodiscard]]
        bool contains(const std::uint64_t price) const noexcept
        {
            return find(price) != nullptr;
        }

        bool push(const PriceLevel& level)
        {
            if (size == capacity && !better(level.price, levels[size - 1].price))
                return false;

            const size_type insertIndex = lowerBound(level.price);
            if (insertIndex < size && levels[insertIndex].price == level.price)
                return false;

            if (size < capacity)
                ++size;

            for (size_type index = size - 1; index > insertIndex; --index)
                levels[index] = levels[index - 1];

            levels[insertIndex] = level;
            return true;
        }

        bool push(const std::uint64_t price, const std::uint64_t quantity)
        {
            return push(PriceLevel { .price = price, .quantity = quantity });
        }

        bool erase(const std::uint64_t price) noexcept
        {
            const size_type index = lowerBound(price);
            if (index == size || levels[index].price != price)
                return false;
            for (size_type current = index + 1; current < size; ++current)
                levels[current - 1] = levels[current];

            --size;

            return true;
        }

        [[nodiscard]]
        PriceLevel& operator[](const size_type index) noexcept {
            return levels[index];
        }

        [[nodiscard]]
        const PriceLevel& operator[](const size_type index) const noexcept{
            return levels[index];
        }

        [[nodiscard]]
        pointer data() noexcept {
            return levels.get();
        }

        [[nodiscard]]
        const_pointer data() const noexcept {
            return levels.get();
        }

        [[nodiscard]]
        size_type sizeValue() const noexcept {
            return size;
        }

        [[nodiscard]]
        size_type maxSize() const noexcept {
            return capacity;
        }

    private:

        static constexpr bool better(const std::uint64_t a,
                                     const std::uint64_t b) noexcept
        {
            if constexpr (ordering == SortOrder::Descending)
                return a > b;
            return a < b;
        }

        size_type size { 0 };
        size_type capacity { 0 };
        std::unique_ptr<value_type[]> levels;
    };
}

namespace
{
    using Price = std::uint64_t;
    using Quantity = std::uint64_t;
    using Clock = std::chrono::steady_clock;

    constexpr std::size_t OperationCount = 5'000'000;
    constexpr std::size_t WarmupCount = 100'000;

    constexpr std::size_t Capacities[] =
    {
        16,
        64,
        256,
        1024
    };

    constexpr Price InitialPrice = 1'000'000;
    constexpr Price PriceRange = 10'000;
    constexpr std::uint64_t RandomSeed = 0x123456789abcdef0ULL;

    volatile std::uint64_t benchmarkSink = 0;

    struct Operation
    {
        Price price;
        Quantity quantity;
    };

    [[nodiscard]]
    std::vector<Operation> generateOperations()
    {
        std::vector<Operation> operations;
        operations.reserve(OperationCount);

        std::mt19937_64 generator { RandomSeed };
        std::uniform_int_distribution<Price> priceDistribution { InitialPrice, InitialPrice + PriceRange};
        std::uniform_int_distribution<Quantity> quantityDistribution { 1, 1'000};

        for (std::size_t index = 0; index < OperationCount; ++index){
            operations.push_back({
                .price = priceDistribution(generator),
                .quantity = quantityDistribution(generator)
            });
        }

        return operations;
    }

    template<typename Function>
    [[nodiscard]]
    double measure(const std::vector<Operation>& operations,
                  Function&& function)
    {
        for (std::size_t index = 0; index < WarmupCount; ++index)
            function(operations[index]);

        const auto start = Clock::now();
        for (const auto& operation : operations)
            function(operation);

        const auto finish = Clock::now();
        return std::chrono::duration<double, std::nano>(finish - start).count();
    }

    template<typename Container>
    void consume(const Container& container) noexcept
    {
        std::uint64_t result = 0;
        for (const auto& [price, quantity] : container)
        {
            result ^= price;
            result ^= quantity + 0x9e3779b97f4a7c15ULL;
        }
        benchmarkSink ^= result;
    }

    template<std::size_t Capacity>
    void benchmarkSortedPriceLevels(
    const std::vector<Operation>& operations)
    {
        SortedPriceLevels<SortOrder::Ascending> levels { Capacity };
        const double elapsed = measure(operations, [&levels](const Operation& operation) {
            if (auto* level = levels.find(operation.price)) {
                level->quantity += operation.quantity;
                return;
            }
            if (levels.sizeValue() < levels.maxSize())
                levels.push(operation.price, operation.quantity);
        });

        std::uint64_t result = 0;
        for (std::uint32_t index = 0; index < levels.sizeValue(); ++index) {
            result ^= levels[index].price;
            result ^= levels[index].quantity;
        }
        benchmarkSink ^= result;

        std::cout << "SortedPriceLevels" << "   capacity=" << Capacity
            << "   time=" << elapsed / 1'000'000.0 << " ms" << "   ns/op=" << elapsed / static_cast<double>(OperationCount)
            << '\n';
    }

    template<std::size_t Capacity>
    void benchmarkFlatMap(const std::vector<Operation>& operations)
    {
        std::flat_map<Price, Quantity> levels;
        const double elapsed = measure(operations, [&levels](const Operation& operation) {
            if (const auto iterator = levels.find(operation.price); iterator != levels.end()){
                iterator->second += operation.quantity;
                return;
            }
            if (levels.size() < Capacity)
                levels.emplace(operation.price, operation.quantity);
        });

        consume(levels);
        std::cout << "std::flat_map" << "         capacity=" << Capacity
            << "   time=" << elapsed / 1'000'000.0 << " ms   ns/op=" << elapsed / static_cast<double>(OperationCount)
            << '\n';
    }

    template<std::size_t Capacity>
    void runBenchmark(const std::vector<Operation>& operations)
    {
        std::cout << '\n';
        std::cout << "Capacity: " << Capacity << '\n';
        std::cout << "----------------------------------------\n";

        benchmarkSortedPriceLevels<Capacity>(operations);
        benchmarkFlatMap<Capacity>(operations);
    }
}

void sorted_price_levels::TestAll()
{
    const auto operations = generateOperations();

    runBenchmark<16>(operations);
    runBenchmark<64>(operations);
    runBenchmark<256>(operations);
    runBenchmark<1024>(operations);

    std::cout << '\n';
    std::cout << "Benchmark sink: " << benchmarkSink << '\n';
}

/**
Capacity: 16
----------------------------------------
SortedPriceLevels   capacity=16   time=69.3205 ms   ns/op=13.8641
std::flat_map       capacity=16   time=61.7375 ms   ns/op=12.3475

Capacity: 64
----------------------------------------
SortedPriceLevels   capacity=64   time=112.413 ms   ns/op=22.4825
std::flat_map       capacity=64   time=94.9451 ms   ns/op=18.989

Capacity: 256
----------------------------------------
SortedPriceLevels   capacity=256   time=147.394 ms   ns/op=29.4788
std::flat_map       capacity=256   time=133.817 ms   ns/op=26.7634

Capacity: 1024
----------------------------------------
SortedPriceLevels   capacity=1024   time=193.759 ms   ns/op=38.7519
std::flat_map       capacity=1024   time=185.766 ms   ns/op=37.1532

**/