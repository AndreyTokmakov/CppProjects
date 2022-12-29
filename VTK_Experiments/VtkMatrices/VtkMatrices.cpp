//============================================================================
// Name        : VtkMatrices.h
// Created on  : 23.11.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : VtkMatrices
//============================================================================

#include "../VtkHeaders.h"
#include "VtkMatrices.h"


#include <Core>
#include <Geometry>


namespace VtkMatrices {

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkMatrix3x3> cast4x4MatrixTo3x3(const vtkSmartPointer<vtkMatrix4x4>& matrix) {
        vtkSmartPointer<vtkMatrix3x3> matrix3x3 { vtkSmartPointer<vtkMatrix3x3>::New() };
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                matrix3x3->SetElement(row, col, matrix->GetElement(row, col));
            }
        }
        return matrix3x3;
    }

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkMatrix4x4> cast3x3MatrixToVTK4x4(const Eigen::Matrix3d& matrix){
        vtkSmartPointer<vtkMatrix4x4> vtkMatrix { vtkSmartPointer<vtkMatrix4x4>::New() };
        for (int row = 0; row < 3; ++row)
            for (int col = 0; col < 3; ++col)
                vtkMatrix->SetElement(col, row,  matrix.data()[row * 3 + col]);
        return vtkMatrix;
    }


    void EigenRotationMatrix() {
        const vtkSmartPointer<vtkTransform> transform{vtkTransform::New()};
        transform->RotateX(0);
        transform->RotateY(15);
        transform->RotateZ(76);

        vtkSmartPointer<vtkMatrix3x3> matrix = cast4x4MatrixTo3x3(transform->GetMatrix());
        std::cout << *matrix << std::endl;

        Eigen::Matrix3d rotationMatrix {matrix->GetData()};
        std::cout << rotationMatrix.transpose() << std::endl;
    }

    void Test()
    {
        vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        transform->RotateX(90);

        vtkSmartPointer<vtkMatrix4x4> matrix = transform->GetMatrix();
        std::cout << *matrix << std::endl;

        vtkSmartPointer<vtkMatrix3x3> matrix3x3 {cast4x4MatrixTo3x3 (matrix) };
        std::cout << *matrix3x3 << std::endl;
    }

    void Test2()
    {
        vtkSmartPointer<vtkMatrix4x4> matrix { vtkMatrix4x4::New() };

        matrix->SetElement(0, 3, .2);
        matrix->SetElement(1, 3, .2);
        matrix->SetElement(2, 3, .2);
        matrix->SetElement(3, 3, .2);

        std::cout << *matrix << std::endl;
    }
};

void VtkMatrices::TestAll()
{
    // Test();
    // Test2();

    EigenRotationMatrix();
};
