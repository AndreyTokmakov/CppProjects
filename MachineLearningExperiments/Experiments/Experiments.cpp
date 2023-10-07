//============================================================================
// Name        : Experiments.cpp
// Created on  : 2021-10-10.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ML Experiments
//============================================================================

#include <iostream>
#include <random>
#include <iomanip>
#include <fstream>
#include <algorithm>

#include "../Utilities/Utilities.h"
#include "Experiments.h"

namespace Experiments {

    template<typename _Ty>
    std::ostream& operator<<(std::ostream& stream, const std::vector<_Ty>& vect) {
        for (const auto& v: vect)
            stream << v << ' ';
        return stream;
    }

    double getRandom(const double start = 0.0f,
                     const double end = 10.0f) {
        std::random_device rd{};
        std::mt19937 generator { rd() };
        std::uniform_real_distribution<double> distribution(start, end);
        return distribution(generator);
    }

    double lineEquation(double a, double b, double x) noexcept {
        return x * a + b;
    };

}

namespace Experiments::Line {

    void Predict() {
        constexpr double a = 3, b = 1.5;
        constexpr size_t count = 100;

        std::vector<double> x(count), y;
        std::iota(x.begin(), x.end(), 1);
        for (const auto v: x)
            y.push_back(lineEquation(a, b, v));

        double a_pred = getRandom(), b_pred = getRandom();
        std::cout << "Start values: [" << a_pred << ", " << b_pred << "]\n";

        std::vector<double> diff(x.size());
        std::vector<double> grad_y_pred(x.size());
        std::vector<double> tmp(x.size());

        constexpr double learning_rate = 1e-6;
        constexpr size_t epochs { 100'000};

        for (size_t i = 0; i < epochs; ++i) {
            for (size_t i = 0; i < count; ++i) {
                diff[i] = lineEquation(a_pred, b_pred, x[i]) - y[i];
            }

            // Compute loss: Mean Squared Error
            // The sum of the squares of the difference between the assumed value and the actual
            const double loss = VectorUtilities::SquareSum(diff);

            VectorUtilities::Multiply(diff, 4.0, grad_y_pred); // [ diff * 4 ]        --> grad_y_pred
            VectorUtilities::Multiply(grad_y_pred, x, tmp);        // [ grad_y_pred * x ] --> tmp

            const double grad_a = std::accumulate(tmp.cbegin(), tmp.cend(), 0.0);
            const double grad_b = std::accumulate(grad_y_pred.cbegin(), grad_y_pred.cend(), 0.0);

            a_pred -= learning_rate * grad_a;
            b_pred -= learning_rate * grad_b;
            if (0.005 > loss) {
                std::cout << "Iter counnt = " << i << std::endl;
                break;
            }
        }

        std::cout << a_pred << std::endl;
        std::cout << b_pred << std::endl;
    }
}

namespace Experiments::Parabola {

    struct Coefficient {
        double a {};
        double b {};
        double c {};
    };

    std::ostream& operator<<(std::ostream& stream, const Coefficient& coef) {
        stream << "A: " << std::setprecision(12) << coef.a << std::endl;
        stream << "B: " << std::setprecision(12) << coef.b << std::endl;
        stream << "C: " << std::setprecision(12) << coef.c;
        return stream;
    }

