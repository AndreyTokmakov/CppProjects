//============================================================================
// Name        : Model.cpp
// Created on  : 10.12.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Model
//============================================================================

#include "ToothAxis.h"
#include "../VtkHeaders.h"
#include "../Utilities/Utilities.h"

namespace Model
{
    void ToothAxis::rotate(double x, double y, double z) {
        const vtkSmartPointer<vtkTransform> transform {vtkTransform::New()};
        transform->RotateX(x);
        transform->RotateY(y);
        transform->RotateZ(z);

        Eigen::Matrix3d rotationMatrix {Utilities::cast4x4MatrixTo3x3(transform->GetMatrix())->GetData()};
        rotationMatrix = rotationMatrix.transpose();

        horizontal = rotationMatrix * horizontal;
        vertical = rotationMatrix * vertical;
        angulation = rotationMatrix * angulation;
    }
}
