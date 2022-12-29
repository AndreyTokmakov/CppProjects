//============================================================================
// Name        : CovnexHull.cpp
// Created on  : 23.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : CovnexHull
//============================================================================

#include "../VtkHeaders.h"
#include "../Types.h"
#include "../Utilities/Utilities.h"
#include "CovnexHull.h"

#include <iostream>
#include <string_view>

namespace CovnexHull
{
    // vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };

    void SingleTooth_ConvexHull_Test() {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_6.stl)"};
        const auto data = Utilities::readStl(filePath);

        vtkSmartPointer<vtkDelaunay3D> delaunay = vtkSmartPointer<vtkDelaunay3D>::New();
        delaunay->SetInputData( data );
        delaunay->Update();

        vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
        surfaceFilter->SetInputConnection(delaunay->GetOutputPort());
        surfaceFilter->Update();

        Utilities::visualize(surfaceFilter->GetOutput());
        Utilities::visualize(data);
    }
}

void CovnexHull::TestAll()
{
    CovnexHull::SingleTooth_ConvexHull_Test();
};


