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

#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

namespace VectorUtilities
{
    template<typename _Ty>
    double mean(const std::vector<_Ty>& data) noexcept {
        return std::accumulate(data.cbegin(), data.cend(), 0.0f) / data.size();
    }

    template<typename _Ty>
    _Ty MultAndSum(const std::vector<_Ty>& data1,
                   const std::vector<_Ty>& data2) noexcept {
        // TODO: Assert sizes
        return std::inner_product(data1.begin(), data1.end(), data2.begin(), 0.0f);
    }

    // TODO: Rename to MultAndSum ?? or keep  'dot'
    template<typename _Ty>
    _Ty dot(const std::vector<_Ty>& data1,
                   const std::vector<_Ty>& data2) noexcept {
        // TODO: Assert sizes
        return std::inner_product(data1.begin(), data1.end(), data2.begin(), 0.0f);
    }

    template<typename T>
    std::vector<T> operator*(const std::vector<T>& vect, T value) {
        const typename std::vector<T>::size_type size { vect.size() };
        std::vector<T> result;
        result.reserve(size);
        for (size_t i = 0; i < size; ++i)
            result[i] *= value;
        return result;
    }

    template<typename T>
    std::vector<T> operator/(const std::vector<T>& vect, T value) {
        const typename std::vector<T>::size_type size { vect.size() };
        std::vector<T> result;
        result.reserve(size);
        for (size_t i = 0; i < size; ++i)
            result[i] /= value;
        return result;
    }


    //-------------------------------------------- Refactor ---------------------------------


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

namespace Utilities {
    /** Test utils **/
    void TestAll();
}

#endif //CPPEXPERIMENTS_UTILITIES_H
