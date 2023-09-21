/**============================================================================
Name        : TwoSidedVector.cpp
Created on  : 21.09.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TwoSidedVector
============================================================================**/

#include "TwoSidedVector.h"

/** For testing only: **/
#include <chrono>
#include <unordered_set>
#include <random>
#include <fstream>
#include <format>
#include <iostream>
#include <deque>

// TODO: Create lib/module for TestUtils
namespace TwoSidedVector::Utilities
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
                       const DVector<_Ty>& vector)
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
    void assertEquals(const DVector<_Ty>& first,
                      const DVector<_Ty>& second)
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

namespace TwoSidedVector
{
    using namespace Utilities;

    void DestructorTest()
    {
        /*
        DVector<Helpers::Long> dVector;

        dVector.emplace_back(101);
        dVector.emplace_back(102);
        dVector.emplace_back(103);
        */
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

namespace TwoSidedVector::Tests::CapacityTests
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

namespace TwoSidedVector::Tests::AtMethodMethodTests
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
            [[maybe_unused]]
            auto x = dVector.at(10);
            std::cerr << "Error: Exception shall be thrown\n";
        }
        catch (const std::exception& exc) {
        }
    }
}

namespace TwoSidedVector::Tests::PopBackMethodTests
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

namespace TwoSidedVector::Tests::PopFrontMethodTests
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

namespace TwoSidedVector::Tests::BackMethodTests
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

namespace TwoSidedVector::Tests::FrontMethodTests
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

namespace TwoSidedVector::Tests::PushBackTests
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

namespace TwoSidedVector::Tests::PushFrontTests
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

namespace TwoSidedVector::Tests::Constructor
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

namespace TwoSidedVector::Tests::CopyConstructor
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

namespace TwoSidedVector::Tests::CopyAssignmentOperator
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

namespace TwoSidedVector::Tests::MoveConstructor
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

namespace TwoSidedVector::Tests::MoveAssignmentOperator
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

namespace TwoSidedVector::Tests::Clear
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

namespace TwoSidedVector::Tests::Sizes
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

namespace TwoSidedVector::Tests::IndexOperator
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

namespace TwoSidedVector::Tests::Empty
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

namespace TwoSidedVector::Tests::Data
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

namespace TwoSidedVector::PerfTests
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

namespace TwoSidedVector::PerfTestsStatistics
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



void TwoSidedVector::TestAll()
{
    using namespace Tests;


    // Front_Back_CapacityTests();

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

    // PerfTests::RunTests();
    // PerfTestsStatistics::RunTests();
}