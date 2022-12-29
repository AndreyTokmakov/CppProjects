//============================================================================
// Name        : SetUpTeethSegments.h
// Created on  : 28.01.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : SetUpTeethSegments
//============================================================================

#include "SetUpTeethSegments.h"


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

#include "SetUpTorksAndPositions.h"

#include "Utils.h"

namespace SetUpTeethSegments
{
    // NOTE: We assume that Axes are already set for the tooth
    void estimate(Model::Tooth& tooth) noexcept
    {
        Model::ToothAxis axes = tooth.getAxis();

        const short signH = tooth.axes.horizontal.x() >= 0 ? 1 : -1;
        const short signV = tooth.axes.vertical.y() >= 0 ? 1 : -1;
        const Eigen::Vector3d verticalOrig {0, static_cast<double>(signV), 0};
        const Eigen::Vector3d horizontalOrig {static_cast<double>(signH), 0, 0};

        vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);
        Eigen::Matrix3d matrix {Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

        // FIXME: Fix Model::ToothAxis::operator*(): M * N != N * M
        axes = axes * matrix;

        vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
        toothData = Utilities::rotatePolyData(toothData, vtkMatrix);
        matrix = Eigen::Quaterniond::FromTwoVectors(axes.vertical, verticalOrig).toRotationMatrix();

        // FIXME: Fix Model::ToothAxis::operator*(): M * N != N * M
        axes = axes * matrix;

        vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
        toothData = Utilities::rotatePolyData(toothData, vtkMatrix);

        std::array<double, 6> bounds {};
        toothData->GetBounds(bounds.data());
        tooth.setMesiodistalWidth(bounds[1] - bounds[0]);

        Eigen::Vector3d center {};
        toothData->GetCenter(center.data());

#if 1
        // FIXME: This is the DEBUG part: Can be removed
        std::cout << tooth.getToothId() << " = " << bounds[1] - bounds[0] << std::endl;
        std::vector<vtkSmartPointer<vtkActor>> actors;
        {
            vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors>::New() };
            auto vertical = Utilities::getLineActor({0, 0, 0}, axes.vertical * 7);
            auto frontAxe = Utilities::getLineActor({0, 0, 0}, axes.angulation * 7);
            auto horizontal = Utilities::getLineActor({0, 0, 0}, axes.horizontal * 7);
            auto horizontalOrigActor = Utilities::getLineActor({0, 0, 0}, horizontalOrig * 7);

            horizontal->GetProperty()->SetDiffuseColor(colors->GetColor3d("Red").GetData());
            horizontalOrigActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());

            const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothData);
            const vtkSmartPointer<vtkActor> outlineActor = Utilities::getOutlineActor(toothData);


            {
                Eigen::Vector3d centerNew {center.x(), center.y(), center.z()};
                if (FDI::isLowerTooth(tooth.getToothId())) {
                    centerNew.y() = bounds[3] - 1;
                } else {
                    centerNew.y() = bounds[2] + 1;
                }

                vtkNew<vtkPlane> plane;
                plane->SetOrigin(centerNew.data());
                plane->SetNormal(axes.vertical.data());

                vtkNew<vtkCutter> cutter;
                cutter->SetCutFunction(plane);
                cutter->SetInputData(toothData);
                cutter->Update();
                vtkSmartPointer<vtkPolyData> cutterData = cutter->GetOutput();

                std::array<double, 6> bnds {};
                cutterData->GetBounds(bnds.data());
                Eigen::Vector3d centerX {0, std::midpoint(bnds[2], bnds[3]) - 1.0,
                                         std::midpoint(bnds[4], bnds[5])};

                const double xLength { bounds[1] - bounds[0] }, yLength { bounds[3] - bounds[2] };
                const short direction = FDI::isLowerTooth(tooth.getToothId()) ? 1 : -1;
                Eigen::Vector3d left { center.x() - xLength / 2, center.y() + direction * yLength/ 2, centerX.z() };
                Eigen::Vector3d right { center.x() + xLength / 2, center.y() + direction * yLength/ 2, centerX.z() };

                actors.push_back(Utilities::getPointsActor({left, right}, {0, 1, 0}));
                actors.push_back(Utilities::getLineActor(left, right, {0, 1, 0}));

                /*
                vtkNew<vtkPolyDataMapper> cutterMapper;
                cutterMapper->SetInputData(cutterData);
                cutterMapper->ScalarVisibilityOff();

                vtkNew<vtkActor> planeActor;
                planeActor->GetProperty()->SetColor(std::array<double, 3>{0, 0, 1}.data());
                planeActor->GetProperty()->SetLineWidth(3);
                planeActor->SetMapper(cutterMapper);

                actors.emplace_back(planeActor);
                actors.push_back(Utilities::getPointsActor({centerX}, {0, 1, 0}));
                */
            }

            actors.push_back(toothActor);
            // actors.push_back(outlineActor);
            // actors.push_back(vertical);
            // actors.push_back(horizontal);
            // actors.push_back(frontAxe);
            Utilities::DisplayActors(actors);
        }
