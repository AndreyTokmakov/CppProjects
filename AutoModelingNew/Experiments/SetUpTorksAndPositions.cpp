//============================================================================
// Name        : SetUpTorksAndPositions.h
// Created on  : 13.01.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : SetUpTorksAndPositions
//============================================================================

#include "SetUpTorksAndPositions.h"

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>
#include <vector>
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

using namespace std::string_literals;

namespace SetUpTorksAndPositions
{

    [[nodiscard]]
    std::unordered_map<unsigned short, Eigen::Vector3d>
    GetTeethEdgePoints(const std::unordered_map<unsigned short, Model::Tooth>& teethMap)
    {
        std::unordered_map<unsigned short, Eigen::Vector3d> edgePoints;
        for (const auto& [toothID, tooth]: teethMap) {
            Eigen::Vector3d center = tooth.GetCenter();
            const std::array<double, 6> bounds = tooth.GetBounds();
            const double height {bounds[3] - bounds[2]};

            // FIXME: Validate that tooth with 'ID' exists or we crush
            auto [iter, ok] = edgePoints.emplace(toothID, std::move(center));
            iter->second.y() += FDI::isLowerTooth(toothID) ? height / 2 : -height / 2;
        }
        return edgePoints;
    }

    void printLowerTeethEdgePoints(const std::unordered_map<unsigned short, Model::Tooth>& teethMap) {
        const std::unordered_map<unsigned short, Eigen::Vector3d>& edgePoints { GetTeethEdgePoints(teethMap) };
        for (const auto id: {47,46,45,44,43,42,41,31,32,33,34,35,36,37}) {
            if (FDI::isLowerTooth(id))
                std::cout << id << " -->  " << edgePoints.at(id).y() << std::endl;
        }
    }

    // TODO: Refactor
    // We choose the closest tooth to the average coordinate of incisors and molars
    unsigned short getControlToothID(const std::unordered_map<unsigned short, Eigen::Vector3d>& edgePoints) {
        // TODO: Move incisors somewhere else
        constexpr std::array<unsigned short, 8> iDs {47, 46, 42, 41, 31, 32, 36, 37};
        double avgY {0}; // TODO: use std::accumulate
        for (unsigned short id: iDs)
            avgY += edgePoints.at(id).y() ;
        avgY /= iDs.size();

        unsigned short closesUnder {0}, closesAbove {0};
        for (unsigned short id: {42, 41, 31, 32}) {
            const double y = edgePoints.at(id).y();
            if (avgY > y) { // Under
                if (closesUnder && edgePoints.at(closesUnder).y() < y)
                    closesUnder = id;
                else if (!closesUnder)
                    closesUnder = id;
            }
            if (y >= avgY) { // Above
                if (closesAbove && edgePoints.at(closesAbove).y() > y)
                    closesAbove = id;
                else if (!closesAbove)
                    closesAbove = id;
            }
        }

        return closesUnder ? closesUnder : closesAbove;
    }

