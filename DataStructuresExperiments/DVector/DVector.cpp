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
#include <deque>
#include <memory>
#include <utility>
#include <algorithm>
#include <filesystem>


/** For testing only: **/
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <random>
#include <fstream>
#include <format>

#include "Wrapper.h"

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

namespace DVector
{

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
}

// TODO: Create lib/module for TestUtils
namespace Utilities
{
    std::random_device randomDevice {};
    std::mt19937 generator = std::mt19937 { randomDevice() };

    int getRandomIntInRange(int start = 0, int end = 100)
    {
        std::uniform_int_distribution intDistribution { start, end  };
        return intDistribution(generator);
    }

    int getRandomUniqueInt(int start = 0, int end = 100)
    {
        static std::unordered_set<int> uniqueInts;
        std::uniform_int_distribution intDistribution { start, end  };

        while (true) {
            const int number = intDistribution(generator);
            if (uniqueInts.insert(number).second)
                return number;
        }
    }

    [[nodiscard]]
    std::deque<int> getRandomIntegerDeque(size_t size)
    {
        std::deque<int> ints;
        while (size--)
            ints.push_back(getRandomIntInRange(0, static_cast<int>(size * 2)));
        return ints;
    }

    [[nodiscard]]
    std::deque<int> getRandomIntegerUniqueDeque(size_t size)
    {
        std::deque<int> ints;
        for (size_t i = 0; i < size; ++i)
            ints.push_back(getRandomUniqueInt(0, static_cast<int>(size * 2)));
        return ints;
    }

    template<typename _Ty1, typename _Ty2>
    void assertEquals(const _Ty1& a, const _Ty2& b)
    {
        if (a != b) {
            std::cerr << "Error: " << a << " != " << b << std::endl;
            std::terminate();
        }
    }

    template<typename _Ty>
    void assertContent(const std::deque<_Ty>& contentExpected,
                       const DVector::DVector<_Ty>& vector)
    {
        if (contentExpected.size() != vector.size())
        {
            std::cerr << "ERROR: Size mismatch: " << contentExpected.size() << " != " << vector.size() << "\n";
            std::terminate();
        }

        for (size_t idx = 0; idx < contentExpected.size(); ++idx)
            assertEquals(vector[idx],  contentExpected[idx]);
    }

    template<typename _Ty>
    void assertEquals(const DVector::DVector<_Ty>& first,
                      const DVector::DVector<_Ty>& second)
    {
        assertEquals(first.size(), second.size());
        assertEquals(first.capacity(), second.capacity());
        assertEquals(first.empty(), second.empty());
        assertEquals(first.front_capacity(), second.front_capacity());
        assertEquals(first.back_capacity(), second.back_capacity());

        for (size_t idx = 0; idx < first.size(); ++idx)
            assertEquals(first[idx],  second[idx]);
    }
}

namespace DVector::Tests
{
    using namespace Utilities;

    void DestructorTest()
    {
        DVector<Helpers::Integer> dVector;

        dVector.emplace_back(101);
        dVector.emplace_back(102);
        dVector.emplace_back(103);
    }


    void Front_Back_CapacityTests()
    {
        DVector<int> dVector;

        dVector.push_back(3);
        dVector.push_back(4);
        dVector.push_back(5);
        dVector.push_front(2);
        dVector.push_front(1);

        std::cout << std::boolalpha
                  << (dVector.capacity() == dVector.size() + dVector.front_capacity() + dVector.back_capacity())
                  << std::endl;

        std::cout << "Size: " << dVector.size()
                  << ", Front: " << dVector.front_capacity()
                  << ", Back: " << dVector.back_capacity()
                  << std::endl;

        // dVector.printInfo();
    }
}

namespace DVector::Tests::CapacityTests
{
    void TestInitialCapacity()
    {
        DVector<int> dVector;

        assertEquals(10UL, dVector.capacity());
        assertEquals(5UL, dVector.front_capacity());
        assertEquals(5UL, dVector.back_capacity());
    }

    void TestCapacityWithConstructor()
    {
        DVector<int> dVector(30);
        assertEquals(30UL, dVector.capacity());
        assertEquals(15UL, dVector.front_capacity());
        assertEquals(15UL, dVector.back_capacity());
    }

