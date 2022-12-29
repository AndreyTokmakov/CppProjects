//============================================================================
// Name        : Edges.cpp
// Created on  : 23.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Edges
//============================================================================

#include "../VtkHeaders.h"
#include "../Types.h"
#include "../Utilities/Utilities.h"
#include "Edges.h"

#include <iostream>
#include <string_view>

namespace Edges
{
    vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };


    void ExtractEdges() {
        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->Update();

        vtkNew<vtkExtractEdges> extractEdges;
        extractEdges->SetInputConnection(sphereSource->GetOutputPort());
        extractEdges->Update();

        // Traverse all of the edges
        // vtkCellArray* lines = extractEdges->GetOutput()->GetLines();
        for (vtkIdType i = 0; i < extractEdges->GetOutput()->GetNumberOfCells(); i++) {
            // extractEdges->GetOutput()->GetCell(i)->GetClassName() << std::endl;
            auto line = dynamic_cast<vtkLine*>(extractEdges->GetOutput()->GetCell(i));
        }

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(extractEdges->GetOutputPort());
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

        Utilities::DisplayActors({ actor });
    }

    void ExtractEdges_Tooth() {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_7.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        vtkNew<vtkExtractEdges> extractEdges;
        extractEdges->SetInputData(polyData);
        // extractEdges->Nu
        extractEdges->Update();

        vtkSmartPointer<vtkPolyData> data {vtkSmartPointer<vtkPolyData>::New()};
        data->DeepCopy(extractEdges->GetOutput());


        const vtkSmartPointer<vtkActor> actor = Utilities::getPolyDataActor(data);
        actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

        Utilities::WriteSTL(data, R"(/home/andtokm/Projects/data/out/Tooths/tooth_7_Edges.stl)");
        Utilities::DisplayActors({ actor });
    }

    void ExtractFeatureEdges_Tooth() {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_16.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        vtkNew<vtkFeatureEdges> featureEdges;
        featureEdges->SetInputData(polyData);
        featureEdges->BoundaryEdgesOn();
        featureEdges->FeatureEdgesOn();
        featureEdges->ManifoldEdgesOn();
        featureEdges->NonManifoldEdgesOn();
        featureEdges->Update();

        const vtkSmartPointer<vtkActor> actorTooth = Utilities::getPolyDataActor(polyData);
        const vtkSmartPointer<vtkActor> actor =
                Utilities::getPolyDataActor(featureEdges->GetOutput());
        actor->GetProperty()->SetLineWidth(5);

        Utilities::DisplayActors({actor, actorTooth});
    }
}

void Edges::TestAll()
{
    // Edges::ExtractEdges();
    // Edges::ExtractEdges_Tooth();
    Edges::ExtractFeatureEdges_Tooth();
};


