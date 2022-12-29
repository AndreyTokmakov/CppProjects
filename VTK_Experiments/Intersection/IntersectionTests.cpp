//============================================================================
// Name        : Utilities.h
// Created on  : 08.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utilities
//============================================================================

#include <iostream>
#include <string_view>
#include <vector>

#include "../VtkHeaders.h"
#include "../Utilities/Utilities.h"
#include "IntersectionTests.h"

namespace IntersectionTests {

    vtkNew<vtkNamedColors> colors;


    vtkSmartPointer<vtkPolyData> MoveTooth(vtkSmartPointer<vtkPolyData> polyData,
                                           const double x, const double y, const double z) {
        const vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        transform->Translate(x, y, z);

        const vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter { vtkTransformPolyDataFilter::New() };
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> rotateData(const vtkSmartPointer<vtkPolyData> polyData,
                                            const double x, const double y, const double z) {
        vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        transform->RotateX(x);
        transform->RotateY(y);
        transform->RotateZ(z);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter { vtkTransformPolyDataFilter::New() };
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();
        return transformFilter->GetOutput();
    }

    void Test() {
        vtkNew<vtkCubeSource> cube;
        cube->SetYLength(.5);

        vtkNew<vtkCylinderSource> cylinder;
        cylinder->SetResolution(32);
        cylinder->SetHeight(2.0);
        cylinder->SetRadius(0.25);
        cylinder->SetCenter(0, .5, 0);

        const auto data = std::vector { cube->GetOutputPort(), cylinder->GetOutputPort() };
        // Utilities::visualize(data);


        vtkNew<vtkBooleanOperationPolyDataFilter> filter;
        filter->SetInputConnection(0, cube->GetOutputPort());
        filter->SetInputConnection(1, cylinder->GetOutputPort());
        filter->SetOperationToIntersection();
        filter->Update();

        vtkSmartPointer<vtkPolyData> out = filter->GetOutput(0);
    }

    void SphereIntersect() {
        vtkNew<vtkSphereSource> obj1;
        obj1->SetCenter(0, 0, 0);
        obj1->SetRadius(30);

        vtkNew<vtkSphereSource> obj2;
        obj2->SetCenter(55, 0, 0);
        obj2->SetRadius(30);

        const auto data = std::vector {obj1->GetOutputPort(), obj2->GetOutputPort() };

        Utilities::visualize(data);


        vtkNew<vtkBooleanOperationPolyDataFilter> filter;
        filter->SetInputData(0, obj1->GetOutput());
        filter->SetInputData(1, obj2->GetOutput());
        // filter->SetTolerance(0.00000002);
        // std::cout << filter->GetTolerance();
        filter->SetOperationToIntersection();
        filter->Update();

        int points = filter->GetOutput()->GetNumberOfPoints();
        std::cout << points << std::endl;

        Utilities::visualize(filter->GetOutput());
    }

    void SphereIntersect_2() {
        vtkNew<vtkSphereSource> sphereSource1;
        sphereSource1->SetCenter(0, 0, 0);
        sphereSource1->SetRadius(30);

        vtkNew<vtkSphereSource> sphereSource2;
        sphereSource2->SetCenter(50, 0, 0);
        sphereSource2->SetRadius(30);

        const auto data = std::vector { sphereSource1->GetOutputPort(), sphereSource2->GetOutputPort() };
        // Utilities::visualize(data);

        vtkNew<vtkIntersectionPolyDataFilter> intersectionPolyDataFilter;
        intersectionPolyDataFilter->SetInputConnection( 0, sphereSource1->GetOutputPort() );
        intersectionPolyDataFilter->SetInputConnection( 1, sphereSource2->GetOutputPort() );
        // intersectionPolyDataFilter->SetInputConnection( 0, sphereSource1->GetOutputPort() );
        // intersectionPolyDataFilter->SetInputConnection( 1, sphereSource2->GetOutputPort() );
        intersectionPolyDataFilter->SplitFirstOutputOn();
        intersectionPolyDataFilter->SplitSecondOutputOn();
        intersectionPolyDataFilter->Update();

        /*
        intersectionPolyDataFilter->SplitFirstOutputOff();
        intersectionPolyDataFilter->SplitSecondOutputOff();

        std::cout << "GetSplitFirstOutput = " << intersectionPolyDataFilter->GetSplitFirstOutput() << std::endl;
        std::cout << "GetSplitSecondOutput = " << intersectionPolyDataFilter->GetSplitSecondOutput() << std::endl;
        */

        Utilities::visualize( intersectionPolyDataFilter->GetOutput(0) );
        // Utilities::visualize( intersectionPolyDataFilter->GetOutputPort() );

    }

    void ImplicitBoolean()
    {
        vtkNew<vtkSphere> sphere1;
        sphere1->SetCenter(1.3, 0, 0);

        vtkNew<vtkSphere> sphere2;
        sphere2->SetCenter(-0.9, 0, 0);

        vtkNew<vtkImplicitBoolean> implicitBoolean;
        implicitBoolean->AddFunction(sphere1);
        implicitBoolean->AddFunction(sphere2);
        implicitBoolean->SetOperationTypeToUnion();
        // implicitBoolean->SetOperationTypeToIntersection();
        // implicitBoolean->SetOperationTypeToDifference();

        // Sample the function
        vtkNew<vtkSampleFunction> sample;
        sample->SetSampleDimensions(50, 50, 50);
        sample->SetImplicitFunction(implicitBoolean);
        double value = 3.0;
        double xmin = -value;
        double xmax = value;
        double ymin = -value;
        double ymax = value;
        double zmin = -value;
        double zmax = value;

        sample->SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax);

        // Create the 0 isosurface
        vtkNew<vtkContourFilter> contours;
        contours->SetInputConnection(sample->GetOutputPort());
        contours->GenerateValues(1, 1, 1);

        Utilities::visualize(contours->GetOutputPort());
    }

