//============================================================================
// Name        : Utilities.h
// Created on  : 10.10.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utilities
//============================================================================

#ifndef CPPEXPERIMENTS_UTILITIES_H
#define CPPEXPERIMENTS_UTILITIES_H

namespace VectorUtilities
{
    template<typename _Ty>
    double mean(const std::vector<_Ty>& data) noexcept {
        return (double)std::accumulate(data.cbegin(), data.cend(), 0) / data.size();
    }

    template<typename _Ty>
    _Ty MultAndSum(const std::vector<_Ty>& data1,
                   const std::vector<_Ty>& data2) noexcept {
        const size_t size { data1.size() };
        _Ty result = 0;
        for (size_t i = 0; i < size; ++i) {
            result += data1[i] * data2[i];
        }
        return result;
    }

    template<typename _Ty>
    _Ty SquareDifferences(const std::vector<_Ty>& data1,
                          const std::vector<_Ty>& data2) noexcept {
        const size_t size { data1.size() };
        _Ty result = 0;
        for (size_t i = 0; i < size; ++i) {
            result += (data1[i] - data2[i]) * (data1[i] - data2[i]);
        }
        return result;
    }
}

#endif //CPPEXPERIMENTS_UTILITIES_H