#endif
    }

    void Estimate_Segments_Test() {
        constexpr std::string_view planJsonFile{R"(/home/andtokm/Projects/data/cases/2280/Plan.json)"};
        constexpr std::string_view filePath{
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)"};

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&&[toothId, polyData]: toothMap) {
            if (auto[iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
                estimate(iter->second);
            }
        }
    }

    //============================================================================================

    void Vis_Segments()
    {
        constexpr std::string_view planJsonFile{R"(/home/andtokm/Projects/data/cases/2280/Plan.json)"};
        constexpr std::string_view filePath{
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)"};

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        std::unique_ptr<Estimators::Estimator> estimator {std::make_unique<Estimators::DistanceEstimator>()};
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&&[toothId, polyData]: toothMap) {
            if (auto[iter, ok] =
                    allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
                estimator->estimate(iter->second);
            }
        }

        Utils::orientTeethToPlane(allTeeth);
        SetUpTorksAndPositions::alignTeethTorks(allTeeth);
        SetUpTorksAndPositions::alignTeethVertical(allTeeth);


        /*
        vtkSmartPointer<vtkTransform> transform{vtkSmartPointer<vtkTransform>::New()};
        transform->RotateX(90);
        transform->RotateY(180);

        Eigen::Matrix3d rotationMatrix { transform->GetMatrix()->GetData()};
        rotationMatrix.transpose();

        for (auto &[toothID, tooth]: allTeeth) {
            tooth.toothPolyData->DeepCopy();
            //tooth.axes = tooth.axes * rotationMatrix;
        }
         */


        // const Eigen::Vector3d &leftMolarCenter { allTeeth.at(37).GetCenter() },
        //                       &rightMolarCenter { allTeeth.at(47).GetCenter() };
        const std::array<double, 6>& leftMolarBounds { allTeeth.at(37).GetBounds() },
                                   & rightMolarBounds { allTeeth.at(47).GetBounds() };

        const Eigen::Vector3d bottomLeftPt {leftMolarBounds[1], leftMolarBounds[3], leftMolarBounds[4]};
        const Eigen::Vector3d bottomRightPt {rightMolarBounds[0], rightMolarBounds[3], rightMolarBounds[4]};


        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (const auto&[toothID, tooth]: allTeeth)
        {
            if (FDI::isUpperTooth(toothID))
                continue;

            const vtkSmartPointer<vtkPolyData> toothData = tooth.toothPolyData;
            const Eigen::Vector3d& center { tooth.GetCenter()};
            const std::array<double, 6>& bounds {tooth.GetBounds()};

            const Model::ToothAxis& axes = tooth.axes;
            Eigen::Vector3d start = center - axes.horizontal,
                            end = center + axes.horizontal;

            const auto [slopeY, interceptY] =
                    Utils::getLineCoefficients({start.x(), start.y()}, {end.x(), end.y()});
            const auto [slopeZ, interceptZ] = Utils::getLineCoefficients({start.x(), start.z()}, {end.x(), end.z()});

            const double segLength = Utils::twoPointDistance(start, end);
            const double ratio = tooth.mesiodistalWidth / segLength;
            const double xDist = std::abs(start.x() - end.x());

            const double yDiff = 4;

            start.x() = center.x() - ratio * xDist/2;
            end.x() = center.x() + ratio * xDist/2;
            start.y() = start.x() * slopeY + interceptY + yDiff;
            end.y() = end.x() * slopeY + interceptY + yDiff;
            start.z() = start.x() * slopeZ + interceptZ;
            end.z() = end.x() * slopeZ + interceptZ;

            Eigen::Vector3d midPoint {std::midpoint(start.x(), end.x()),
                                      std::midpoint(start.y(), end.y()),
                                      std::midpoint(start.z(), end.z())};

            if ((toothID % 10) < 4)
            {
                constexpr double cutSize { 1.0 };
                Eigen::Vector3d centerNew {center.x(), center.y(), center.z()};
                centerNew.y() = FDI::isLowerTooth(tooth.getToothId()) ? bounds[3] - cutSize : bounds[2] + cutSize;

                const vtkNew<vtkPlane> plane;
                plane->SetOrigin(centerNew.data());
                plane->SetNormal(axes.vertical.data());

                const vtkNew<vtkCutter> cutter;
                cutter->SetCutFunction(plane);
                cutter->SetInputData(toothData);
                cutter->Update();
                vtkSmartPointer<vtkPolyData> cutterData = cutter->GetOutput();

                /*
                vtkNew<vtkPolyDataMapper> cutterMapper;
                cutterMapper->SetInputData(cutterData);
                cutterMapper->ScalarVisibilityOff();

                vtkNew<vtkActor> planeActor;
                planeActor->GetProperty()->SetColor(std::array<double, 3>{0, 0, 1}.data());
                planeActor->GetProperty()->SetLineWidth(3);
                planeActor->SetMapper(cutterMapper);

                actors.emplace_back(planeActor);
                */

                std::array<double, 6> bnds {};
                cutterData->GetBounds(bnds.data());
                const Eigen::Vector3d cuttingEdge {std::midpoint(bnds[0], bnds[1]),
                                                   std::midpoint(bnds[2], bnds[3]) + 1.6,
                                                   std::midpoint(bnds[4], bnds[5])};
                // actors.push_back(Utilities::getPointsActor({cuttingEdge}, {1, 1, 0}));

                const Eigen::Vector3d diff = cuttingEdge - midPoint;
                start += diff;
                end += diff;
            }

            /*
            {
                auto vertical = Utilities::getLineActor(center, center + axes.vertical * 7);
                auto frontAxe = Utilities::getLineActor(center, center + axes.angulation * 7);
                auto horizontal = Utilities::getLineActor(center, center + axes.horizontal * 7);

                actors.push_back(vertical);
                actors.push_back(frontAxe);
                actors.push_back(horizontal);
            }
             */

            actors.push_back(Utilities::getPolyDataActor(toothData));
            actors.push_back(Utilities::getLineActor(start, end, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({start, end }, {1, 0, 0}));
        }

        {
            // actors.push_back(Utilities::getPointsActor({bottomLeftPt, bottomRightPt}));
        }

        Utilities::DisplayActors(actors);

    }
};

void SetUpTeethSegments::TestAll() {
    // Estimate_Segments_Test();

    Vis_Segments();
};
