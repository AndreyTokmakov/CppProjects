//============================================================================
// Name        : Custom_Vector.h
// Created on  : 2021-11-03.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ custom vector
//============================================================================

#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>


#include "CustomVector.h"
#include "../Utilities/Integer.h"
#include "../Utilities/Long.h"

namespace CustomVector {

    template<typename Type,
             typename Allocator = std::allocator<Type>>
    class Vector {
    private:
        using object_type = Type;
        using pointer = object_type*;
        using size_type = size_t;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        static constexpr size_type initialCapacity { 10 };
        static constexpr size_type growthFactor { 2 };

    private:
        /** Elements collection block: **/
        pointer data { nullptr };

        /** Capacity: **/
        size_type capacity { 0 };

        /** Count of elements: **/
        size_t size { 0 };

        /** The allocator to use for allocating and deallocating chunks: **/
        Allocator allocator;

    private:

        void grow_vector(const size_type capacityDesired)
        {
            /** Allocate a new chunk of memory. **/
            pointer newData { allocator.allocate(capacityDesired) };

            /** Move all data from original location to the new one. **/
            std::uninitialized_move_n(data, size, newData);

            /** Swap data pointers. **/
            std::swap(data, newData);

            /** Call destructors for the old data and de-allocate old storage **/
            std::destroy_n(newData, size);
            allocator.deallocate(newData, capacity);

            /** set capacity to the new value **/
            capacity = capacityDesired;
        }

        void increaseCapacity()
        {
            grow_vector(capacity * growthFactor);
        }

    public:

        void reserve(const size_type capacityDesired)
        {
            if (capacityDesired > capacity)
                grow_vector(capacityDesired);
        }

        // Vector(size_t s): data { allocator.allocate(s) }, capacity { s }  {
        // }

        explicit Vector(const size_type s = initialCapacity)
        {
            capacity = s > 0 ? s : initialCapacity;
            data = allocator.allocate(capacity);
        }

        Vector(const std::initializer_list<object_type>& args) {
            for (const auto v: args)
                push_back(v);
        }

        ~Vector()
        {
            /** Invoke destructors for all contained objects: **/
            std::destroy_n(data, size);

            /** Deallocate all memory: **/
            allocator.deallocate(data, capacity);
        }

        Vector(const Vector<object_type, Allocator>& other):
                capacity { other.capacity }, size { other.size }
        {
            std::cout << "Vector(copy constructor)\n";
            data = allocator.allocate(other.capacity);
            std::copy_n(other.data, size, data);
        }

        Vector(Vector<object_type, Allocator>&& other) noexcept:
                data { std::exchange(other.data, nullptr)},
                capacity { std::exchange(other.capacity, 0) },
                size { std::exchange(other.size, 0) }
        {
            // std::cout << "Vector(move constructor)\n";
        }

        Vector<object_type, Allocator>& operator=(const Vector<object_type, Allocator>& other)
        {
            Vector copy(other);
            Vector::swap(copy, *this);
            return *this;
        }

        Vector<object_type, Allocator>& operator=(Vector<object_type, Allocator>&& other) noexcept
         {
            data = std::exchange(other.data, nullptr);
            capacity = std::exchange(other.capacity, 0);
            size = std::exchange(other.size, 0);
            return *this;
        }

    public:
        [[nodiscard]]
        object_type& operator[] (size_type index) {
            return this->data[index];
        }

        [[nodiscard]]
        inline size_type Size() const noexcept {
            return size;
        }

        [[nodiscard]]
        inline size_type Capacity() const noexcept {
            return capacity;
        }

        [[nodiscard]]
        inline bool Empty() const noexcept {
            return 0 == size;
        }

        void push_back(const object_type& v)
        {
            if (size >= capacity)
                increaseCapacity();

            this->data[size] = v;
            ++size;
        }

        void push_back(object_type&& v)
        {
            if (size >= capacity)
                increaseCapacity();

            this->data[size] = std::move(v);
            ++size;
        }

        template<typename ... Args>
        object_type& emplace_back(Args&& ...  params)
        {
            if (size >= capacity)
                increaseCapacity();

            /** Construct element in place: **/
            new (data + size) object_type { std::forward<Args>(params)... };
            return data[size++];
        }

        void swap(Vector<object_type, Allocator> &other) noexcept
        {
            std::swap(this->data, other.data);
            std::swap(this->size, other.size);
            std::swap(this->capacity, other.capacity);
        }

        static void swap(Vector<object_type, Allocator> &first,
                         Vector<object_type, Allocator> &second) noexcept
        {
            std::swap(first.data, second.data);
            std::swap(first.size, second.size);
            std::swap(first.capacity, second.capacity);
        }
    };

    /** Vector base iterator class: **/
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


    template <typename T>
    using vector_iterator = VectorBaseIterator<T, Vector<T>>;

    template <typename T>
    using vector_const_iterator = VectorBaseIterator<T, const Vector<T>>;