    void ToothIntersect()
    {
        constexpr std::string_view fileName { R"(/home/andtokm/Projects/data/out/Tooths/Two_Connected_Tooth_1.stl)"};
        const vtkSmartPointer<vtkPolyData> data = Utilities::readStl(fileName);
        std::vector<vtkSmartPointer<vtkPolyData>> pdVector =
                Utilities::separate_connected_components_parallel_test(data);

        std::cout << pdVector.size() << std::endl;

        const auto toothActor0 = Utilities::getPolyDataActor(pdVector[0]);
        const auto toothActor1 = Utilities::getPolyDataActor(pdVector[1]);

        vtkSmartPointer<vtkIntersectionPolyDataFilter> intersectionPolyDataFilter =
                vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
        intersectionPolyDataFilter->SetInputData(0, pdVector[0]);
        intersectionPolyDataFilter->SetInputData(1, pdVector[1]);
        intersectionPolyDataFilter->Update();

        vtkSmartPointer<vtkActor> intersectionActor =
                Utilities::getPolyDataActor(intersectionPolyDataFilter->GetOutput());
        intersectionActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        intersectionActor->GetProperty()->SetLineWidth(5);

        Utilities::DisplayActors({toothActor0, toothActor1, intersectionActor});
    }

    void ToothIntersect_BooleanFilter()
    {
        constexpr std::string_view fileName { R"(/home/andtokm/Projects/data/out/Tooths/Two_Connected_Tooth_1.stl)"};
        vtkSmartPointer<vtkPolyData> data = Utilities::readStl(fileName);

        data = rotateData(data, 0, 70, 10);
        std::vector<vtkSmartPointer<vtkPolyData>> pdVector =
                Utilities::separate_connected_components_parallel_test(data);

        vtkSmartPointer<vtkPolyData> tooth1 = pdVector[0];
        vtkSmartPointer<vtkPolyData> tooth2 = pdVector[1];

        tooth1 = MoveTooth(tooth1, -0.3, 0, 0);

        // Types::Point<3, double> center;
        // tooth1->GetCenter(center.data());

        vtkNew<vtkBooleanOperationPolyDataFilter> filter;
        filter->SetInputData(0, tooth1);
        filter->SetInputData(1, tooth2);
        // filter->SetTolerance(0.00000002);
        // std::cout << filter->GetTolerance();
        // filter->SetOperationToIntersection();
        filter->Update();

        int points = filter->GetOutput()->GetNumberOfPoints();
        std::cout << points << std::endl;

        const vtkSmartPointer<vtkActor> toothActor0 = Utilities::getPolyDataActor(tooth1);
        const vtkSmartPointer<vtkActor> toothActor1 = Utilities::getPolyDataActor(tooth2);

        Utilities::DisplayActors({toothActor0, toothActor1});
    }

