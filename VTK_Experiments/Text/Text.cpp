//============================================================================
// Name        : Text.cpp
// Created on  : 23.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Points
//============================================================================

#include "../VtkHeaders.h"
#include "../Types.h"
#include "../Utilities/Utilities.h"
#include "Text.h"

#include <iostream>
#include <string_view>

namespace Text
{
    vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };

    vtkSmartPointer<vtkActor> getLineActor(const Types::Line<3>& line,
                                           const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkLineSource> lineSource {vtkLineSource::New()};
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        lineSource->SetPoint1(line.getFirstPoint().data());
        lineSource->SetPoint2(line.getSecondPoint().data()) ;
        lineSource->Update();
        mapper->SetInputData( lineSource->GetOutput() );
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor( 0, 1, 0 );
        actor->GetProperty()->SetLineWidth(4);
        return actor;
    }

    void ShowText() {
        vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };
        vtkSmartPointer<vtkSphereSource> sphereSource { vtkSphereSource::New() };
        sphereSource->SetCenter(0.0, 0.0, 0.0);
        sphereSource->SetRadius(5.0);
        sphereSource->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapper{ vtkPolyDataMapper::New() };
        mapper->SetInputData(sphereSource->GetOutput());

        vtkSmartPointer<vtkActor> actor{ vtkActor::New() };
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

        vtkNew<vtkTextActor> textActor;
        textActor->SetInput("50");
        textActor->SetPosition(50, 50);
        textActor->GetTextProperty()->SetFontSize(24);
        textActor->GetTextProperty()->SetColor(colors->GetColor3d("Red").GetData());

        vtkNew<vtkTextActor> textActor2;
        textActor2->SetInput("75");
        textActor2->SetPosition(120, 50);
        textActor2->GetTextProperty()->SetFontSize(24);
        textActor2->GetTextProperty()->SetColor(colors->GetColor3d("Green").GetData());


        vtkSmartPointer<vtkRenderer> renderer{ vtkRenderer::New() };
        // renderer->AddActor(actor);
        renderer->AddActor2D(textActor);
        renderer->AddActor2D(textActor2);
        renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

        // Create a render window
        vtkNew<vtkRenderWindow> renderWindow;
        renderWindow->AddRenderer(renderer);
        renderWindow->SetWindowName("DrawText");

        // Create an interactor
        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        renderWindowInteractor->SetRenderWindow(renderWindow);

        // Render and interact
        renderWindow->Render();
        renderWindowInteractor->Start();
    }

    void ShowText2() {
        vtkNew<vtkVectorText> textSource;
        textSource->SetText("Hello");

        // Create a mapper
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(textSource->GetOutputPort());

        // Create a subclass of vtkActor: a vtkFollower that remains facing the camera
        vtkNew<vtkFollower> follower;
        follower->SetMapper(mapper);
        follower->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

        // Create a transparent cube that does not move around to face the camera
        vtkNew<vtkCubeSource> cubeSource;
        vtkNew<vtkPolyDataMapper> cubeMapper;
        cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
        vtkNew<vtkActor> cubeActor;
        cubeActor->SetMapper(cubeMapper);
        cubeActor->GetProperty()->SetColor(colors->GetColor3d("MidnightBlue").GetData());
        cubeActor->GetProperty()->SetOpacity(0.3);

        // Create a renderer, render window, and interactor
        vtkNew<vtkRenderer> renderer;
        vtkNew<vtkRenderWindow> renderWindow;
        renderWindow->AddRenderer(renderer);
        renderWindow->SetWindowName("Follower");

        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        dynamic_cast<vtkInteractorStyle*>(renderWindowInteractor->GetInteractorStyle())->AutoAdjustCameraClippingRangeOn();
        renderWindowInteractor->SetRenderWindow(renderWindow);

        // Add the actor to the scene
        renderer->AddActor(follower);
        renderer->AddActor(cubeActor);
        renderer->SetBackground(colors->GetColor3d("LightSlateGray").GetData());
        follower->SetCamera(renderer->GetActiveCamera());

        // Render and interact
        renderWindow->Render();
        renderer->ResetCamera();
        renderWindow->Render();
        renderWindowInteractor->Start();
    }

    void DrawLine_And_ShowText() {
        constexpr size_t fontSize {18};
        Types::Point<3, double> pt1 {0, 10, 0}, pt2 {300, 10, 0};
        const int lineLengthActual = static_cast<int>(pt1.distanceTo(pt2));
        const auto midPoint = Types::Point<3, double>::midPoint(pt1, pt2);
        std::string lineText {std::to_string(lineLengthActual) + " / 1234"};



        const vtkSmartPointer<vtkActor> lineActor = getLineActor({pt1, pt2}, colors);
        const vtkSmartPointer<vtkTextActor3D> textActor3D { vtkTextActor3D::New() };
        textActor3D->SetPosition(midPoint[0] - lineText.size() * fontSize / 4, midPoint[1] + 3, midPoint[2]);
        textActor3D->SetInput(lineText.c_str());
        textActor3D->GetTextProperty()->SetFontSize(fontSize);
        textActor3D->GetTextProperty()->SetColor(colors->GetColor3d("Red").GetData());

        Utilities::DisplayActorsEx({lineActor, textActor3D});
    }

    void DrawLineWithTextSizes() {
        constexpr size_t fontSize {16};
        constexpr size_t expectedSize {123};                       /** INPUT VALUE **/
        Types::Point<3, double> pt1 {0, 10, 0}, pt2 {300, 10, 0};  /** INPUT VALUE **/

        const vtkSmartPointer<vtkActor> lineActor = getLineActor({pt1, pt2}, colors);
        lineActor->GetProperty()->SetColor(colors->GetColor3d("Gray").GetData());

        const int lineLengthActual = static_cast<int>(pt1.distanceTo(pt2));
        std::string lineText {std::to_string(lineLengthActual)};
        auto textPosPt = Types::Point<3, double>::midPoint(pt1, pt2);
        textPosPt[0] -= lineText.size() * fontSize / 4,
                textPosPt[1] += 3;

        const vtkSmartPointer<vtkTextActor3D> textActor1 { vtkTextActor3D::New() };
        textActor1->SetPosition(textPosPt[0], textPosPt[1], textPosPt[2]);
        textActor1->SetInput(lineText.c_str());
        textActor1->GetTextProperty()->SetFontSize(fontSize);
        textActor1->GetTextProperty()->SetColor(colors->GetColor3d("Red").GetData());

        textPosPt[0] += lineText.size()  * fontSize / 2 + 2;

        const vtkSmartPointer<vtkTextActor3D> textActor2 { vtkTextActor3D::New() };
        textActor2->SetPosition(textPosPt[0], textPosPt[1], textPosPt[2]);
        textActor2->SetInput("/");
        textActor2->GetTextProperty()->SetFontSize(fontSize);
        textActor2->GetTextProperty()->SetColor(colors->GetColor3d("Red").GetData());

        textPosPt[0] += 5;

        const vtkSmartPointer<vtkTextActor3D> textActor3 { vtkTextActor3D::New() };
        textActor3->SetPosition(textPosPt[0], textPosPt[1], textPosPt[2]);
        textActor3->SetInput(std::to_string(expectedSize).c_str());
        textActor3->GetTextProperty()->SetFontSize(fontSize);
        textActor3->GetTextProperty()->SetColor(colors->GetColor3d("Green").GetData());

        Utilities::DisplayActorsEx({lineActor, textActor1, textActor2, textActor3});
    }
};

void Text::TestAll()
{
    DrawLineWithTextSizes();
    // DrawLine_And_ShowText();
    // ShowText2();
    // ShowText();
};