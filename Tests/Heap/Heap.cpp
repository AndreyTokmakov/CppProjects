/**============================================================================
Name        : Heap.cpp
Created on  : 13.08.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Heap
============================================================================**/

#include "Heap.h"

#include <iostream>
#include <vector>
#include <string_view>
#include <algorithm>
#include <random>
#include <unordered_set>

namespace
{
    std::random_device rd{};
    std::mt19937 generator = std::mt19937 { rd() };

    int getRandomUniqueInt(int start = 0, int end = 100)
    {
        static std::unordered_set<int> uniqueInts;
        std::uniform_int_distribution intDistribution { std::uniform_int_distribution<>{ start, end } };

        while (true) {
            const int number = intDistribution(generator);
            if (uniqueInts.insert(number).second)
                return number;
        }
    }
}

namespace Heap::MinHeapImpl
{
    template<typename __Type>
    struct MinHeap
    {
        using value_type = __Type;

        static_assert(!std::is_same_v<value_type, void>,
                      "Type of the value shall not be void");

        std::vector<value_type> data;

    public:

        MinHeap& add(value_type value)
        {
            size_t index = data.size();  // Index of new element to be inserted
            data.push_back(value);       // and insert that element

            /** Parent element index will :**/
            size_t parentIndex = (index - 1) / 2;

            while (index > 0 && data[parentIndex] > data[index])
            {
                std::swap(data[parentIndex], data[index]);
                index = parentIndex;
                parentIndex = (index - 1) / 2;
            }

            return *this;
        }

        void heapify(size_t index)
        {
            size_t left, right, current;
            while (true)
            {
                left = 2 * index + 1;
                right = 2 * index + 2;
                current = index;

                if (data.size() > left && this->vector[left] > this->vector[current])
                    current = left;
                if (data.size()  > right && this->vector[right] > this->vector[current])
                    current = right;
                if (current == index)
                    break;

                std::swap(this->vector[index], this->vector[current]);
                index = current;
            }
        }

        [[nodiscard("Dont ignore the value")]]
        bool isValid() const noexcept {
            // index of the parent of the last element ((SIZE - 1) - 1) / 2
            const size_t lastParentIdx = (data.size() - 2) / 2;

            // Check MaxHeap condition expect the 'last parent'. Will be checked at the end
            for (size_t idx = 0; idx < lastParentIdx; ++idx) {
                if (data[idx] > data[idx * 2 + 1] || data[idx] > data[idx * 2 + 2])
                    return false;
            }

            if (data[lastParentIdx] > data[lastParentIdx * 2 + 1])
                return false;
            return data.size() <= (lastParentIdx * 2 + 2) || data[lastParentIdx * 2 + 2] > data[lastParentIdx];
        }



        void print() const noexcept
        {
            for (const value_type& val: data)
                std::cout << val << ' ';
            std::cout << std::endl;
        }

    };
};

namespace Heap::Tests
{
    using namespace MinHeapImpl;

    void addTest()
    {
        MinHeap<int> minHeap;
        minHeap.add(10).add(9).add(8).add(7).add(6).add(5).add(4);

        minHeap.print();
        std::cout << std::boolalpha << minHeap.isValid() << std::endl;
        std::cout << std::boolalpha << std::is_heap(minHeap.data.cbegin(), minHeap.data.cend()) << std::endl;
    }

    void addTest2()
    {
        constexpr size_t count = 50;
        MinHeap<int> minHeap;

        for (size_t n = 0; n < count; ++n)
            minHeap.add(getRandomUniqueInt(0, 100));

        minHeap.print();
        std::cout << std::boolalpha << minHeap.isValid() << std::endl;
        std::cout << std::boolalpha << std::is_heap(minHeap.data.cbegin(), minHeap.data.cend(), std::greater<>()) << std::endl;
    }

    bool _is_max_heap(const std::vector<int>& data)
    {   // index of the parent of the last element ((SIZE - 1) - 1) / 2
        const size_t lastParentIdx = (data.size() - 2) / 2;

        // Check MaxHeap condition expect the 'last parent'. Will be checked at the end
        for (size_t idx = 0; idx < lastParentIdx; ++idx) {
            if (data[idx * 2 + 1] > data[idx] || data[idx * 2 + 2] > data[idx])
                return false;
        }

        if (data[lastParentIdx * 2 + 1] > data[lastParentIdx])
            return false;
        return data.size() <= (lastParentIdx * 2 + 2) || data[lastParentIdx] > data[lastParentIdx * 2 + 2];
    }

    bool _is_min_heap(const std::vector<int>& data)
    {
        // index of the parent of the last element ((SIZE - 1) - 1) / 2
        const size_t lastParentIdx = (data.size() - 2) / 2;

        // Check MaxHeap condition expect the 'last parent'. Will be checked at the end
        for (size_t idx = 0; idx < lastParentIdx; ++idx)
        {
            std::cout << data[idx] << " <-> " << data[idx * 2 + 1] << std::endl;
            std::cout << data[idx] << " <-> " << data[idx * 2 + 2] << std::endl;

            if (data[idx] > data[idx * 2 + 1] || data[idx] > data[idx * 2 + 2])
                return false;
        }

        std::cout << data[lastParentIdx] << " <-> " << data[lastParentIdx * 2 + 1] << std::endl;
        if (data[lastParentIdx] > data[lastParentIdx * 2 + 1])
            return false;

        if (data.size() > (lastParentIdx * 2 + 2))
        {
            std::cout << data[lastParentIdx] << " <-> " << data[lastParentIdx * 2 + 2] << std::endl;
        }

        return data.size() <= (lastParentIdx * 2 + 2) || data[lastParentIdx * 2 + 2] > data[lastParentIdx];
    }

    bool _is_max_heap_1(const std::vector<int>& data) {
        for (size_t i = 0; i <= (data.size() / 2 - 1); i++) {
            if (data[i * 2 + 1] > data[i]) // If left child is greater, return false
                return false;
            else if (data.size() > i * 2 + 2 && data[i * 2 + 1] > data[i]) // If right child EXISTS and is greater, return false
                return false;
        }
        return true;
    }

    bool _is_max_heap_2(const std::vector<int>& data) {
        const size_t size = data.size();
        for (size_t i = size; i >= 1; i--) {
            if (data[i / 2 - 1] < data[i - 1]) {
                return false;
            }
        }
        return true;
    }

    void Check_Parent_Nodes()
    {
        std::vector<int> numbers {0,1,2,3,4,5,6,7,8,9,10};

        for (size_t idx = 1; idx < numbers.size(); ++idx) {
            size_t parent_index = (idx - 1) / 2;
            std::cout << numbers[parent_index] << " --> " << numbers[idx]  << std::endl;
        }
    }
}

// TODO
//  validate() / for Min and MAX
//  min / max strategy? function
//  MAKE_HEAP

void Heap::TestAll()
{
    // Tests::addTest();
    Tests::addTest2();

    // Tests::Check_Parent_Nodes();

    /*
    std::vector<int> numbers {0,1,2,3,4,5,6,7,8,9, 10};
    Tests::_is_max_heap(numbers);
    */

};
