//============================================================================
// Name        : Matrices.cpp
// Created on  : 09.11.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : LeastSquares
//============================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <algorithm>

#include <thread>
#include <future>
#include <mutex>
#include <atomic>

#include <vector>
#include <array>
#include <set>

#include <math.h>
#include <Core>
#include <Geometry>

#include "Matrices.h"

namespace Matrices {
    void Create_and_Init() {
        Eigen::Matrix3f A;

        A << 1.0f, 0.0f, 0.0f,
                0.0f, 2.0f, 0.0f,
                0.0f, 0.0f, 3.0f;

        for (int i = 0; i < 3; ++i) {
            for (int n = 0; n < 3; ++n) {
                std::cout << A (i , n) << "  ";
            }
            std::cout << std::endl;
        }
    }


    void Matrix_Basic_Operations() {
        Eigen::Matrix<double, 2, 3> my_matrix;
        my_matrix << 1, 2, 3,
                     4, 5, 6;

        std::cout << my_matrix << std::endl;
        std::cout << my_matrix.transpose()<< std::endl;

        int i,j;
        std::cout << "minCoeff: " <<my_matrix.minCoeff(&i, &j)<<std::endl;
        std::cout << "maxCoeff: " <<my_matrix.maxCoeff(&i, &j)<<std::endl;
        std::cout << "prod: " <<my_matrix.prod()<<std::endl;
        std::cout << "sum: " <<my_matrix.sum()<<std::endl;
        std::cout << "mean: " <<my_matrix.mean()<<std::endl;
        std::cout << "trace: " <<my_matrix.trace()<<std::endl;
        std::cout << "colwise: " <<my_matrix.colwise().mean()<<std::endl;
        std::cout << "rowwise: " <<my_matrix.rowwise().maxCoeff()<<std::endl;
        std::cout << "lpNorm<2>: " <<my_matrix.lpNorm<2>()<<std::endl;
        std::cout << "minCoeff: " <<my_matrix.lpNorm<Eigen::Infinity>()<<std::endl;
        std::cout << "all elemnts are positive: " <<(my_matrix.array()>0).all()<<std::endl;
        std::cout << "any element is greater than 2: " <<(my_matrix.array()>2).any()<<std::endl;
        std::cout << "count the number of elements greater than 1: " <<(my_matrix.array()>1).count()<<std::endl;
        std::cout << "array() - 2: "  << my_matrix.array() - 2 << std::endl;
        std::cout << "array().abs(): "  << my_matrix.array().abs() << std::endl;
        std::cout << "array().square() : "  << my_matrix.array().square() << std::endl;
        std::cout << "minCoeff: "  << my_matrix.array() * my_matrix.array() << std::endl;
        std::cout << "array().exp() : "  << my_matrix.array().exp() << std::endl;
        std::cout << "array().log() : "  << my_matrix.array().log() << std::endl;
        std::cout << "array().sqrt() : "  << my_matrix.array().sqrt() << std::endl;
    }

    void Summ_and_Substract() {
        Eigen::Matrix2d a;
        a << 1, 2,
                3, 4;

        Eigen::MatrixXd b(2,2);
        b << 2, 3,
                1, 4;

        std::cout << "~~~~~~~~~~~~~~~~~ a + b =~~~~~~~~~~~~~~~~~ \n" << a + b << std::endl << std::endl;
        std::cout << "~~~~~~~~~~~~~~~~~ a - b =~~~~~~~~~~~~~~~~~ \n" << a - b << std::endl << std::endl;

        std::cout << "~~~~~~~~~~~~~~~~~ a += b ~~~~~~~~~~~~~~~~~ " << std::endl;
        a += b;
        std::cout << a << std::endl << std::endl;
    }


    void Multiplication_and_Division() {
        Eigen::Matrix2d a;
        Eigen::Vector3d v(1,2,3);

        a << 1, 2,
                3, 4;

        std::cout << "a * 2.5 =\n" << a * 2.5 << std::endl;
        std::cout << "0.1 * v =\n" << 0.1 * v << std::endl;
        std::cout << "Doing v *= 2;" << std::endl;
        v *= 2;
        std::cout << "Now v =\n" << v << std::endl;

    }

