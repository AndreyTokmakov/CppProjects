/**============================================================================
Name        : FindMinMaxValues.cpp
Created on  : 20.08.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FindMinMaxValues
============================================================================**/

#include "FindMinMaxValues.h"

#include <iostream>
#include <array>
#include <vector>
#include <span>
#include <algorithm>

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
}

namespace FindMinMaxValues
{
// TODO: add comparator
    template<typename _Ty, size_t _Size>
    struct ArrayWithTop final
    {
        using value_type = _Ty;

        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        std::vector<value_type> values {};
        // std::array<value_type> values {};

        ArrayWithTop() {
            values.reserve(_Size);
        }

        // TODO: rename 'tryAdd' ??
        void add(const value_type& val)
        {
            if (_Size > values.size())
                values.push_back(val);
            else if (values[0] > val)
                values[0] = val;
            else
                return;

            update();
            // updateSTD();
        }

        void update()
        {
            size_t topIndex = 0;
            for (size_t idx = 1; idx < values.size(); ++idx) {
                if (values[idx] > values[topIndex])
                    topIndex = idx;
            }

            std::swap(values[0], values[topIndex]);
        }

        void updateSTD()
        {
            std::nth_element(values.begin(), values.begin() + 1, values.end(), std::greater<>());
        }
    };


    // TODO: add comparator
    template<typename _Ty,
            size_t _Size,
            typename _Cmp = std::less<>>
    struct ArrayWithTopEx final
    {
        using value_type = _Ty;

        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        _Cmp comparator {};
        std::array<value_type, _Size> values {};

        explicit ArrayWithTopEx(std::span<value_type> data)
        {
            if (data.size() != _Size)
                return;
            std::copy_n(data.begin(), _Size, values.begin());
        }

        void add(const value_type& val)
        {
            if (comparator(val, values[0]))
            {
                values[0] = val;
                // update();
                updateSTD();
            }
        }

        void update()
        {
            size_t topIndex = 0;
            for (size_t idx = 1; idx < values.size(); ++idx) {
                if (comparator(values[topIndex], values[idx]))
                    topIndex = idx;
            }

            std::swap(values[0], values[topIndex]);
        }

        void updateSTD()
        {
            static auto reversed_compare = [&](const value_type& a, const value_type& b) {
                return comparator(b, a);
            };
            std::nth_element(values.begin(), values.begin() + 1, values.end(), reversed_compare);
        }
    };

    void find_N_Min_Values()
    {
        ArrayWithTop<int, 3> tmp {};

        std::vector<int> values {12,3,13,5,9,14,7,1,44,32,6,21,2}; // Mins: 1, 2 ,3
        // std::vector<int> values {12,3, 13,5,9,14,7,1,2, 44,32,6,21, 2};

        for (const int v: values)
            tmp.add(v);

        std::cout << tmp.values << std::endl;
    }


    void testEx()
    {
        std::vector<int> values {12,3,13,5,9,14,7,1,44,32,6,21,2}; // Mins: 1, 2 ,3
        ArrayWithTopEx<int, 3> tmp {std::span<int>{values.data(), 3}};

        for (const int v: values)
            tmp.add(v);

        std::cout << tmp.values << std::endl;
    }
};


void FindMinMaxValues::TestAll()
{
    // find_N_Min_Values();
    testEx();
};
