//============================================================================
// Name        : TeethAnglesOrientation.cpp
// Created on  : 29.12.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : TeethAnglesOrientation
//============================================================================

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

#include "Utils.h"
#include "TeethAnglesOrientation.h"


namespace TeethAnglesOrientation
{

    void VisualizeTeethOriented()
    {
        constexpr std::string_view planJsonFile {
                R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        for (unsigned short toothId: FDI::teethIds ) {
            // for (unsigned short toothId: { 11}) {
            if (18 == toothId || 28 == toothId || 38 == toothId || 48 == toothId )
                continue;

            Model::Tooth tooth(toothMap[toothId], toothId);
            Model::ToothAxis& axes = tooth.setAxis(teethAxes[toothId]);

            const short signH = tooth.axes.horizontal.x() >= 0 ? 1 : -1;
            const short signV = tooth.axes.vertical.y() >= 0 ? 1 : -1;
            const Eigen::Vector3d verticalOrig {0, static_cast<double>(signV), 0};
            const Eigen::Vector3d horizontalOrig {static_cast<double>(signH), 0, 0};

            vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);

            {
                Eigen::Matrix3d matrix {Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

                axes.horizontal = matrix * axes.horizontal;
                axes.vertical = matrix * axes.vertical;
                axes.angulation = matrix * axes.angulation;

                vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                toothData = Utilities::rotatePolyData(toothData, vtkMatrix);

                matrix = Eigen::Quaterniond::FromTwoVectors(axes.vertical, verticalOrig).toRotationMatrix();

                axes.horizontal = matrix * axes.horizontal;
                axes.vertical = matrix * axes.vertical;
                axes.angulation = matrix * axes.angulation;

                vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                toothData = Utilities::rotatePolyData(toothData, vtkMatrix);
            }

            auto verticalAxeActor = Utilities::getLineActor({0, 0, 0}, axes.vertical * 7, {1, 0, 0});
            auto frontAxeActor = Utilities::getLineActor({0, 0, 0}, axes.angulation * 7, {1, 0, 0});
            auto horizontalAxeActor = Utilities::getLineActor({0, 0, 0}, axes.horizontal * 7, {1, 0, 0});
            auto horizontalOrigActor = Utilities::getLineActor({0, 0, 0}, horizontalOrig * 7, {1, 0, 0});

            horizontalAxeActor->GetProperty()->SetDiffuseColor(std::array {1.0, 1.0, 0.0}.data());
            horizontalOrigActor->GetProperty()->SetDiffuseColor(std::array {1.0, 1.0, 0.0}.data());

            const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothData);
            const vtkSmartPointer<vtkActor> outlineActor = Utilities::getOutlineActor(toothData);

            Utilities::DisplayActors({toothActor, outlineActor,
                                      verticalAxeActor, horizontalAxeActor, frontAxeActor
                                     });
        }
    }

