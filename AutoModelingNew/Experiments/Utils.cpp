//============================================================================
// Name        : Utils.h
// Created on  : 13.01.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utils
//============================================================================

#include "Utils.h"

#include <iostream>
#include <string>
#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>
#include <vector>
#include <algorithm>

#include "../Utilities/Utilities.h"

namespace Utils {

    // TODO: refactor
    Eigen::Vector3d toPoint(std::string_view strPoints) {
        const size_t pos1 = strPoints.find(" ");
        if (std::string::npos == pos1)
            return {};

        const size_t pos2 = strPoints.find(" ", pos1 + 1);
        if (std::string::npos == pos2)
            return {};

        Eigen::Vector3d pt { Eigen::Vector3d::Zero () };
        pt[0] = atof(strPoints.substr(0, pos1).data());
        pt[1] = atof(strPoints.substr(pos1 + 1, strPoints.size() - pos1 - 1).data());
        pt[2] = atof(strPoints.substr(pos2 + 1, strPoints.size() - pos2 - 1).data());
        return pt;
    }

    // TODO: refactor
    std::array<int, 3> toCell(std::string_view strPoints) {
        const size_t pos1 = strPoints.find(" ");
        if (std::string::npos == pos1)
            return {};

        const size_t pos2 = strPoints.find(" ", pos1 + 1);
        if (std::string::npos == pos2)
            return {};

        std::array<int, 3> pt {};
        pt[0] = atoi(strPoints.substr(0, pos1).data());
        pt[1] = atoi(strPoints.substr(pos1 + 1, strPoints.size() - pos1 - 1).data());
        pt[2] = atoi(strPoints.substr(pos2 + 1, strPoints.size() - pos2 - 1).data());
        return pt;
    }

    // TODO: Refactor
    // TODO: Move to Utilities ?????
    // Read .obj file prepared by preprocessing.py module
    std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>>
    ReadTeethObj(std::string_view path) noexcept
    {
        std::ifstream input(path.data());
        std::vector<std::string> lines;
        while(std::getline(input, lines.emplace_back())) { /** Read all file lines. **/ }
        input.close();

        const size_t size {lines.size()};
        constexpr size_t APPROX_DATA_BLOCK_SIZE = 32 * 2;

        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> dataMap;
        std::vector<Eigen::Vector3d> points;
        points.reserve(APPROX_DATA_BLOCK_SIZE);

        for (size_t i {0}; i < size && 'g' == lines[i++][0]; /* skip. */) {
            /** For each tooth: **/
            while (i < size && 'o' == lines[i][0]) {
                const unsigned short toothId = static_cast<unsigned short>(atoi(std::string_view(lines[i]).substr(3, lines[i].size() - 3).data()));
                dataMap.emplace(toothId, vtkPolyData::New());
                ++i;

                for (/* use outer 'i' */; i < size && 'v' == lines[i][0]; ++i) {
                    const std::string_view pointsStr = std::string_view(lines[i]).substr(2, lines[i].size() - 2);
                    points.push_back(toPoint(pointsStr));
                }

                // TODO: Check for MEM LEAK
                vtkSmartPointer<vtkPoints> vtkPoints = vtkPoints::New();
                for (const Eigen::Vector3d& pt: points)
                    vtkPoints->InsertNextPoint(pt[0], pt[1], pt[2]);

                const int pointsCount { static_cast<int>(points.size())};
                vtkSmartPointer<vtkCellArray> vtkCells { vtkSmartPointer<vtkCellArray>::New() };
                for (/* use outer 'i' */; i < size && 'f' == lines[i][0]; ++i) {
                    const std::string_view cellStr = std::string_view(lines[i]).substr(2, lines[i].size() - 2);
                    const auto& [x,y,z] = toCell(cellStr);
                    vtkCells->InsertNextCell({pointsCount + x, pointsCount+ y, pointsCount + z});
                    // vtkCells->InsertNextCell(3, toCellEx(cellStr, pointsCount));
                }

                dataMap[toothId]->SetPoints(vtkPoints);
                dataMap[toothId]->SetPolys(vtkCells);
                points.clear();
            }
        }
        return dataMap;
    }

    //-----------------------------------------------------------------------------------

