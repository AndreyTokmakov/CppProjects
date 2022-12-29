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

    vtkSmartPointer<vtkMatrix3x3> PrepareOrientationMatrix() {
        /** Prepare the horizontal matrix: **/
        vtkSmartPointer<vtkTransform> transform { vtkSmartPointer<vtkTransform>::New() };
        transform->RotateX(90);
        transform->RotateY(180);

        return Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
    }

    // VTK 3x3 Matrix
    const vtkSmartPointer<vtkMatrix3x3> orientationMatrix { PrepareOrientationMatrix() };

    // TODO: Move this matrix to global-config??
    const Eigen::Matrix3d rotationMatrix {Global::orientationMatrix->GetData()};
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
        // TODO: Assert planAxes size()
        const Eigen::Array<double, 9, 1> axesVector (planAxes.data());

        //  TODO: Check for transpose(). Do we need it here?
        /** Transform original axis (pt1, pt2, pt3) using the matrix: **/
        const auto matrix = Global::rotationMatrix.transpose() ;
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

    [[nodiscard("Make sure to handle return value")]]
    Eigen::Vector3d Tooth::GetCenter() const noexcept {
        Eigen::Vector3d center {};
        this->toothPolyData->GetCenter(center.data());
        return center;
    }

    [[nodiscard("Make sure to handle return value")]]
    std::array<double, 6> Tooth::GetBounds() const noexcept {
        std::array<double, 6> bounds {};
        this->toothPolyData->GetBounds(bounds.data());
        return bounds;
    }
}