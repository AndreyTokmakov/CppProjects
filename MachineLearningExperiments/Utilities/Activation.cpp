//============================================================================
// Name        : Activaton.h
// Created on  : 27.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Activaton functions
//============================================================================

#include "Activation.h"

#include <cmath>
#include <numbers>

namespace Activation
{
    // Sigmoid activation function
    // - The sigmoid activation function is also called the logistic function.
    // - It is the same function used in the logistic regression classification algorithm
    double Sigmoid(double z) {
        return 1 / (1 + std::pow(std::numbers::e, (-1 * z)));
    }
}
