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


#include "CustomVector.h"

namespace Utilities
{
    class Long {
        long value {0};

    public:
        Long() {
            std::cout << "Long::Long(DEFAULT)\n";
        }

        Long(long v): value {v} {
            std::cout << "Long::Long(" << value << ")\n";
        }

        ~Long() {
            std::cout << "Long::~Long(" << value << ")\n";
        }

        Long(const Long &obj): value {obj.value} {
            std::cout << "Long::Long(" << value << ") Copy constructor\n";
        }

        Long(Long&& obj) noexcept : value { std::exchange(obj.value, 0)} {
            std::cout << "Long::Long(" << value << ") Move constructor\n";
        }

        Long& operator=(const Long& right) {
            std::cout << "[Copy assignment operator from Integer -> Integer]" << std::endl;
            if (this != &right)
                value = right.value;
            return *this;
        }

        Long& operator=(Long&& right) noexcept {
            std::cout << "[Move assignment operator]" << std::endl;
            if (this != &right)
                this->value = std::exchange(right.value, 0);
            return *this;
        }

        /*
        Long(const Long& l) = delete;
        Long& operator=(const Long& l) = delete;
        Long(Long&& l) noexcept = delete;
        Long& operator=(Long&& l) noexcept = delete;
        */

        friend Long operator+(const Long& l1, const Long& l2);

        friend std::ostream& operator<<(std::ostream& stream, const Long& obj);
    };

    class Integer {
        int value {0};

    public:
        Integer() = default;

        Integer(int v): value {v} {
        }

        ~Integer() {
        }

        Integer(const Integer &obj): value {obj.value} {
        }

        Integer(Integer&& obj) noexcept : value { std::exchange(obj.value, 0)} {
        }

        Integer& operator=(const Integer& right) {
            if (this != &right)
                value = right.value;
            return *this;
        }

        Integer& operator=(Integer&& right) noexcept {
            if (this != &right)
                this->value = std::exchange(right.value, 0);
            return *this;
        }

        friend Integer operator+(const Integer& l1, const Integer& l2);
        friend std::ostream& operator<<(std::ostream& stream, const Integer& obj);
    };

    std::ostream& operator<<(std::ostream& stream, const Integer& obj) {
        stream << obj.value;
        return stream;
    }

    Integer operator+(const Integer& l1, const Integer& l2) {
        return Integer(l1.value + l2.value);
    }

    std::ostream& operator<<(std::ostream& stream, const Long& obj) {
        stream << obj.value;
        return stream;
    }

    Long operator+(const Long& l1, const Long& l2) {
        return Long(l1.value + l2.value);
    }
}

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
        Vector<Utilities::Integer> data(0);

        for (int i: {1,2,3})
            data.emplace_back(i);

        for (const auto& v: data)
            std::cout << v << std::endl;

    }

    void IteratorTest2() {
        Vector<Utilities::Long> data;
        for (int i: {1, 2, 3, 4 ,5})
            data.emplace_back(i);

        VectorBaseIterator<Utilities::Long, Vector<Utilities::Long>> iter {data, 0 };
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
    Testing::IteratorTest2();

    /*
    //  Vector<Utilities::Long> data(0);
    std::vector<Utilities::Long> data;

    for (int i: {1,2,3})
        data.emplace_back(i);

    for (const auto& v: data)
        std::cout << v << std::endl;
    */
};
