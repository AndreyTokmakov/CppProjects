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
    // FIXME: We have a bug here
    void ToothAxis::rotate(double x, double y, double z) {
        const vtkSmartPointer<vtkTransform> transform {vtkTransform::New()};
        transform->RotateX(x);
        transform->RotateY(y);
        transform->RotateZ(z);

        Eigen::Matrix3d matrix {Utilities::cast4x4MatrixTo3x3(transform->GetMatrix())->GetData()};
        matrix = matrix.transpose();

        horizontal = matrix * horizontal;
        vertical = matrix * vertical;
        angulation = matrix * angulation;
    }

    // TODO: Fix bug axes * matrix --> matrix * axes
    ToothAxis operator*(const ToothAxis& axes,
                        const Eigen::Matrix3d& matrix) noexcept {
        return ToothAxis {
                matrix * axes.horizontal,
                matrix * axes.angulation,
                matrix * axes.vertical,
        };
    }

    ToothAxis operator*(const ToothAxis& axes,
                        const float x) noexcept {
        return ToothAxis {
            axes.horizontal * x,
            axes.angulation * x,
            axes.vertical * x,
        };
    }
}
