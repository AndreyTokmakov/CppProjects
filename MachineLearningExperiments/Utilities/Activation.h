//============================================================================
// Name        : Activation.h
// Created on  : 27.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Activation functions
//============================================================================

#ifndef CPPCLIONPROJECTS_ACTIVATION_H
#define CPPCLIONPROJECTS_ACTIVATION_H

namespace Activation
{
    // Sigmoid activation function
    // - The sigmoid activation function is also called the logistic function.
    // - It is the same function used in the logistic regression classification algorithm
    double Sigmoid(double z);
}

#endif //CPPCLIONPROJECTS_ACTIVATION_H