    void CapacityAfterReallocation_PushBack()
    {
        DVector<int> dVector;
        for (size_t i = 0; i < 15; ++i)
            dVector.push_back(i);

        assertEquals(40UL, dVector.capacity());
        assertEquals(20UL, dVector.front_capacity());
        assertEquals(5UL, dVector.back_capacity());
    }

    void CapacityAfterReallocation_PushFront()
    {
        DVector<int> dVector;
        for (size_t i = 0; i < 15; ++i)
            dVector.push_front(i);

        assertEquals(40UL, dVector.capacity());
        assertEquals(5UL, dVector.front_capacity());
        assertEquals(20UL, dVector.back_capacity());
    }
}

namespace DVector::Tests::AtMethodMethodTests
{
    void GetElement()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(5);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        for (size_t idx = 0; idx < dVector.size() ; ++idx)
            assertEquals(dVector.at(idx), testValues.at(idx));
    }

    void GetElement_Reallocation()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(55);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        for (size_t idx = 0; idx < dVector.size() ; ++idx)
            assertEquals(dVector.at(idx), testValues.at(idx));
    }

    void OutOufRange()
    {
        DVector<int> dVector;
        for (int i = 0; i < 5; ++i)
            dVector.push_back(i);

        try {
            [[maybe_unused]] auto x = dVector.at(10);
            std::cerr << "Error: Exception shall be thrown\n";
        }
        catch (const std::exception& exc) {
        }
    }
}

namespace DVector::Tests::PopBackMethodTests
{
    void PopBack()
    {
        std::deque<int> testValues {1, 2, 3, 4, 5};
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        dVector.pop_back();
        testValues.pop_back();

        assertEquals(testValues.back(), dVector.back());
        assertEquals(testValues.front(), dVector.front());
        assertEquals(testValues.size(), dVector.size());
        assertContent(testValues, dVector);
    }

    void PopBack_Reallocation()
    {
        std::deque<int> testValues {1,2,3,4,5,7,8,9,10,11,12,13,14,15};
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        dVector.pop_back();
        testValues.pop_back();

        assertEquals(testValues.back(), dVector.back());
        assertEquals(testValues.front(), dVector.front());
        assertEquals(testValues.size(), dVector.size());
        assertEquals(testValues.empty(), dVector.empty());
        assertContent(testValues, dVector);
    }


    void PopBack_UntilEmpty()
    {
        std::deque<int> testValues {1,2,3,4,5,7,8,9,10,11,12,13,14,15};
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        while (!dVector.empty())
        {
            assertEquals(testValues.back(), dVector.back());
            assertEquals(testValues.front(), dVector.front());
            assertEquals(testValues.size(), dVector.size());
            assertEquals(testValues.empty(), dVector.empty());
            assertContent(testValues, dVector);

            dVector.pop_back();
            testValues.pop_back();
        }
    }
}

namespace DVector::Tests::PopFrontMethodTests
{
    void PopFront()
    {
        std::deque<int> testValues {1, 2, 3, 4, 5};
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        dVector.pop_front();
        testValues.pop_front();

        assertEquals(testValues.back(), dVector.back());
        assertEquals(testValues.front(), dVector.front());
        assertEquals(testValues.size(), dVector.size());
        assertContent(testValues, dVector);
    }

    void PopFront_Reallocation()
    {
        std::deque<int> testValues {1,2,3,4,5,7,8,9,10,11,12,13,14,15};
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        dVector.pop_front();
        testValues.pop_front();

        assertEquals(testValues.back(), dVector.back());
        assertEquals(testValues.front(), dVector.front());
        assertEquals(testValues.size(), dVector.size());
        assertEquals(testValues.empty(), dVector.empty());
        assertContent(testValues, dVector);
    }

    void PopFront_UntilEmpty()
    {
        std::deque<int> testValues {1,2,3,4,5,7,8,9,10,11,12,13,14,15};
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        while (!dVector.empty())
        {
            assertEquals(testValues.back(), dVector.back());
            assertEquals(testValues.front(), dVector.front());
            assertEquals(testValues.size(), dVector.size());
            assertEquals(testValues.empty(), dVector.empty());
            assertContent(testValues, dVector);

            dVector.pop_front();
            testValues.pop_front();
        }
    }
}

