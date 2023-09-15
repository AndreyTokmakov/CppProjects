/**============================================================================
Name        : DVector.cpp
Created on  : 15.09.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DVector.cpp
============================================================================**/

#include "DVector.h"

#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>

#include "../Helpers/Long.h"


namespace DVector
{

    template<typename Type,
             typename Allocator = std::allocator<Type>>
    class DVector
    {
        using object_type = Type;
        using pointer = object_type*;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        static constexpr size_t initialCapacity { 10 };
        static constexpr size_t growthFactor { 4 };

    private:
        /** Elements collection block: **/
        pointer data { nullptr };

        /** Capacity: **/
        size_t capacity { 0 };

        /** Index of the element **/
        size_t left { 0 };
        size_t right { 0 };

        /** The allocator to use for allocating and deallocating chunks: **/
        Allocator allocator;

    //private:
    public:

        void growVector()
        {
            const size_t size = right - left - 1;
            const size_t left_old = left, left_center_dist = capacity / 2 - left - 1;

            capacity *= growthFactor;
            left = capacity / 2 - left_center_dist  - 1;
            right = left + size + 1;

            pointer newData { allocator.allocate(capacity) };
            std::uninitialized_move_n(data + left_old + 1, size, newData + left + 1);
            std::swap(data, newData);

            std::destroy_n(newData + left_old + 1, size);
            allocator.deallocate(newData, capacity);
        }

    public:

        explicit DVector(const size_t s = initialCapacity)
        {
            capacity = s > 0 ? s : initialCapacity;
            data = allocator.allocate(capacity);

            right = capacity / 2;
            left = right - 1;   // TODO: check right > 1 ??
        }

        ~DVector()
        {
            if (0 == capacity)
                return;

            /** Invoke destructors for all contained objects: **/
            const size_t size = right - left - 1;
            std::destroy_n(data + left + 1, size);

            /** Deallocate all memory: **/
            allocator.deallocate(data, capacity);
        }

        DVector(const DVector<object_type, Allocator>& other):
                capacity { other.capacity }, left { other.left } , right { other.right }
        {
            data = allocator.allocate(other.capacity);
            std::copy_n(other.data + left + 1, right - left - 1, data + left + 1);
        }

        DVector(DVector<object_type, Allocator>&& other) noexcept:
                data { std::exchange(other.data, nullptr) },
                capacity { std::exchange(other.capacity, 0) },
                left { std::exchange(other.left, 0) },
                right { std::exchange(other.right, 0) } {
            /** **/
        }

        DVector<object_type, Allocator>& operator=(const DVector<object_type, Allocator>& other)
        {
            if (&other != this) {
                DVector localCopy(other);
                DVector::swap(localCopy, *this);
            }
            return *this;
        }

        DVector<object_type, Allocator>& operator=(DVector<object_type, Allocator>&& other) noexcept
        {
            if (&other != this)
            {
                data = std::exchange(other.data, nullptr);
                capacity = std::exchange(other.capacity, 0);
                left = std::exchange(other.left, 0);
                right = std::exchange(other.right, 0);
            }
            return *this;
        }

    public:

        [[nodiscard]]
        object_type& operator[] (size_t index) {
            return this->data[index];
        }

        [[nodiscard]]
        inline size_t Size() const noexcept {
            return right - left - 1;
        }

        [[nodiscard]]
        inline size_t Capacity() const noexcept {
            return capacity;
        }

        [[nodiscard]]
        inline bool Empty() const noexcept {
            return 1 == (right - left);
        }

        void push_back(const object_type& v)
        {
            if (right >= capacity)
                growVector();
            this->data[right++] = v;
        }

        void push_front(const object_type& v)
        {
            if (0 >= left)
                growVector();
            this->data[left--] = v;
        }

        void push_back(object_type&& v)
        {
            if (right >= capacity)
                growVector();
            this->data[right++] = std::move(v);
        }

        void push_front(object_type&& v)
        {
            if (0 >= left)
                growVector();
            this->data[left--] = std::move(v);
        }

        template<typename ... Args>
        object_type& emplace_back(Args&&... params)
        {
            if (right >= capacity)
                growVector();

            // Construct element in place:
            new (data + right) object_type { std::forward<Args>(params)... };
            return data[right++];
        }

        template<typename ... Args>
        object_type& emplace_front(Args&&... params)
        {
            if (0 >= left)
                growVector();

            // Construct element in place:
            new (data + left) object_type { std::forward<Args>(params)... };
            return data[left--];
        }

