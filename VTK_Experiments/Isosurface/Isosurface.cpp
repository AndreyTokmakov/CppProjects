//============================================================================
// Name        : Isosurface.cpp
// Created on  : 23.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Isosurface
//============================================================================

#include "../VtkHeaders.h"
#include "../Types.h"
#include "../Utilities/Utilities.h"
#include "Isosurface.h"

#include <iostream>
#include <string_view>

namespace Isosurface
{
    vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };

    void Test() {
        vtkNew<vtkRenderer> ren1;
        vtkNew<vtkRenderWindow> renWin;
        renWin->AddRenderer(ren1);

        vtkNew<vtkRenderWindowInteractor> iren;
        iren->SetRenderWindow(renWin);

        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_6.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        vtkNew<vtkMergePoints> locator;
        locator->SetDivisions(64, 64, 92);
        locator->SetNumberOfPointsPerBucket(2);
        locator->AutomaticOff();

#ifdef USE_FLYING_EDGES
        vtkNew<vtkFlyingEdges3D> iso;
#else
        vtkNew<vtkMarchingCubes> iso;
#endif
        iso->SetInputData(polyData);
        iso->ComputeGradientsOn();
        iso->ComputeScalarsOff();
        iso->SetValue(0, 1150);
#ifndef USE_FLYING_EDGES
        iso->SetLocator(locator);
#endif

        vtkNew<vtkPolyDataMapper> isoMapper;
        isoMapper->SetInputConnection(iso->GetOutputPort());
        isoMapper->ScalarVisibilityOff();

        vtkNew<vtkActor> isoActor;
        isoActor->SetMapper(isoMapper);
        isoActor->GetProperty()->SetColor(colors->GetColor3d("Ivory").GetData());

        vtkNew<vtkOutlineFilter> outline;
        outline->SetInputData(polyData);

        vtkNew<vtkPolyDataMapper> outlineMapper;
        outlineMapper->SetInputConnection(outline->GetOutputPort());

        vtkNew<vtkActor> outlineActor;
        outlineActor->SetMapper(outlineMapper);

        // Add the actors to the renderer, set the background and size
        //
        ren1->AddActor(outlineActor);
        ren1->AddActor(isoActor);
        ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
        ren1->GetActiveCamera()->SetFocalPoint(0, 0, 0);
        ren1->GetActiveCamera()->SetPosition(0, -1, 0);
        ren1->GetActiveCamera()->SetViewUp(0, 0, -1);
        ren1->ResetCamera();
        ren1->GetActiveCamera()->Dolly(1.5);
        ren1->ResetCameraClippingRange();

        renWin->SetSize(640, 480);
        renWin->SetWindowName("HeadBone");

        renWin->Render();
        iren->Start();
    }
}

void Isosurface::TestAll()
{
    Test();
};