    void PredictDemo() {
        constexpr size_t pointsCount = 100;
        const Coefficient coef { getRandom(), getRandom(), getRandom()};
        auto equationParabola = [](double x, const Coefficient& coef) {
            return coef.a * (x * x) + coef.b * x + coef.c;
        };

        const std::vector<double> x = [pointsCount]() {
            std::vector<double> data(pointsCount);
            std::iota(data.begin(), data.end(), 1);
            const auto xMax = *std::max_element(data.cbegin(), data.cend());
            VectorUtilities::Devide(data, xMax);
            return data;
        }();

        const std::vector<double> y = [&]() {
            std::vector<double> data;
            for (const auto v: x)
                data.push_back(equationParabola(v, coef));
            return data;
        }();

        Coefficient coef_predicted {getRandom(),getRandom(),getRandom()};
        std::vector<double> diff(x.size()), grad_y_pred(x.size());
        std::vector<double> tmp(x.size()), tmp2(x.size());
        constexpr double learning_rate = 1e-5;
        constexpr size_t epochs { 900'000'000};

        std::cout << coef << std::endl;

        for (size_t i = 0; i < epochs; ++i)
        {
            for (size_t i = 0; i < pointsCount; ++i) {
                diff[i] = equationParabola(x[i], coef_predicted) - y[i];
            }

            // Compute loss: Mean Squared Error
            // The sum of the squares of the difference between the assumed value and the actual
            const double loss = VectorUtilities::SquareSum(diff);
            // if (0 == i % 1000)
            //    std::cout << loss << std::endl;

            VectorUtilities::Multiply(diff, 4.0, grad_y_pred); // [ diff * 4 ]        --> grad_y_pred

            // Gradient for 'C' coefficient: Sum of 'grad_y_pred'
            const double grad_c = std::accumulate(grad_y_pred.cbegin(), grad_y_pred.cend(), 0.0f);

            // [ grad_y_pred * x ] --> tmp
            VectorUtilities::Multiply(grad_y_pred, x, tmp);
            const double grad_b = std::accumulate(tmp.cbegin(), tmp.cend(), 0.0f);

            // [ grad_y_pred * x * x] --> [ tmp * x] --> tm2
            VectorUtilities::Multiply(tmp, x, tmp2);
            const double grad_a = std::accumulate(tmp2.cbegin(), tmp2.cend(), 0.0f);


            coef_predicted.a -= learning_rate * grad_a;
            coef_predicted.b -= learning_rate * grad_b;
            coef_predicted.c -= learning_rate * grad_c;
            if (0.001 > loss) {
                std::cout << "Iter count = " << i << std::endl;
                break;
            }
        }
        std::cout << coef_predicted << std::endl;
    }