namespace DVector::Tests::BackMethodTests
{
    void CheckBack_AfterPushBack()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(4);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.back(), dVector.back());
    }

    void CheckBack_AfterPushFront()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(4);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_front(v);

        assertEquals(testValues.front(), dVector.back());
    }

    void CheckBack_AfterPushBack_Reallocation()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(45);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.back(), dVector.back());
    }

    void CheckBack_AfterPushFront_Reallocation()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(45);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_front(v);

        assertEquals(testValues.front(), dVector.back());
    }
}

namespace DVector::Tests::FrontMethodTests
{
    void CheckFront_AfterPushBack()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(4);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.front(), dVector.front());
    }

    void CheckFront_AfterPushFront()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(4);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_front(v);

        assertEquals(testValues.back(), dVector.front());
    }

    void CheckFront_AfterPushBack_Reallocation()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(45);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.front(), dVector.front());
    }

    void CheckFront_AfterPushFront_Reallocation()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(45);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_front(v);

        assertEquals(testValues.back(), dVector.front());
    }
}

namespace DVector::Tests::PushBackTests
{
    void PushBack()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(4);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.size(), dVector.size());
        assertEquals(testValues.empty(), dVector.empty());
        assertEquals(10UL, dVector.capacity());

        assertContent(testValues, dVector);
    }

    void PushBack_RValue()
    {
        const std::deque<std::string> testValues { "I", "II", "III", "IV"};
        DVector<std::string> dVector;
        for (std::string v: testValues)
            dVector.push_back(std::move(v));

        assertEquals(testValues.size(), dVector.size());
        assertEquals(testValues.empty(), dVector.empty());
        assertEquals(10UL, dVector.capacity());

        assertContent(testValues, dVector);
    }

    void PushBack_Realloc()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(15);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.size(), dVector.size());
        assertEquals(testValues.empty(), dVector.empty());
        assertEquals(40UL, dVector.capacity());

        assertContent(testValues, dVector);
    }

    void PushBack_CustomTypes_TODO()
    {
        const std::deque<std::string> testValues { "I", "II", "III", "IV"};
        DVector<std::string> dVector;
        for (const auto& v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.size(), dVector.size());
        assertEquals(testValues.empty(), dVector.empty());
        assertEquals(10UL, dVector.capacity());

        assertContent(testValues, dVector);
    }
}

namespace DVector::Tests::PushFrontTests
{
    void PushFront()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(4);
        DVector<int> dVector;
        for (auto iter = testValues.rbegin(); testValues.rend() != iter; ++iter) {
            dVector.push_front(*iter);
        }

        assertEquals(testValues.size(), dVector.size());
        assertEquals(testValues.empty(), dVector.empty());
        assertEquals(10UL, dVector.capacity());

        assertContent(testValues, dVector);
    }

    void PushFront_Realloc()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(15);
        DVector<int> dVector;
        for (auto iter = testValues.rbegin(); testValues.rend() != iter; ++iter) {
            dVector.push_front(*iter);
        }

        assertEquals(testValues.size(), dVector.size());
        assertEquals(testValues.empty(), dVector.empty());
        assertEquals(40UL, dVector.capacity());

        assertContent(testValues, dVector);
    }

    void PushFront_CustomTypes_TODO()
    {
        const std::deque<std::string> testValues { "I", "II", "III", "IV"};
        DVector<std::string> dVector;
        for (auto iter = testValues.rbegin(); testValues.rend() != iter; ++iter) {
            dVector.push_front(*iter);
        }

        assertEquals(testValues.size(), dVector.size());
        assertEquals(testValues.empty(), dVector.empty());
        assertEquals(10UL, dVector.capacity());

        assertContent(testValues, dVector);
    }
}

namespace DVector::Tests::Constructor
{
    void CreateVectorTest()
    {
        DVector<int> dVector;
        assertEquals(0UL, dVector.size());
        assertEquals(true, dVector.empty());
        assertEquals(10UL, dVector.capacity());
    }

    void CreateVector_CustomCapacity()
    {
        DVector<int> dVector (100);
        assertEquals(0UL, dVector.size());
        assertEquals(true, dVector.empty());
        assertEquals(100UL, dVector.capacity());
    }
}

