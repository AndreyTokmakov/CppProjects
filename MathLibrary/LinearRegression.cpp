//============================================================================
// Name        : LinearRegression.cpp
// Created on  : 08.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Math src
//============================================================================

#include <iomanip>
#include <string>
#include <random>
#include <algorithm>
#include <numeric>
#include <fstream>

#include "LinearRegression.h"
#include "Utilities.h"

namespace LinearRegression::Utilities {

    std::pair<std::vector<float>, std::vector<float>>
    GenerateData(double slope, double intercept,
                 double start, double end,
                 size_t size, double noise)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distribution(-noise, noise);

        std::vector<float> X(size, start), Y(size, 0);
        const double step = (end - start) / (size - 1);
        for (size_t i = 0; i < size; ++i) {
            X[i] += i * step;
            Y[i] = X[i] * slope + intercept + distribution(gen);
        }

        return {X, Y};
    }
}


namespace LinearRegression::AlgoritmOne {
    // TODO: Taken from https://www.bragitoff.com/2015/09/c-program-to-linear-fit-the-data-using-least-squares-method/

    template<typename _Ty>
    void Interpolate_LeastSqures_2D(const std::vector<Types::Point<2, _Ty>>& points)
    {
        _Ty xSum{}, ySum{}, x2Sum{}, xySum{};
        for (const auto& pt : points) {
            xSum  += pt[0];          // calculate sigma(xi)
            ySum  += pt[1];          // calculate sigma(yi)
            x2Sum += pt[0] * pt[0];   // calculate sigma(x^2i)
            xySum += pt[0] * pt[1];   // calculate sigma(xi*yi)
        }

        std::cout << xSum << std::endl;
        std::cout << ySum << std::endl;
        std::cout << x2Sum << std::endl;
        std::cout << xySum << std::endl;

        const size_t size{ points.size() };
        const _Ty a = (size * xySum - xSum * ySum) / (size * x2Sum - xSum * xSum);  // calculate slope
        const _Ty b = (x2Sum * ySum - xSum * xySum) / (size * x2Sum - xSum * xSum);  // calculate intercept

        std::vector<_Ty> yFit;
        yFit.reserve(size);
        for (const auto& pt : points)
            yFit.push_back(a * pt[0] + b);

        std::cout << "S.no" << std::setw(5) << "x" << std::setw(19)
                 << "Y(observed)" << std::setw(19) << "y(fitted)" << std::endl;
        std::cout << "-----------------------------------------------------------------\n";
        // for (size_t i = 0; const auto & pt: points) {
        for (size_t i = 0; i < yFit.size() && i < points.size(); ++i) {
            const auto& pt = points[i];
            std::cout << i + 1 << "." << std::setw(8) << pt[0] << std::setw(15)
                      << pt[1] << std::setw(18) << yFit[i] << std::endl;
        }
        std::cout << "The linear fit line is of the form:" << a << "x + " << b << std::endl;
    }


    void Test2D() {
        const std::vector<Types::Point<2, double>> points{
                {50, 12}, {70, 15}, {100, 21}, {120, 25},
        };
        Interpolate_LeastSqures_2D(points);
    }

    void Test2D_1() {
        const std::vector<Types::Point<2, double>> points{
                {20.5, 765}, {32.7, 826}, {51.0, 873}, {73.2, 942}, {95.7 , 1032}
        };
        Interpolate_LeastSqures_2D(points);
    }

    void Test2D_2() {
        const std::vector<Types::Point<2, double>> points{
                {-15.3918, 13.8971 },
                {-15.3918, 13.8971 },
                {-15.3374, 14.1145 },
                {-15.1991, 14.3688 },
                {-15.0578, 14.5046 },
                {-14.9991, 14.6887 },
                {-14.7168, 14.9945 },
                {-14.5061, 15.3488 },
                {-14.2221, 15.2444 },
                {-13.9874, 15.4124 },
                {-13.7869, 15.7041 },
                {-13.5104, 15.7543 },
                {-13.5273, 16.1772 },
                {-13.2595, 16.1171 },
                {-13.0147, 16.1466 },
                {-12.8194, 16.5808 },
                {-12.5179, 16.9036 },
                {-12.5179, 16.9036 },
                {-12.2713, 17.2161 },
                {-12.0587, 17.2951 },
                {-12.0587, 17.2951 },
                {-11.8347, 17.3427 },
                {-11.7607, 17.4489 },
                {-11.7372, 17.5896 },
                {-11.7372, 17.5896 },
                {-11.3333, 17.7547 },
                {-11.3333, 17.7547 },
                {-11.1578, 17.9409 },
                {-11.1578, 17.9409 },
                {-10.8993, 18.0502 },
                {-10.8432, 18.11 },
                {-10.6483, 18.1455 },
                {-9.81791, 17.846 },
                {-9.70709, 17.7418 },
                {-9.63578, 17.6865 },
                {-9.66328, 17.7162 },
                {-9.49559, 17.4512 },
                {-9.49559, 17.4512 },
                {-9.5804,  17.5612 },
                {-9.45364, 17.3248 }
        };
        Interpolate_LeastSqures_2D(points);
    }
}

