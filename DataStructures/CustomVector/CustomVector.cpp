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

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        static constexpr size_t initialCapacity { 10 };
        static constexpr size_t growthFactor { 2 };

    private:
        /** Elements collection block: **/
        pointer data { nullptr };

        /** Capacity: **/
        size_t capacity { 0 };

        /** Count of elements: **/
        size_t size { 0 };

        /** The allocator to use for allocating and deallocating chunks: **/
        Allocator allocator;

    private:
        void increaseCapacity()
        {
            std::cout << "* * * * increaseCapacity() * * * *" << std::endl;

            capacity *= growthFactor;
            pointer newData { allocator.allocate(capacity) };

            // std::move(data, data + size, newData);
            std::uninitialized_move_n(data, size, newData);
            std::swap(data, newData);

            std::destroy_n(newData, size);
            allocator.deallocate(newData, capacity);
        }

    public:
        // Vector(size_t s): data { allocator.allocate(s) }, capacity { s }  {
        // }

        explicit Vector(size_t s = initialCapacity) {
            capacity = s > 0 ? s : initialCapacity;
            data = allocator.allocate(s);

            std::cout << "capacity = " << capacity << ", size = " << size << std::endl;
        }

        ~Vector() {
            /** Invoke destructors for all contained objects: **/
            std::destroy_n(data, size);

            /** Deallocate all memory: **/
            allocator.deallocate(data, capacity);
        }

    public:
        [[nodiscard]]
        Type& operator[] (size_t index) {
            // TODO: Check size & index ???
            return this->data[index];
        }

        [[nodiscard]]
        inline size_t Size() const noexcept {
            return size;
        }

        [[nodiscard]]
        inline size_t Capacity() const noexcept {
            return capacity;
        }

        [[nodiscard]]
        inline bool Empty() const noexcept {
            return 0 == size;
        }

        void push_back(const Type& v) {
            if (size >= capacity)
                increaseCapacity();
            this->data[size++] = v;
        }

        template<typename ... Args>
        void emplace_back(Args&& ...  params) {
            if (size >= capacity)
                increaseCapacity();
            /** Construct element in place: **/
            new (data + size) object_type{ std::forward<Args>(params)... };
            ++size;
        }


        void swap(Vector &other) noexcept {
            std::swap(this->data, other.data);
            std::swap(this->size, other.size);
        }

        static void swap(Vector &first, Vector &second) noexcept {
            std::swap(first.data, second.data);
            std::swap(first.size, second.size);
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

namespace CustomVector::Testing {

    void IteratorTests() {
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
}

//-----------------------------------------------------------------------------

void CustomVector::TestAll()
{
    // Testing::IteratorTests();
    // Testing::IteratorTest2();


    Vector<Long> data(0);


    for (int i: {1,2,3,4,5}) {
        data.push_back(Long(i));
        // data.emplace_back(i);
    }

    /*
    for (const auto& v: data)
        std::cout << v << std::endl;
    */
};
