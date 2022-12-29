//============================================================================
// Name        : BoundsAndBorders.cpp
// Created on  : 23.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Points
//============================================================================

#include "../VtkHeaders.h"
#include "../Types.h"
#include "../Utilities/Utilities.h"
#include "BoundsAndBorders.h"

#include <iostream>
#include <string_view>

namespace BoundsAndBorders
{
    vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };

    using Point3d = Types::Point<3, double>;

    void Draw_BoundingBox()
    {
        // constexpr std::string_view fileName{ R"(/home/andtokm/Projects/data/cases/13758/models/5fd2_scan_crown.obj)" };
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/out/Tooths/tooth_4.stl)" };

        const vtkSmartPointer<vtkPolyData> data = Utilities::readStl(filePath);
        Utilities::visualize(data, true);
    }

    void Draw_BoundingBox_UsingPoints() {
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };
        constexpr std::string_view singleTooth { R"(/home/andtokm/Projects/data/out/13181/Tooth_1.stl)" };

        const vtkSmartPointer<vtkPolyData> toothPolyData = Utilities::readStl(singleTooth);
        // const vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(filePath);
        // const std::vector<vtkSmartPointer<vtkPolyData>> teeth = Utilities::separate_connected_components_parallel_test(polyData);

        // const vtkSmartPointer<vtkPolyData> data = teeth[1];
        const vtkSmartPointer<vtkPolyData> data = toothPolyData;

        vtkBoundingBox boundingBox;
        for( int i = 0; i < data->GetNumberOfPoints(); ++i ){
            boundingBox.AddPoint(data->GetPoint( i ) );
        }
        double bounds[6] = { 0 };
        boundingBox.GetBounds( bounds );

        std::vector<Types::Point<3, double>> pts;
        for( int i = 0; i < 2; ++i ) {
            for( int j = 2; j < 4; ++j ) {
                for( int k = 4; k < 6; ++k ) {
                    pts.push_back({bounds[i], bounds[j], bounds[k]});
                }
            }
        }

        vtkSmartPointer<vtkPolyData> boundsPolydata = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkPoints> boundsPoints = vtkSmartPointer<vtkPoints>::New();
        for( int i = 0 ; i < 8; ++i )
            boundsPoints->InsertNextPoint( pts[i].data() );
        boundsPolydata->SetPoints( boundsPoints );

        vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
        vtkIdType cell[2] = { 0, 1 };
        cells->InsertNextCell( 2, cell );
        cell[0] = 0; cell[1] = 2;
        cells->InsertNextCell( 2, cell );
        cell[0] = 3; cell[1] = 2;
        cells->InsertNextCell( 2, cell );
        cell[0] = 3; cell[1] = 1;
        cells->InsertNextCell( 2, cell );
        cell[0] = 4; cell[1] = 5;
        cells->InsertNextCell( 2, cell );
        cell[0] = 4; cell[1] = 6;
        cells->InsertNextCell( 2, cell );
        cell[0] = 7; cell[1] = 5;
        cells->InsertNextCell( 2, cell );
        cell[0] = 7; cell[1] = 6;
        cells->InsertNextCell( 2, cell );
        cell[0] = 1; cell[1] = 5;
        cells->InsertNextCell( 2, cell );
        cell[0] = 0; cell[1] = 4;
        cells->InsertNextCell( 2, cell );
        cell[0] = 2; cell[1] = 6;
        cells->InsertNextCell( 2, cell );
        cell[0] = 3; cell[1] = 7;
        cells->InsertNextCell( 2, cell );
        boundsPolydata->SetLines( cells );

        vtkSmartPointer<vtkPolyDataMapper> boundsMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
        boundsMapper->SetInputData( boundsPolydata );
        vtkSmartPointer<vtkActor> boundsActor =vtkSmartPointer<vtkActor>::New();
        boundsActor->SetMapper( boundsMapper );
        boundsActor->GetProperty()->SetColor( 1, 0, 0 );

        // Setup the window
        vtkNew<vtkRenderer> renderer;
        vtkNew<vtkRenderWindow> window;
        window->AddRenderer(renderer);
        window->SetSize(1200, 800);
        window->SetPosition(0, 50);
        window->SetWindowName("Outline");

        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        renderWindowInteractor->SetRenderWindow(window);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(data);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

        renderer->AddActor(actor);
        renderer->AddActor(boundsActor);
        renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData()); // Background color white

        // Render and interact
        window->Render();
        renderWindowInteractor->Start();
    }

    void Split_And_TryDrawBoundingBox() {
        // constexpr std::string_view filePath { R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };
        // const vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(filePath);

        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/cases/13758/automodeling/crowns/13758_lower.stl)" };
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        const std::vector<vtkSmartPointer<vtkPolyData>> teeth =
                Utilities::separate_connected_components_parallel_test(polyData);

        Utilities::visualize(teeth, true);
    }

    Point3d GetCentroid(vtkSmartPointer<vtkPolyData> polyData) {
        // Compute the center of mass
        vtkNew<vtkCenterOfMass> centerOfMassFilter;
        centerOfMassFilter->SetInputData(polyData);
        centerOfMassFilter->SetUseScalarsAsWeights(false);
        centerOfMassFilter->Update();

        Point3d pt;
        centerOfMassFilter->GetCenter(pt.data());
        return pt;
    }

    Point3d GetCentroid(vtkAlgorithmOutput* polyData) {
        // Compute the center of mass
        vtkNew<vtkCenterOfMass> centerOfMassFilter;
        centerOfMassFilter->SetInputConnection(polyData);
        centerOfMassFilter->SetUseScalarsAsWeights(false);
        centerOfMassFilter->Update();

        Point3d pt;
        centerOfMassFilter->GetCenter(pt.data());
        return pt;
    }

    void Cutter_Tests_OLD()
    {
        /*
        vtkNew<vtkCubeSource> cube;
        cube->SetXLength(10);
        cube->SetYLength(10);
        cube->SetZLength(10);

        const Point3d center = GetCentroid(cube->GetOutputPort());
        Point3d normal {1,0,0};


        vtkSmartPointer<vtkPlaneSource> planeSource { vtkPlaneSource::New() };
        planeSource->SetCenter( center.coordinates[0],center.coordinates[1],center.coordinates[2]);
        planeSource->SetNormal( normal.coordinates[0],normal.coordinates[1],normal.coordinates[2]);
        planeSource->Update();

        std::vector data {
            cube->GetOutputPort(),
            planeSource->GetOutputPort(),
        };

        Utilities::visualize(data);
        */



        vtkSmartPointer<vtkConeSource> cone { vtkConeSource::New() };
        //cone->SetDirection( 1, 1, 0 );
        cone->Update();

        double boundingBox[6] = { 0 };
        cone->GetOutput()->GetBounds( boundingBox );
        Point3d pt0({boundingBox[0], boundingBox[3], boundingBox[5]});
        Point3d pt1({boundingBox[0], boundingBox[3], boundingBox[4]});
        Point3d pt2({boundingBox[0], boundingBox[2], boundingBox[4]});
        Point3d centerPt = pt0 + pt2;

        // centerPt /= 2; // TODO: implement

        centerPt[0] /= 2;
        centerPt[1] /= 2;
        centerPt[2] /= 2;



        Point3d center, max, mid, min, size;
        vtkSmartPointer<vtkOBBTree> obbTree { vtkOBBTree::New() };
        obbTree->ComputeOBB(cone->GetOutput(),
                            center.data(),
                            max.data(),
                            mid.data(),
                            min.data(),
                            size.data());

        vtkMath::Normalize( center.data() );
        vtkMath::Normalize( max.data() );
        vtkMath::Normalize( mid.data() );
        vtkMath::Normalize( min.data() );

        cout << "center: " << center;
        cout << "max: " << max;
        cout << "mid: " << mid;
        cout << "min: " << min;





        vtkSmartPointer<vtkPlaneSource> planeSource { vtkPlaneSource::New() };
        planeSource->SetCenter( centerPt.data() );
        planeSource->SetNormal( 1, 0, 0 );
        planeSource->Update();


        vtkSmartPointer<vtkPolyDataMapper> planeMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper->SetInputData( planeSource->GetOutput() );
        planeMapper->Update();

        vtkSmartPointer<vtkActor> planeActor { vtkActor::New() };
        planeActor->SetMapper( planeMapper );
        planeActor->GetProperty()->SetColor( 0, 1, 0 );


        vtkNew<vtkPlane> plane;
        plane->SetOrigin( centerPt.data() );
        plane->SetNormal( 1, 0, 0 );

        vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
        cutter->SetCutFunction( plane );
        cutter->SetInputData( cone->GetOutput() );
        cutter->Update();

        cout << "Cutter GetNumberOfPoints: " << cutter->GetOutput()->GetNumberOfPoints() << endl;
        vtkSmartPointer<vtkPolyDataMapper> cutterMapper =vtkSmartPointer<vtkPolyDataMapper>::New();
        cutterMapper->SetInputData( cutter->GetOutput() );

        vtkSmartPointer<vtkActor> cutterActor = vtkSmartPointer<vtkActor>::New();
        cutterActor->SetMapper( cutterMapper );
        cutterActor->GetProperty()->SetColor( 1, 0, 0 );



        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        mapper->SetInputData( cone->GetOutput() );

        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        actor->SetMapper( mapper );

        vtkSmartPointer<vtkRenderer> renderer { vtkRenderer::New() };
        renderer->AddActor(actor);
        ///renderer->AddActor(planeActor);
        renderer->AddActor(cutterActor);
        renderer->SetBackground( 0, 0, 0 );

        vtkSmartPointer<vtkRenderWindow> renderWindow { vtkRenderWindow::New() };
        renderWindow->AddRenderer( renderer );

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor { vtkRenderWindowInteractor::New() };
        renderWindowInteractor->GetInteractorStyle()->SetCurrentRenderer( renderer );
        renderWindowInteractor->SetRenderWindow( renderWindow );

        renderer->ResetCamera();
        renderWindow->Render();
        renderWindowInteractor->Start();
    }
}

void BoundsAndBorders::TestAll()
{
    // BoundsAndBorders::Draw_BoundingBox();
    // BoundsAndBorders::Draw_BoundingBox_UsingPoints();
    // BoundsAndBorders::Split_And_TryDrawBoundingBox();
    // BoundsAndBorders::Cutter_Tests_OLD();
};