    void alignTeethVertical(std::unordered_map<unsigned short, Model::Tooth>& teethMap)
    {
        const std::unordered_map<unsigned short, Eigen::Vector3d>& edgePoints { GetTeethEdgePoints(teethMap) };
        unsigned short ctrlToothID = getControlToothID(edgePoints);
        const Eigen::Vector3d& ctrlTooth {edgePoints.at(ctrlToothID) };
        double yTarget {ctrlTooth.y()};

        printLowerTeethEdgePoints(teethMap);

        // TODO: Move incisors somewhere else
        constexpr std::array<unsigned short, 4> incisors {42, 41, 31, 32};
        for (unsigned short id: incisors) {
            const Eigen::Vector3d& pt {edgePoints.at(id) };
            const double diff { yTarget - pt.y() };

            Model::Tooth& tooth { teethMap.at(id) };
            tooth.toothPolyData->DeepCopy(Utilities::moveTooth(tooth.toothPolyData, 0, diff, 0));
        }

        printLowerTeethEdgePoints(teethMap);

        constexpr std::array<unsigned short, 2> canines {43, 33};
        {
            // TODO: Validate all fangs are present
            const Eigen::Vector3d &toothPt1 {edgePoints.at(canines.front())},
                                  &toothPt2 {edgePoints.at(canines.back())};
            // NOTE: Just some random diff between fangs and incisors
            constexpr double diffMin = 0.15, diffMax = 0.35;

            // INFO: Expecting to fangs to be higher than incisors
            const double diff1 {toothPt1.y() - yTarget}, diff2 {toothPt2.y() - yTarget};

            // INFO: Very naive
            // select the distance with the minimum offset in the range 0.15 - 0.35 from the control tooth
            double diff = 0;
            if (diff1 > diffMin && diff2 > diffMin) {
                diff = std::min(diff1, diff2);
                diff = std::min(diff, diffMax);
            } else {
                diff = diffMin;
            }

            Model::Tooth &tooth1 {teethMap.at(canines.front())}, &tooth2 {teethMap.at(canines.back())};
            tooth1.toothPolyData->DeepCopy(Utilities::moveTooth(
                    tooth1.toothPolyData, 0, diff - diff1, 0));
            tooth2.toothPolyData->DeepCopy(Utilities::moveTooth(
                    tooth2.toothPolyData, 0, diff - diff2, 0));
        }

        constexpr std::array<unsigned short, 4> molars {47, 46, 36, 37};


        {
            // NOTE: Just some random diff between fangs and incisors
            // TODO: Move to some utils class ???
            constexpr std::pair<double, double> limits { -0.5, 0.5};
            const std::pair<double, double> yMolarRange { yTarget + limits.first, yTarget + limits.second};

            // NOTE: Molars cutting edges points
            // TODO: Refactor code!! make molarEdges const?? use references?
            // TODO: Validate missing
            const std::array<Eigen::Vector3d, 4> molarEdges {{
                { edgePoints.at(molars[0]) }, { edgePoints.at(molars[1])},
                { edgePoints.at(molars[2]) }, { edgePoints.at(molars[3])}
            }};

            // TODO: Check if (46, 36) not higher then (47, 37)
            #if 0
            double yMax { std::numeric_limits<double>::min()}, yMin { std::numeric_limits<double>::max()};
            for (const Eigen::Vector3d& edge: molarEdges) {
                yMax = std::max(yMax, edge.y());
                yMin = std::min(yMin, edge.y());
            }
            #endif

            // Here We find the position of the molars as close as possible to yTarget,
            // taking into account the test limits 'limits'
            // TODO: Need to add maximum move limits that we can have here
            double yMolarTarget = 0;
            if (molarEdges.front().y() > yTarget && molarEdges.back().y() > yTarget) {
                yMolarTarget = std::min(molarEdges.front().y(), molarEdges.back().y());
                yMolarTarget = std::min(yMolarTarget, yMolarRange.second);
            } else if (molarEdges.front().y() < yTarget && molarEdges.back().y() < yTarget) {
                yMolarTarget = std::max(molarEdges.front().y(), molarEdges.back().y());
                yMolarTarget = std::max(yMolarTarget, yMolarRange.first);
            } else {
                yMolarTarget = std::midpoint(molarEdges.front().y(), molarEdges.back().y());
            }

            yTarget = yMolarTarget;

            // TODO: Move BACK molars (47, 37) only:
            Model::Tooth &tooth1 {teethMap.at(molars.front())}, &tooth2 {teethMap.at(molars.back())};
            tooth1.toothPolyData->DeepCopy(Utilities::moveTooth(
                    tooth1.toothPolyData, 0, yMolarTarget - molarEdges.front().y(), 0));
            tooth2.toothPolyData->DeepCopy(Utilities::moveTooth(
                    tooth2.toothPolyData, 0, yMolarTarget - molarEdges.back().y(), 0));
        }

        // NOTE: Move 46, 36
        {
            // NOTE: Just some random diff between fangs and incisors
            // TODO: Move to some utils class ???
            constexpr std::pair<double, double> limits { -0.2, 0.0};
            const std::pair<double, double> yMolarRange { yTarget + limits.first, yTarget + limits.second};

            // NOTE: Molars cutting edges points
            // TODO: Refactor code!! make molarEdges const?? use references?
            // TODO: Validate missing
            const std::array<Eigen::Vector3d, 2> molarEdges {{
                { edgePoints.at(molars[1])}, { edgePoints.at(molars[2]) }
            }};

            // TODO: Check if (46, 36) not higher then (47, 37)
#if 0
            double yMax { std::numeric_limits<double>::min()}, yMin { std::numeric_limits<double>::max()};
            for (const Eigen::Vector3d& edge: molarEdges) {
                yMax = std::max(yMax, edge.y());
                yMin = std::min(yMin, edge.y());
            }
#endif

            // Here We find the position of the molars as close as possible to yTarget,
            // taking into account the test limits 'limits'
            // TODO: Need to add maximum move limits that we can have here
            double yMolarTarget = 0;
            if (molarEdges.front().y() > yTarget && molarEdges.back().y() > yTarget) {
                yMolarTarget = std::min(molarEdges.front().y(), molarEdges.back().y());
                yMolarTarget = std::min(yMolarTarget, yMolarRange.second);
            } else if (molarEdges.front().y() < yTarget && molarEdges.back().y() < yTarget) {
                yMolarTarget = std::max(molarEdges.front().y(), molarEdges.back().y());
                yMolarTarget = std::max(yMolarTarget, yMolarRange.first);
            } else {
                yMolarTarget = std::midpoint(molarEdges.front().y(), molarEdges.back().y());
            }

            // TODO: Move BACK molars (46, 36) only:
            Model::Tooth &tooth1 {teethMap.at(molars[1])}, &tooth2 {teethMap.at(molars[2])};
            tooth1.toothPolyData->DeepCopy(Utilities::moveTooth(
                    tooth1.toothPolyData, 0, yMolarTarget - molarEdges.front().y(), 0));
            tooth2.toothPolyData->DeepCopy(Utilities::moveTooth(
                    tooth2.toothPolyData, 0, yMolarTarget - molarEdges.back().y(), 0));
        }
    }