        void swap(DVector<object_type, Allocator> &other) noexcept
        {
            std::swap(this->data, other.data);
            std::swap(this->left, other.left);
            std::swap(this->right, other.right);
            std::swap(this->capacity, other.capacity);
        }

        static void swap(DVector<object_type, Allocator> &first,
                         DVector<object_type, Allocator> &second) noexcept
        {
            std::swap(first.data, second.data);
            std::swap(first.left, second.left);
            std::swap(first.right, second.right);
            std::swap(first.capacity, second.capacity);
        }

        void printInfo()
        {
            const size_t size = right - left - 1;
            std::cout << "[ capacity = " << capacity << ", size = " << size << " ] "
                      << "[ left: " << left << ", right: " << right << " ]\n";

            for (size_t idx = 0; idx < capacity; ++idx)
                std::cout << '[' << idx << "] = " << data[idx] << std::endl;
        }
    };

    /*
    /// Vector base iterator class
    template <typename Ty, typename Container>
    class VectorBaseIterator {
    private:
        size_t index {0};
        Container& collection;

    public:
        [[maybe_unused]]
        VectorBaseIterator(Container& collection, const size_t index):
                index { index }, collection { collection } {
        }

        bool operator!=(const VectorBaseIterator& other) const noexcept {
            return index != other.index;
        }

        const Ty& operator*() const {
            return collection[index];
        }

        const VectorBaseIterator& operator++() noexcept {
            ++index;
            return *this;
        }

        const VectorBaseIterator operator++(int) noexcept {
            VectorBaseIterator old = *this;
            ++index;
            return old;
        }

        const VectorBaseIterator& operator--() noexcept {
            --index;
            return *this;
        }

        const VectorBaseIterator operator--(int) noexcept {
            VectorBaseIterator old = *this;
            --index;
            return old;
        }

        const VectorBaseIterator& operator+(size_t step) noexcept {
            index += step;
            return *this;
        }

        const VectorBaseIterator& operator-(size_t step) noexcept {
            index -= step;
            return *this;
        }
    };
    */
}

namespace DVector::Tests
{
    void CreateTest()
    {
        DVector<int> vect;
    }

    void PushBack()
    {
        DVector<int> vect;
        vect.push_back(21);
        vect.push_back(22);
        vect.push_back(23);

        vect.printInfo();
    }

    void PushBack_Realloc()
    {
        DVector<int> vect;

        for (int i = 0; i < 50; ++i)
            vect.push_back(100 + i);

        vect.printInfo();
    }

    void PushFront()
    {
        DVector<int> vect;

        vect.push_front(21);
        vect.push_front(22);
        vect.push_front(23);

        vect.printInfo();
    }

    void EstimateDistances()
    {
        DVector<int> vect;

        vect.push_front(21);
        vect.push_front(22);
        vect.push_front(23);

        vect.push_back(33);
        vect.push_back(34);

        vect.printInfo();
        vect.growVector();
        vect.printInfo();
    }


    void DestructorTest()
    {
        DVector<Helpers::Long> vect;

        vect.emplace_back(101);
        vect.emplace_back(102);
        vect.emplace_back(103);

        vect.printInfo();
    }

    void CopyConstructorTests()
    {
        DVector<Helpers::Long> vect1;

        vect1.emplace_back(101);
        vect1.emplace_back(102);
        vect1.emplace_back(103);

        vect1.printInfo();

        DVector<Helpers::Long> vect2 = vect1;

        vect2.printInfo();
    }

    void MoveConstructorTests()
    {
        DVector<Helpers::Long> vect1;

        vect1.emplace_back(5);
        vect1.emplace_back(6);
        vect1.emplace_back(7);

        vect1.emplace_front(4);
        vect1.emplace_front(3);

        // vect1.printInfo();

        DVector<Helpers::Long> vect2 = std::move(vect1);

        vect2.printInfo();
    }
}


// TODO:
//  Test destruction after Reallocation
//  Test Copy_Constructor
//  Test Copy_Assignment
//  Test Move_Constructor
//  Test Move_Assignment

void DVector::TestAll()
{
    // Tests::CreateTest();

    // Tests::PushBack();
    // Tests::PushBack_Realloc();

    // Tests::PushFront();

    // Tests::EstimateDistances();

    // Tests::DestructorTest();


    // Tests::CopyConstructorTests();
    Tests::MoveConstructorTests();
}