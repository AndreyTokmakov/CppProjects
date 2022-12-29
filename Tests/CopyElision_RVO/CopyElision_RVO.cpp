/**============================================================================
Name        : CopyElision_RVO.cpp
Created on  : 21.12.2022.
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CopyElision_RVO
============================================================================**/

#include "CopyElision_RVO.h"

#include <iostream>
#include <memory>


namespace CopyElision_RVO::SimpleExample {

    struct NonMoveAndCopy final {
        NonMoveAndCopy() {
            std::cout << "NonMoveAndCopy()\n";
        }

        ~NonMoveAndCopy() {
            std::cout << "~NonMoveAndCopy()\n";
        }

        // NonMoveAndCopy(const NonMoveAndCopy&) = delete;
        NonMoveAndCopy(const NonMoveAndCopy&) {
            std::cout << "NonMoveAndCopy(COPY)\n";
        }

        // NonMoveAndCopy& operator=(const NonMoveAndCopy&) = delete;
        NonMoveAndCopy& operator=(const NonMoveAndCopy&) {
            std::cout << "Copy assignment\n";
            return *this;
        }

        // NonMoveAndCopy(NonMoveAndCopy&&) noexcept = delete;
        NonMoveAndCopy(NonMoveAndCopy&&) noexcept {
            std::cout << "NonMoveAndCopy(MOVE)\n";
        }

        // NonMoveAndCopy& operator=(NonMoveAndCopy&&) noexcept = delete;
        NonMoveAndCopy& operator=(NonMoveAndCopy&&) noexcept {
            std::cout << "Move assignment\n";
            return *this;
        }
    };


    NonMoveAndCopy create1() {
        return NonMoveAndCopy{};
    }

    NonMoveAndCopy create2() {
        NonMoveAndCopy a;
        return a;
    }


    void Test()
    {
        std::cout << "-------------------------- Test1 -----------------------------\n";
        {
            NonMoveAndCopy obj = create1();
        }
        std::cout << "-------------------------- Test2 -----------------------------\n";
        {
            NonMoveAndCopy obj = create2();
        }
    }
}



namespace CopyElision_RVO
{
    struct Integer
    {
        int value {0};

        Integer(int val = 0) : value {val} {
            std::cout << "Integer(" << value << ")\n";
        }

        Integer(const Integer &obj) {
            this->value = obj.value;
            std::cout << "Integer(" << value << ") [Copy constructor]\n";
        }

        Integer(Integer &&obj) noexcept: value{std::exchange(obj.value, 0)} {
            std::cout << "Integer(" << value << ") [Move constructor]\n";
        }

        inline void setValue(int v) noexcept {
            value = v;
        }

        [[nodiscard]]
        inline int getValue() const noexcept {
            return value;
        }

        ~Integer() {
            std::cout << "~Integer(" << value << ")\n";
        }

        Integer &operator=(const Integer &right) {
            std::cout << "Right: Integer(" << right.value << ")\n";
            std::cout << "[Copy assignment] (" << value << " -> " << right.value << ")" << std::endl;
            if (&right != this)
                value = right.value;
            return *this;
        }

        Integer &operator=(int val) {
            std::cout << "[Copy assignment (from int)]" << std::endl;
            this->value = val;
            return *this;
        }

        Integer &operator=(Integer &&right) noexcept {
            std::cout << "[Move assignment operator]" << std::endl;
            if (this != &right) {
                this->value = std::exchange(right.value, 0);
            }
            return *this;
        }

        Integer &operator*(const Integer &right) noexcept {
            this->value *= right.value;
            return *this;
        }

        /** Postfix increment: **/
        Integer operator++(int) {
            auto prev = *this;
            ++value;
            return prev;
        }

        /** Prefix increment: **/
        Integer operator++() {
            ++value;
            return *this;
        }

        friend Integer operator*(const Integer &left, int v) noexcept {
            return Integer(left.value * v);
        }

        friend std::ostream &operator<<(std::ostream &stream, const Integer &l) {
            stream << l.value;
            return stream;
        }
    };

    struct Holder
    {
        Integer value {};

        [[nodiscard]]
        Integer build() &&
        {
            return std::move(value);
        }

        [[nodiscard]]
        Integer build() const &
        {
            return value;
        }

        Holder& setValue()
        {
            return *this;
        }
    };
}

namespace CopyElision_RVO::Tests
{
    Integer create_RVO()
    {
        return {1};
    }

    Integer create_NRVO()
    {
        Integer i {2};
        return i;
    }

    void CreateTest()
    {
        std::cout << "-------------- RVO ---------------\n";
        {
            auto v = create_RVO();
        }
        std::cout << "-------------- RNVO ---------------\n";
        {
            auto v = create_NRVO();
        }
    }

    void UsingBuilder()
    {
        Integer v = Holder().build();
        std::cout << "Value = " << v.getValue() << std::endl;
    }

    void UsingBuilder2()
    {
        Integer v = Holder().build();
        std::cout << "Value = " << v.getValue() << std::endl;
    }

    void UsingBuilder3()
    {
        Integer v = Holder().setValue().build();
        std::cout << "Value = " << v.getValue() << std::endl;
    }
}

void CopyElision_RVO::TestAll()
{
    SimpleExample::Test();

    // Tests::CreateTest();

    // Tests::UsingBuilder();
    // Tests::UsingBuilder2();
    // Tests::UsingBuilder3();
}