    void OrientJaw_OcclusionPlane() {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        //double jawDistTotal = 0;
        Model::Jaw lowerJaw { Model::TeethType::Lower }, upperJaw { Model::TeethType::Lower };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isLowerTooth(toothId)) {
                if (auto [iter, ok] = lowerJaw.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                    //jawDistTotal += iter->second.mesiodistalWidth;
                }
            } else {
                if (auto [iter, ok] = upperJaw.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        // To orient whole JAW horizontaly
        // TODO: you need to learn how to find an approximate plane based on the vertices of the teeth
        std::vector<vtkSmartPointer<vtkActor>> actors;
        {
            Eigen::Vector3d ptCenter1 { lowerJaw.teeth.at(37).GetCenter() },
                            ptCenter2 { lowerJaw.teeth.at(47).GetCenter() };
            Eigen::Vector3d midPointBack = Utils::middlePoint(ptCenter1, ptCenter2);

            ptCenter1 = lowerJaw.teeth.at(31).GetCenter();
            ptCenter2 = lowerJaw.teeth.at(41).GetCenter();

            Eigen::Vector3d midPointFront = Utils::middlePoint(ptCenter1, ptCenter2);
            Eigen::Vector3d ptBackYZero {midPointBack.x(), midPointFront.y(), midPointBack.z()};

            double angle = Utils::getAngle(midPointBack -midPointFront, ptBackYZero - midPointFront);
            const Eigen::Matrix3d matrix {
                Eigen::Quaterniond::FromTwoVectors(midPointBack -midPointFront, ptBackYZero - midPointFront)};
            std::cout << angle << std::endl;

            for (auto&[id, tooth]: lowerJaw.teeth)
            {
                Model::ToothAxis& axes = tooth.axes;
                axes.horizontal = matrix * axes.horizontal * 7;
                axes.vertical = matrix * axes.vertical * 7;
                axes.angulation = matrix * axes.angulation * 7;

                tooth.toothPolyData->DeepCopy(Utilities::rotatePolyData(tooth.toothPolyData, -angle, 0, 0));
                Eigen::Vector3d center { tooth.GetCenter()};

                actors.push_back(Utilities::getLineActor(center, axes.vertical + center, {1, 0, 0}));
                actors.push_back(Utilities::getLineActor(center, axes.angulation + center, {1, 0, 0}));
                actors.push_back(Utilities::getLineActor(center, axes.horizontal + center, {1, 0, 0}));
                actors.push_back(Utilities::getPolyDataActor(tooth.toothPolyData));
            }

            actors.push_back(Utilities::getPointsActor({midPointBack, midPointFront, ptBackYZero}));
            actors.push_back(Utilities::getLineActor(midPointFront, midPointBack));
            actors.push_back(Utilities::getLineActor(midPointFront, ptBackYZero, {0, 1 ,0}));
        }
        Utilities::DisplayActors(actors, {50, 100});
    }

    void OrientJaw_OcclusionPlane_RecalcTorks() {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        // TODO: Check for performance: pass polyData&& (as RValue ref) ???
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&& [toothId, polyData]: toothMap) {
            if (auto [iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
            }
        }

        Utils::orientTeethToPlane(allTeeth);

        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (auto& [toothId, tooth]: allTeeth) {
            estimator->estimate(tooth);
        }

#if 0 // OLD
        // To orient whole JAW horizontaly
        // TODO: you need to learn how to find an approximate plane based on the vertices of the teeth
        {
            Eigen::Vector3d ptCenter1 { allTeeth.at(37).GetCenter() },
                            ptCenter2 { allTeeth.at(47).GetCenter() };
            Eigen::Vector3d midPointBack = Utils::middlePoint(ptCenter1, ptCenter2);

            ptCenter1 = allTeeth.at(31).GetCenter();
            ptCenter2 = allTeeth.at(41).GetCenter();

            Eigen::Vector3d midPointFront = Utils::middlePoint(ptCenter1, ptCenter2);
            Eigen::Vector3d ptBackYZero {midPointBack.x(), midPointFront.y(), midPointBack.z()};

            double angle = Utils::getAngle(midPointBack -midPointFront, ptBackYZero - midPointFront);
            const Eigen::Matrix3d matrix {
                    Eigen::Quaterniond::FromTwoVectors(midPointBack -midPointFront, ptBackYZero - midPointFront)};
            std::cout << angle << std::endl;

            for (auto&[id, tooth]: allTeeth) {
                tooth.axes = tooth.axes * matrix ;
                tooth.toothPolyData->DeepCopy(Utilities::rotatePolyData(tooth.toothPolyData, -angle, 0, 0));
            }
        }
#endif

        constexpr std::array<int, 14> lowerIDs { 47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37 };
        std::vector<vtkSmartPointer<vtkActor>> actors;
        double xPos = 0, prevToothLength = 0;
        std::array<double, 32> torkAngles {};

        // for (const auto& [id, tooth]: lowerJaw.teeth)
        for (const int id: lowerIDs)
        // for (const int id: {43, 42, 41, 31, 32, 33})
        {
            Model::Tooth& tooth = allTeeth.at(id);
            Eigen::Vector3d ptCenter { 0, 0 ,0};
            vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, ptCenter);
            Model::ToothAxis axes = tooth.getAxis();

            const double signH = axes.horizontal.x() >= 0 ? 1.0 : -1.0, signV = axes.vertical.y() >= 0 ? 1.0 : -1.0;
            Eigen::Vector3d verticalOrig {0, signV, 0}, horizontalOrig {signH, 0, 0};
            Eigen::Matrix3d matrix { Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig) };

            axes.horizontal = matrix * axes.horizontal * 10;
            axes.vertical = matrix * axes.vertical * 7;
            axes.angulation = matrix * axes.angulation * 7;
            verticalOrig = verticalOrig * 7;

            xPos += (prevToothLength + tooth.mesiodistalWidth)/2 + 0.1;
            prevToothLength = tooth.mesiodistalWidth;
            ptCenter.x() = xPos;

            vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
            toothData = Utilities::rotatePolyData(toothData, vtkMatrix);
            toothData = Utilities::setPolyDataCenter(toothData, ptCenter);


            axes.vertical.x()  = 0;
            torkAngles[id] = Utils::getAngle(axes.vertical, verticalOrig);
            std::cout << "[" << id << "]: angle = " << torkAngles[id] << std::endl;


            actors.push_back(Utilities::getLineActor(ptCenter, axes.vertical + ptCenter , {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(ptCenter, axes.horizontal + ptCenter, {1, 1, 0}));
            // actors.push_back(Utilities::getLineActor(ptCenter, axes.angulation + ptCenter, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(ptCenter, verticalOrig + ptCenter, {1, 1, 0}));
            // actors.push_back(Utilities::getOutlineActor(toothData));
            actors.push_back(Utilities::getPolyDataActor(toothData));
        }

        Utilities::DisplayActors(actors);
    }

    void Test() {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        //double jawDistTotal = 0;
        Model::Jaw lowerJaw { Model::TeethType::Lower }, upperJaw { Model::TeethType::Lower };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isLowerTooth(toothId)) {
                if (auto [iter, ok] = lowerJaw.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                    //jawDistTotal += iter->second.mesiodistalWidth;
                }
            } else {
                if (auto [iter, ok] = upperJaw.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }


        constexpr std::array<int, 14> lowerIDs { 47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37 };
        std::vector<vtkSmartPointer<vtkActor>> actors;
        double xPos = 0, prevToothLength = 0;

        std::array<double, 32> torkAngles {};

        // for (const auto& [id, tooth]: lowerJaw.teeth)
        for (const int id: lowerIDs)
        // for (const int id: {43, 42, 41, 31, 32, 33})
        {
            Model::Tooth& tooth = lowerJaw.teeth.at(id);
            Eigen::Vector3d ptCenter { 0, 0 ,0};
            vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, ptCenter);
            Model::ToothAxis axes = tooth.getAxis();


            const double signH = axes.horizontal.x() >= 0 ? 1.0 : -1.0, signV = axes.vertical.y() >= 0 ? 1.0 : -1.0;
            Eigen::Vector3d verticalOrig {0, signV, 0}, horizontalOrig {signH, 0, 0};

            Eigen::Matrix3d matrix { Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig) };

            axes.horizontal = matrix * axes.horizontal * 10;
            axes.vertical = matrix * axes.vertical * 7;
            axes.angulation = matrix * axes.angulation * 7;
            verticalOrig = verticalOrig * 7;

            xPos += (prevToothLength + tooth.mesiodistalWidth)/2 + 0.1;
            prevToothLength = tooth.mesiodistalWidth;
            ptCenter.x() = xPos;

            vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
            toothData = Utilities::rotatePolyData(toothData, vtkMatrix);
            toothData = Utilities::setPolyDataCenter(toothData, ptCenter);


            axes.vertical.x()  = 0;
            torkAngles[id] = Utils::getAngle(axes.vertical, verticalOrig);
            std::cout << "[" << id << "]: angle = " << torkAngles[id] << std::endl;


            actors.push_back(Utilities::getLineActor(ptCenter, axes.vertical + ptCenter , {1, 0, 0}));
            // actors.push_back(Utilities::getLineActor(ptCenter, axes.horizontal + ptCenter, {1, 1, 0}));
            // actors.push_back(Utilities::getLineActor(ptCenter, axes.angulation + ptCenter, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(ptCenter, verticalOrig + ptCenter, {1, 1, 0}));
            // actors.push_back(Utilities::getOutlineActor(toothData));
            actors.push_back(Utilities::getPolyDataActor(toothData));
        }

