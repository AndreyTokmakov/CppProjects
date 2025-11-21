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
    // TODO: IsBis --> Enum
    template<typename Ty, bool IsBid>
    struct SortedArray
    {
        using size_type  = size_t;
        using value_type = Ty;
        using array_type = value_type[];

        size_type size {0};
        size_type capacity {0};
        std::unique_ptr<array_type> data { nullptr }; //  std::make_unique_for_overwrite<int[]>(len);

        explicit SortedArray(const size_type capacity) :
                capacity { capacity }, data { std::make_unique<array_type>(capacity) }  {
        }

        [[nodiscard]]
        size_type find_insert_pos(const value_type item) const noexcept
        {
            size_type left = 0, right = size;
            while (left < right)
            {
                const size_type mid = (left + right) >> 1;
                if (better(item, data[mid]))
                    right = mid;
                else
                    left = mid + 1;
            }
            return left;
        }

        static bool better(const value_type a, const value_type b) noexcept
        {
            if constexpr (IsBid)
                return a > b;   // bids: high → low
            else
                return a < b;   // asks: low → high
        }

        // TODO:
        //  - find place to insert new element --> lower_bound
        //  - Sort from place of insertions    --> move right
        //  - When and how to Increment 'Size'

        // TODO:
        //  - Copy constructor
        //  - Copy assignment
        //  - Move constructor
        //  - Move assignment


        // TODO:
        //  - Copy front()

        // TODO:
        //  - usee 'buildin_prefetch'

        void print()
        {
            for (size_type i = 0; i < size; ++i)
                std::cout << data[i] << " ";
            std::cout << std::endl;
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

    std::vector<int> values { 0, 3, 6, 9, 12, 15, 18, 21, 24, 27 };
    int item = 11;
    const int pos = 4;


    for (uint32_t idx = values.size() - 1; idx > pos; --idx) {
        values[idx] = values[idx - 1];
    }
    values[pos] = item;

    std::cout << values << std::endl;
}