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


        void __add(value_type value, std::vector<value_type>& heap)
        {
            size_t index = heap.size();  // Index of new element to be inserted
            heap.push_back(value);       // and insert that element

            /** Parent element index will :**/
            size_t parentIndex = (index - 1) / 2;

            while (index > 0 && heap[parentIndex] > heap[index])
            {
                std::swap(heap[parentIndex], heap[index]);
                index = parentIndex;
                parentIndex = (index - 1) / 2;
            }
        }

        MinHeap& add(value_type value)
        {
            __add(value, data);
            return *this;
        }

        void heapify(size_t index)
        {
            size_t left, right, parent;
            while (true)
            {
                left = 2 * index + 1;
                right = 2 * index + 2;
                parent = index;

                if (data.size() > left && data[parent] > data[left])
                    parent = left;
                if (data.size()  > right && data[parent] > data[right])
                    parent = right;
                if (parent == index)
                    break;

                std::swap(data[index], data[parent]);
                index = parent;
            }
        }

        void makeHeap_Rebuild()
        {
            std::vector<value_type> tmp;
            tmp.reserve(data.size());

            for (const value_type& val: data)
                __add(val, tmp);

            tmp.swap(data);
        }

        void makeHeap()
        {
            for (int idx = (data.size() - 1) / 2; idx >= 0; idx--)
                heapify(static_cast<size_t>(idx));
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

    void makeHeapTest_Rebuild()
    {
        constexpr size_t count = 50;
        MinHeap<int> minHeap;

        for (size_t n = 0; n < count; ++n)
            minHeap.data.push_back(getRandomUniqueInt(0, 100));

        minHeap.makeHeap_Rebuild();
        minHeap.print();

        std::cout << std::boolalpha << minHeap.isValid() << std::endl;
        std::cout << std::boolalpha << std::is_heap(minHeap.data.cbegin(), minHeap.data.cend(), std::greater<>()) << std::endl;
    }

    void makeHeapTest()
    {
        constexpr size_t count = 50;
        MinHeap<int> minHeap;

        for (size_t n = 0; n < count; ++n)
            minHeap.data.push_back(getRandomUniqueInt(0, 100));

        minHeap.makeHeap();
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


// TODO:
//   1. validate() / for Min and MAX
//   2. min / max strategy? function (make comparator --> class Type)
//   3. MAKE_HEAP | Run perf tests
//   4. pop()
//   5. add option to Limit size of the heap (to find N-Max elements)

void Heap::TestAll()
{
    // Tests::addTest();
    // Tests::addTest2();

    Tests::makeHeapTest();
    Tests::makeHeapTest_Rebuild();

    // Tests::Check_Parent_Nodes();

    /*
    std::vector<int> numbers {0,1,2,3,4,5,6,7,8,9, 10};
    Tests::_is_max_heap(numbers);
    */

};