        Utilities::DisplayActors(actors);
    }
}

namespace TeethAnglesOrientation::OcclusionPlane
{

    // NOTE: Estimate plane using LinearRegression
    void OrientTeethByPlane() {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        std::vector<Eigen::Vector3d> points;
        points.reserve(toothMap.size());
        for (const auto& [toothId, polyData]: toothMap) {
            Eigen::Vector3d& center = points.emplace_back();
            polyData->GetCenter(center.data());

            std::array<double, 6> bounds{};
            polyData->GetBounds(bounds.data());

            const double yLength {bounds[3] - bounds[2]};
            Eigen::Vector3d& edgePoint = points.emplace_back(center.x(), center.y(), center.z());
            edgePoint.y() += FDI::isLowerTooth(toothId) ? yLength / 2 : - yLength / 2;
        }
        std::sort(points.begin(), points.end(), [](const auto& pt1, const auto& pt2) {
            return pt2.x() > pt1.x();
        });

        std::vector<double> X, Y, Z;
        for (const auto &pt: points) {
            X.push_back(pt.x());
            Y.push_back(pt.y());
            Z.push_back(pt.z());
        }

        const Utils::LinearRegression<double> linReg;
        const Utils::Coefficients predictedX = linReg.estimate(Z, Y);
        const Utils::Coefficients predictedZ = linReg.estimate(X, Y);

        const double xAngle = std::atan(predictedX.slope) * 180 / std::numbers::pi;
        const double zAngle = std::atan(predictedZ.slope) * 180 / std::numbers::pi;
        std::cout << "xAngle = " << xAngle << ", zAngle = " << zAngle << std::endl;

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (auto& [toothId, polyData]: toothMap)
            polyData->DeepCopy(Utilities::rotatePolyData(polyData, xAngle, 0, -zAngle));


        /*
        { // Just to check angles after transformation
            std::vector<Eigen::Vector3d> points2;
            for (const auto&[toothId, polyData]: toothMap) {
                Eigen::Vector3d center{};
                polyData->GetCenter(center.data());

                std::array<double, 6> bounds{};
                polyData->GetBounds(bounds.data());

                const double yLength{bounds[3] - bounds[2]};
                Eigen::Vector3d edgePoint{center.x(), center.y(), center.z()};
                edgePoint.y() += FDI::isLowerTooth(toothId) ? yLength / 2 : -yLength / 2;

                points2.push_back(center);
                points2.push_back(edgePoint);
                actors.push_back(Utilities::getPointsActor({center, edgePoint}));
            }
            std::sort(points2.begin(), points2.end(), [](const auto &pt1, const auto &pt2) {
                return pt2.x() > pt1.x();
            });
            std::vector<double> X2(points.size()), Y2(points.size()), Z2(points.size());
            for (size_t i = 0; const auto &pt: points2) {
                X2[i] = pt.x(); Y2[i] = pt.y(); Z2[i++] = pt.z();
            }

            const Coefficients predictedX2 = linReg.estimate(Z2, Y2);
            const Coefficients predictedZ2 = linReg.estimate(X2, Y2);
            const double xAngle2 = std::atan(predictedX2.slope) * 180 / std::numbers::pi;
            const double zAngle2 = std::atan(predictedZ2.slope) * 180 / std::numbers::pi;
            std::cout << "xAngle2 = " << xAngle2 << ", zAngle2 = " << zAngle2 << std::endl;
        }
        */

        for (auto& [toothId, polyData]: toothMap)
            actors.push_back(Utilities::getPolyDataActor(polyData));
        Utilities::DisplayActors(actors);
    }

