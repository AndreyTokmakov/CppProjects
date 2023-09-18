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
#include <deque>
#include <memory>
#include <utility>
#include <algorithm>


/** For testing only: **/
#include <chrono>
#include <unordered_set>
#include <random>
#include <fstream>
#include <format>

#include "../Helpers/Long.h"

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

    private:

        void growVector()
        {
            // std::cout << "* * * * ReAlloc (" << capacity << " ==> " << capacity * growthFactor << ") * * * * \n";

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

        void destroy()
        {
            const size_t size = right - left - 1;

            /** Invoke destructors for all contained objects: **/
            std::destroy_n(data + left + 1, size);
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
            destroy();

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
        object_type& operator[] (size_t index) const {
            return this->data[index + left + 1];
        }

        [[nodiscard]]
        inline size_t Size() const noexcept {
            return 0 != capacity ? right - left - 1 : 0;
        }

        [[nodiscard]]
        inline size_t Capacity() const noexcept {
            return capacity;
        }

        [[nodiscard]]
        inline size_t FrontCapacity() const noexcept {
            return left + 1;
        }

        [[nodiscard]]
        inline size_t BackCapacity() const noexcept {
            return capacity - right;
        }

        [[nodiscard]]
        inline bool Empty() const noexcept {
            return 0 == capacity || 1 == (right - left);
        }

        [[nodiscard]]
        inline pointer Data() const noexcept {
            return data + left + 1;
        }

        inline void Clear() noexcept
        {
            /** Invoke destructors for all contained objects: **/
            destroy();

            right = capacity / 2;
            left = right - 1;
        }

        object_type& push_back(const object_type& v)
        {
            if (right >= capacity)
                growVector();
            this->data[right] = v;
            return data[right++];
        }

        object_type& push_front(const object_type& v)
        {
            if (0 >= left)
                growVector();
            this->data[left] = v;
            return data[left--];
        }

        object_type& push_back(object_type&& v)
        {
            if (right >= capacity)
                growVector();
            this->data[right] = std::move(v);
            return data[right++];
        }

        object_type& push_front(object_type&& v)
        {
            if (0 >= left)
                growVector();
            this->data[left] = std::move(v);
            return data[left--];
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

    public:  /** Debug methods: **/

        void printInfo()
        {
            const size_t size = right - left - 1;
            std::cout << "[ capacity = " << capacity << ", size = " << size << " ] "
                      << "[ left: " << left << ", right: " << right << " ]\n";

            for (size_t idx = 0; idx < capacity; ++idx)
                std::cout << '[' << idx << "] = " << data[idx] << std::endl;
        }

        void callGrowVector()
        {
            growVector();
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
    std::vector<int> getRandomIntegerVector(size_t size)
    {
        std::vector<int> intVector;
        while (size--)
            intVector.push_back(getRandomIntInRange(0, static_cast<int>(size * 2)));
        return intVector;
    }

    [[nodiscard]]
    std::vector<int> getRandomIntegerUniqueVector(size_t size)
    {
        std::vector<int> intVector;
        for (size_t i = 0; i < size; ++i)
            intVector.push_back(getRandomUniqueInt(0, static_cast<int>(size * 2)));
        return intVector;
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
    void assertContent(const std::vector<_Ty>& contentExpected,
                       const DVector::DVector<_Ty>& vector)
    {
        if (contentExpected.size() != vector.Size())
        {
            std::cerr << "ERROR: Size mismatch: " << contentExpected.size() << " != " << vector.Size() << "\n";
            std::terminate();
        }

        for (size_t idx = 0; idx < contentExpected.size(); ++idx)
            assertEquals(vector[idx],  contentExpected[idx]);
    }

    template<typename _Ty>
    void assertEquals(const DVector::DVector<_Ty>& first,
                      const DVector::DVector<_Ty>& second)
    {
        assertEquals(first.Size(), second.Size());
        assertEquals(first.Capacity(), second.Capacity());
        assertEquals(first.Empty(), second.Empty());
        assertEquals(first.FrontCapacity(), second.FrontCapacity());
        assertEquals(first.BackCapacity(), second.BackCapacity());

        for (size_t idx = 0; idx < first.Size(); ++idx)
            assertEquals(first[idx],  second[idx]);
    }
}

namespace DVector::Tests
{
    using namespace Utilities;




    void PushFront()
    {
        DVector<int> dVector;

        dVector.push_front(21);
        dVector.push_front(22);
        dVector.push_front(23);

        dVector.printInfo();
    }

    void DestructorTest()
    {
        DVector<Helpers::Long> dVector;

        dVector.emplace_back(101);
        dVector.emplace_back(102);
        dVector.emplace_back(103);

        dVector.printInfo();
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
                  << (dVector.Capacity() == dVector.Size() + dVector.FrontCapacity() + dVector.BackCapacity())
                  << std::endl;

        std::cout << "Size: " << dVector.Size()
                  << ", Front: " << dVector.FrontCapacity()
                  << ", Back: " << dVector.BackCapacity()
                  << std::endl;

        // dVector.printInfo();
    }
}

namespace DVector::Tests::PushBackTests
{
    void PushBack()
    {
        const std::vector<int> testValues = getRandomIntegerVector(4);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.size(), dVector.Size());
        assertEquals(testValues.empty(), dVector.Empty());
        assertEquals(10UL, dVector.Capacity());

        assertContent(testValues, dVector);
    }

    void PushBack_Realloc()
    {
        const std::vector<int> testValues = getRandomIntegerVector(15);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.size(), dVector.Size());
        assertEquals(testValues.empty(), dVector.Empty());
        assertEquals(40UL, dVector.Capacity());

        assertContent(testValues, dVector);
    }

    void PushBack_CustomTypes_TODO()
    {
        const std::vector<std::string> testValues { "I", "II", "III", "IV"};
        DVector<std::string> dVector;
        for (const auto& v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.size(), dVector.Size());
        assertEquals(testValues.empty(), dVector.Empty());
        assertEquals(10UL, dVector.Capacity());

        assertContent(testValues, dVector);
    }
}

namespace DVector::Tests::PushFrontTests
{
    void PushBack()
    {
        const std::vector<int> testValues = getRandomIntegerVector(4);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.size(), dVector.Size());
        assertEquals(testValues.empty(), dVector.Empty());
        assertEquals(10UL, dVector.Capacity());

        assertContent(testValues, dVector);
    }

    void PushBack_Realloc()
    {
        const std::vector<int> testValues = getRandomIntegerVector(15);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.size(), dVector.Size());
        assertEquals(testValues.empty(), dVector.Empty());
        assertEquals(40UL, dVector.Capacity());

        assertContent(testValues, dVector);
    }

    void PushBack_CustomTypes_TODO()
    {
        const std::vector<std::string> testValues { "I", "II", "III", "IV"};
        DVector<std::string> dVector;
        for (const auto& v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.size(), dVector.Size());
        assertEquals(testValues.empty(), dVector.Empty());
        assertEquals(10UL, dVector.Capacity());

        assertContent(testValues, dVector);
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

        assertEquals(size, dVector.Size());
        assertEquals(false, dVector.Empty());
    }

    void CheckAfterClear()
    {
        constexpr size_t size {55};
        DVector<int> dVector;
        for (int i = 0; i < static_cast<int>(size); ++i)
            dVector.push_back(i);

        assertEquals(size, dVector.Size());
        assertEquals(false, dVector.Empty());

        dVector.Clear();

        assertEquals(0UL, dVector.Size());
        assertEquals(true, dVector.Empty());
    }

    void CheckAfterMove()
    {
        constexpr size_t size {55};
        DVector<int> dVector1;
        for (int i = 0; i < static_cast<int>(size); ++i)
            dVector1.push_back(i);

        DVector<int> dVector2 = std::move(dVector1);

        assertEquals(0UL, dVector1.Size());
        assertEquals(true, dVector1.Empty());

        assertEquals(size, dVector2.Size());
        assertEquals(false, dVector2.Empty());
    }
}