    void SquaredNorm()
    {
        Eigen::MatrixXf m(2,2);

        constexpr std::array<int, 4> values {1, -2, -3, 4};
        std::cout << "Original: " <<  std::accumulate(values.cbegin(), values.end(), 0, [](int a, int b) {
            return a + b * b; }) << std::endl;

        m << values[0], values[1],
             values[2], values[3];

        std::cout << "\nm.squaredNorm() = " << m.squaredNorm() << std::endl;
        std::cout << "m.norm() = " << m.norm() << std::endl;
        std::cout << "m.lpNorm<1>() = " << m.lpNorm<1>() << std::endl;
        std::cout << "m.lpNorm<Infinity>() = " << m.lpNorm<Eigen::Infinity>() << std::endl;
    }

    void Create1() {
        Eigen::Matrix <short, 2 ,2> matrix {
                {1, 2},
                {3, 4}
        };

        std::cout << matrix << std::endl;
    }

    void CreateRandom() {
        Eigen::Matrix<short, 2 ,2> a =  Eigen::Matrix <short, 2 ,2>::Random(2, 2);
        std::cout << a << std::endl << std::endl;

        Eigen::Matrix<short, 4 ,2>  b =  Eigen::Matrix <short, 4 ,2>::Random(4, 2);
        std::cout << b << std::endl << std::endl;
    }

    void Multiply() {
        Eigen::Matrix <short, 2 ,2> matrix {
                {1, 2},
                {3, 4}
        };

        std::cout << matrix * 2 << std::endl;
    }

    void Multiply_With_Vector() {
        Eigen::Matrix <short, 1 ,2> m {
                {1, 2},
                {3, 4}
        };
        const Eigen::Vector3i v {1, 2};

        // ???????????????
    }

    void MultiplyMatrices() {
        Eigen::Matrix <short, 2 ,2> a, b;

        a << 1, 2,
             3, 4;
        b << 5, 6,
             7, 8;

        std::cout << a * b << std::endl;
    }


    void Access_Rows_Columns() {
        Eigen::Matrix <short,2 ,3> a {
            { 1, 2, 3 },
            { 4, 5, 6 },
        };

        std::cout << "rows count: " << a.rows()  << std::endl;
        for (Eigen::Index i = 0; i < a.rows(); ++i) {
            Eigen::Matrix <short,1 ,3> row = a.row(i);
            std::cout << "Row[" << i << "]  = {" << row << "}\n";
        }

        std::cout << "\ncols = " << a.cols()  << std::endl;
        for (Eigen::Index i = 0; i < a.cols(); ++i)
            std::cout << "Row[" << i << "]  = {" << a.col(i) << "}\n";
    }


    void Column_Wise_Operations()
    {
        Eigen::Matrix<short, 2, 3> a{
                {1, 2, 3},
                {4, 5, 6},
        };

        for (size_t i = 0; i < a.rows(); ++i) {
            Eigen::Matrix <short,1 ,3> row = a.row(i);
            for (long n = 0; n < row.size(); ++n)
                std::cout << row[n] << " " ;
            std::cout << std::endl;
        }
    }

    void Product() {
        Eigen::Matrix <short,2 ,3> a {
                { 1, 2, 3 },
                { 4, 5, 6 }
        };

        std::cout << a.prod() << std::endl; // 720
    }

    void ColumnWise_Product() {
        {
            Eigen::Matrix<short, 2, 3> a{
                    {1, 2, 3},
                    {4, 5, 6}
            };
            Eigen::Matrix<short, 2, 3> b{
                    {2, 3, 4},
                    {5, 6, 7}
            };
            std::cout << a.cwiseProduct(b) << std::endl;
        }
    }

    void Tests() {
        std::array<double, 9> values {1,2,3,4,5,6,7,8,9};
        Eigen::Matrix3d matrix ( values.data() );

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                std::cout << matrix.data()[row * 3 + col] << ' ';
            }
            std::cout << std::endl;
        }


    }

    void Transpose() {
        Eigen::Matrix3f A;
        A << 1, 2, 3,
             4, 5, 6,
             7, 8, 9;

        std::cout << A << std::endl << std::endl;
        std::cout << A.transpose() << std::endl << std::endl;
        std::cout << A << std::endl << std::endl;
    }

    void Determinant()
    {
        Eigen::Matrix3f A;
        A << 1,  2,  1,
             2,  1,  0,
             -1, 1,  2;
        std::cout << "Here is the matrix A:\n" << A << std::endl;
        std::cout << "The determinant of A is " << A.determinant() << std::endl;
        std::cout << "The inverse of A is:\n" << A.inverse() << std::endl;
    }
}