namespace LinearRegression::AlgoritmTwo {

    template<typename _Ty>
    std::pair<double, double> EstimateCoef(const std::vector<_Ty>& x,
                                           const std::vector<_Ty>& y)
    {
        const size_t size { x.size() };
        const double meanX = VectorUtilities::mean(x);
        const double meanY = VectorUtilities::mean(y);

        const double crossDeviationXX = VectorUtilities::MultAndSum(x, x) - size * meanX * meanX;
        const double crossDeviationXY = VectorUtilities::MultAndSum(x, y) - size * meanX * meanY;

        const double a = crossDeviationXY / crossDeviationXX;
        const double b = meanY - a * meanX;
        return { a, b };
    }

    void Test1() {
        std::vector<int> x { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::vector<int> y { 1, 3, 2, 5, 7, 8, 8, 9, 10, 12 };

        auto [a,b] = EstimateCoef(x, y);

        std::cout << "x" << std::setw(19) << "y(observed)" << std::setw(19) << "y(fitted)" << std::endl;
        for (size_t i = 0; i < x.size(); ++i) {
            auto y_pred = a * x[i] + b;
            std::cout << x[i] << std::setw(19) << y[i] << std::setw(19) << y_pred << std::endl;
        }
    }
}

namespace LinearRegression::ClassTest {

    template<typename Ty>
    class LinearRegression {
    public:
        std::pair<double, double> estimate(const std::vector<Ty>& x,
                                           const std::vector<Ty>& y)
        {
            const size_t size { x.size() };
            const double meanX { mean(x) };
            const double meanY { mean(y) };

            const double crossDeviationXX = multiplyAndSum(x, x) - size * meanX * meanX;
            const double crossDeviationXY = multiplyAndSum(x, y) - size * meanX * meanY;

            const double a = crossDeviationXY / crossDeviationXX;
            const double b = meanY - a * meanX;
            return { a, b };
        }

    private:
        [[nodiscard]]
        Ty mean(const std::vector<Ty>& data) noexcept {
            return (double)std::accumulate(data.cbegin(), data.cend(), 0.0f) / data.size();
        }

        [[nodiscard]]
        Ty multiplyAndSum(const std::vector<Ty>& data1,
                          const std::vector<Ty>& data2) noexcept {
            // TODO: Assert sizes
            return std::inner_product(data1.begin(), data1.end(), data2.begin(), 0.0f);
        }
    };


    void Test()
    {
        const float A = 3.2, B = 5;
        const auto& [X, Y] = Utilities::GenerateData(A, B, 1, 30, 60, 5);

        LinearRegression<float> linReg;
        std::pair<double, double> predicted = linReg.estimate(X, Y);

        // std::cout << predicred.first << ", " << predicred.second << std::endl;

        // std::ofstream myfile(R"(S:\Projects\TEST_DATA\CSV\ml.csv)", std::ios::app);
        // myfile<< "x,y,y_pred" << std::endl;
        for (size_t i = 0; i < X.size(); ++i) {
            const float predictedY = X[i] * predicted.first + predicted.second;
            // myfile<< X[i] << "," << Y[i] << "," << predictedY << std::endl;
            std::cout << X[i] << "," << Y[i] << "," << predictedY << std::endl;
        }
        // myfile.close();
    }
}

void LinearRegression::TestAll() {
    // AlgoritmOne::Test();

    // AlgoritmOne::Test2D();
    // AlgoritmOne::Test2D_1();
    // AlgoritmOne::Test2D_2();

    // AlgoritmTwo::Test1();


    ClassTest::Test();
}