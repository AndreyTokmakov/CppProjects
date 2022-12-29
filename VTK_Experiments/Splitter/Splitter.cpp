//============================================================================
// Name        : Splitter.cpp
// Created on  : 23.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Splitter
//============================================================================

#include "../VtkHeaders.h"
#include "../Types.h"
#include "../Utilities/Utilities.h"
#include "Splitter.h"

#include <iostream>
#include <string_view>

namespace Splitter
{
    const vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors> ::New() };

    void SplitSphereWithPlane()
    {
        const vtkSmartPointer<vtkSphereSource> sphere { vtkSmartPointer<vtkSphereSource>::New() };
        sphere->SetRadius(6);
        sphere->SetCenter(-8,0,-14);
        sphere->Update();
        const vtkSmartPointer<vtkPolyData> spherePolyData = sphere->GetOutput();

        Types::Point<3, double> center;
        sphere->GetCenter(center.data());

        const vtkSmartPointer<vtkPlane> plane { vtkSmartPointer<vtkPlane>::New() };
        plane->SetNormal(-1.0, 0.0, 0.0);
        // plane->SetNormal(0.0, 1.0, 0.0);
        plane->SetOrigin(center.data());

        vtkSmartPointer<vtkClipPolyData> clip  { vtkSmartPointer<vtkClipPolyData>::New()};
        clip->SetInputData(spherePolyData);
        clip->SetValue(0.0);
        clip->GenerateClippedOutputOn();
        clip->SetClipFunction (plane);
        clip->Update();

        vtkSmartPointer<vtkPolyData> resultData = clip->GetOutput();
        auto cuttedActor = Utilities::getPolyDataActor(resultData);
        cuttedActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());

        Utilities::DisplayActors({ cuttedActor});
    }


    void SplitToothWithPlane()
    {
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/out/Tooths/tooth_4.stl)" };
        const vtkSmartPointer<vtkPolyData> toothPolyData = Utilities::readStl(filePath);

        Types::Point<3, double> center;
        toothPolyData->GetCenter(center.data());

        center[1] += 4;

        const vtkSmartPointer<vtkPlane> plane { vtkSmartPointer<vtkPlane>::New() };
        plane->SetNormal(0.0, 0.8, 0.0);
        plane->SetOrigin(center.data());

        vtkSmartPointer<vtkClipPolyData> clip  { vtkSmartPointer<vtkClipPolyData>::New()};
        clip->SetInputData(toothPolyData);
        clip->SetValue(0.0);
        clip->GenerateClippedOutputOn();
        clip->SetClipFunction (plane);
        clip->Update();

        auto toothActor = Utilities::getPolyDataActor(toothPolyData);
        toothActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Green").GetData());

        vtkSmartPointer<vtkPolyData> resultData = clip->GetOutput();
        auto cuttedActor = Utilities::getPolyDataActor(resultData);
        cuttedActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());

        // Utilities::DisplayActors({ cuttedActor}, colors);

        Utilities::visualize(resultData, true);
    }
}

void Splitter::TestAll()
{
    Splitter::SplitToothWithPlane();
    // Splitter::SplitSphereWithPlane();
};
