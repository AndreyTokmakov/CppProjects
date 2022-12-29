//============================================================================
// Name        : Arrays.h
// Created on  : 11.01.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Arrays
//============================================================================

#include "Arrays.h"

#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include <iomanip>

#include <Core>
#include <Geometry>

namespace Arrays {

    using Vector2d = Eigen::Vector2d;
    using Vector3d = Eigen::Vector3d;
    using Vector4d = Eigen::Vector4d;

    void SimpleTest() {
        Eigen::Array<double, Eigen::Dynamic, 1> data;

        size_t rowsCount = data.rows();
        std::cout << "rowsCount = " << rowsCount << std::endl;

        data.resize(4);

        data << 1, 2, 3 ,4;

        rowsCount = data.rows();
        std::cout << "rowsCount = " << rowsCount << std::endl;
        std::cout << data << std::endl;

        size_t N = data.rows();
        for (size_t i = 0; i < N; ++i)
            std::cout << data[i] << ", ";
        std::cout << std::endl;
    }


    void Array_Loop1() {
        Eigen::Array<double, Eigen::Dynamic, 1> data;

        data.resize(4);
        data << 1, 2, 3 ,4;

        size_t rowsCount = data.rows();
        for (size_t i = 0; i < rowsCount; ++i)
            std::cout << data[i] << ", ";
        std::cout << std::endl;
    }

    void Array_Loop2() {
        Eigen::Array<double, Eigen::Dynamic, 1> data;

        data.resize(4);
        data << 1, 2, 3 ,4;
        for (auto x: data) {
            std::cout << x << std::endl;
        }
    }

    void Test2()
    {
        Eigen::ArrayXXf  m(2,2);

        // assign some values coefficient by coefficient
        m(0,0) = 1.0;
        m(0,1) = 2.0;

        m(1,0) = 3.0;
        m(1,1) = m(0,1) + m(1,0);

        // print values to standard output
        std::cout << m << std::endl << std::endl;

        // using the comma-initializer is also allowed
        m << 1.0, 2.0,
                3.0, 4.0;

        // print values to standard output
        std::cout << m << std::endl;
    }

    void SquaredNorm() {
        Eigen::VectorXf v(2);

        constexpr int a = -1;
        constexpr int b = 2;

        v << a, b;

        std::cout << "\nOriginal: " << (std::pow(a, 2) + std::pow(b, 2)) << "\n" << std::endl;

        std::cout << "\nv.squaredNorm() = " << v.squaredNorm() << std::endl;
        std::cout << "v.norm() = " << v.norm() << std::endl;
        std::cout << "v.lpNorm<1>() = " << v.lpNorm<1>() << std::endl;
        std::cout << "v.lpNorm<Infinity>() = " << v.lpNorm<Eigen::Infinity>() << std::endl << std::endl;
    }

    void SquaredNorm_3D() {
        Eigen::Vector3d v;

        constexpr int a = -1;
        constexpr int b = 2;
        constexpr int c = 3;

        v << a, b, c;

        std::cout << "\nOriginal: " << (std::pow(a, 2) + std::pow(b, 2) + std::pow(c, 2)) << "\n" << std::endl;

        std::cout << "\nv.squaredNorm() = " << v.squaredNorm() << std::endl;
        std::cout << "v.norm() = " << v.norm() << std::endl;
        std::cout << "v.lpNorm<1>() = " << v.lpNorm<1>() << std::endl;
        std::cout << "v.lpNorm<Infinity>() = " << v.lpNorm<Eigen::Infinity>() << std::endl << std::endl;
    }

    void Sum_of_Arrays()
    {
        Eigen::ArrayXXf a(3,3), b(3,3);

        a << 1,2,3,
                4,5,6,
                7,8,9;
        b << 1,2,3,
                1,2,3,
                1,2,3;

        // Adding two arrays
        std::cout << "a + b = " << std::endl << a + b << std::endl << std::endl;

        // Subtracting a scalar from an array
        std::cout << "a - 2 = " << std::endl << a - 2 << std::endl;
    }


    void Minus_of_Arrays() {
        Vector3d pt1;
        pt1 << -15.8383, -0.0820843, 4.55381;

        Vector3d pt2;
        pt2 << 15.4368, -0.329181, 4.56863;

        Vector3d pt3 = pt2 - pt1;
        std::cout << pt3 << std::endl;
    }
}

void Arrays::TestAll()
{
    // Arrays::SimpleTest();
    // Arrays::Test2();
    // Arrays::Sum_of_Arrays();
    // Arrays::Minus_of_Arrays();
    // Arrays::SquaredNorm();
    // Arrays::SquaredNorm_3D();
    // Arrays::Array_Loop1();
    // Arrays::Array_Loop2();
};