    template <typename T>
    inline vector_iterator<T> begin(Vector<T>& collection) {
        return vector_iterator<T>(collection, 0);
    }

    template <typename T>
    inline vector_iterator<T> end(Vector<T>& collection) {
        return vector_iterator<T>(collection, collection.Size());
    }


    template <typename T>
    inline vector_const_iterator<T> begin(const Vector<T>& collection) {
        return vector_const_iterator<T>(collection, 0);
    }

    template <typename T>
    inline vector_const_iterator<T> end(const Vector<T>& collection) {
        return vector_const_iterator<T>(collection, collection.Size());
    }
};

//-----------------------------------------------------------------------------

namespace CustomVector::Testing
{
    void AccessElements()
    {
        Vector<Integer> values (0);
        for (int i: {1,2,3})
            values.emplace_back(i);

        for (int i = 0; i < 3; ++i)
            std::cout << values[i] << ' ';
        std::cout << std::endl;
    }

    void AccessElementsConst()
    {
        Vector<int> values (std::initializer_list<int>{1,2,3,4,5});

        for (int i = 0; i < 3; ++i)
            std::cout << values[i] << ' ';
        std::cout << std::endl;
    }

    void IteratorTests()
    {
        Vector<Integer> data(0);

        for (int i: {1,2,3})
            data.emplace_back(i);

        for (const auto& v: data)
            std::cout << v << std::endl;

    }

    void IteratorTest2() {
        Vector<Long> data;
        for (int i: {1, 2, 3, 4 ,5})
            data.emplace_back(i);

        VectorBaseIterator<Long, Vector<Long>> iter {data, 0 };
        // vector_iterator<Utilities::Integer> iter {data, 0 };

        std::cout << *iter << std::endl;
        ++iter;
        std::cout << *iter << std::endl;
        iter++;
        std::cout << *iter << std::endl;
    }

    void PushBack()
    {
        Vector<int> data;

        for (int i = 0; i < 11; ++i)
        {
            data.emplace_back(i);
        }
    }

    void Reserve()
    {
        Vector<int> data;
        for (int i = 0; i < 5; ++i)
            data.emplace_back(i);

        std::cout << "size = " << data.Size() << ", capacity: " << data.Capacity() << std::endl;
        data.reserve(20);

        std::cout << "size = " << data.Size() << ", capacity: " << data.Capacity() << std::endl;
        for (int i = 0; i < 5; ++i) {
            if (i != data[i]) {
                std::cerr << "ERROR: " << i << " != " << data[i] << std::endl;
                return;
            }
        }

        std::cout << "OK\n";
    }

    void Copy_Constructor()
    {
        Vector<Long> original;
        for (int i = 0; i < 3; ++i)
            original.emplace_back(i);

        Vector<Long> copy = original;

        std::cout << "------------------------ original --------------------------- \n";
        for (const auto& v: original)
            std::cout << v << std::endl;

        std::cout << "------------------------ copy --------------------------- \n";
        for (const auto& v: copy)
            std::cout << v << std::endl;
    }

    void Move_Constructor()
    {
        Vector<Long> original;
        for (int i: {1, 2, 3, 4 ,5})
            original.emplace_back(i);

        Vector<Long> movedTo = std::move(original);

        std::cout << "------------------------ original --------------------------- \n";
        for (const auto& v: original)
            std::cout << v << std::endl;

        std::cout << "------------------------ movedTo --------------------------- \n";
        for (const auto& v: movedTo)
            std::cout << v << std::endl;
    }

    void CopyVector()
    {
        Vector<Long> original;
        for (int i: {1, 2, 3, 4 ,5})
            original.emplace_back(i);

        Vector<Long> movedTo = std::move(original);

        std::cout << "------------------------ original --------------------------- \n";
        for (const auto& v: original)
            std::cout << v << std::endl;

        std::cout << "------------------------ movedTo --------------------------- \n";
        for (const auto& v: movedTo)
            std::cout << v << std::endl;
    }


    void MoveVector()
    {
        Vector<Long> data;
        for (int i: {1, 2, 3, 4 ,5})
            data.emplace_back(i);

        Vector<Long> data1 = std::move(data);

        for (const auto& v: data)
            std::cout << v << std::endl;

        std::cout << std::endl;
        for (const auto& v: data1)
            std::cout << v << std::endl;
    }
}

//-----------------------------------------------------------------------------

void CustomVector::TestAll()
{

    // Testing::AccessElements();
    // Testing::AccessElementsConst();

    // Testing::PushBack();
    Testing::Reserve();


    // Testing::IteratorTests();
    // Testing::IteratorTest2();

    // Testing::Copy_Constructor();
    // Testing::Move_Constructor();

    // Testing::CopyVector();
    // Testing::MoveVector();



    /*
    Vector<Long> data(0);
    // std::vector<Long> data(0);

    for (int i: {1,2,3,4,5}) {
        // data.push_back(Long(i));
        data.emplace_back(i);
    }

    for (const auto& v: data)
        std::cout << v << std::endl;
    */
};
