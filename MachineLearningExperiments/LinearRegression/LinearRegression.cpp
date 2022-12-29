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
#include <algorithm>
#include <numeric>
#include <random>
#include <fstream>

#include "LinearRegression.h"
#include "../Utilities//Utilities.h"
#include "../Types/Types.h"

namespace LinearRegression::Utilities {

    [[nodiscard]]
    double getRandom(const double start = 0.0f,
                     const double end = 10.0f) {
        std::random_device rd{};
        std::mt19937 generator { rd() };
        std::uniform_real_distribution<double> distribution(start, end);
        return distribution(generator);
    }

    [[nodiscard]]
    std::pair<std::vector<double>, std::vector<double>>
    GenerateData(double slope, double intercept,
                 double start, double end,
                 size_t size, double noise)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distribution(-noise, noise);

        std::vector<double> X(size, start), Y(size, 0);
        const double step = (end - start) / (size - 1);
        for (size_t i = 0; i < size; ++i) {
            X[i] += i * step;
            Y[i] = X[i] * slope + intercept + distribution(gen);
        }

        return {X, Y};
    }
}

namespace LinearRegression::SimpleTests {

    double lineEquation(double a, double b, double x) noexcept {
        return x * a + b;
    };

    void DemoTest() {
        const float A = 3.2, B = 5;
        const auto& [X, Y] = Utilities::GenerateData(A, B, 1, 100, 200, 10);

        double yPredicted {0}, error { std::numeric_limits<double>::max() }, bPredicted {0}, aPredicted {0};
        constexpr double learningRate = 1e-6; // {0.01}
        const size_t dataSize { X.size() }, epochs { 1'000'000 }; // 500
        size_t epochsCounter = 0;

        /** Training Phase: **/
        for (size_t n = 0; n < epochs; ++n, ++epochsCounter) {
            for (size_t i = 0; i < dataSize; i ++) {
                yPredicted = lineEquation(aPredicted, bPredicted, X[i]); // calculating prediction
                const double err = yPredicted - Y[i];                    // calculating error
                bPredicted = bPredicted - learningRate * err;            // updating b0
                aPredicted = aPredicted - learningRate * err * X[i];     // updating b1
                error = std::min(error, std::abs(err));
            }
            if (learningRate > error)
                break;
        }
        std::cout << "Predicted: [" << aPredicted << ", " << bPredicted << "]. Error = " << error << "\n";
        std::cout << "Error = " << error << ". epochsCounter = " << epochsCounter << "\n";

        std::ofstream outFile(R"(/home/andtokm/tmp/data.csv)", std::ios::trunc);
        outFile << "x,y,y_pred" << std::endl;
        for (size_t i = 0; i < dataSize; i ++) {
            const double predictedY =  aPredicted * X[i] + bPredicted;
            outFile << X[i] << "," << Y[i] << "," << predictedY << std::endl;
            // std::cout << pt[0] << "," << pt[1]  << "," << predictedY << std::endl;
        }
    }

