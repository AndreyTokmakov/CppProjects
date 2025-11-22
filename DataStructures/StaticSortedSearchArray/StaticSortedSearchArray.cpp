/**============================================================================
Name        : StaticSortedSearchArray.cpp
Created on  : 21.11.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StaticSortedSearchArray.cpp
============================================================================**/

#include "StaticSortedSearchArray.hpp"

#include <iostream>
#include <memory>
#include <vector>

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
        using size_type  = size_t;
        using value_type = Ty;
        using pointer    = value_type*;
        using const_pointer  = const pointer;
        using array_type = value_type[];

        size_type size { 0 };
        size_type capacity { 0 };
        std::unique_ptr<array_type> elements { nullptr }; //  std::make_unique_for_overwrite<int[]>(len);

        explicit SortedArray(const size_type capacity) :
                size { 0 }, capacity { capacity }, elements { std::make_unique<array_type>(capacity) }  {
        }

        SortedArray(const SortedArray &other) = default;
        SortedArray & operator=(const SortedArray &other) = default;

        /*
        SortedArray(const SortedArray && other)
            : size { other.size }, capacity { other.capacity }, elements { other.elements }
        {
        }

        SortedArray & operator=(SortedArray &&other) noexcept
        {
            if (this == &other)
                return *this;
            size = other.size;
            capacity = other.capacity;
            elements = std::move(other.elements);
            return *this;
        }*/

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


        // TODO:
        //  - Copy constructor
        //  - Copy assignment
        //  - Move constructor
        //  - Move assignment

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
}

namespace
{
    template<typename Ty>
    std::ostream& operator<<(std::ostream& os, const std::vector<Ty>& values)
    {
        for (const auto& value : values)
            os << value << " ";
        return os;
    }
}

void static_sorted_search_array::TestAll()
{
    /*
    SortedArray<int, false> values (10);

    for (int i = 0; i < 10; ++i)
        values.data[i] = i * 3;
    values.size = 10;

    values.print();

    const auto item = values.find_insert_pos(11);
    std::cout << item << std::endl;
    */

    /*
    std::vector<int> values { 0, 3, 6, 9, 12, 15, 18, 21, 24, 27 };
    int item = 11;
    const int pos = 4;


    for (uint32_t idx = values.size() - 1; idx > pos; --idx) {
        values[idx] = values[idx - 1];
    }
    values[pos] = item;

    std::cout << values << std::endl;
    */


    constexpr uint32_t initialSize = 5;
    SortedArray<int> values (initialSize);

    /*
    for (int i = 0; i < initialSize; ++i) {
        values.elements[i] = i * 3;
        values.size++;
    }*/

    values.print();

    values.push(11);
    std::cout << std::string(120, '-') << std::endl;

    values.print();

    values.push(23);
    std::cout << std::string(120, '-') << std::endl;

    values.print();

    values.push(3);
    std::cout << std::string(120, '-') << std::endl;

    values.print();

    std::cout << std::string(120, '-') << std::endl;

    SortedArray<int> values1  = values;
}