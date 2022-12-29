//============================================================================
// Name        : LeftRectanglesMethod.cpp
// Created on  : 10.10.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : LeftRectanglesMethod
//============================================================================

#include <iostream>
#include <memory>
#include <math.h>
#include <algorithm>
#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <string>
#include <mutex>
#include <atomic>
#include <numeric>
#include <iomanip>
#include <functional>

#include "IntegralCalculations.h"

namespace IntegralCalculations::LeftRectanglesMethod {

}

namespace IntegralCalculations::LeftRectanglesMethod::Example1
{

    double rectangle_integral(std::function<double(double)> func,
                              const double start,
                              const double end,
                              const size_t numSteps) {
        const double step = (end - start) / numSteps;
        double sum { 0.0f }, xCoord {};

        for (size_t i = 0; i < numSteps; ++i) {
            // we calculate the argument of the function at this step which is the value
            // of the X coordinate at this step: START + ITER * STEP_SIZEs
            xCoord = start + i * step;

            // calculating the sum of the function values
            sum += func(xCoord);
        }
        return (sum * step);
    }

    double rectangle_integral2(std::function<double(double)> func,
                               const double start,
                               const double end,
                               const size_t numSteps) {
        const double step = (end - start) / numSteps;
        double result = { 0.0f };

        for (double value = start; (value + step) <= end; value += step)
            result += func(value) * step;
        return result;
    }

    void Linear_Function_Test() {
        // Simple linear function:
        std::function<double(double)> func = [](double x) { return x; };

        double value = 10;
        double numSteps = 10000;

        auto expected = func(value) * value / 2;
        auto actual1 = rectangle_integral(func, 0, value, numSteps);
        auto actual2 = rectangle_integral2(func, 0, value, numSteps);

        std::cout << "Actual1  = " << actual1 << std::endl;
        std::cout << "Actual2  = " << actual2 << std::endl;
        std::cout << "Expected = " << expected << std::endl;
    }

}


void IntegralCalculations::LeftRectanglesMethodTests() {
    LeftRectanglesMethod::Example1::Linear_Function_Test();

}