    void DemoTest2() {
        const float A = 3.2, B = 5;
        const auto& [X, Y] = Utilities::GenerateData(A, B, 1, 60, 120, 10);
        const size_t dataSize { X.size() };

        /*
        const size_t dataSize { 100 };
        std::vector<double> X(dataSize), Y;
        std::iota(X.begin(), X.end(), 1);
        for (const auto v: X) {
            Y.push_back(lineEquation(A, B, v));
            Y.back() += Utilities::getRandom(-Y.back()/3, Y.back()/3);
        }
         */

        double aPredicted = Utilities::getRandom(), bPredicted = Utilities::getRandom();
        std::cout << "Start values: [" << aPredicted << ", " << bPredicted << "]\n";

        std::vector<double> diff(dataSize), grad_y_pred(dataSize), tmp(dataSize);
        constexpr double learning_rate = 1e-6;
        constexpr size_t epochs { 100'000};

        for (size_t i = 0; i < epochs; ++i)
        {
            for (size_t i = 0; i < dataSize; ++i)
                diff[i] = lineEquation(aPredicted, bPredicted, X[i]) - Y[i];

            // Compute loss: Mean Squared Error
            // The sum of the squares of the difference between the assumed value and the actual
            const double loss = VectorUtilities::SquareSum(diff);

            VectorUtilities::Multiply(diff, 4.0, grad_y_pred);  // [ diff * 4 ]        --> grad_y_pred
            VectorUtilities::Multiply(grad_y_pred, X, tmp);     // [ grad_y_pred * x ] --> tmp

            const double grad_a = std::accumulate(tmp.cbegin(), tmp.cend(), 0.0);
            const double grad_b = std::accumulate(grad_y_pred.cbegin(), grad_y_pred.cend(), 0.0);

            aPredicted -= learning_rate * grad_a;
            bPredicted -= learning_rate * grad_b;
            if (0.005 > loss) {
                std::cout << "Iter count = " << i << std::endl;
                break;
            }
        }
        std::cout << aPredicted << ", " << bPredicted << std::endl;

        std::ofstream outFile(R"(/home/andtokm/tmp/data.csv)", std::ios::trunc);
        outFile << "x,y,y_pred" << std::endl;
        for (size_t i = 0; i < dataSize; i ++) {
            const double predictedY =  aPredicted * X[i] + bPredicted;
            outFile << X[i] << "," << Y[i] << "," << predictedY << std::endl;
            // std::cout << pt[0] << "," << pt[1]  << "," << predictedY << std::endl;
        }
    }

    void DemoTest_Normalization() {
        const float A = 3.2, B = 5;
        const auto& [X_Orig, Y_Orig] = Utilities::GenerateData(A, B, 1, 200, 500, 30);
        const size_t dataSize { X_Orig.size() };
        double aPredicted = Utilities::getRandom(), bPredicted = Utilities::getRandom();

        double max {0};
        std::vector<double> X {X_Orig}, Y {Y_Orig};
        {
            const double xMax = *std::max_element(X.cbegin(), X.cend());
            const double yMax = *std::max_element(Y.cbegin(), Y.cend());
            max = std::max(xMax, yMax);
            std::for_each(X.begin(), X.end(), [max](auto &v) { v = v / max;} );
            std::for_each(Y.begin(), Y.end(), [max](auto &v) { v = v / max;} );
        }

        std::vector<double> diff(dataSize), grad_y_pred(dataSize), tmp(dataSize);
        constexpr double learning_rate = 1e-6;
        constexpr size_t epochs { 100'000};

        for (size_t i = 0; i < epochs; ++i)
        {
            for (size_t i = 0; i < dataSize; ++i)
                diff[i] = lineEquation(aPredicted, bPredicted, X[i]) - Y[i];

            // Compute loss: Mean Squared Error
            // The sum of the squares of the difference between the assumed value and the actual
            const double loss = VectorUtilities::SquareSum(diff);

            VectorUtilities::Multiply(diff, 4.0, grad_y_pred);  // [ diff * 4 ]        --> grad_y_pred
            VectorUtilities::Multiply(grad_y_pred, X, tmp);     // [ grad_y_pred * x ] --> tmp

            const double grad_a = std::accumulate(tmp.cbegin(), tmp.cend(), 0.0);
            const double grad_b = std::accumulate(grad_y_pred.cbegin(), grad_y_pred.cend(), 0.0);

            aPredicted -= learning_rate * grad_a;
            bPredicted -= learning_rate * grad_b;
            if (0.001 > loss) {
                std::cout << "Iter count = " << i << std::endl;
                break;
            }
        }
        std::cout << aPredicted << ", " << bPredicted << std::endl;

        std::ofstream outFile(R"(/home/andtokm/tmp/data.csv)", std::ios::trunc);
        outFile << "x,y,y_pred" << std::endl;
        for (size_t i = 0; i < dataSize; i ++) {
            const double predictedY =  aPredicted * X[i] + bPredicted;
            outFile << X[i] * max << "," << Y[i]* max << "," << predictedY * max << std::endl;
            // std::cout << pt[0] << "," << pt[1]  << "," << predictedY << std::endl;
        }
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

        const size_t size{ points.size() };
        const _Ty a = (size * xySum - xSum * ySum) / (size * x2Sum - xSum * xSum);  // calculate slope
        const _Ty b = (x2Sum * ySum - xSum * xySum) / (size * x2Sum - xSum * xSum);  // calculate intercept

        std::vector<_Ty> yFit;
        yFit.reserve(size);
        for (const auto& pt : points)
            yFit.push_back(a * pt[0] + b);

        std::cout << "S.no" << std::setw(10) << "x" << std::setw(19)
                  << "y (actual)" << std::setw(19) << "y (predicted)" << std::endl;
        std::cout << "-----------------------------------------------------------------\n";
        // for (size_t i = 0; const auto & pt: points) {
        for (size_t i = 0; i < yFit.size() && i < points.size(); ++i) {
            const auto& pt = points[i];
            std::cout << i + 1 << "." << std::setw(15) << pt[0] << std::setw(15)
                      << pt[1] << std::setw(18) << yFit[i] << std::endl;
        }
        std::cout << "The linear fit line is of the form :  " << a << " * x + " << b << std::endl;
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

        LinearRegression<double> linReg;
        std::pair<double, double> predicted = linReg.estimate(X, Y);

        std::ofstream outFile(R"(/home/andtokm/tmp/data.csv)", std::ios::trunc);
        outFile << "x,y,y_pred" << std::endl;
        for (size_t i = 0; i < X.size(); ++i) {
            const double predictedY = X[i] * predicted.first + predicted.second;
            outFile << X[i] << "," << Y[i] << "," << predictedY << std::endl;
            // std::cout << X[i] << "," << Y[i] << "," << predictedY << std::endl;
        }
    }
}


void LinearRegression::TestAll()
{
    // SimpleTests::DemoTest();
    // SimpleTests::DemoTest2();
    SimpleTests::DemoTest_Normalization();

    // AlgoritmOne::Test2D();
    // AlgoritmOne::Test2D_1();
    // AlgoritmOne::Test2D_2();

    // AlgoritmTwo::Test1();

    // ClassTest::Test();
}