namespace DVector::Tests::Constructor
{
    void CreateVectorTest()
    {
        DVector<int> dVector;
        assertEquals(0UL, dVector.Size());
        assertEquals(true, dVector.Empty());
        assertEquals(10UL, dVector.Capacity());
    }

    void CreateVector_CustomCapacity()
    {
        DVector<int> dVector (100);
        assertEquals(0UL, dVector.Size());
        assertEquals(true, dVector.Empty());
        assertEquals(100UL, dVector.Capacity());
    }
}

namespace DVector::Tests::CopyConstructor
{
    void CopyConstructorTests()
    {
        const std::vector<int> testValues = getRandomIntegerVector(7);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        const DVector<int> dVectorCopy (dVectorOrig);
        assertEquals(dVectorOrig, dVectorCopy);
    }

    void CopyConstructorTests_Realloc()
    {
        const std::vector<int> testValues = getRandomIntegerVector(50);
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
        const std::vector<int> testValues = getRandomIntegerVector(7);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        DVector<int> dVectorCopy;
        dVectorCopy = dVectorOrig;

        assertEquals(dVectorOrig, dVectorCopy);
    }

    void CopyAssignmentTests_Reallocation()
    {
        const std::vector<int> testValues = getRandomIntegerVector(50);
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
        const std::vector<int> testValues = getRandomIntegerVector(7);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        DVector<int> dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.Size());
        assertEquals(true, dVectorOrig.Empty());
        assertEquals(0UL, dVectorOrig.Capacity());