namespace DVector::Tests::CopyConstructor
{
    void CopyConstructorTests()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(7);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        const DVector<int> dVectorCopy (dVectorOrig);
        assertEquals(dVectorOrig, dVectorCopy);
    }

    void CopyConstructorTests_Realloc()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(50);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        const DVector<int> dVectorCopy (dVectorOrig);
        assertEquals(dVectorOrig, dVectorCopy);
    }
}

namespace DVector::Tests::CopyAssignmentOperator
{
    void CopyAssignmentTests()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(7);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        DVector<int> dVectorCopy;
        dVectorCopy = dVectorOrig;

        assertEquals(dVectorOrig, dVectorCopy);
    }

    void CopyAssignmentTests_Reallocation()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(50);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        DVector<int> dVectorCopy;
        dVectorCopy = dVectorOrig;

        assertEquals(dVectorOrig, dVectorCopy);
    }
}

namespace DVector::Tests::MoveConstructor
{
    void MoveConstructorTests()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(7);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        DVector<int> dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.size());
        assertEquals(true, dVectorOrig.empty());
        assertEquals(0UL, dVectorOrig.capacity());

        assertEquals(testValues.size(), dVectorDest.size());
        assertEquals(false, dVectorDest.empty());
        assertEquals(40UL, dVectorDest.capacity());

        assertContent(testValues, dVectorDest);
    }

    void MoveConstructorTests_Reallocation()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(50);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        DVector<int> dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.size());
        assertEquals(true, dVectorOrig.empty());
        assertEquals(0UL, dVectorOrig.capacity());

        assertEquals(testValues.size(), dVectorDest.size());
        assertEquals(false, dVectorDest.empty());
        assertEquals(160UL, dVectorDest.capacity());

        assertContent(testValues, dVectorDest);
    }
}

namespace DVector::Tests::MoveAssignmentOperator
{
    void MoveAssignmentTests()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(7);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        DVector<int> dVectorDest;
        dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.size());
        assertEquals(true, dVectorOrig.empty());
        assertEquals(0UL, dVectorOrig.capacity());

        assertEquals(testValues.size(), dVectorDest.size());
        assertEquals(false, dVectorDest.empty());
        assertEquals(40UL, dVectorDest.capacity());

        assertContent(testValues, dVectorDest);
    }

    void MoveAssignmentTests_Reallocation()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(50);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        DVector<int> dVectorDest;
        dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.size());
        assertEquals(true, dVectorOrig.empty());
        assertEquals(0UL, dVectorOrig.capacity());

        assertEquals(testValues.size(), dVectorDest.size());
        assertEquals(false, dVectorDest.empty());
        assertEquals(160UL, dVectorDest.capacity());

        assertContent(testValues, dVectorDest);
    }
}

namespace DVector::Tests::Clear
{
    void Basic()
    {
        constexpr size_t size {55};
        DVector<int> dVector;
        for (int i = 0; i < static_cast<int>(size); ++i)
            dVector.push_back(i);

        assertEquals(size, dVector.size());
        assertEquals(false, dVector.empty());

        dVector.clear();

        assertEquals(0UL, dVector.size());
        assertEquals(true, dVector.empty());
    }
}

namespace DVector::Tests::Sizes
{
    void CheckAfterPush_NoReallocation()
    {
        constexpr size_t size {5};
        DVector<int> dVector;
        for (int i = 0; i < static_cast<int>(size); ++i)
            dVector.push_back(i);

        assertEquals(10U, dVector.capacity());
        assertEquals(size, dVector.size());
    }

    void CheckAfterPush_Reallocation()
    {
        constexpr size_t size {15};
        DVector<int> dVector;
        for (int i = 0; i < static_cast<int>(size); ++i)
            dVector.push_back(i);

        assertEquals(40U, dVector.capacity());
        assertEquals(size, dVector.size());
    }

    void CheckAfterMove()
    {
        constexpr size_t size {20};
        DVector<int> dVectorOrig;
        for (int i = 0; i < static_cast<int>(size); ++i)
            dVectorOrig.push_back(i);

        DVector<int> dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.size());
        assertEquals(0UL, dVectorOrig.capacity());
        assertEquals(true, dVectorOrig.empty());

        assertEquals(size, dVectorDest.size());
        assertEquals(40U, dVectorDest.capacity());
        assertEquals(false, dVectorDest.empty());
    }
}

