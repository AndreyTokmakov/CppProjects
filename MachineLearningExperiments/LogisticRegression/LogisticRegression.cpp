//============================================================================
// Name        : LogisticRegression.cpp
// Created on  : 26.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : LogisticRegression C++ implementation
//============================================================================


#include "../CSVReader/Csv.h"
#include "../Utilities/Activation.h"
#include "LogisticRegression.h"

#include <iostream>
#include <vector>
#include <array>
#include <math.h>
#include <fstream>
#include <string>

// TODO: https://github.com/anirudhdggl/logistic-regression-cpp
namespace LogisticRegression_ONE
{
    constexpr std::string_view dataSetFile {
            R"(S:\Projects\cppClion\CppCLionProjects\MachineLearningExperiments\data\Iris.csv)"};

    std::vector<double> expectedOutput;
    std::vector<std::vector<double>> dataSet;

    // Some random weights:
    std::array<double, 4> weight {0.2, 0.2, 0.2, 0.2};
    constexpr double learningRate = 0.001;


    void updateWeight(double predictedValue,
                      double expectedOutput,
                      const std::vector<double>& inputValue) {
        for (size_t i = 0; i < inputValue.size(); i++) {
            const double gradientDescent = (predictedValue - expectedOutput) * inputValue[i];
            weight[i] = weight[i] - (learningRate * gradientDescent);
        }
    }

    void calculateAccuracy() {
        long totalCorrect = 0;
        for (size_t i = 0; i < dataSet.size(); i++) {
            double z = 0;
            for (size_t j = 0; j < dataSet[0].size(); j++)
                z += dataSet[i][j] * weight[j];

            const double predictedValue = Activation::Sigmoid(z) < 0.5 ? 0 : 1;
            if (predictedValue == expectedOutput[i])
                totalCorrect++;
        }
        std::cout << "Accuracy is: " << (totalCorrect * 100) / dataSet.size() << "%" << std::endl;
    }

    void runModel()
    {
        Csv::CSVReader reader;
        reader.readFile(dataSetFile);
        for (const auto& row: reader.dataSet) {
            auto& input = dataSet.emplace_back();
            for (size_t i = 1; i <= 4; i++)
                input.push_back(std::atof(row[i].c_str()));
            expectedOutput.push_back(stod(row[5]));
        }

        long epoch = 10;
        while (epoch--)
        {
            calculateAccuracy();

            for (size_t i = 0; i < dataSet.size(); i++) {
                double z = 0;
                for (size_t j = 0; j < dataSet[0].size(); j++)
                    z += dataSet[i][j] * weight[j];

                const double predictedValue = Activation::Sigmoid(z);
                updateWeight(predictedValue, expectedOutput[i], dataSet[i]);
            }
        }
        calculateAccuracy();
    }

    void RunTest()
    {
        runModel();

        // std::array<double, 4> data {4.8, 3.0, 1.4, 0.3};
        std::array<double, 4> data1 {7.0,3.2,	4.7,	1.4};

        double z = 0;
        for (size_t i = 0; i < 4; i++) {
            z += weight[i] * data1[i];
        }

        const double predictedValue = Activation::Sigmoid(z);
        std::cout << (predictedValue < 0.5 ? "0" : "1") << std::endl;
    }
}

namespace LogisticRegression_TWO
{
    constexpr std::string_view dataSetFile {
            R"(/home/andtokm/Projects/CppProjects/MachineLearningExperiments/data/Iris.csv)"};

    std::array<double, 4> runModel()
    {
        std::vector<double> expectedOutput;
        std::vector<std::vector<double>> dataSet;

        Csv::CSVReader reader;
        reader.readFile(dataSetFile);
        for (const auto& row: reader.dataSet) {
            auto& input = dataSet.emplace_back();
            for (size_t i = 1; i <= 4; i++)
                input.push_back(std::atof(row[i].c_str()));
            expectedOutput.push_back(stod(row[5]));
        }

        // Some random weights:
        std::array<double, 4> weights {0.2, 0.2, 0.2, 0.2};
        constexpr double learningRate = 0.001;
        long epoch = 10;


        while (epoch--) {
            size_t totalCorrect = 0;
            for (size_t i = 0; i < dataSet.size(); i++) {
                double zSum = 0;
                for (size_t j = 0; j < dataSet[0].size(); j++)
                    zSum += dataSet[i][j] * weights[j];

                const double prediction = Activation::Sigmoid(zSum);

                {    /** Update weights block **/
                    const std::vector<double>& inputValue = dataSet[i];
                    const double output = expectedOutput[i];

                    for (size_t n = 0; n < inputValue.size(); n++) {
                        const double gradientDescent = (prediction - output) * inputValue[n];
                        weights[n] = weights[n] - (learningRate * gradientDescent);
                    }
                }

                // TODO: Just to calc accuracy
                if ((prediction < 0.5 ? 0 : 1) == expectedOutput[i])
                    totalCorrect++;
            }
            std::cout << "Accuracy is: " << (totalCorrect * 100) / dataSet.size() << "%" << std::endl;
        }

        return weights;
    }

    void RunTest()
    {
        const std::array<double, 4> weights = runModel();

        // std::array<double, 4> data1 {4.8, 3.0, 1.4, 0.3};
        std::array<double, 4> data1 {7.0,3.2,	4.7,	1.4};

        double z = 0;
        for (size_t i = 0; i < 4; i++) {
            z += weights[i] * data1[i];
        }

        const double predictedValue = Activation::Sigmoid(z);
        std::cout << (predictedValue < 0.5 ? "0" : "1") << std::endl;
    }
}

void LogisticRegression::TestAll()
{
    // LogisticRegression_ONE::RunTest();
    LogisticRegression_TWO::RunTest();

}