namespace Matrices::Rotation
{
    [[nodiscard]]
    constexpr double degToRad(const double angle) noexcept {
        return angle * std::numbers::pi / 180;
    };

    [[nodiscard]]
    Eigen::Matrix3d createRotationMatrix(const double xAngle,
                                         const double yAngle,
                                         const double zAngle) noexcept {
        [[maybe_unused]] const double sinA = std::sin(degToRad(zAngle));
        [[maybe_unused]] const double cosA = std::cos(degToRad(zAngle));

        [[maybe_unused]] const double sinB = std::sin(degToRad(yAngle));
        [[maybe_unused]] const double cosB = std::cos(degToRad(yAngle));

        [[maybe_unused]] const double sinX = std::sin(degToRad(xAngle));
        [[maybe_unused]] const double cosX = std::cos(degToRad(xAngle));

        //  operator()(Index row, Index col)
        Eigen::Matrix3d Rot_matrix(3, 3);
        Rot_matrix(0, 0) = cosA * cosB; // OK
        Rot_matrix(0, 1) = -sinA * cosB;
        Rot_matrix(0, 2) = -sinB; // OK

        Rot_matrix(1, 0) = cosA * sinB * sinX - sinA * cosX; // OK
        Rot_matrix(1, 1) = sinA * sinB * sinX + cosA * cosX; // OK
        Rot_matrix(1, 2) = cosB * sinX; // OK

        Rot_matrix(2, 0) = cosA * sinB * cosX + sinA * sinX;
        Rot_matrix(2, 1) = sinA * sinB * cosX - cosA * sinX; // OK
        Rot_matrix(2, 2) = cosB * cosX; // OK

        return Rot_matrix;
    }


    void Quaternion_From_Matrix() {
        Eigen::Matrix3f matrix;

        matrix << 0.9988237836840151,  0.044008217428877484,  -0.020354998056510223,
                 -0.04536624999068998, 0.9963775959122178,   -0.07192767009970338,
                 0.017115855483443205, 0.07276649753095299,  0.9972021281205471;

        Eigen::Quaternion<float> quaternion(matrix);
        std::cout << std::setprecision(12) << quaternion << std::endl;

        // Expected:
        // -0.0362079404294
        // 0.00937662180513
        // 0.0223648641258
        // 0.999049961567
    }


    void CreateRotationMatrix()
    {
        // const Eigen::Matrix3d M = createRotationMatrix(0, 15, 76);
        // std::cout << M << std::endl;

        constexpr float xAngle = degToRad(37);
        constexpr float zAngle = degToRad(55);

        const float sinX = std::sin(xAngle);
        const float cosX = std::cos(xAngle);
        const float sinZ = std::sin(zAngle);
        const float cosZ = std::cos(zAngle);

        const Eigen::Matrix3f xM {
                {1, 0, 0},
                {0, cosX, -sinX},
                {0, sinX, cosX}
        };
        const Eigen::Matrix3f zM {
                {cosZ, -sinZ, 0},
                {sinZ, cosZ, 0},
                {0, 0, 1}
        };

        const Eigen::Matrix3f x2 {
                {1 * cosZ, -sinZ, 0},
                {cosX * sinZ, cosX * cosZ, -sinX},
                {sinX * sinZ, sinX * cosZ, cosX}
        };

        const Eigen::Matrix3f M = xM * zM;
        std::cout << M << std::endl;
        std::cout << x2 << std::endl;
    }
}

void Matrices::TestAll()
{
    // Create1();
    // CreateRandom();

    // Multiply();
    // Multiply_With_Vector();
    // MultiplyMatrices();


    // Product();
    // ColumnWise_Product();

    // Create_and_Init();
    // Matrix_Basic_Operations();
    // Summ_and_Substract();
    // Multiplication_and_Division();
    // SquaredNorm();

    // Access_Rows_Columns();
    // Column_Wise_Operations();

    // Determinant();

    // Transpose();

    // Tests();

    // Rotation::Quaternion_From_Matrix();
    Rotation::CreateRotationMatrix();

};