    void displayTeethAndAxis(const std::unordered_map<unsigned short, Model::Tooth>& teeth,
                             bool lower, bool upper) {
        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (const auto& [toothId, tooth]: teeth)
        {
            if ((lower == false && FDI::isLowerTooth(toothId)) || (upper == false && FDI::isUpperTooth(toothId)))
                continue;

            const Eigen::Vector3d center { tooth.GetCenter()};
            const Model::ToothAxis &axes = tooth.axes;

            actors.push_back(Utilities::getLineActor(center - axes.vertical * 5 ,
                                                     center + axes.vertical * 8, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(center - axes.angulation * 6 ,
                                                     center + axes.angulation * 6, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(center - axes.horizontal * 6 ,
                                                     center + axes.horizontal * 6, {1, 0, 0}));
            actors.push_back(Utilities::getPolyDataActor(tooth.toothPolyData));
        }
        Utilities::DisplayActors(actors);
    }

    // TODO: Move to Utilities ????
    [[nodiscard]]
    double twoPointDistance(const Eigen::Vector3d& pt1,
                            const Eigen::Vector3d& pt2) noexcept {
        double sqrDist{ 0 };
        for (size_t i { 0 }; i < 3; ++i)
            sqrDist += std::pow(pt1[i] - pt2[i], 2);
        return std::sqrt(sqrDist);
    }

    // TODO: Move to Utilities ????
    [[nodiscard]]
    Eigen::Vector3d middlePoint(const Eigen::Vector3d& pt1,
                                const Eigen::Vector3d& pt2) noexcept {
        return Eigen::Vector3d {std::midpoint(pt1.x(), pt2.x()),
                                std::midpoint(pt1.y(), pt2.y()),
                                std::midpoint(pt1.z(), pt2.z())};
    }

    [[nodiscard]]
    std::pair<double, double> getLineCoefficients(const Eigen::Vector2d& pt1,
                                                  const Eigen::Vector2d& pt2) noexcept {
        double slope = (pt2[1] - pt1[1]) / (pt2[0] - pt1[0]);
        double intercept = (pt2[0] * pt1[1] - pt1[0] * pt2[1]) / (pt2[0] - pt1[0]);
        return std::make_pair(slope, intercept);
    };

    // TODO: Move to Utilities ?????
    [[nodiscard("Make sure to handle return value")]]
    double getAngle(const Eigen::Vector3d& vect1,
                    const Eigen::Vector3d& vect2) noexcept {
        const double a { vect1.dot(vect2) };
        const double b { std::sqrt(vect1.dot(vect1)) * std::sqrt(vect2.dot(vect2)) };
        return (acos(a / b) * 180) / std::numbers::pi;
    }


    //===============================================================================================

    std::pair<double, double>  orientTeethToPlane(std::unordered_map<unsigned short, Model::Tooth>& teethMap) {
        // INFO: Teeth edge points and centroids:
        std::vector<Eigen::Vector3d> points;
        points.reserve(teethMap.size());

        for (const auto& [toothId, tooth]: teethMap) {
            const Eigen::Vector3d& center { tooth.GetCenter() };
            const std::array<double, 6> bounds { tooth.GetBounds() };
            const double height {bounds[3] - bounds[2]};

            Eigen::Vector3d& edgePoint = points.emplace_back(center.x(), center.y(), center.z());
            edgePoint.y() += FDI::isLowerTooth(toothId) ? height / 2 : - height / 2;
        }
        std::sort(points.begin(), points.end(), [](const auto& pt1, const auto& pt2) {
            return pt2.x() > pt1.x();
        });

        // TODO: Refactor this block ? (next 25 lines)
        std::vector<double> X, Y, Z;
        X.reserve(points.size());
        Y.reserve(points.size());
        Z.reserve(points.size());
        for (const auto &pt: points) {
            X.push_back(pt.x());
            Y.push_back(pt.y());
            Z.push_back(pt.z());
        }

        const Utils::LinearRegression<double> linReg;
        const Utils::Coefficients predictedX = linReg.estimate(Z, Y);
        const Utils::Coefficients predictedZ = linReg.estimate(X, Y);

        // TODO: Refactor ??? To much variables and function calls IMHO
        const double xAngle = std::atan(predictedX.slope) * 180 / std::numbers::pi; // TODO: to function?
        const double zAngle = std::atan(predictedZ.slope) * 180 / std::numbers::pi;
        const double sinX = std::sin(Utils::degToRad(xAngle)), cosX = std::cos(Utils::degToRad(xAngle));
        const double sinZ = std::sin(Utils::degToRad(-zAngle)), cosZ = std::cos(Utils::degToRad(-zAngle));
        // std::cout << "xAngle = " << xAngle << ", zAngle = " << zAngle << std::endl;

        /** Prepare the rotation X * Z matrix: **/
        const Eigen::Matrix3d matrix {
                {1 * cosZ, -sinZ, 0},
                {cosX * sinZ, cosX * cosZ, -sinX},
                {sinX * sinZ, sinX * cosZ, cosX}
        };

        // INFO: Transform teeth data and axes
        // TODO: Move tooth axes rotation to the Model::Tooth class
        for (auto& [toothId, tooth]: teethMap) {
            tooth.toothPolyData->DeepCopy(Utilities::rotatePolyData(tooth.toothPolyData, xAngle, 0, -zAngle));
            tooth.axes = tooth.axes * matrix;
        }
        return {xAngle, -zAngle};
    }
};


