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
            size = other.size;
            capacity = other.capacity;
            elements = std::make_unique_for_overwrite<array_type>(capacity);
            std::copy_n(other.elements.get(), size, elements.get());

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
        size_type findInsertIndex(const value_type item) const noexcept
        {
            size_type left = 0, right = size;
            while (left < right)
            {
                const size_type mid = (left + right) >> 1;
                if (better(item, elements[mid]))
                    right = mid;
                else
                    left = mid + 1;
            }
            return left;
        }

        bool push(const value_type item)
        {
            if (size == capacity && item > elements[size - 1]) {
                return false;
            }
            // TODO: Check if size > 0 && size == capacity  && item > elements[capacity - 1]
            /*if (size > 0 && item > elements[size - 1]) {
                elements[size++] = item;;
                return true;
            }*/

            const size_type idxInsert = findInsertIndex(item);
            if (capacity == idxInsert || item == elements[idxInsert]) {
                return false;
            }

            size = (capacity == size) ? size : size + 1;
            for (size_type i = size - 1; i > idxInsert; --i) /** TODO: Prefetch **/
                elements[i] = elements[i - 1];
            elements[idxInsert] = item;
            return true;
        }

        [[nodiscard]]
        pointer data() {
            return elements.get();
        }

        [[nodiscard]]
        const_pointer data() const {
            return elements.get();
        }

        // TODO: compiler flags 'always inline'
        // TODO: Rename
        static constexpr bool better(const value_type a, const value_type b) noexcept
        {
            if constexpr (SortOrder::Descending == ordering)
                return a >= b;
            else
                return a <= b;
        }

        [[nodiscard]]
        size_type Size() const noexcept {
            return size;
        }

        // TODO: Methods
        //  - size()

        // TODO:
        //  - Iterators

        // TODO:
        //  - Copy front()

        // TODO:
        //  - usee 'builtin_prefetch'

        void print()
        {
            for (size_type i = 0; i < size; ++i)
                std::cout << "[" << i << "] = " << elements[i] << "\n";
        }

        [[nodiscard]]
        size_type find_insert_pos_debug(const value_type item) const noexcept
        {
            size_type left = 0, right = size;
            while (left < right)
            {
                const size_type mid = (left + right) >> 1;
                // std::cout << "mid = " << mid;
                if (better(item, elements[mid]))
                    right = mid;
                else
                    left = mid + 1;
                // std::cout << " left = " << left << " right = " << right << std::endl;
            }
            return left;
        }

        void push_debug(const value_type item)
        {
            const size_type idxInsert = find_insert_pos_debug(item);
            // std::cout << "idxInsert = " << idxInsert << std::endl;
            if (capacity == idxInsert || item == elements[idxInsert]) {
                // std::cout << "Item already exists or out-of-range" << std::endl;
                return;
            }

            size = (capacity == size) ? size : size + 1;
            for (size_type i = size - 1; i > idxInsert; --i)
                elements[i] = elements[i - 1];
            elements[idxInsert] = item;
        }
    };

    template <typename Ty, typename Collection>
    class base_iterator
    {
        Collection::size_type index { 0 };
        Collection& collection;

    public:
        base_iterator(Collection& collection, const size_t index) :
            index(index), collection(collection) {
        }

        bool operator!= (const base_iterator & other) const {
            return index != other.index;
        }

        const Ty& operator*() const {
            return collection.elements[index];
        }

        const base_iterator& operator++ () {
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
        return array_iterator<T>(collection, collection.Size());
    }

    template <typename T>
    array_const_iterator<T> begin(const SortedArray<T>& collection) {
        return array_const_iterator<T>(collection, 0);
    }

    template <typename T>
    array_const_iterator<T> end(const SortedArray<T>& collection) {
        return array_const_iterator<T>(collection, collection.Size());
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
        std::cout << array.Size() << std::endl;
    }
}


void static_sorted_search_array::TestAll()
{
    //testing::benchmark();

    SortedArray<int> array (10);

    for (int i = 0; i < 10; ++i) {
        array.push(i);
    }

    for (const auto & e : array) {
        std::cout << e << std::endl;
    }
}