//============================================================================
// Name        : Vectors.h
// Created on  : 11.11.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Eigen Vectors
//============================================================================

#include <iostream>
#include <string>
#include <iomanip>
#include <string_view>

#include <Core>
#include <Dense>
#include "Vectors.h"

namespace Vectors {
    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vect) {
        std::cout << "[" << std::setprecision(12);
        for (const T& entry : vect)
            stream << entry << ' ';
        std::cout << "]";
        return stream;
    }

    template<typename _Ty, size_t _Size>
    std::ostream& operator<<(std::ostream& stream, const std::array<_Ty, _Size>& data) {
        std::cout << "[" << std::setprecision(12);
        for (const _Ty& entry : data)
            stream << entry << ' ';
        std::cout << "]";
        return stream;
    }
}


namespace Vectors
{
    void Create()
    {
        {
            Eigen::Vector3d vect;
            vect.setZero();
            std::cout << vect ;
        }
        std::cout << "\n-------------------------------------------------------\n";
        {
            std::vector<double> src{1, 2, 3};

            Eigen::Vector3d pt2(src.data());
            std::cout << pt2;
        }
        std::cout << "\n-------------------------------------------------------\n";
        {
            std::vector<double> src{1, 2, 3};
            Eigen::Vector3d pt2 {Eigen::Vector3d::Zero () };

            std::uninitialized_move_n(src.data(), 3, pt2.data());
            std::cout << pt2;
        }

        std::cout << "\n-------------------------------------------------------\n";
        {
            std::vector<double> src{1, 2, 3};
            Eigen::Vector3d pt2 {Eigen::Vector3d::Zero () };

            std::uninitialized_copy_n(src.data(), pt2.size(), pt2.data());
            std::cout << pt2;
        }
    }


    void Sum() {
        const Eigen::Vector3d v1 {1, 2, 3}, v2 {3, 5 ,7};

        Eigen::Vector3d v3 = v1 + v2;
        std::cout << v3 << std::endl;
    }

    void Minus() {
        const Eigen::Vector3d v1 {1, 2, 3}, v2 {3, 5 ,7};
        Eigen::Vector3d v3 = v2 - v1;

        std::cout << v3 << std::endl;
        // std::cout << v3.squaredNorm() << std::endl;
    }

    void Segment_Sublist() {
        Eigen::ArrayXd v(9);
        v << 1, 2, 3, 4, 5, 6, 7, 8, 9;


        std::cout << v.segment(0, 3) << std::endl;
        std::cout << v.segment(3, 3) << std::endl;
        std::cout << v.segment(6, 3) << std::endl;

    }

    void Tests() {

        Eigen::VectorXd xv(9);
        std::cout << xv << std::endl;
        std::cout << "-------------------------------------" << std::endl;

        std::vector<double> src {1,2,3,4,5,6,7,8,9};
        Eigen::Array<double, 9, 1> v(src.data());

        auto v1 = v.segment(0, 3);
        auto v2 = v.segment(3, 3);
        auto v3 = v.segment(6, 3);



        std::cout << v1<< std::endl;
        std::cout << v2 << std::endl;
        std::cout << v3 << std::endl;

    }

    void BasicOperations() {
        const Eigen::Vector3d v {1,2,3}, w{0,1,2};

        std::cout << "Dot product: " << v.dot(w) << std::endl;
        double dp = v.adjoint()*w; // automatic conversion of the inner product to a scalar
        std::cout << "Dot product via a matrix product: " << dp << std::endl;
        std::cout << "Cross product:\n" << v.cross(w) << std::endl;
    }

    void Loop() {
        const Eigen::Vector3d v {1, 2, 3};

        for (auto i: v) {
            std::cout << i << std::endl;
        }
    }

    void Normalize() {
        Eigen::Vector3d v {3, 2, 1};
        std::cout << v << std::endl;

        v.normalize();
        std::cout << v << std::endl;
    }


    void Vector_vs_StdArray() {
        [[maybe_unused]]
        std::array<double, 3> pt1 {1.1, 2.0, 3.0};
        std::cout << pt1 << std::endl;


        Eigen::Vector3d pt2 {1.1, 2.0, 3.0};
        std::cout << pt2 << std::endl;
    }
};

void Vectors::TestAll()
{
    // Create();

    // Loop();
    // Sum();
    // Minus();

    // Segment_Sublist();

    // BasicOperations();

    // Normalize();

    // Vector_vs_StdArray();

    Tests();
};