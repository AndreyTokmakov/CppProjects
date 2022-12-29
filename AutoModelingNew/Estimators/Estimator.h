//============================================================================
// Name        : Estimator.h
// Created on  : 21.12.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Estimator
//============================================================================

#ifndef CPPPROJECTS_ESTIMATOR_H
#define CPPPROJECTS_ESTIMATOR_H

#include "../Model/Tooth.h"

namespace Estimators
{

    class Estimator {
    public:
        virtual void estimate(Model::Tooth& tooth) noexcept = 0;
        virtual ~Estimator() = default;
    };


    class DistanceEstimator final : public  Estimator{
    public:
        virtual void estimate(Model::Tooth& tooth) noexcept override;
    };
};

#endif //CPPPROJECTS_ESTIMATOR_H