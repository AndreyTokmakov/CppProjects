//============================================================================
// Name        : Holes.cpp
// Created on  : 23.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Holes
//============================================================================

#include "../VtkHeaders.h"
#include "../Types.h"
#include "../Utilities/Utilities.h"
#include "Holes.h"

#include <iostream>
#include <string_view>

namespace Holes
{
    vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };

    void SingleTooth_FillHolesTest() {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_15.stl)"};
        const auto data = Utilities::readStl(filePath);

        vtkNew<vtkFillHolesFilter> fillHoles;
        fillHoles->SetInputData(data);
        fillHoles->SetHoleSize(1000.0);
        fillHoles->Update();

        // Make the triangle winding order consistent
        /*
        vtkNew<vtkPolyDataNormals> normals;
        normals->SetInputConnection(fillHoles->GetOutputPort());
        normals->ConsistencyOn();
        normals->SplittingOff();
        normals->Update();
        normals->GetOutput()->GetPointData()->SetNormals(data->GetPointData()->GetNormals());
        */

        Utilities::visualize(fillHoles->GetOutput());
        Utilities::visualize(data);
    }
}

void Holes::TestAll()
{
    SingleTooth_FillHolesTest();
};