    void MoveTeeth_CheckIntersection()
    {
        constexpr std::string_view fileName { R"(/home/andtokm/Projects/data/out/Tooths/Two_Connected_Tooth_1.stl)"};
        vtkSmartPointer<vtkPolyData> data = Utilities::readStl(fileName);

        data = rotateData(data, 0, 70, 10);
        std::vector<vtkSmartPointer<vtkPolyData>> pdVector =
                Utilities::separate_connected_components_parallel_test(data);

        vtkSmartPointer<vtkPolyData> tooth1 = pdVector[0];
        vtkSmartPointer<vtkPolyData> tooth2 = pdVector[1];

        tooth1 = MoveTooth(tooth1, 0.1, 0, 0);

        std::array<double, 3> center;
        tooth1->GetCenter(center.data());

        vtkSmartPointer<vtkIntersectionPolyDataFilter> intersectionPolyDataFilter =
                vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
        intersectionPolyDataFilter->SetInputData(0, tooth1);
        intersectionPolyDataFilter->SetInputData(1, tooth2);
        intersectionPolyDataFilter->GlobalWarningDisplayOff();
        intersectionPolyDataFilter->Update();

        const int intersectionPoints = intersectionPolyDataFilter->GetNumberOfIntersectionPoints();

        std::vector<vtkSmartPointer<vtkActor>> actors {};
        if (intersectionPoints > 0)
        {
            vtkSmartPointer<vtkActor> intersectionActor =
                    Utilities::getPolyDataActor(intersectionPolyDataFilter->GetOutput());
            intersectionActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
            intersectionActor->GetProperty()->SetLineWidth(5);
            actors.push_back(intersectionActor);
        } else {
            std::cout << "No intersection\n";
        }

        actors.push_back(Utilities::getPolyDataActor(tooth1));
        actors.push_back(Utilities::getPolyDataActor(tooth2));

        Utilities::DisplayActors(actors);
    }

    void vtkIntersectionPolyDataFilter_Test() {
        vtkSmartPointer<vtkSphereSource> sphereSource1 = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource1->SetCenter(0.0, 0.0, 0.0);
        sphereSource1->SetRadius(2.0f);
        sphereSource1->Update();

        vtkSmartPointer<vtkPolyDataMapper> sphere1Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        sphere1Mapper->SetInputConnection( sphereSource1->GetOutputPort() );
        sphere1Mapper->ScalarVisibilityOff();
        vtkSmartPointer<vtkActor> sphere1Actor = vtkSmartPointer<vtkActor>::New();
        sphere1Actor->SetMapper( sphere1Mapper );
        sphere1Actor->GetProperty()->SetOpacity(.3);
        sphere1Actor->GetProperty()->SetColor(1,0,0);

        vtkSmartPointer<vtkSphereSource> sphereSource2 = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource2->SetCenter(1.0, 0.0, 0.0);
        sphereSource2->SetRadius(2.0f);
        vtkSmartPointer<vtkPolyDataMapper> sphere2Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        sphere2Mapper->SetInputConnection( sphereSource2->GetOutputPort() );
        sphere2Mapper->ScalarVisibilityOff();
        vtkSmartPointer<vtkActor> sphere2Actor = vtkSmartPointer<vtkActor>::New();
        sphere2Actor->SetMapper( sphere2Mapper );
        sphere2Actor->GetProperty()->SetOpacity(.3);
        sphere2Actor->GetProperty()->SetColor(0,1,0);
        // Отображение тестового самолета
        vtkSmartPointer<vtkPlaneSource> planeSource = vtkSmartPointer<vtkPlaneSource>::New();
        planeSource->SetOrigin(0.5, 0, 0);
        planeSource->SetNormal(1, 0, 0);
        planeSource->SetResolution(10,10);

        planeSource->Update();
        vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper->SetInputConnection(planeSource->GetOutputPort());
        planeMapper->ScalarVisibilityOff();
        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor>::New();
        planeActor->SetMapper(planeMapper);
        planeActor->GetProperty()->SetOpacity(.5);
        planeActor->GetProperty()->SetColor(0, 0, 1);

        vtkSmartPointer<vtkIntersectionPolyDataFilter> intersectionPolyDataFilter =
                vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
        intersectionPolyDataFilter->SetInputConnection( 0, sphereSource1->GetOutputPort() );
        intersectionPolyDataFilter->SetInputConnection( 1, sphereSource2->GetOutputPort() );
        intersectionPolyDataFilter->Update();

        vtkSmartPointer<vtkPolyDataMapper> intersectionMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        intersectionMapper->SetInputConnection( intersectionPolyDataFilter->GetOutputPort() );
        intersectionMapper->ScalarVisibilityOff();

        vtkSmartPointer<vtkActor> intersectionActor = vtkSmartPointer<vtkActor>::New();
        intersectionActor->SetMapper( intersectionMapper );

        vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
        renderer->AddViewProp(sphere1Actor);
        renderer->AddViewProp(sphere2Actor);
        renderer->AddViewProp(intersectionActor);
        renderer->AddViewProp(planeActor);

        vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->AddRenderer( renderer );

        vtkSmartPointer<vtkRenderWindowInteractor> renWinInteractor =
                vtkSmartPointer<vtkRenderWindowInteractor>::New();
        renWinInteractor->SetRenderWindow( renderWindow );

        renderWindow->Render();
        renWinInteractor->Start();
    }
};

void IntersectionTests::TestAll() {
    // Test();
    // ImplicitBoolean();
    // SphereIntersect();
    // SphereIntersect_2();
    // vtkIntersectionPolyDataFilter_Test();

    // ToothIntersect();
    // ToothIntersect_BooleanFilter();
    MoveTeeth_CheckIntersection();
};