    void OrientTeethAndAxisByPlane() {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        // TODO: Check for performance: pass polyData&& (as RValue ref) ???
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&& [toothId, polyData]: toothMap) {
            if (auto [iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
            }
        }

        Utils::orientTeethToPlane(allTeeth);

        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (auto& [toothId, tooth]: allTeeth) {
            estimator->estimate(tooth);
        }

        // Just to test teeth AXES rotation with
        /*
        {
            constexpr float zAngle = 45;
            const double sinZ = std::sin(Utils::degToRad(zAngle));
            const double cosZ = std::cos(Utils::degToRad(zAngle));
            const Eigen::Matrix3d matrix {
                    {cosZ, -sinZ, 0},
                    {sinZ, cosZ, 0},
                    {0, 0, 1}
            };

            for (auto& [toothId, tooth]: allTeeth) {
                tooth.toothPolyData->DeepCopy(Utilities::rotatePolyData(tooth.toothPolyData, 0, 0, zAngle));
                tooth.axes = tooth.axes * matrix;
            }
            Utils::displayTeethAndAxis(allTeeth, true, false);
        }
        */

        Utils::displayTeethAndAxis(allTeeth, true, true);
    }
}

namespace TeethAnglesOrientation::Shpee_Curve_Analysis
{
    void PlaceTeethToOneStraightLine() {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        // TODO: Check for performance: pass polyData&& (as RValue ref) ???
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&& [toothId, polyData]: toothMap) {
            if (auto [iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
            }
        }

        Utils::orientTeethToPlane(allTeeth);

        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        std::for_each(allTeeth.begin(), allTeeth.end(), [&estimator](auto& iter) {
            estimator->estimate(iter.second);
        });

        //===========================================================================================

        std::vector<Eigen::Vector3d> points;
        std::unordered_map<int, Eigen::Vector3d> pointsMap;
        std::vector<vtkSmartPointer<vtkProp3D>> actors;
        constexpr std::array<int, 14> lowerIDs { 47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37 };
        double xPos = 0, prevToothLength = 0;

        // NOTE: [ToothID, [Tork, Dist to Spee Line]]
        std::map<int, std::pair<double, double>> metrics {};

        for (const int toothID: lowerIDs)
        {
            const Model::Tooth& tooth = allTeeth.at(toothID);
            Model::ToothAxis axes = tooth.getAxis();

            const double signH = axes.horizontal.x() >= 0 ? 1.0 : -1.0, signV = axes.vertical.y() >= 0 ? 1.0 : -1.0;
            Eigen::Vector3d verticalOrig {0, signV, 0}, horizontalOrig {signH, 0, 0};
            Eigen::Matrix3d matrix { Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig) };

            axes = axes * matrix * 10;
            verticalOrig = verticalOrig * 10;

            xPos += (prevToothLength + tooth.mesiodistalWidth)/2 + 0.1;
            prevToothLength = tooth.mesiodistalWidth;

            vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
            vtkSmartPointer<vtkPolyData> toothData = Utilities::rotatePolyData(tooth.toothPolyData, vtkMatrix);

            Eigen::Vector3d ptCenter { xPos, tooth.GetCenter().y(), 0};
            toothData = Utilities::setPolyDataCenter(toothData, ptCenter);

            {
                // Calc edge point: [Y coord --> Dist to SHPEE]
                Eigen::Vector3d center {};
                toothData->GetCenter(center.data());

                std::array<double, 6> bounds {};
                toothData->GetBounds(bounds.data());

                const double height{bounds[3] - bounds[2]};

                Eigen::Vector3d &edgePoint = points.emplace_back(center.x(), center.y(), center.z());
                edgePoint.y() += FDI::isLowerTooth(toothID) ? height / 2 : -height / 2;
                pointsMap.emplace(toothID, edgePoint);

                // Calc tork:
                axes.vertical.x() = 0;
                const double torkAngle = Utils::getAngle(axes.vertical, verticalOrig);

                auto [iter, ok] = metrics.emplace(toothID, std::make_pair(torkAngle, edgePoint.y()));
                std::cout << toothID << ": [" << iter->second.first << ", " << iter->second.second << "]" << std::endl;
            }

            {
                const vtkSmartPointer<vtkTextActor3D> textActor { vtkTextActor3D::New() };
                textActor->SetPosition(ptCenter.x() - tooth.mesiodistalWidth / 2 + 2, ptCenter.y() - 15, ptCenter.z());
                textActor->SetInput(std::to_string(toothID).c_str());

                const vtkSmartPointer<vtkTextProperty> prop = textActor->GetTextProperty();
                prop->SetFontSize(6);

                actors.emplace_back(textActor);
            }

            actors.emplace_back(Utilities::getLineActor(ptCenter, axes.vertical + ptCenter , {1, 0, 0}));
            actors.emplace_back(Utilities::getLineActor(ptCenter, axes.horizontal + ptCenter, {1, 1, 0}));
            // actors.emplace_back(Utilities::getLineActor(ptCenter, axes.angulation + ptCenter, {1, 0, 0}));
            actors.emplace_back(Utilities::getLineActor(ptCenter, verticalOrig + ptCenter, {1, 1, 0}));
            // actors.emplace_back(Utilities::getOutlineActor(toothData));
            actors.emplace_back(Utilities::getPolyDataActor(toothData));
        }

