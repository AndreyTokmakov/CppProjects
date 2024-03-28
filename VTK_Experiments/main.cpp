//============================================================================
// Name        : main.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : VTK Experiments
//============================================================================

#include <string_view>
#include <vector>
#include <vtkParametricSpline.h>
// #include <vtkParametricSpline.h>

#include "VtkHeaders.h"
#include "Intersection/IntersectionTests.h"
#include "Text/Text.h"
#include "VtkMatrices/VtkMatrices.h"
#include "Utilities/Utilities.h"
#include "Rotation/Rotation.h"
#include "Curvatures/Curvatures.h"
#include "OBBTreeTests/OBBTreeTests.h"
#include "BoundsAndBorders/BoundsAndBorders.h"
#include "CovnexHull/CovnexHull.h"
#include "Holes/Holes.h"
#include "Lines/Lines.h"
#include "Edges/Edges.h"
#include "Isosurface/Isosurface.h"
#include "Cutters/Cutters.h"
#include "Splitter/Splitter.h"
#include "TestsAndExperiments/TestsAndExperiments.h"
#include "Distances/Distances.h"
#include "GeometricFigures/GeometricFigures.h"
#include "Objects_Geometry/PointsAndLines.h"


void Plane () {
    vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors> ::New() };

    // Create a plane
    vtkNew<vtkPlaneSource> planeSource;
    planeSource->SetCenter(1.0, 0.0, 0.0);
    planeSource->SetNormal(1.0, 0.0, 1.0);
    planeSource->Update();

    vtkSmartPointer<vtkPolyData> data {planeSource->GetOutput()};
    vtkSmartPointer<vtkActor> planeActor = Utilities::getPolyDataActor(data);
    planeActor->GetProperty()->SetColor(std::array<double,3> {1, 0, 0}.data());

    Utilities::DisplayActors({planeActor});
}


void Curve () {
    const std::vector<std::array<double, 3>> pts {{
        { 1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 1.0},
        { 1.0, 2.0, 3.0 },
    }};

    vtkSmartPointer<vtkPoints> points { vtkSmartPointer<vtkPoints>::New() };
    for (const auto& pt: pts)
        points->InsertNextPoint(pt.data());

    vtkSmartPointer<vtkParametricSpline> spline { vtkSmartPointer<vtkParametricSpline>::New() };
    spline->SetPoints(points);

    vtkSmartPointer<vtkParametricFunctionSource> functionSource { vtkSmartPointer<vtkParametricFunctionSource>::New() };
    functionSource->SetParametricFunction(spline);
    functionSource->Update();

    const auto actor = Utilities::getPolyDataActor(functionSource->GetOutput());
    Utilities::DisplayActors({actor});
}



int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> params {argv, argv + argc};


    // IntersectionTests::TestAll();
    // Curvatures::TestAll();
    // Splitters::SplitSphereWithPlane();
    // VtkMatrices::TestAll();
    // Rotation::TestAll();
    // PointsAndLines::TestAll();
    // Text::TestAll();
    // OBBTreeTests::TestAll();
    // BoundsAndBorders::TestAll();
    // CovnexHull::TestAll();
    // Holes::TestAll();
    Lines::TestAll();
    // Edges::TestAll();
    // Isosurface::TestAll();
    // Cutters::TestAll();
    // Splitter::TestAll();
    // TestsAndExperiments::TestAll();
    // Distances::TestAll();
    // GeometricFigures::TestAll();

    // Plane();
    // Curve();

    return 0;
}