
#include "Utilities/Utilities.h"
#include "Types/Types.h"
#include "LinearRegression/LinearRegression.h"
#include "Experiments/Experiments.h"
#include "Eigen/EigenLinearRegression.h"
#include "Eigen/PolynomialFitting.h"
#include "LogisticRegression/LogisticRegression.h"

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    // LinearRegression::TestAll();
    // LogisticRegression::TestAll();

    // CodingAIExamples::Tests();
    // Experiments::TestAll();

    // EigenLinearRegression::TestAll();
    PolynomialFitting::TestAll();

    // Utilities::TestAll();
}