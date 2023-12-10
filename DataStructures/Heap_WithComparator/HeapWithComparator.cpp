/**============================================================================
Name        : HeapWithComparator.cpp
Created on  : 28.09.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : HeapWithComparator
============================================================================**/

#include "HeapWithComparator.h"

#include <iostream>
#include <vector>
#include <string_view>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <chrono>


namespace HeapWithComparator
{
    template<typename __Type, typename __Cmp>
    struct Heap
    {
        using value_type = __Type;

        __Cmp comparator {};

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

            while (index > 0 && comparator(heap[index], heap[parentIndex]))
            {
                std::swap(heap[parentIndex], heap[index]);
                index = parentIndex;
                parentIndex = (index - 1) / 2;
            }
        }

        Heap& add(value_type value)
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

                if (data.size() > left && comparator(data[left], data[parent]))
                    parent = left;
                if (data.size()  > right && comparator(data[right], data[parent]))
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

        value_type pop()
        {
            const value_type top = data.front();
            data.erase(data.begin());

            heapify(0);         // FIXME: Not working
            // heapifyRecursive(0);   // FIXME: Not working
            // makeHeap();

            return top;
        }

        [[nodiscard("Dont ignore the value")]]
        bool isValid() const noexcept {
            // index of the parent of the last element ((SIZE - 1) - 1) / 2
            const size_t lastParentIdx = (data.size() - 2) / 2;

            // Check MaxHeap condition expect the 'last parent'. Will be checked at the end
            for (size_t idx = 0; idx < lastParentIdx; ++idx) {
                if (comparator(data[idx * 2 + 1], data[idx]) || comparator(data[idx * 2 + 2], data[idx]))
                    return false;
            }

            if (comparator(data[lastParentIdx * 2 + 1], data[lastParentIdx]))
                return false;
            return data.size() <= (lastParentIdx * 2 + 2) || comparator(data[lastParentIdx], data[lastParentIdx * 2 + 2]);
        }

        void print() const noexcept
        {
            for (const value_type& val: data)
                std::cout << val << ' ';
            std::cout << std::endl;
        }
    };
};

namespace Tests
{
    using namespace HeapWithComparator;

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

    void addTest()
    {
        Heap<int, std::less<>> heap;
        heap.add(10).add(9).add(8).add(7).add(6).add(5).add(4).add(2);

        heap.print();
        std::cout << std::boolalpha << heap.isValid() << std::endl;
        std::cout << std::boolalpha << std::is_heap(heap.data.cbegin(), heap.data.cend(), std::greater<>()) << std::endl;
    }


    void addTest2_Less()
    {
        constexpr size_t count = 50;
        Heap<int, std::less<>> heap;

        for (size_t n = 0; n < count; ++n)
            heap.add(getRandomUniqueInt(0, 100));

        heap.print();
        std::cout << std::boolalpha << heap.isValid() << std::endl;
        std::cout << std::boolalpha << std::is_heap(heap.data.cbegin(), heap.data.cend(), std::greater<>()) << std::endl;
    }

    void addTest2_Greater()
    {
        constexpr size_t count = 50;
        Heap<int, std::greater<>> heap;

        for (size_t n = 0; n < count; ++n)
            heap.add(getRandomUniqueInt(0, 100));

        heap.print();
        std::cout << std::boolalpha << heap.isValid() << std::endl;
        std::cout << std::boolalpha << std::is_heap(heap.data.cbegin(), heap.data.cend(), std::less<>()) << std::endl;
    }

    void makeHeapTest_Greater()
    {
        uint32_t testCount = 10;
        while (--testCount)
        {
            const size_t count = getRandomUniqueInt();
            Heap<int, std::greater<>> heap;

            for (size_t n = 0; n < count; ++n)
                heap.data.push_back(getRandomUniqueInt(0, 10 * count));

            heap.makeHeap();
            // heap.print();

            if (!heap.isValid() || !std::is_heap(heap.data.cbegin(), heap.data.cend(), std::less<>()) )
            {
                std::cout << "ERROR" << std::endl;

                // std::cout << std::boolalpha << heap.isValid() << std::endl;
                // std::cout << std::boolalpha << std::is_heap(heap.data.cbegin(), heap.data.cend(), std::less<>()) << std::endl;
            }

            std::cout << "OK\n";
        }
    }

    void makeHeapTest_Less()
    {
        constexpr size_t count = 25;
        Heap<int, std::less<>> heap;

        for (size_t n = 0; n < count; ++n)
            heap.data.push_back(getRandomUniqueInt(0, 10 * count));

        heap.makeHeap();
        heap.print();

        std::cout << std::boolalpha << heap.isValid() << std::endl;
        std::cout << std::boolalpha << std::is_heap(heap.data.cbegin(), heap.data.cend(), std::greater<>()) << std::endl;
    }
}



void HeapWithComparator::TestAll()
{
    // Tests::addTest();
    // Tests::addTest2_Less();
    // Tests::addTest2_Greater();

    Tests::makeHeapTest_Greater();
    // Tests::makeHeapTest_Less();
};