        assertEquals(testValues.size(), dVectorDest.Size());
        assertEquals(false, dVectorDest.Empty());
        assertEquals(40UL, dVectorDest.Capacity());

        assertContent(testValues, dVectorDest);
    }

    void MoveConstructorTests_Reallocation()
    {
        const std::vector<int> testValues = getRandomIntegerVector(50);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        DVector<int> dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.Size());
        assertEquals(true, dVectorOrig.Empty());
        assertEquals(0UL, dVectorOrig.Capacity());

        assertEquals(testValues.size(), dVectorDest.Size());
        assertEquals(false, dVectorDest.Empty());
        assertEquals(160UL, dVectorDest.Capacity());

        assertContent(testValues, dVectorDest);
    }
}

namespace DVector::Tests::MoveAssignmentOperator
{
    void MoveAssignmentTests()
    {
        const std::vector<int> testValues = getRandomIntegerVector(7);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        DVector<int> dVectorDest;
        dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.Size());
        assertEquals(true, dVectorOrig.Empty());
        assertEquals(0UL, dVectorOrig.Capacity());

        assertEquals(testValues.size(), dVectorDest.Size());
        assertEquals(false, dVectorDest.Empty());
        assertEquals(40UL, dVectorDest.Capacity());

        assertContent(testValues, dVectorDest);
    }

    void MoveAssignmentTests_Reallocation()
    {
        const std::vector<int> testValues = getRandomIntegerVector(50);
        DVector<int> dVectorOrig;
        for (int v: testValues)
            dVectorOrig.push_back(v);

        DVector<int> dVectorDest;
        dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.Size());
        assertEquals(true, dVectorOrig.Empty());
        assertEquals(0UL, dVectorOrig.Capacity());

        assertEquals(testValues.size(), dVectorDest.Size());
        assertEquals(false, dVectorDest.Empty());
        assertEquals(160UL, dVectorDest.Capacity());

        assertContent(testValues, dVectorDest);
    }
}

namespace DVector::Tests::Data
{
    void SimpleTest()
    {
        const std::vector<int> testValues = getRandomIntegerUniqueVector(5);

        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        const int* data = dVector.Data();
        for (size_t idx = 0; idx < dVector.Size(); ++idx)
            assertEquals(testValues[idx], data[idx]);
    }

    void SimpleTestTwo()
    {
        const std::vector<int> testValues = getRandomIntegerUniqueVector(10);

        constexpr int N = 2;
        DVector<int> dVector;
        for (size_t idx = N; idx < testValues.size(); ++idx)
            dVector.push_back(testValues[idx]);
        for (int idx = N - 1; idx >= 0; --idx)
            dVector.push_front(testValues[idx]);

        const int* data = dVector.Data();
        for (size_t idx = 0; idx < dVector.Size(); ++idx)
            assertEquals(testValues[idx], data[idx]);
    }

    void ReallocTest()
    {
        const std::vector<int> testValues = getRandomIntegerUniqueVector(100);

        constexpr int N = 2;
        DVector<int> dVector;
        for (size_t idx = N; idx < testValues.size(); ++idx)
            dVector.push_back(testValues[idx]);
        for (int idx = N - 1; idx >= 0; --idx)
            dVector.push_front(testValues[idx]);

        const int* data = dVector.Data();
        for (size_t idx = 0; idx < dVector.Size(); ++idx)
            assertEquals(testValues[idx], data[idx]);
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

        assertEquals(size, dVector.Size());
        assertEquals(false, dVector.Empty());

        dVector.Clear();

        assertEquals(0UL, dVector.Size());
        assertEquals(true, dVector.Empty());
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

        assertEquals(10U, dVector.Capacity());
        assertEquals(size, dVector.Size());
    }

    void CheckAfterPush_Reallocation()
    {
        constexpr size_t size {15};
        DVector<int> dVector;
        for (int i = 0; i < static_cast<int>(size); ++i)
            dVector.push_back(i);

        assertEquals(40U, dVector.Capacity());
        assertEquals(size, dVector.Size());
    }

    void CheckAfterMove()
    {
        constexpr size_t size {20};
        DVector<int> dVectorOrig;
        for (int i = 0; i < static_cast<int>(size); ++i)
            dVectorOrig.push_back(i);

        DVector<int> dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.Size());
        assertEquals(0UL, dVectorOrig.Capacity());
        assertEquals(true, dVectorOrig.Empty());

        assertEquals(size, dVectorDest.Size());
        assertEquals(40U, dVectorDest.Capacity());
        assertEquals(false, dVectorDest.Empty());
    }
}

