//============================================================================
// Name        : MLPack.h
// Created on  : 19.10.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Math src
//============================================================================

#include <iostream>
#include <string>
#include <string_view>

#include "MLPack.h"

#if 0
#include <mlpack/core.hpp>
#include <mlpack/methods/random_forest/random_forest.hpp>
#include <mlpack/methods/decision_tree/random_dimension_select.hpp>
#include <mlpack/core/cv/k_fold_cv.hpp>
#include <mlpack/core/cv/metrics/accuracy.hpp>
#include <mlpack/core/cv/metrics/precision.hpp>
#include <mlpack/core/cv/metrics/recall.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>

#include "../Utilities/Utilities.h"
#include "../Types/Types.h"

using namespace arma;
using namespace mlpack;
using namespace mlpack::tree;
using namespace mlpack::cv;
using namespace mlpack::regression;

namespace {
    constexpr std::string_view MLPACK_TEST_DATA_DIR {
        R"(/home/andtokm/Projects/mlpack/tests/mlpack/src/data/)"
    };

    constexpr std::string_view PROJECT_DATA_DIR {
            R"(/home/andtokm/Projects/CppProjects/MachineLearningExperiments/data/)"
    };
}

namespace MLPack {

    void Example_N1()
    {
        mat dataset;
        const std::string dataFile { std::string(MLPACK_TEST_DATA_DIR) + std::string("german.csv")};
        if (bool loaded = mlpack::data::Load(dataFile, dataset); false == loaded)
            return;

        const Row<size_t> labels = conv_to<Row<size_t>>::from(dataset.row(dataset.n_rows - 1));
        dataset.shed_row(dataset.n_rows - 1);

        constexpr size_t numClasses = 2;
        constexpr size_t minimumLeafSize = 5;
        constexpr size_t numTrees = 10;
        const RandomForest<GiniGain, RandomDimensionSelect> randomForest =
                RandomForest<GiniGain, RandomDimensionSelect>(dataset, labels, numClasses, numTrees, minimumLeafSize);

        Row<size_t> predictions;
        randomForest.Classify(dataset, predictions);
        const size_t correct = arma::accu(predictions == labels);
        std::cout << "Training Accuracy: " << (double(correct) / double(labels.n_elem)) << std::endl;


        constexpr size_t k = 10;
        KFoldCV<RandomForest<GiniGain, RandomDimensionSelect>, Accuracy> crossValidation(k, dataset, labels, numClasses);
        const double cvAccuracy = crossValidation.Evaluate(numTrees, minimumLeafSize);
        std::cout << "KFoldCV Accuracy: " << cvAccuracy << std::endl;

        const double cvPrecision = Precision<Binary>::Evaluate(randomForest, dataset, labels);
        const double cvRecall = Recall<Binary>::Evaluate(randomForest, dataset, labels);

        std::cout << "Precision: " << cvPrecision << std::endl;
        std::cout << "Recall: " << cvRecall << std::endl;


        const std::string fileToSave { std::string(PROJECT_DATA_DIR) + std::string("mymodel.xml")};
        mlpack::data::Save(fileToSave, "model", randomForest, false);
    }
}

namespace MLPack::LinearRegression {
    void SimpleTest0()
    {
        const std::vector<Types::Point<2, double>> points {
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

        arma::mat predictors(1, points.size());
        arma::mat pointsToPredict(1, points.size());
        arma::rowvec responses(points.size());

        for (size_t i = 0; const Types::Point<2, double>& pt: points) {
            predictors.row(0)[i] = pt[0];
            pointsToPredict.row(0)[i] = pt[0]; // X same as in training data
            responses[i] = pt[1];
            i++;
        }

        // The values we get back when we predict for points.
        arma::rowvec predictions(points.size());

        regression::LinearRegression linearRegression(predictors, responses);
        linearRegression.Predict(pointsToPredict, predictions);

        for (size_t i = 0; i < predictions.n_cols; ++i)
            std::cout << "Actual: " << responses(i) << "  Predicted: " <<  predictions(i) << std::endl;
    }

    void SimpleTest()
    {
        // Predictors  10x3 matrices.
        arma::mat predictors(3, 10);
        // Responses is the "correct" value for each point in predictors and points.
        arma::rowvec responses(10);
        // The values we get back when we predict for points.
        arma::rowvec predictions(10);

        // We'll randomly select some coefficients for the linear response.
        arma::vec coeffs;
        coeffs.randu(4);

        // Now generate each point.
        for (size_t row = 0; row < 3; row++)
            predictors.row(row) = arma::linspace<arma::rowvec>(0, 9, 10);

        arma::mat points(3, 10);
        points = predictors;

        // Now add a small amount of noise to each point.
        for (size_t elem = 0; elem < points.n_elem; elem++) {
            // Max added noise is 0.02.
            points[elem] += mlpack::math::Random() / 50.0;
            predictors[elem] += mlpack::math::Random() / 50.0;
        }

        // Generate responses.
        for (size_t elem = 0; elem < responses.n_elem; elem++)
            responses[elem] = coeffs[0] + dot(coeffs.rows(1, 3), arma::ones<arma::rowvec>(3) * elem);

        std::cout << "coeffs: " << coeffs  << std::endl;
        std::cout << "points: " << points  << std::endl;
        std::cout << "predictors: " << points  << std::endl;
        std::cout << "responses : " << responses  << std::endl;

        // Initialize and predict.
        regression::LinearRegression linearRegression(predictors, responses);
        linearRegression.Predict(points, predictions);

        // Output result
        for (size_t i = 0; i < predictions.n_cols; ++i)
            std::cout << "Actual: " << responses(i) << "  Predicted: " <<  predictions(i) << std::endl;
    }
}
#endif

void MLPack::TestAll()
{
    // MLPack::Example_N1();
    // LinearRegression::SimpleTest0();
    // LinearRegression::SimpleTest();
}