//============================================================================
// Name        : PolynomialFitting.cpp
// Created on  : 10.11.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ML Eigen polynomial fitting experiments
//============================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <cmath>
#include <vector>

#include <Dense>
#include "matplotlibcpp.h"
#include <cmath>
#include "PolynomialFitting.h"

namespace PolynomialFitting
{
    void polyfit(const std::vector<double> &x,
                 const std::vector<double> &y,
                 std::vector<double> &coeff,
                 const size_t order)
    {
        // Create Matrix Placeholder of size n x k, n= number of data points,
        // k = order of polynomial, for example k = 3 for cubic polynomial
        Eigen::MatrixXd T(x.size(), order + 1);
        Eigen::VectorXd V = Eigen::VectorXd::Map(&y.front(), y.size());

        // check to make sure inputs are correct
        assert(t.size() == v.size());
        assert(t.size() >= order + 1);

        // Populate the matrix
        for(size_t i = 0 ; i < x.size(); ++i){
            for(size_t j = 0; j < order + 1; ++j) {
                T(i, j) = pow(x[i], j);
            }
        }
        std::cout << T << std::endl;

        // Solve for linear least square fit
        const Eigen::VectorXd result  = T.householderQr().solve(V);
        coeff.resize(order + 1);
        for (size_t k = 0; k < order + 1; k++)
            coeff[k] = result[k];
    }

    void LeastSquarePolynomialFit()
    {
        const std::vector<double> X { // time value
            0, 0.0192341804504395, 0.0394501686096191, 0.059575080871582, 0.0790810585021973, 0.0792751312255859,
            0.0987141132354736, 0.119336366653442,  0.138712167739868, 0.159000158309937,  0.178890228271484,
            0.19960618019104,   0.219112157821655,  0.23919415473938,  0.259442090988159,  0.279186248779297,
            0.299112319946289,  0.319219350814819,  0.339494228363037, 0.339675188064575,  0.359552145004272,
            0.37941837310791,   0.399189233779907,  0.419828176498413, 0.439810276031494,  0.459331274032593,
            0.479461193084717,  0.499663114547729,  0.519809246063232, 0.539092063903809,  0.559118270874023,
            0.579315185546875,  0.598889112472534,  0.619685173034668, 0.638863086700439,  0.639052152633667,
            0.658920288085938,  0.679149150848389,  0.699787139892578, 0.71905517578125,   0.73898720741272,
            0.739143371582031,  0.758654117584229,  0.779210329055786, 0.799195289611816,  0.819046258926392,
            0.839539289474487,  0.85923433303833,   0.87903618812561,  0.899263143539429,  0.919251203536987,
            0.939138174057007,  0.959244251251221,  0.979074239730835, 0.998935222625732,  1.01904726028442,
            1.0387852191925,    1.03895926475525,   1.05906510353088,  1.07873225212097,   1.09908628463745,
            1.11907029151917,   1.13899827003479,   1.15879201889038};

        const std::vector<double> Y = { // velocity value
                1.8, 1.86, 2.03, 2.08, 2.14, 2.14, 2.25, 2.36, 2.42, 2.59, 2.7,
                2.81, 2.87, 3.04, 3.15, 3.26, 3.32, 3.43, 3.54, 3.54, 3.6, 3.71,
                3.83, 3.94, 4.11, 4.22, 4.33, 4.44, 4.56, 4.67, 4.78, 4.84, 4.84,
                4.89, 4.89, 4.89, 4.95, 5.01, 5.06, 5.06, 5.06, 5.06, 5.01, 5.06,
                5.12, 5.18, 5.18, 5.23, 5.23, 5.23, 5.29, 5.34, 5.29,  5.4,  5.4,
                5.46, 5.51, 5.51, 5.51, 5.46,  5.4, 5.34, 5.34, 5.34};

        // placeholder for storing polynomial coefficient
        std::vector<double> coefficients;
        polyfit(X, Y, coefficients, 3);

        // std::cout << "Validate results: \n";
        std::vector<double> yPredicted;
        for(size_t i = 0; i < X.size(); ++i) {
            const double predicted = coefficients[0] + coefficients[1] * X[i] +
                               coefficients[2] * (pow(X[i], 2)) +
                               coefficients[3] * (pow(X[i], 3)) ;
            // std::cout << Y[i] << "     " <<  predicted << "\n";
            yPredicted.push_back(predicted);
        }

        matplotlibcpp::plot(X, Y);
        matplotlibcpp::plot(X, yPredicted,"r--");
        matplotlibcpp::show();
    }

    void PolynomialFit_ForAutoModeling()
    {
        const std::vector<double> X { -25.5461, -18.9445, 0.0308352, 20.1581, 28.2013};
        const std::vector<double> Y = { -3.96054, 13.5125, 29.693, 14.1476, -3.13935};

        // placeholder for storing polynomial coefficient
        std::vector<double> coefficients;
        polyfit(X, Y, coefficients, 3);

        std::vector<double> xPoints, yPoints;
        const double stepsCount = 100;
        const double step = (X.back() - X.front()) / stepsCount;
        for(double i = X.front(); i < X.back(); i += step) {
            const double predicted = coefficients[0] + coefficients[1] * i +
                    coefficients[2] * (pow(i, 2)) + coefficients[3] * (pow(i, 3)) ;
            xPoints.push_back(i);
            yPoints.push_back(predicted);
        }

        matplotlibcpp::scatter(X, Y);
        matplotlibcpp::plot(xPoints, yPoints,"r--");
        matplotlibcpp::show();
    }
};

void PolynomialFitting::TestAll() {
    // LeastSquarePolynomialFit();
    PolynomialFit_ForAutoModeling();
};