        // Draw Shpee LINES !!!
        {
            const Eigen::Vector3d& pt1 = pointsMap[47];
            const Eigen::Vector3d& pt2 = pointsMap[43];
            const std::pair<double, double>& K =
                    Utils::getLineCoefficients({pt1.x(), pt1.y()}, {pt2.x(), pt2.y()});

            const double x = pointsMap[41].x();
            const Eigen::Vector3d& pt3 {x, x * K.first + K.second, 0};

            actors.emplace_back(Utilities::getLineActor(pt1, pt3, {1, 1, 0}));
        }

        {
            const Eigen::Vector3d& pt1 = pointsMap[37];
            const Eigen::Vector3d& pt2 = pointsMap[33];
            const std::pair<double, double>& K =
                    Utils::getLineCoefficients({pt1.x(), pt1.y()}, {pt2.x(), pt2.y()});

            const double x = pointsMap[31].x();
            const Eigen::Vector3d& pt3 {x, x * K.first + K.second, 0};

            actors.emplace_back(Utilities::getLineActor(pt1, pt3, {1, 1, 0}));
        }

        actors.emplace_back(Utilities::getPointsActor(points, {0, 1, 0}));
        Utilities::DisplayActorsEx(actors);
    }


    void MoveTeeth_UP_DOWN_CureShpee() {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        // TODO: Check for performance: pass polyData&& (as RValue ref) ???
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&& [toothId, polyData]: toothMap) {
            if (auto [iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
            }
        }

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            Utils::orientTeethToPlane(allTeeth);

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "Duration:" << time_span.count() << "seconds";
        }


        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        std::for_each(allTeeth.begin(), allTeeth.end(), [&estimator](auto& iter) {
            estimator->estimate(iter.second);
        });

        //===========================================================================================

        std::vector<Eigen::Vector3d> points;
        std::unordered_map<int, Eigen::Vector3d> pointsMap;
        std::vector<vtkSmartPointer<vtkProp3D>> actors;
        constexpr std::array<int, 14> lowerIDs { 47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37 };
        double xPos = 0, prevToothLength = 0;

        // NOTE: [ToothID, [Tork, Dist to Spee Line]]
        std::map<int, std::pair<double, double>> metrics {};

        for (const int toothID: lowerIDs)
        {
            const Model::Tooth& tooth = allTeeth.at(toothID);
            Model::ToothAxis axes = tooth.getAxis();

            const double signH = axes.horizontal.x() >= 0 ? 1.0 : -1.0, signV = axes.vertical.y() >= 0 ? 1.0 : -1.0;
            Eigen::Vector3d verticalOrig {0, signV, 0}, horizontalOrig {signH, 0, 0};
            Eigen::Matrix3d matrix { Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig) };

            axes = axes * matrix * 10;
            verticalOrig = verticalOrig * 10;

            xPos += (prevToothLength + tooth.mesiodistalWidth)/2 + 0.1;
            prevToothLength = tooth.mesiodistalWidth;

            vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
            vtkSmartPointer<vtkPolyData> toothData = Utilities::rotatePolyData(tooth.toothPolyData, vtkMatrix);

            Eigen::Vector3d ptCenter { xPos, tooth.GetCenter().y(), 0};
            toothData = Utilities::setPolyDataCenter(toothData, ptCenter);

            {
                // Calc edge point: [Y coord --> Dist to SHPEE]
                Eigen::Vector3d center {};
                toothData->GetCenter(center.data());

                std::array<double, 6> bounds {};
                toothData->GetBounds(bounds.data());

                const double height{bounds[3] - bounds[2]};

                Eigen::Vector3d &edgePoint = points.emplace_back(center.x(), center.y(), center.z());
                edgePoint.y() += FDI::isLowerTooth(toothID) ? height / 2 : -height / 2;
                pointsMap.emplace(toothID, edgePoint);

                // Calc tork:
                axes.vertical.x() = 0;
                const double torkAngle = Utils::getAngle(axes.vertical, verticalOrig);

                auto [iter, ok] = metrics.emplace(toothID, std::make_pair(torkAngle, edgePoint.y()));
                std::cout << toothID << ": [" << iter->second.first << ", " << iter->second.second << "]" << std::endl;
            }

            {
                const vtkSmartPointer<vtkTextActor3D> textActor { vtkTextActor3D::New() };
                textActor->SetPosition(ptCenter.x() - tooth.mesiodistalWidth / 2 + 2, ptCenter.y() - 15, ptCenter.z());
                textActor->SetInput(std::to_string(toothID).c_str());

                const vtkSmartPointer<vtkTextProperty> prop = textActor->GetTextProperty();
                prop->SetFontSize(6);

                actors.emplace_back(textActor);
            }

            actors.emplace_back(Utilities::getLineActor(ptCenter, axes.vertical + ptCenter , {1, 0, 0}));
            actors.emplace_back(Utilities::getLineActor(ptCenter, axes.horizontal + ptCenter, {1, 1, 0}));
            // actors.emplace_back(Utilities::getLineActor(ptCenter, axes.angulation + ptCenter, {1, 0, 0}));
            actors.emplace_back(Utilities::getLineActor(ptCenter, verticalOrig + ptCenter, {1, 1, 0}));
            // actors.emplace_back(Utilities::getOutlineActor(toothData));
            actors.emplace_back(Utilities::getPolyDataActor(toothData));
        }

        // Draw Shpee LINES !!!
        {
            const Eigen::Vector3d& pt1 = pointsMap[47];
            const Eigen::Vector3d& pt2 = pointsMap[43];
            const std::pair<double, double>& K =
                    Utils::getLineCoefficients({pt1.x(), pt1.y()}, {pt2.x(), pt2.y()});

            const double x = pointsMap[41].x();
            const Eigen::Vector3d& pt3 {x, x * K.first + K.second, 0};

            actors.emplace_back(Utilities::getLineActor(pt1, pt3, {1, 1, 0}));
        }

        {
            const Eigen::Vector3d& pt1 = pointsMap[37];
            const Eigen::Vector3d& pt2 = pointsMap[33];
            const std::pair<double, double>& K =
                    Utils::getLineCoefficients({pt1.x(), pt1.y()}, {pt2.x(), pt2.y()});

            const double x = pointsMap[31].x();
            const Eigen::Vector3d& pt3 {x, x * K.first + K.second, 0};

            actors.emplace_back(Utilities::getLineActor(pt1, pt3, {1, 1, 0}));
        }

        actors.emplace_back(Utilities::getPointsActor(points, {0, 1, 0}));
        Utilities::DisplayActorsEx(actors);
    }
}



void TeethAnglesOrientation::TestAll()
{
    // TeethAnglesOrientation::VisualizeTeethOriented();

    // TeethAnglesOrientation::OrientJaw_OcclusionPlane();
    // TeethAnglesOrientation::OrientJaw_OcclusionPlane_RecalcTorks();

    // TeethAnglesOrientation::Test();

    // OcclusionPlane::OrientTeethByPlane();
    // OcclusionPlane::OrientTeethAndAxisByPlane();

    // Shpee_Curve_Analysis::PlaceTeethToOneStraightLine();
    // Shpee_Curve_Analysis::MoveTeeth_UP_DOWN_CureShpee();
}