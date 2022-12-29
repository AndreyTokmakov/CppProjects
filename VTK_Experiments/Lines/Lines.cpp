//============================================================================
// Name        : Lines.cpp
// Created on  : 23.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Lines
//============================================================================

#include "../VtkHeaders.h"
#include "../Utilities/Utilities.h"
#include "Lines.h"

#include <iostream>
#include <string_view>

namespace Lines
{
    const vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors>::New() };


    void DrawLine()
    {
        Eigen::Vector3d pt1 {0, 0, 0}, pt2 {10, 10, 10};
        const vtkSmartPointer<vtkActor> lineActor = Utilities::getLineActor(pt1, pt2);
        Utilities::DisplayActors({ lineActor });
    }
}

void Lines::TestAll()
{
    DrawLine();
};