namespace DVector::Tests::IndexOperator
{
    void BasicTest()
    {
        const std::vector<int> testValues = getRandomIntegerVector(7);
        DVector<int> dVector;
        for (int v: testValues)
            dVector.push_back(v);

        assertEquals(testValues.size(), dVector.Size());
        for (size_t idx = 0; idx < testValues.size(); ++idx)
            assertEquals(dVector[idx], testValues[idx]);
    }

    void CheckValues_With_PushBack_and_PushFront()
    {
        const std::vector<int> testValues = getRandomIntegerVector(8);

        constexpr int N = 3;
        DVector<int> dVector;
        for (size_t idx = N; idx < testValues.size(); ++idx)
            dVector.push_back(testValues[idx]);
        for (int idx = N - 1; idx >= 0; --idx)
            dVector.push_front(testValues[idx]);

        assertEquals(testValues.size(), dVector.Size());
        for (size_t idx = 0; idx < testValues.size(); ++idx)
            assertEquals(dVector[idx], testValues[idx]);
    }

    void CheckValues_With_PushBack_and_PushFront_Realloc()
    {
        const std::vector<int> testValues = getRandomIntegerVector(100);

        constexpr int N = 40;
        DVector<int> dVector;
        for (size_t idx = N; idx < testValues.size(); ++idx)
            dVector.push_back(testValues[idx]);
        for (int idx = N - 1; idx >= 0; --idx)
            dVector.push_front(testValues[idx]);

        assertEquals(testValues.size(), dVector.Size());
        for (size_t idx = 0; idx < testValues.size(); ++idx)
            assertEquals(dVector[idx], testValues[idx]);
    }

    void CheckValues_AfterMove()
    {
        const std::vector<int> testValues = getRandomIntegerVector(100);

        constexpr int N = 40;
        DVector<int> dVectorOrig;
        for (size_t idx = N; idx < testValues.size(); ++idx)
            dVectorOrig.push_back(testValues[idx]);
        for (int idx = N - 1; idx >= 0; --idx)
            dVectorOrig.push_front(testValues[idx]);

        const DVector<int> dVectorDest = std::move(dVectorOrig);

        assertEquals(0UL, dVectorOrig.Size());
        assertEquals(0UL, dVectorOrig.Capacity());
        assertEquals(true, dVectorOrig.Empty());

        assertEquals(testValues.size(), dVectorDest.Size());
        for (size_t idx = 0; idx < testValues.size(); ++idx)
            assertEquals(dVectorDest[idx], testValues[idx]);
    }

    void CheckValues_WithCopyConstructor() {
        const std::vector<int> testValues = getRandomIntegerVector(100);

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
                std::vector<Type> vector;

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
    constexpr size_t testsCount { 10'000 };

    using Type = int;
    // using Type = size_t;
    // using Type = std::string;

    const Type element {};

    void RunTests()
    {
        std::vector<std::string> results;
        constexpr size_t from = 9000, until = 10000;

        for (size_t elementsCount = from; elementsCount < until; elementsCount += 50)
        {
            const size_t pushBacksMax = elementsCount / 2;
            const size_t pushFrontMax = elementsCount / 2;
            const size_t blockSize  = elementsCount > 40 ? 20 : elementsCount;

            std::string& result = results.emplace_back(std::to_string(elementsCount) + ",");
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
                // std::cout << "\tIt took me " << time_span.count() << " seconds.\n";
                result += std::to_string(time_span.count()) + ",";
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
                // std::cout << "\tIt took me " << time_span.count() << " seconds.\n";
                result += std::to_string(time_span.count()) + ",";
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
                // std::cout << "\tIt took me " << time_span.count() << " seconds.\n";
                result += std::to_string(time_span.count()) + ",";
            }
        }

        if (std::fstream file(std::format("/home/andtokm/DiskS/Temp/DVectorData/{}_{}_results.csv", from, until),
                              std::ios::in | std::ios::out | std::ios::trunc);
            file.is_open() && file.good())
        {
            for (const std::string& str: results)
                file << str << '\n';
        }
    }
}


// TODO: TESTS
//   +  Constructor
//      destruction after Reallocation
//   +  Copy_Constructor
//   +  Copy_Assignment
//   +  Move_Constructor
//   +  Move_Assignment
//   +  Size()
//   +  Empty()
//   +  Clear()
//      Resize()
//   +  Data()
//      Capacity: Front_Capacity() && Back_Capacity()
//   +  index operator[]
//      at[]

// TODO: Add Default Contructable CONCEPT


void DVector::TestAll()
{
    using namespace Tests;

    // PushFront();
    // MoveAssignmentTests();
    // Front_Back_CapacityTests();


    PushBackTests::PushBack();
    PushBackTests::PushBack_Realloc();
    PushBackTests::PushBack_CustomTypes_TODO();


    /*
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
    // PerfTestsStatistics::RunTests();
}