namespace DVector::Tests::IndexOperator
{
    void BasicTest()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(7);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.size(), dVector.size());
        for (size_t idx = 0; idx < testValues.size(); ++idx)
            assertEquals(dVector[idx], testValues[idx]);
    }

    void CheckValues_With_PushBack_and_PushFront()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(8);

        constexpr int N = 3;
        DVector<int> dVector;
        for (size_t idx = N; idx < testValues.size(); ++idx)
            dVector.push_back(testValues[idx]);
        for (int idx = N - 1; idx >= 0; --idx)
            dVector.push_front(testValues[idx]);

        assertEquals(testValues.size(), dVector.size());
        for (size_t idx = 0; idx < testValues.size(); ++idx)
            assertEquals(dVector[idx], testValues[idx]);
    }

    void CheckValues_With_PushBack_and_PushFront_Realloc()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(100);

        constexpr int N = 40;
        DVector<int> dVector;
        for (size_t idx = N; idx < testValues.size(); ++idx)
            dVector.push_back(testValues[idx]);
        for (int idx = N - 1; idx >= 0; --idx)
            dVector.push_front(testValues[idx]);

        assertEquals(testValues.size(), dVector.size());
        for (size_t idx = 0; idx < testValues.size(); ++idx)
            assertEquals(dVector[idx], testValues[idx]);
    }

    void CheckValues_AfterMove()
    {
        const std::deque<int> testValues = getRandomIntegerDeque(100);

        constexpr int N = 40;
        DVector<int> dVectorOrig;
        for (size_t idx = N; idx < testValues.size(); ++idx)
            dVectorOrig.push_back(testValues[idx]);
        for (int idx = N - 1; idx >= 0; --idx)
            dVectorOrig.push_front(testValues[idx]);

        const DVector<int> dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.size());
        assertEquals(0UL, dVectorOrig.capacity());
        assertEquals(true, dVectorOrig.empty());

        assertEquals(testValues.size(), dVectorDest.size());
        for (size_t idx = 0; idx < testValues.size(); ++idx)
            assertEquals(dVectorDest[idx], testValues[idx]);
    }

    void CheckValues_WithCopyConstructor() {
        const std::deque<int> testValues = getRandomIntegerDeque(100);

        constexpr int N = 40;
        DVector<int> dVectorOrig;
        for (size_t idx = N; idx < testValues.size(); ++idx)
            dVectorOrig.push_back(testValues[idx]);
        for (int idx = N - 1; idx >= 0; --idx)
            dVectorOrig.push_front(testValues[idx]);

        const DVector<int> dVectorDest(dVectorOrig);

        for (size_t idx = 0; idx < testValues.size(); ++idx)
            assertEquals(dVectorDest[idx], testValues[idx]);

    }
}

namespace DVector::Tests::Empty
{
    void PushElementsAndCheckIsEmpty()
    {
        constexpr size_t size {55};
        DVector<int> dVector;
        for (int i = 0; i < static_cast<int>(size); ++i)
            dVector.push_back(i);

        assertEquals(size, dVector.size());
        assertEquals(false, dVector.empty());
    }

    void CheckAfterClear()
    {
        constexpr size_t size {55};
        DVector<int> dVector;
        for (int i = 0; i < static_cast<int>(size); ++i)
            dVector.push_back(i);

        assertEquals(size, dVector.size());
        assertEquals(false, dVector.empty());

        dVector.clear();

        assertEquals(0UL, dVector.size());
        assertEquals(true, dVector.empty());
    }

    void CheckAfterMove()
    {
        constexpr size_t size {55};
        DVector<int> dVector1;
        for (int i = 0; i < static_cast<int>(size); ++i)
            dVector1.push_back(i);

        DVector<int> dVector2 = std::move(dVector1);

        assertEquals(0UL, dVector1.size());
        assertEquals(true, dVector1.empty());

        assertEquals(size, dVector2.size());
        assertEquals(false, dVector2.empty());
    }
}

namespace DVector::Tests::Data
{
    void SimpleTest()
    {
        const std::deque<int> testValues = getRandomIntegerUniqueDeque(5);

        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        const int* data = dVector.data();
        for (size_t idx = 0; idx < dVector.size(); ++idx)
            assertEquals(testValues[idx], data[idx]);
    }