    // TODO: Remove std::array<double, 49> angles{} !!! 49 - overhead !!!
    void alignTeethAngulation(std::unordered_map<unsigned short, Model::Tooth>& teethMap) {
        std::array<double, 49> angles{}, anglesToAppy{};
        constexpr std::array<int, 14> lowerIDs {47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};

        for (const int toothID: lowerIDs) {
            const Model::Tooth &tooth = teethMap.at(toothID);
            Model::ToothAxis axes = tooth.getAxis();

            const short signH = tooth.axes.horizontal.x() >= 0 ? 1 : -1;
            const short signV = tooth.axes.vertical.y() >= 0 ? 1 : -1;
            const Eigen::Vector3d horizontalOrig {static_cast<double>(signH), 0, 0};
            double yAngle = Utils::getAngle(horizontalOrig, axes.horizontal);
            yAngle *= FDI::isLeftSideTooth(toothID) ? -1 : 1;

            const double sinY = std::sin(Utils::degToRad(yAngle)),
                         cosY = std::cos(Utils::degToRad(yAngle));
            const Eigen::Matrix3d matrix {
                    { cosY, 0,  sinY },
                    { 0,    1,     0 },
                    {-sinY, 0,  cosY }
            };

            axes = axes * matrix;

            const Eigen::Vector3d verticalProjection {0, static_cast<double>(signV), axes.vertical.z()};
            angles[toothID]  = Utils::getAngle(verticalProjection, axes.vertical);

            // INFO: Orient tooth along the horizont axe
            // vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);
            // toothData = Utilities::rotatePolyData(toothData, vtkMatrix);
            // toothData = Utilities::rotatePolyData(toothData, 0, yAngle, 0);

            // std::vector<vtkSmartPointer<vtkActor>> actors;
            // actors.push_back(Utilities::getLineActor({0, 0, 0}, axes.vertical * 8, {1, 0, 0}));
            // actors.push_back(Utilities::getLineActor({0, 0, 0}, verticalProjection * 6, {0, 1, 0}));
            // actors.push_back(Utilities::getLineActor({0, 0, 0},axes.angulation * 6, {1, 0, 0}));
            // actors.push_back(Utilities::getLineActor({0, 0, 0}, axes.horizontal * 6, {1, 0, 0}));
            // actors.push_back(Utilities::getPolyDataActor(toothData));
            // Utilities::DisplayActors(actors);
        }
    }

