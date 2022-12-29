//============================================================================
// Name        : Utils.h
// Created on  : 13.01.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utils
//============================================================================

#ifndef CPPPROJECTS_UTILS_H
#define CPPPROJECTS_UTILS_H

#include <iostream>
#include <memory>
#include <string>

#include <thread>
#include <future>

#include <mutex>
#include <atomic>

#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>

#include <vector>
#include <list>
#include <algorithm>
#include <array>
#include <set>
#include <span>
#include <concepts>

#include "../VtkHeaders.h"
#include "../Model/Common.h"
#include "../Model/FDI.h"
#include "../Model/Jaw.h"
#include "../Model/Tooth.h"
#include "../Model/ToothAxis.h"
#include "../Utilities/Utilities.h"
#include "../TreatmentPlan/TreatmentPlan.h"
#include "../Estimators/Estimator.h"

namespace Utils
{
    std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>>
    ReadTeethObj(std::string_view path) noexcept;


    struct [[nodiscard]] Coefficients {
        double slope {};
        double intercept {};
    };

    template<typename Ty>
    class LinearRegression {
    public:
        Coefficients estimate(const std::vector<Ty>& x,
                              const std::vector<Ty>& y) const noexcept {
            {
                const size_t size { x.size() };
                const double meanX { mean(x) };
                const double meanY { mean(y) };

                const double crossDeviationXX = multiplyAndSum(x, x) - size * meanX * meanX;
                const double crossDeviationXY = multiplyAndSum(x, y) - size * meanX * meanY;

                const double a = crossDeviationXY / crossDeviationXX;
                const double b = meanY - a * meanX;
                return { a, b };
            }
        }

    private:
        [[nodiscard]]
        Ty mean(const std::vector<Ty>& data) const noexcept  {
            return std::accumulate(data.cbegin(), data.cend(), 0.0f) / data.size();
        }

        [[nodiscard]]
        Ty multiplyAndSum(const std::vector<Ty>& data1,
                          const std::vector<Ty>& data2) const noexcept {
            // TODO: Assert sizes
            return std::inner_product(data1.begin(), data1.end(), data2.begin(), 0.0);
        }

    };

    template<typename T>
    struct UnorderedPair: std::pair<T, T> {
        UnorderedPair(T a, T b): std::pair<T, T> {a, b} {
            ///
        }

        template<typename _T>
        friend std::ostream& operator<<(std::ostream& stream, const UnorderedPair<_T>& pair);

        template<typename _T>
        friend bool operator==(const UnorderedPair<_T>& p1, const UnorderedPair<_T>& p2);
    };

    template<typename T>
    struct PairHashUnordered {
        std::size_t operator()(const UnorderedPair<T>& pair) const noexcept {
            std::size_t h1 = std::hash<T>{}(std::min(pair.first, pair.second));
            std::size_t h2 = std::hash<T>{}(std::max(pair.first, pair.second));
            return h1 ^ (h2 << 1);
        }
    };

    template<typename _T>
    std::ostream& operator<<(std::ostream& stream, const UnorderedPair<_T>& pair) {
        stream << "(" << pair.first << ", " << pair.second << ")";
        return stream;
    }

    template<typename _T>
    bool operator==(const UnorderedPair<_T>& p1, const UnorderedPair<_T>& p2) {
        return (p1.first == p2.first && p1.second == p2.second) ||
               (p1.second == p2.first && p1.first == p2.second);
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vect) {
        std::cout << "[" << std::setprecision(12);
        for (const T& entry : vect)
            stream << entry << ' ';
        std::cout << "]";
        return stream;
    }

    template<typename _Ty, size_t _Size>
    std::ostream& operator<<(std::ostream& stream, const std::array<_Ty, _Size>& data) {
        std::cout << "[" << std::setprecision(12);
        for (const _Ty& entry : data)
            stream << entry << ' ';
        std::cout << "]";
        return stream;
    }

    void displayTeethAndAxis(const std::unordered_map<unsigned short, Model::Tooth>& teeth,
                             bool lower = true,
                             bool upper = true);

    // TODO: Move to Utilities ????
    [[nodiscard]]
    double twoPointDistance(const Eigen::Vector3d& pt1,
                            const Eigen::Vector3d& pt2) noexcept;

    // TODO: Move to Utilities ????
    [[nodiscard]]
    Eigen::Vector3d middlePoint(const Eigen::Vector3d& pt1,
                                const Eigen::Vector3d& pt2) noexcept;

    [[nodiscard]]
    std::pair<double, double> getLineCoefficients(const Eigen::Vector2d& pt1,
                                                  const Eigen::Vector2d& pt2) noexcept;

    // TODO: Move to Utilities ?????
    [[nodiscard("Make sure to handle return value")]]
    double getAngle(const Eigen::Vector3d& vect1,
                    const Eigen::Vector3d& vect2) noexcept;

    // TODO: Move to Utilities ?????
    [[nodiscard]]
    constexpr double degToRad(const double angle) noexcept {
        return angle * std::numbers::pi / 180;
    };


    std::pair<double, double> orientTeethToPlane(std::unordered_map<unsigned short, Model::Tooth>& teethMap);

};

#endif //CPPPROJECTS_UTILS_H