    void SimpleTestTwo()
    {
        const std::deque<int> testValues = getRandomIntegerUniqueDeque(10);

        constexpr int N = 2;
        DVector<int> dVector;
        for (size_t idx = N; idx < testValues.size(); ++idx)
            dVector.push_back(testValues[idx]);
        for (int idx = N - 1; idx >= 0; --idx)
            dVector.push_front(testValues[idx]);

        const int* data = dVector.data();
        for (size_t idx = 0; idx < dVector.size(); ++idx)
            assertEquals(testValues[idx], data[idx]);
    }

    void ReallocTest()
    {
        const std::deque<int> testValues = getRandomIntegerUniqueDeque(100);

        constexpr int N = 2;
        DVector<int> dVector;
        for (size_t idx = N; idx < testValues.size(); ++idx)
            dVector.push_back(testValues[idx]);
        for (int idx = N - 1; idx >= 0; --idx)
            dVector.push_front(testValues[idx]);

        const int* data = dVector.data();
        for (size_t idx = 0; idx < dVector.size(); ++idx)
            assertEquals(testValues[idx], data[idx]);
    }
}

namespace DVector::PerfTests
{
    constexpr size_t testsCount { 1'000 };
    constexpr size_t pushBacksMax { 4'000 };
    constexpr size_t pushFrontMax { 4'000 };
    constexpr size_t blockSize { 20 };

    // using Type = int;
    // using Type = size_t;
    using Type = std::string;

    const Type element {};

    void RunTests()
    {
        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            for (size_t test = 0; test < testsCount; ++test)
            {
                std::deque<Type> vector;

                size_t pushBacks = 0, pushFronts = 0;
                while (pushBacksMax > pushBacks && pushFrontMax > pushFronts)
                {
                    for (size_t n = 0; pushBacks < pushBacksMax && n < blockSize; ++n, ++pushBacks)
                        vector.push_back(element);
                    for (size_t n = 0; pushFronts < pushFrontMax && n < blockSize; ++n, ++pushFronts)
                        vector.insert(vector.cbegin(), element);
                }
            }

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            for (size_t test = 0; test < testsCount; ++test)
            {
                std::deque<Type> deque;

                size_t pushBacks = 0, pushFronts = 0;
                while (pushBacksMax > pushBacks && pushFrontMax > pushFronts)
                {
                    for (size_t n = 0; pushBacks < pushBacksMax && n < blockSize; ++n, ++pushBacks)
                        deque.push_back(element);
                    for (size_t n = 0; pushFronts < pushFrontMax && n < blockSize; ++n, ++pushFronts)
                        deque.push_front( element);
                }
            }

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            for (size_t test = 0; test < testsCount; ++test)
            {
                DVector<Type> vector;

                size_t pushBacks = 0, pushFronts = 0;
                while (pushBacksMax > pushBacks && pushFrontMax > pushFronts)
                {
                    for (size_t n = 0; pushBacks < pushBacksMax && n < blockSize; ++n, ++pushBacks)
                        vector.push_back(element);
                    for (size_t n = 0; pushFronts < pushFrontMax && n < blockSize; ++n, ++pushFronts)
                        vector.push_front( element);
                }
            }

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }
    }
}

namespace DVector::PerfTestsStatistics
{
    struct TimeMeasurements
    {
        double vectorTime = 0.0;
        double dequeTime = 0.0;
        double dVectorTime = 0.0;
    };

    std::vector<std::string> split(std::string_view input,
                                   std::string_view delimiter = ",") {
        std::vector<std::string> output;
        for (size_t first = 0; first < input.size(); ) {
            const auto second = input.find_first_of(delimiter, first);
            if (first != second)
                output.emplace_back(input.substr(first, second - first));
            if (second == std::string_view::npos)
                break;
            first = second + 1;
        }
        return output;
    }

    std::map<size_t, TimeMeasurements> readResultsFile(const std::filesystem::path& filePath)
    {
        std::map<size_t, TimeMeasurements> results;
        if (std::fstream file = std::fstream(filePath.string()); file.is_open() && file.good())
        {
            std::string line;
            while (std::getline(file, line)){
                const std::vector<std::string> values = split(line);

                if (4 != values.size()) {
                    std::cerr << "Error: " << values.size() << " != 4. Value: " << std::quoted(line) << std::endl;
                    break;
                }
                // FIXME: ignore line with column names
                if (0 == atoi(values[0].data()))
                    continue;
                results.emplace(atoi(values[0].data()),TimeMeasurements{atof(values[1].data()),
                                                                        atof(values[2].data()),
                                                                        atof(values[3].data())});
            }
        }
        return results;
    }

    void updateResultsFile(const  std::vector<std::pair<size_t, TimeMeasurements>>& results)
    {
        constexpr std::string_view filePath {R"(/home/andtokm/DiskS/Temp/DVectorData/32_Bytes/total.csv)"};
        std::map<size_t, TimeMeasurements> existingData {  readResultsFile(filePath.data()) };

        for (const auto& [count, measurements]: results) {
            existingData[count] = measurements;
        }

        if (std::fstream file(filePath.data(), std::ios::in | std::ios::out | std::ios::trunc);
                file.is_open() && file.good())
        {
            file << "elemetns,vector,deque,dvector\n";
            for (const auto& [count, stats]: existingData)
                file << count << ',' << std::to_string(stats.vectorTime)
                               << ',' << std::to_string(stats.dequeTime)
                               << ',' << std::to_string(stats.dVectorTime) << '\n';
        }
    }

    constexpr size_t testsCount { 10'00 };

    // using Type = int;
    // using Type = size_t;
    using Type = std::string;

    const Type element {};

    void RunTests()
    {
        std::vector<std::pair<size_t, TimeMeasurements>> results;
        // std::vector<std::string> results;

        constexpr size_t from = 2000, until = 3'000;

        for (size_t elementsCount = from; elementsCount < until; elementsCount += 100)
        {
            const size_t pushBacksMax = elementsCount / 2;
            const size_t pushFrontMax = elementsCount / 2;
            const size_t blockSize  = elementsCount > 40 ? 20 : elementsCount;

            auto& [count, measurements] = results.emplace_back(elementsCount, TimeMeasurements{});
            // std::string& result = results.emplace_back(std::to_string(elementsCount) + ",");
            std::cout << elementsCount << ",";

            {
                std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

                for (size_t test = 0; test < testsCount; ++test) {
                    std::vector<Type> vector;

                    size_t pushBacks = 0, pushFronts = 0;
                    while (pushBacksMax > pushBacks && pushFrontMax > pushFronts) {
                        for (size_t n = 0; pushBacks < pushBacksMax && n < blockSize; ++n, ++pushBacks)
                            vector.push_back(element);
                        for (size_t n = 0; pushFronts < pushFrontMax && n < blockSize; ++n, ++pushFronts)
                            vector.insert(vector.cbegin(), element);
                    }
                }

                std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

                measurements.vectorTime = time_span.count();
                // result += std::to_string(time_span.count()) + ",";
                std::cout << time_span.count() << ",";
            }

            {
                std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

                for (size_t test = 0; test < testsCount; ++test) {
                    std::deque<Type> deque;

                    size_t pushBacks = 0, pushFronts = 0;
                    while (pushBacksMax > pushBacks && pushFrontMax > pushFronts) {
                        for (size_t n = 0; pushBacks < pushBacksMax && n < blockSize; ++n, ++pushBacks)
                            deque.push_back(element);
                        for (size_t n = 0; pushFronts < pushFrontMax && n < blockSize; ++n, ++pushFronts)
                            deque.push_front(element);
                    }
                }

                std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

                measurements.dequeTime = time_span.count();
                // result += std::to_string(time_span.count()) + ",";
                std::cout << time_span.count() << ",";
            }

            {
                std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

                for (size_t test = 0; test < testsCount; ++test) {
                    DVector<Type> vector;

                    size_t pushBacks = 0, pushFronts = 0;
                    while (pushBacksMax > pushBacks && pushFrontMax > pushFronts) {
                        for (size_t n = 0; pushBacks < pushBacksMax && n < blockSize; ++n, ++pushBacks)
                            vector.push_back(element);
                        for (size_t n = 0; pushFronts < pushFrontMax && n < blockSize; ++n, ++pushFronts)
                            vector.push_front(element);
                    }
                }

                std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);

                measurements.dVectorTime = time_span.count();
                // result += std::to_string(time_span.count());
                std::cout << time_span.count() << "\n";
            }
        }

        updateResultsFile(results);

        /*
        if (std::fstream file(std::format("/home/andtokm/DiskS/Temp/DVectorData/{}_{}_results.csv", from, until),
                              std::ios::in | std::ios::out | std::ios::trunc);
                file.is_open() && file.good())
        {
            for (const auto& [count, stats]: results)
                file << count << ',' << stats.vectorTime << ',' << stats.dequeTime << ',' << stats.dVectorTime << '\n';

            for (const std::string& str: results)
                file << str << '\n';
        }*/
    }
}

// TODO: New functions:
//      Resize()

// TODO: TESTS
//   +  Constructor
//      destruction after Reallocation
//   +  push_back
//   +  push_front
//   +  pop_back
//   +  pop_from
//   +  front
//   +  back
//   +  Copy_Constructor
//   +  Copy_Assignment
//   +  Move_Constructor
//   +  Move_Assignment
//   +  Size()
//   +  Empty()
//   +  Clear()
//   +  Data()
//      Capacity: Front_Capacity() && Back_Capacity()
//   +  index operator[]
//   +  at[]

// TODO: Add Default Contructable CONCEPT


void DVector::TestAll()
{
    using namespace Tests;

    // Front_Back_CapacityTests();
    /*
    CapacityTests::TestInitialCapacity();
    CapacityTests::TestCapacityWithConstructor();
    CapacityTests::CapacityAfterReallocation_PushBack();
    CapacityTests::CapacityAfterReallocation_PushFront();

    AtMethodMethodTests::GetElement();
    AtMethodMethodTests::OutOufRange();

    BackMethodTests::CheckBack_AfterPushBack();
    BackMethodTests::CheckBack_AfterPushFront();
    BackMethodTests::CheckBack_AfterPushBack_Reallocation();
    BackMethodTests::CheckBack_AfterPushFront_Reallocation();

    FrontMethodTests::CheckFront_AfterPushBack();
    FrontMethodTests::CheckFront_AfterPushFront();
    FrontMethodTests::CheckFront_AfterPushBack_Reallocation();
    FrontMethodTests::CheckFront_AfterPushFront_Reallocation();

    PopBackMethodTests::PopBack();
    PopBackMethodTests::PopBack_Reallocation();
    PopBackMethodTests::PopBack_UntilEmpty();

    PopFrontMethodTests::PopFront();
    PopFrontMethodTests::PopFront_Reallocation();
    PopFrontMethodTests::PopFront_UntilEmpty();

    PushBackTests::PushBack();
    PushBackTests::PushBack_RValue();

    PushBackTests::PushBack_Realloc();
    PushBackTests::PushBack_CustomTypes_TODO();

    PushFrontTests::PushFront();
    PushFrontTests::PushFront_Realloc();
    PushFrontTests::PushFront_CustomTypes_TODO();

    Constructor::CreateVectorTest();
    Constructor::CreateVector_CustomCapacity();

    CopyConstructor::CopyConstructorTests();
    CopyConstructor::CopyConstructorTests_Realloc();

    CopyAssignmentOperator::CopyAssignmentTests();
    CopyAssignmentOperator::CopyAssignmentTests_Reallocation();

    MoveConstructor::MoveConstructorTests();
    MoveConstructor::MoveConstructorTests_Reallocation();

    MoveAssignmentOperator::MoveAssignmentTests();
    MoveAssignmentOperator::MoveAssignmentTests_Reallocation();

    IndexOperator::BasicTest();
    IndexOperator::CheckValues_With_PushBack_and_PushFront();
    IndexOperator::CheckValues_With_PushBack_and_PushFront_Realloc();
    IndexOperator::CheckValues_AfterMove();
    IndexOperator::CheckValues_WithCopyConstructor();

    Sizes::CheckAfterPush_NoReallocation();
    Sizes::CheckAfterPush_Reallocation();
    Sizes::CheckAfterMove();

    Empty::PushElementsAndCheckIsEmpty();
    Empty::CheckAfterClear();
    Empty::CheckAfterMove();

    Data::SimpleTest();
    Data::SimpleTestTwo();
    Data::ReallocTest();

    Clear::Basic();
     */
    // PerfTests::RunTests();

    PerfTestsStatistics::RunTests();
}