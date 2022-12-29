//============================================================================
// Name        : GeometricFigures.cpp
// Created on  : 27.12.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : GeometricFigures
//============================================================================

#include "../VtkHeaders.h"
#include "../Utilities/Utilities.h"
#include "GeometricFigures.h"

#include <iostream>
#include <string_view>

namespace GeometricFigures
{
    const vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors>::New() };

    void DrawLine()
    {
        Eigen::Vector3d pt1 {0, 0, 0}, pt2 {10, 10, 10};
        const vtkSmartPointer<vtkActor> lineActor = Utilities::getLineActor(pt1, pt2);
        Utilities::DisplayActors({ lineActor });
    }

    [[nodiscard]]
    double equationEllipse(double x, double a, double b)  {
        if (x == a || 0 == a || 0 == b)
            return 0;
        return std::sqrt((b * b) * (1 - (x * x) / (a * a)));
    }

    [[nodiscard]]
    double twoPointDistance(const Eigen::Vector3d& pt1,
                            const Eigen::Vector3d& pt2) noexcept {
        double sqrDist{ 0 };
        for (size_t i { 0 }; i < 3; ++i)
            sqrDist += std::pow(pt1[i] - pt2[i], 2);
        return std::sqrt(sqrDist);
    }


    [[nodiscard]]
    Eigen::Vector3d findClosetPoint(const Eigen::Vector3d& pt,
                                    const std::function<double(double)>& func) noexcept {
        double step = 0.01;

        // NOTE: Gradient like: Just to define the step direction
        {
            Eigen::Vector3d pt1 { pt.x() - step, 0, 0 }, pt2 { pt.x() + step, 0, 0 };
            pt1.y() = func(pt1.x());
            pt2.y() = func(pt2.x());
            const double d1 = twoPointDistance(pt, pt1);
            const double d2 = twoPointDistance(pt, pt2);
            if (d1 < d2)
                step *= -1;
        }

        double distPrev { std::numeric_limits<double>::max() };
        Eigen::Vector3d ptCloset { pt };
        while (true) {
            ptCloset.y() = func(ptCloset.x());
            const double distCurrent = twoPointDistance(pt, ptCloset);
            if (distCurrent > distPrev)
                break;
            distPrev = std::min(distPrev, distCurrent);
            ptCloset.x() += step;
        }
        return ptCloset;
    }


    void DrawEllipse()
    {
        std::vector<Eigen::Vector3d> points;
        const double A = 26.8737, B= 37.6536;

        const double stepsCount = 100;
        const double step = (2 * A) / stepsCount;
        double xBegin = -A, xEnd = A;

        for (double x = xBegin; x <= xEnd; x += step) {
            const double y = equationEllipse(x, A, B);
            std::cout << "[" << x << ", " << y << "]\n";
            points.emplace_back(x, y, 0);
        }

        const vtkSmartPointer<vtkActor> lineActor = Utilities::getPointsActor(points, {1, 0, 0}, 7);
        Utilities::DisplayActors({ lineActor });
    }

    void DistFromPoint_To_Ellipse()
    {
        const Eigen::Vector3d pt { 8, 32, 0 };
        std::vector<Eigen::Vector3d> points;
        const double A = 26.8737, B= 37.6536;

        const double stepsCount = 10000;
        const double step = (2 * A) / stepsCount;
        double xBegin = -A, xEnd = A;

        auto ellipse = [A, B](double x) {
            return equationEllipse(x, A, B);
        };

        for (double x = xBegin; x <= xEnd; x += step) {
            const double y = ellipse(x);
            // std::cout << "[" << x << ", " << y << "]\n";
            points.emplace_back(x, y, 0);
        }

        [[maybe_unused]]
        auto ptCloset = findClosetPoint(pt, ellipse);

        const vtkSmartPointer<vtkActor> ellipseActor = Utilities::getPointsActor(points, {1, 0, 0}, 7);
        const vtkSmartPointer<vtkActor> ptActor = Utilities::getPointsActor({pt, ptCloset}, {0, 1, 0});
        Utilities::DisplayActors({ ellipseActor, ptActor });
    }
}

void GeometricFigures::TestAll() {
    // DrawLine();

    // DrawEllipse();
    DistFromPoint_To_Ellipse();


}