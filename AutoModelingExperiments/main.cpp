//============================================================================
// Name        : main.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : main
//============================================================================

#include <string_view>
#include <vector>

#include "ModelOrientation.h"
#include "Experiments.h"
#include "Preprocessing.h"
#include "DiagnosticsMethods.h"
#include "TreatmentPlan/TreatmentPlan.h"
#include "Features.h"
#include "Axis.h"
#include "ClassifyTeeth.h"
#include "Types.h"
#include "ContactPoints/ContactPoints.h"
#include "MachineLearningExperiments.h"

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    /** Get input params and store them to the vector: **/
    const std::vector<std::string_view> params(argv + 1, argv + argc);

    // Types::Tests();

    // TreatmentPlan_Tests::TestAll();
    // Features::TestAll(params);
    // Experiments::TestAll(params);
    // DiagnosticsMethods::TestAll(params);
    // Axis::TestAll(params);
    // ClassifyTeeth::TestAll(params);
    // ContactPoints::TestAll(params);
    // ModelOrientation::TestAll(params);
    // MachineLearningExperiments::TestAll(params);
    Preprocessing::TestAll();

    return 0;
}