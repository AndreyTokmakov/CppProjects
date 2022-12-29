//============================================================================
// Name        : Estimators.cpp
// Created on  : 21.12.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Estimators
//============================================================================

#include "Estimator.h"
#include "../Utilities/Utilities.h"

#include "../VtkHeaders.h"


namespace Estimators {

    // NOTE: We assume that Axes are already set for the tooth
    void DistanceEstimator::estimate(Model::Tooth& tooth) noexcept
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

#if 0
        // FIXME: This is the DEBUG part: Can be removed
        std::cout << tooth.getToothId() << " = " << bounds[1] - bounds[0] << std::endl;
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

            Utilities::DisplayActors({toothActor, outlineActor, vertical, horizontal, frontAxe});
        }
#endif
    }
}