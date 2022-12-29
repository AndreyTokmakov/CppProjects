//============================================================================
// Name        : Tooth.cpp
// Created on  : 10.12.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Tooth
//============================================================================

#include "Tooth.h"
#include "../VtkHeaders.h"
#include "../Utilities/Utilities.h"

// TODO: Move to the separate file
namespace Global {

    vtkSmartPointer<vtkMatrix3x3> PreparaOrientationMatrix() {
        /** Prepare the horizontal matrix: **/
        vtkSmartPointer<vtkTransform> transform{vtkTransform::New()};
        transform->RotateX(90);
        transform->RotateY(180);

        return Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
    }

    const vtkSmartPointer<vtkMatrix3x3> orientationMatrix = PreparaOrientationMatrix();
}



namespace Model
{
    Tooth::Tooth(const vtkSmartPointer<vtkPolyData>& toothData, FDI::IdType id) noexcept:
            toothPolyData { toothData }, toothId { id } {
        // do something??
    }

    ToothAxis& Tooth::setAxis(const std::vector<double> &planAxes) {
        this->axes = prepareToothAxes(planAxes);
        return this->axes;
    }

    [[nodiscard("Make sure to handle return value")]]
    ToothAxis Tooth::prepareToothAxes(const std::vector<double>& planAxes) const noexcept {
        const Eigen::Array<double, 9, 1> axesVector (planAxes.data());
        // TODO: Move this matrix to global-config??
        const Eigen::Matrix3d rotationMatrix {Global::orientationMatrix->GetData()};

        //  TODO: Check for transpose(). Do we need it here?
        /** Transform original axis (pt1, pt2, pt3) using the horizontal matrix: **/
        const auto matrix = rotationMatrix.transpose() ;
        return ToothAxis {
                matrix * static_cast<Eigen::VectorXd>(axesVector.segment(0, 3)),
                matrix * static_cast<Eigen::VectorXd>(axesVector.segment(3, 3)),
                matrix * static_cast<Eigen::VectorXd>(axesVector.segment(6, 3))
        };
#if 0
        // Same as above, just using VTK instead Eigen library:
            const double* transData = rotateMatrix.data();
            vtkMatrix3x3::MultiplyPoint(transData, axesVector.segment(0, 3).data(), axes.horizontal.data());
            vtkMatrix3x3::MultiplyPoint(transData, axesVector.segment(3, 3).data(), axes.angulation.data());
            vtkMatrix3x3::MultiplyPoint(transData, axesVector.segment(6, 3).data(), axes.vertical.data());
#endif
    }
}