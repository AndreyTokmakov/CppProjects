//============================================================================
// Name        : ExpressionTemplates.cpp
// Created on  : 01.04.2022
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ExpressionTemplates
//============================================================================

#include "ExpressionTemplates.h"

#include <iostream>
#include <vector>
#include <cassert>

namespace ExpressionTemplates::Classic
{

    template<typename T>
    class MyVector{
        std::vector<T> cont;

    public:
        explicit MyVector(const std::size_t n) : cont(n) {}

        MyVector(const std::size_t n, const double initialValue) : cont(n, initialValue){
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return cont.size();
        }

        T operator[](const std::size_t i) const {
            return cont[i];
        }

        T& operator[](const std::size_t i) {
            return cont[i];
        }

    };

    template<typename T>
    MyVector<T> operator+ (const MyVector<T>& a, const MyVector<T>& b)
    {
        MyVector<T> result(a.size());
        for (std::size_t s = 0; s <= a.size(); ++s){
            result[s] = a[s] + b[s];
        }
        return result;
    }

    template<typename T>
    MyVector<T> operator* (const MyVector<T>& a, const MyVector<T>& b)
    {
        MyVector<T> result(a.size());
        for (std::size_t s = 0; s <= a.size(); ++s){
            result[s] = a[s] * b[s];
        }
        return result;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const MyVector<T>& cont)
    {
        for (size_t i = 0; i < cont.size(); ++i) {
            os << cont[i] << ' ';
        }
        os << '\n';
        return os;
    }


    void Test() {
        MyVector<double> x(10,5.4), y(10,10.3), result(10);
        result = x + x + y * y;
        std::cout << result << '\n';
    }
}


namespace ExpressionTemplates::LazyExpression
{
    template<typename T, typename Cont= std::vector<T> >
    class MyVector{
        Cont cont;

    public:
        explicit MyVector(const std::size_t n) : cont(n){ }

        MyVector(const std::size_t n, const double initialValue) : cont(n, initialValue){
        }

        explicit MyVector(const Cont& other) : cont(other){
        }

        template<typename T2, typename R2>
        MyVector& operator=(const MyVector<T2, R2>& other){
            assert(size() == other.size());
            for (std::size_t i = 0; i < cont.size(); ++i) cont[i] = other[i];
            return *this;
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return cont.size();
        }

        T operator[](const std::size_t i) const{
            return cont[i];
        }

        T& operator[](const std::size_t i){
            return cont[i];
        }

        [[nodiscard]] const Cont& data() const{
            return cont;
        }

        Cont& data(){
            return cont;
        }
    };

    template<typename T, typename Op1 , typename Op2>
    class MyVectorAdd{
        const Op1& op1;
        const Op2& op2;

    public:
        MyVectorAdd(const Op1& a, const Op2& b): op1(a), op2(b){}

        T operator[](const std::size_t i) const{
            return op1[i] + op2[i];
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return op1.size();
        }
    };

    template< typename T, typename Op1 , typename Op2 >
    class MyVectorMul {
        const Op1& op1;
        const Op2& op2;

    public:
        MyVectorMul(const Op1& a, const Op2& b ): op1(a), op2(b){}

        T operator[](const std::size_t i) const{
            return op1[i] * op2[i];
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return op1.size();
        }
    };

    template<typename T, typename R1, typename R2>
    MyVector<T, MyVectorAdd<T, R1, R2> >
    operator+ (const MyVector<T, R1>& a, const MyVector<T, R2>& b)
    {
        return MyVector<T, MyVectorAdd<T, R1, R2> >(MyVectorAdd<T, R1, R2 >(a.data(), b.data()));   // (1)
    }

    template<typename T, typename R1, typename R2>
    MyVector<T, MyVectorMul< T, R1, R2> >
    operator* (const MyVector<T, R1>& a, const MyVector<T, R2>& b)
    {
        return MyVector<T, MyVectorMul<T, R1, R2> >(MyVectorMul<T, R1, R2 >(a.data(), b.data()));  // (2)
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const MyVector<T>& cont)
    {
        for (size_t i = 0; i < cont.size(); ++i) {
            os << cont[i] << ' ';
        }
        os << '\n';
        return os;
    }

    void Test() {
        MyVector<double> x(10,5.4), y(10,10.3), result(10);
        result = x + x + y * y;
        std::cout << result << '\n';
    }
}

void ExpressionTemplates::TestAll()
{
    Classic::Test();
    LazyExpression::Test();
}