    void alignTeethTorks(std::unordered_map<unsigned short, Model::Tooth>& teethMap) {
        std::array<double, 49> angles{}, anglesToAppy {};
        constexpr std::array<int, 14> lowerIDs {47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};

        for (const int toothID: lowerIDs) {
            const Model::Tooth &tooth = teethMap.at(toothID);
            Model::ToothAxis axes = tooth.getAxis();

            const double signH = axes.horizontal.x() >= 0 ? 1.0 : -1.0, signV = axes.vertical.y() >= 0 ? 1.0 : -1.0;
            const Eigen::Vector3d verticalOrig {0, signV, 0}, horizontalOrig{signH, 0, 0};
            const Eigen::Matrix3d matrix {Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

            axes = axes * matrix;

            const short sign = axes.vertical.z() >= 0 ? 1 : -1;
            axes.vertical.x() = 0;
            angles[toothID] = sign * Utils::getAngle(axes.vertical, verticalOrig);
            // std::cout << "[" << toothID << "]: angle = " << angles[toothID] << std::endl;
        }

        const double agvMolarsAngle = (angles[47] + angles[46] + angles[36] + angles[37])/ 4.0;
        // std::cout << "agvMolarsAngle --> " << agvMolarsAngle << std::endl;


        double anglePrev {0.0};

        { // 31, 41
            std::pair<double, double> limits {9.27, 14.61}; // INFO: (11.94 +/- 2.67) for 31, 47
            const double avgLateralIncisorsActual = std::midpoint(angles[32], angles[42]);
            const double avgMedialIncisorsExpected = std::midpoint(limits.first, limits.second);
            const double diff31 = std::abs(avgMedialIncisorsExpected - angles[31]),
                         diff41 = std::abs(avgMedialIncisorsExpected - angles[41]);
            anglePrev = diff31 > diff41 ? angles[41] : angles[31];

            // INFO: ~3 is average diff of torks 41 vs 42
            if (anglePrev > avgLateralIncisorsActual + 3)
                anglePrev = avgMedialIncisorsExpected;

            anglesToAppy[31] = anglePrev - angles[31];
            anglesToAppy[41] = anglePrev - angles[41];

            // std::cout << "[31] = " << anglesToAppy[31] << "\n[41] = " << anglesToAppy[41] << std::endl;
        }

        { // 32, 42
            constexpr std::pair<double, double> limits = {5.65, 10.31};
            const double diff32 = std::abs(anglePrev - angles[32]),
                         diff42 = std::abs(anglePrev - angles[42]);
            anglePrev = diff32 > diff42 ? angles[42] : angles[32];

            anglesToAppy[32] = anglePrev - angles[32];
            anglesToAppy[42] = anglePrev - angles[42];

            // std::cout << "[32] = " << anglesToAppy[32] << "\n[42] = " << anglesToAppy[42] << std::endl;
        }

        { // 33, 43
            constexpr std::pair<double, double> limits = {5.65, 10.31};
            const double avgLateralIncisorsExpected = std::midpoint(limits.first, limits.second);
            const double avgFangsExpected = std::midpoint(angles[34] + angles[44], avgLateralIncisorsExpected);

            anglesToAppy[33] = avgFangsExpected - angles[33];
            anglesToAppy[43] = avgFangsExpected - angles[43];

            // std::cout << "[33] = " << anglesToAppy[33] << "\n[43] = " << anglesToAppy[43] << std::endl;
        }

        // TODO: Very BAD BAD way to transform teeth (apply torks)
        for (const int toothID: lowerIDs) {
            Model::Tooth &tooth = teethMap.at(toothID);
            Model::ToothAxis& axes = tooth.axes;
            const Eigen::Vector3d& centerOrigin { tooth.GetCenter() };

            const double signH = axes.horizontal.x() >= 0 ? 1.0 : -1.0;
            const Eigen::Vector3d horizontalOrig{signH, 0, 0};
            const Eigen::Matrix3d matrix {Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

            auto M  {Utilities::cast3x3MatrixToVTK4x4(matrix)};
            tooth.toothPolyData->DeepCopy(
                    Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0));
            tooth.toothPolyData->DeepCopy(
                    Utilities::rotatePolyData(tooth.toothPolyData,M));
            tooth.toothPolyData->DeepCopy(
                    Utilities::rotatePolyData(tooth.toothPolyData, anglesToAppy[toothID], 0, 0));
            M->Transpose();
            tooth.toothPolyData->DeepCopy(
                    Utilities::rotatePolyData(tooth.toothPolyData,M));
            tooth.toothPolyData->DeepCopy(
                    Utilities::setPolyDataCenter(tooth.toothPolyData, centerOrigin));

            {
                const double sinX = std::sin(Utils::degToRad(anglesToAppy[toothID]));
                const double cosX = std::cos(Utils::degToRad(anglesToAppy[toothID]));
                Eigen::Matrix3d matX {
                        {1, 0,     0},
                        {0, cosX, -sinX},
                        {0, sinX,  cosX}
                };
                axes = tooth.axes * matX;
            }
        }

        // INFO: Just to print angles once again
        for (const int toothID: lowerIDs) {
            const Model::Tooth &tooth = teethMap.at(toothID);
            Model::ToothAxis axes = tooth.getAxis();

            const double signH = axes.horizontal.x() >= 0 ? 1.0 : -1.0, signV = axes.vertical.y() >= 0 ? 1.0 : -1.0;
            const Eigen::Vector3d verticalOrig {0, signV, 0}, horizontalOrig{signH, 0, 0};
            const Eigen::Matrix3d matrix {Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

            axes = axes * matrix;

            const short sign = axes.vertical.z() >= 0 ? 1 : -1;
            axes.vertical.x() = 0;
            const double angle = sign * Utils::getAngle(axes.vertical, verticalOrig);
            // std::cout << "[" << toothID << "]: angle = " << angle << std::endl;
        }

    }

    void VisualizeTeeth(const std::unordered_map<unsigned short, Model::Tooth>& teethMap) {
        std::vector<vtkSmartPointer<vtkProp3D>> actors;
        constexpr std::array<int, 14> lowerIDs { 47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37 };
        for (const int toothID: lowerIDs)
        {
            const Model::Tooth& tooth = teethMap.at(toothID);
            actors.emplace_back(Utilities::getPolyDataActor(tooth.toothPolyData));
        }
        Utilities::DisplayActorsEx(actors);
    }

    void VisualizeTeethSingleLine(const std::unordered_map<unsigned short, Model::Tooth>& teethMap) {
        std::unordered_map<int, Eigen::Vector3d> pointsMap;
        std::vector<vtkSmartPointer<vtkProp3D>> actors;
        std::array<double, 32> torkAngles {};

        constexpr std::array<int, 14> lowerIDs { 47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37 };
        double xPos = 0, prevToothLength = 0;

        for (const int toothID: lowerIDs)
        {
            const Model::Tooth& tooth = teethMap.at(toothID);
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
                short sign = axes.vertical.z() >= 0 ? 1 : -1;
                // std::cout << toothID << ": [" << axes.vertical.x() << ", "
                //           << axes.vertical.y() << ", " << axes.vertical.z() << "]\n";

                axes.vertical.x()  = 0;
                torkAngles[toothID] = sign * Utils::getAngle(axes.vertical, verticalOrig);
                // std::cout << "[" << toothID << "]: angle = " << torkAngles[toothID] << std::endl;
            }

            {   // Calculate edge point:
                const Model::Tooth toothLocal {toothData, static_cast<FDI::IdType>(toothID)};
                const Eigen::Vector3d& center { toothLocal.GetCenter() };
                const std::array<double, 6>& bounds { toothLocal.GetBounds() };
                const double height{bounds[3] - bounds[2]};

                auto [iter, ok] =
                        pointsMap.emplace(toothID, Eigen::Vector3d{center.x(), center.y(), center.z()});
                Eigen::Vector3d& edgePoint = iter->second;
                edgePoint.y() += FDI::isLowerTooth(toothID) ? height / 2 : -height / 2;

                // std::cout << toothID << ": [" << edgePoint.y() << "]" << std::endl;
                actors.emplace_back(Utilities::getPointsActor({edgePoint}, {0, 1, 0}));
            }

            {   // Add toothID text on screen:
                const vtkSmartPointer<vtkTextActor3D> textActor { vtkTextActor3D::New() };
                textActor->SetPosition(ptCenter.x() - tooth.mesiodistalWidth / 2 + 2, ptCenter.y() - 15, ptCenter.z());
                textActor->SetInput(std::to_string(toothID).c_str());

                const vtkSmartPointer<vtkTextProperty> prop = textActor->GetTextProperty();
                prop->SetFontSize(6);

                actors.emplace_back(textActor);
            }

            actors.emplace_back(Utilities::getLineActor(ptCenter, axes.vertical + ptCenter , {1, 0, 0}));
            actors.emplace_back(Utilities::getLineActor(ptCenter, axes.horizontal + ptCenter, {1, 1, 0}));
            actors.emplace_back(Utilities::getLineActor(ptCenter, verticalOrig + ptCenter, {1, 1, 0}));
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


        Utilities::DisplayActorsEx(actors);
    }

    void AlignTeeth(const std::vector<std::string_view>& params) {
        // 2280 2880 2425 2457 2494 2630 2929
        const std::string id = params.empty() ? "2280" : std::string(params.front());
        const std::string planJsonFile = R"(/home/andtokm/Projects/data/cases/)" + id + R"(/Plan.json)";
        const std::string filePath = R"(/home/andtokm/Projects/data/cases/)" + id
                + R"(/automodeling/out/)" + id + "_teeth.obj"s;

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        // TODO: Check for performance: pass polyData&& (as RValue ref) ???
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&& [toothId, polyData]: toothMap) {
            if (auto [iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
                estimator->estimate(iter->second);
            }
        }

        Utils::orientTeethToPlane(allTeeth);
        // VisualizeTeethSingleLine(allTeeth);

        alignTeethAngulation(allTeeth);
        alignTeethTorks(allTeeth);
        alignTeethVertical(allTeeth);


        VisualizeTeeth(allTeeth);
        // VisualizeTeethSingleLine(allTeeth);
    }

    void VisualizeTeethAngles_Original(const std::vector<std::string_view>& params) {
        // 2280 2880 2425 2457 2494 2630 2929
        const std::string id = params.empty() ? "2280" : std::string(params.front());
        const std::string planJsonFile = R"(/home/andtokm/Projects/data/cases/)" + id + R"(/Plan.json)";
        const std::string filePath = R"(/home/andtokm/Projects/data/cases/)" + id
                                     + R"(/automodeling/out/)" + id + "_teeth.obj"s;

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        // TODO: Check for performance: pass polyData&& (as RValue ref) ???
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&& [toothId, polyData]: toothMap) {
            if (auto [iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
                estimator->estimate(iter->second);
            }
        }

        Utils::displayTeethAndAxis(allTeeth, true, false);
    }
};

// INFO: Bad axes for case 2929

void SetUpTorksAndPositions::TestAll(const std::vector<std::string_view>& params) {
    AlignTeeth(params);

    // VisualizeTeethAngles_Original(params);
};

