//============================================================================
// Name        : EigenLinearRegression.cpp
// Created on  : 2021-10-31.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ML Experiments
//============================================================================

#include "EigenLinearRegression.h"

#include <Dense>
#include <IterativeLinearSolvers>

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

namespace EigenLinearRegression
{
    std::pair<Eigen::MatrixXf, Eigen::MatrixXf> GenerateData(size_t size) {
        std::vector<float> x_data(size), y_data(size);

        std::iota(x_data.begin(), x_data.end(), 0);
        std::iota(y_data.begin(), y_data.end(), 0);

        // mutate data
        std::random_device rd;
        std::mt19937 re(rd());
        std::uniform_real_distribution<float> dist(-1.5f, 1.5f);

        for (size_t i = 0; i < size; ++i) { // add noise
            x_data[i] += dist(re);
            y_data[i] += dist(re);
        }

        Eigen::Map<Eigen::MatrixXf> x(x_data.data(), static_cast<Eigen::Index>(size), 1);
        Eigen::Map<Eigen::MatrixXf> y(y_data.data(), static_cast<Eigen::Index>(size), 1);
        return {x, y};
    }

    void Test() {
        constexpr size_t size = 1000;
        constexpr float A = 1.3f, B = 4.0f;

        // generate training data
        Eigen::MatrixXf x1, y;
        std::tie(x1, y) = GenerateData(size);
        Eigen::MatrixXf x0 = Eigen::MatrixXf::Ones(size, 1);

        // setup line coeficients y = b(4) + k(0.3)*x
        y.array() *= A;
        y.array() += B;
        Eigen::MatrixXf x(size, 2);
        x << x0, x1;

        // train estimator
        Eigen::LeastSquaresConjugateGradient<Eigen::MatrixXf> gd;
        gd.setMaxIterations(100);
        gd.setTolerance(0.001f);
        gd.compute(x);
        Eigen::VectorXf b = gd.solve(y);
        std::cout << "Estimated parameters vector : " << b << std::endl;

        // normal equations
        Eigen::VectorXf b_norm = (x.transpose() * x).ldlt().solve(x.transpose() * y);
        std::cout << "Estimated with normal equation parameters vector : " << b_norm << std::endl;

        // predict
        /*
        Eigen::MatrixXf new_x(5, 2);
        new_x << 1, 1, 1, 2, 1, 3, 1, 4, 1, 5;
        auto new_y = new_x.array().rowwise() * b.transpose().array();
        std::cout << "Predicted values : \n" << new_y << std::endl;

        auto new_y_norm = new_x.array().rowwise() * b_norm.transpose().array();
        std::cout << "Predicted(norm) values : \n" << new_y_norm << std::endl;
        */
    };
}


namespace EigenLinearRegression::SimpleLinearSolvers {

    void Basic() {
        Eigen::Matrix3f A;
        Eigen::Vector3f b;
        A << 1,  2,  3,
             4,  5,  6,
             7,  8,  10;
        b << 3, 3, 4;

        std::cout << "Here is the matrix A:\n" << A << std::endl;
        std::cout << "Here is the vector b:\n" << b << std::endl;

        const Eigen::Vector3f x = A.colPivHouseholderQr().solve(b);
        std::cout << "The solution is:\n" << x << std::endl;
    }

    void Basic2()
    {
        Eigen::Matrix2f A, b;
        A << 2, -1, -1, 3;
        b << 1, 2, 3, 1;

        std::cout << "Here is the matrix A:\n" << A << std::endl;
        std::cout << "Here is the right hand side b:\n" << b << std::endl;

        Eigen::Matrix2f x = A.ldlt().solve(b);
        std::cout << "The solution is:\n" << x << std::endl;
    }

    void LeastSquares() {
        Eigen::MatrixXf A = Eigen::MatrixXf::Random(3, 2);
        Eigen::VectorXf b = Eigen::VectorXf::Random(3);

        std::cout << "Here is the matrix A:\n" << A << std::endl;
        std::cout << "Here is the right hand side b:\n" << b << std::endl;
        std::cout << "The least-squares solution is:\n"
             << A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b) << std::endl;
    }

}

void EigenLinearRegression::TestAll()
{
    // Test();

    // SimpleLinearSolvers::Basic();
    // SimpleLinearSolvers::Basic2();
    SimpleLinearSolvers::LeastSquares();
};
