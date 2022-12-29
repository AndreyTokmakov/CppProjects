//============================================================================
// Name        : MachineLearningExperiments.h
// Created on  : 11.10.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Machine Learning experiments
//============================================================================

#ifndef CPPPROJECTS_MACHINELEARNINGEXPERIMENTS_H
#define CPPPROJECTS_MACHINELEARNINGEXPERIMENTS_H

#include <string_view>
#include <vector>
#include <numeric>

namespace MachineLearningExperiments::VectorUtilities
{
    template<typename _Ty>
    double mean(const std::vector<_Ty>& data) noexcept {
        return std::accumulate(data.cbegin(), data.cend(), 0.0f) / data.size();
    }

    template<typename _Ty>
    void Multiply(const std::vector<_Ty>& in,
                  _Ty val,
                  std::vector<_Ty>& out) noexcept {
        out.clear();
        for (const auto& v: in)
            out.push_back(val * v);
    }

    template<typename _Ty>
    void Multiply(const std::vector<_Ty>& a,
                  const std::vector<_Ty>& b,
                  std::vector<_Ty>& out) noexcept {
        out.clear();
        for (size_t i = 0; i < a.size(); ++i)
            out.push_back(a[i] * b[i]);
    }

    template<typename _Ty>
    void Devide(std::vector<_Ty>& data,
                const _Ty val) noexcept {
        for (_Ty& v: data)
            v = v / val;
    }

    template<typename _Ty>
    _Ty MultAndSum(const std::vector<_Ty>& data1,
                   const std::vector<_Ty>& data2) noexcept {
        const size_t size { data1.size() };
        _Ty result = 0;
        for (size_t i = 0; i < size; ++i)
            result += data1[i] * data2[i];
        return result;
    }

    template<typename _Ty>
    _Ty SquareSum(const std::vector<_Ty>& data) noexcept {
        _Ty result = 0;
        for (auto& v: data)
            result += v * v;
        return result;
    }

    template<typename _Ty>
    _Ty SquareDifferences(const std::vector<_Ty>& data1,
                          const std::vector<_Ty>& data2) noexcept {
        const size_t size { data1.size() };
        _Ty result = 0;
        for (size_t i = 0; i < size; ++i)
            result += (data1[i] - data2[i]) * (data1[i] - data2[i]);
        return result;
    }
}

namespace MachineLearningExperiments
{
    using namespace Types;
    using TreatmentPlan::Point3d;
    using TreatmentPlan::ToothFrame;

    void TestAll([[maybe_unused]] const std::vector<std::string_view>& params);
}

#endif //CPPPROJECTS_MACHINELEARNINGEXPERIMENTS_H