    void PredictTest2() {
        constexpr double xStart = -10, xEnd = 10;
        constexpr size_t pointsCount = 100;

        const Coefficient coefficients { getRandom(), getRandom(), getRandom()};
        auto equationParabola = [](double x, const Coefficient& coef) {
            return coef.a * (x * x) + coef.b * x + coef.c;
        };

        std::vector<double> X_Orig, Y_Orig;
        const double step = (xEnd - xStart) / pointsCount;
        for (double i = xStart; i < xEnd; i += step)
            X_Orig.push_back(i);
        for (const auto x: X_Orig)
            Y_Orig.push_back(equationParabola(x, coefficients));

        Coefficient coef_predicted {getRandom(), getRandom(), getRandom()};
        std::vector<double> diff(X_Orig.size()), grad_y_pred(X_Orig.size());
        std::vector<double> tmp(X_Orig.size()), tmp2(X_Orig.size());
        constexpr double learning_rate = 1e-5;
        constexpr size_t epochs { 1000'000};

        double max {0};
        std::vector<double> X {X_Orig}, Y {Y_Orig};
        {
            const double xMax = *std::max_element(X.cbegin(), X.cend());
            const double yMax = *std::max_element(Y.cbegin(), Y.cend());
            max = std::max(xMax, yMax);
            std::for_each(X.begin(), X.end(), [max](auto &v) { v = v / max;} );
            std::for_each(Y.begin(), Y.end(), [max](auto &v) { v = v / max;} );
        }
        std::cout << max << std::endl;

        std::cout << coefficients << std::endl;
        for (size_t i = 0; i < epochs; ++i) {
            for (size_t i = 0; i < pointsCount; ++i)
                diff[i] = equationParabola(X[i], coef_predicted) - Y[i];

            // Compute loss: Mean Squared Error
            // The sum of the squares of the difference between the assumed value and the actual
            const double loss = VectorUtilities::SquareSum(diff);
            VectorUtilities::Multiply(diff, 4.0, grad_y_pred); // [ diff * 4 ]        --> grad_y_pred

            // Gradient for 'C' coefficient: Sum of 'grad_y_pred'
            const double grad_c = std::accumulate(grad_y_pred.cbegin(), grad_y_pred.cend(), 0.0);

            // [ grad_y_pred * x ] --> tmp
            VectorUtilities::Multiply(grad_y_pred, X, tmp);
            const double grad_b = std::accumulate(tmp.cbegin(), tmp.cend(), 0.0);

            // [ grad_y_pred * x * x] --> [ tmp * x] --> tm2
            VectorUtilities::Multiply(tmp, X, tmp2);
            const double grad_a = std::accumulate(tmp2.cbegin(), tmp2.cend(), 0.0);

            coef_predicted.a -= learning_rate * grad_a;
            coef_predicted.b -= learning_rate * grad_b;
            coef_predicted.c -= learning_rate * grad_c;
            if (0.001 > loss) {
                std::cout << "Iter count = " << i << std::endl;
                break;
            }
        }
        std::cout << coef_predicted << std::endl;

        std::ofstream outFile(R"(/home/andtokm/tmp/data.csv)", std::ios::trunc);
        outFile << "x,y,y_pred" << std::endl;
        for (size_t i = 0; i < pointsCount; i ++) {
            const double predictedY =  coef_predicted.a * X[i] * X[i] +
                    coef_predicted.b * X[i] + coef_predicted.c;
            outFile << X[i] * max << "," << Y[i]* max << "," << predictedY * max << std::endl;
            // std::cout << pt[0] << "," << pt[1]  << "," << predictedY << std::endl;
        }
    }

    void PredictTest3() {
        constexpr double xStart = -10, xEnd = 10;
        constexpr size_t pointsCount = 100;

        const Coefficient coefficients { getRandom(), getRandom(), getRandom()};
        auto equationParabola = [](double x, const Coefficient& coef) {
            return coef.a * (x * x) + coef.b * x + coef.c;
        };

        std::vector<double> X_Orig, Y_Orig;
        const double step = (xEnd - xStart) / pointsCount;
        for (double i = xStart; i < xEnd; i += step)
            X_Orig.push_back(i);
        for (const auto x: X_Orig)
            Y_Orig.push_back(equationParabola(x, coefficients));

        double max {0};
        std::vector<double> X {X_Orig}, Y {Y_Orig};
        {
            const double xMax = *std::max_element(X.cbegin(), X.cend());
            const double yMax = *std::max_element(Y.cbegin(), Y.cend());
            max = std::max(xMax, yMax);
            std::for_each(X.begin(), X.end(), [max](auto &v) { v = v / max;} );
            std::for_each(Y.begin(), Y.end(), [max](auto &v) { v = v / max;} );
        }

        std::cout << max << std::endl;
        std::cout << coefficients << std::endl;

        Coefficient coef_predicted {getRandom(), getRandom(), getRandom()};
        double yPredicted {0}, error { std::numeric_limits<double>::max() };
        constexpr double learningRate = 1e-6; // {0.01}
        constexpr size_t epochs { 100'000};
        size_t epochsCounter = 0;

        /** Training Phase: **/
        for (size_t n = 0; n < epochs; ++n, ++epochsCounter) {
            for (size_t i = 0; i < pointsCount; i ++) {
                const double err = equationParabola(X[i], coef_predicted) - Y[i];
                coef_predicted.c = coef_predicted.c - learningRate * err;
                coef_predicted.b = coef_predicted.b - learningRate * err * X[i];
                coef_predicted.a = coef_predicted.a - learningRate * err * X[i]* X[i];
                error = std::min(error, std::abs(err));
            }
            if (learningRate > error)
                break;
            if (0 == n % 1000)
                std::cout << error << std::endl;
        }


        std::cout << coef_predicted << std::endl;

        std::ofstream outFile(R"(/home/andtokm/tmp/data.csv)", std::ios::trunc);
        outFile << "x,y,y_pred" << std::endl;
        for (size_t i = 0; i < pointsCount; i ++) {
            const double predictedY =  coef_predicted.a * X[i] * X[i] +
                                       coef_predicted.b * X[i] + coef_predicted.c;
            outFile << X[i] * max << "," << Y[i]* max << "," << predictedY * max << std::endl;
            // std::cout << pt[0] << "," << pt[1]  << "," << predictedY << std::endl;
        }
    }
}

void Experiments::TestAll() {
    // Line::Predict();

    // Parabola::Predict();
    // Parabola::PredictTest2();
    Parabola::PredictTest3();
}