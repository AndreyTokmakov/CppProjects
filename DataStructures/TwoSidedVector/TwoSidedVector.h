/**============================================================================
Name        : TwoSidedVector.h
Created on  : 21.09.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TwoSidedVector
============================================================================**/

#ifndef CPPPROJECTS_TWOSIDEDVECTOR_H
#define CPPPROJECTS_TWOSIDEDVECTOR_H

#include <memory>
#include <algorithm>
#include <utility>
#include <format>

namespace TwoSidedVector
{
    template<typename _Ty>
    struct Allocator: std::allocator<_Ty>
    {
        _Ty* allocate(size_t size)
        {
            return new _Ty[size];
        }

        void deallocate(_Ty* ptr, size_t)
        {
            delete[] ptr;
        }
    };


    template<typename Type,
            typename Allocator = Allocator<Type>>
    class DVector
    {
        using object_type = Type;
        using pointer = object_type*;
        using size_type = size_t;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

        static constexpr size_type initialCapacity { 10 };
        static constexpr size_type growthFactor { 4 };

    private:
        /** Elements collection block: **/
        pointer _data { nullptr };

        /** Capacity: **/
        size_type _capacity { 0 };

        /** Index of the element **/
        size_type _left { 0 };
        size_type _right { 0 };

        /** The allocator to use for allocating and deallocating chunks: **/
        Allocator allocator;

    private:

        void growVector()
        {
            const size_type size = _right - _left - 1;
            const size_type left_old = _left, left_center_dist = _capacity / 2 - _left - 1;

            _capacity *= growthFactor;
            _left = _capacity / 2 - left_center_dist  - 1;
            _right = _left + size + 1;

            pointer newData { allocator.allocate(_capacity) };
            std::uninitialized_move_n(_data + left_old + 1, size, newData + _left + 1);
            std::swap(_data, newData);

            std::destroy_n(newData + left_old + 1, size);
            allocator.deallocate(newData, _capacity);
        }

        void destroy()
        {
            const size_type size = _right - _left - 1;

            /** Invoke destructors for all contained objects: **/
            std::destroy_n(_data + _left + 1, size);
        }

    public:

        explicit DVector(const size_type s = initialCapacity)
        {
            _capacity = s > 0 ? s : initialCapacity;
            _data = allocator.allocate(_capacity);

            _right = _capacity / 2;
            _left = _right - 1;   // TODO: check right > 1 ??
        }

        ~DVector()
        {
            if (0 == _capacity)
                return;

            /** Invoke destructors for all contained objects: **/
            destroy();

            /** Deallocate all memory: **/
            allocator.deallocate(_data, _capacity);
        }

        DVector(const DVector<object_type, Allocator>& other):
                _capacity { other._capacity }, _left { other._left } , _right { other._right }
        {
            _data = allocator.allocate(other._capacity);
            std::copy_n(other._data + _left + 1, _right - _left - 1, _data + _left + 1);
        }

        DVector(DVector<object_type, Allocator>&& other) noexcept:
                _data { std::exchange(other._data, nullptr) },
                _capacity { std::exchange(other._capacity, 0) },
                _left { std::exchange(other._left, 0) },
                _right { std::exchange(other._right, 0) } {
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
                _data = std::exchange(other._data, nullptr);
                _capacity = std::exchange(other._capacity, 0);
                _left = std::exchange(other._left, 0);
                _right = std::exchange(other._right, 0);
            }
            return *this;
        }

    public:

        [[nodiscard]]
        object_type& front() const noexcept {
            return this->_data[_left + 1];
        }

        [[nodiscard]]
        object_type& back() const noexcept {
            return this->_data[_right - 1];
        }

        [[nodiscard]]
        object_type& operator[] (size_type index) const {
            return this->_data[index + _left + 1];
        }

        [[nodiscard]]
        object_type& at(size_type index) const {
            if (index >= (_right - _left - 1))
                throw std::out_of_range(std::format("{} index is out of range", index));
            return this->_data[index + _left + 1];
        }

        [[nodiscard]]
        inline size_type size() const noexcept {
            return 0 != _capacity ? _right - _left - 1 : 0;
        }

        [[nodiscard]]
        inline size_type capacity() const noexcept {
            return _capacity;
        }

        [[nodiscard]]
        inline size_type front_capacity() const noexcept {
            return _left + 1;
        }

        [[nodiscard]]
        inline size_type back_capacity() const noexcept {
            return _capacity - _right;
        }

        [[nodiscard]]
        inline bool empty() const noexcept {
            return 0 == _capacity || 1 == (_right - _left);
        }

        [[nodiscard]]
        inline pointer data() const noexcept {
            return _data + _left + 1;
        }

        inline void clear() noexcept
        {
            /** Invoke destructors for all contained objects: **/
            destroy();

            _right = _capacity / 2;
            _left = _right - 1;
        }

        object_type& push_back(const object_type& v)
        {
            if (_right >= _capacity)
                growVector();
            this->_data[_right] = v;
            return _data[_right++];
        }

        object_type& push_back(object_type&& v)
        {
            if (_right >= _capacity)
                growVector();
            this->_data[_right] = std::move(v);
            return _data[_right++];
        }

        object_type& push_front(const object_type& v)
        {
            if (0 >= _left)
                growVector();
            this->_data[_left] = v;
            return _data[_left--];
        }

        object_type& push_front(object_type&& v)
        {
            if (0 >= _left)
                growVector();
            this->_data[_left] = std::move(v);
            return _data[_left--];
        }

        void pop_back()
        {
            _data[--_right].~object_type();
        }

        void pop_front()
        {
            _data[++_left].~object_type();
        }

        template<typename ... Args>
        object_type& emplace_back(Args&&... params)
        {
            if (_right >= _capacity)
                growVector();

            // Construct element in place:
            new (_data + _right) object_type { std::forward<Args>(params)... };
            return _data[_right++];
        }

        template<typename ... Args>
        object_type& emplace_front(Args&&... params)
        {
            if (0 >= _left)
                growVector();

            // Construct element in place:
            new (_data + _left) object_type { std::forward<Args>(params)... };
            return _data[_left--];
        }

        void swap(DVector<object_type, Allocator> &other) noexcept
        {
            std::swap(this->_data, other._data);
            std::swap(this->_left, other._left);
            std::swap(this->_right, other._right);
            std::swap(this->_capacity, other._capacity);
        }

        static void swap(DVector<object_type, Allocator> &first,
                         DVector<object_type, Allocator> &second) noexcept
        {
            std::swap(first._data, second._data);
            std::swap(first._left, second._left);
            std::swap(first._right, second._right);
            std::swap(first._capacity, second._capacity);
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
};

namespace TwoSidedVector
{
    void TestAll();
}


#endif //CPPPROJECTS_TWOSIDEDVECTOR_H
