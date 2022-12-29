//============================================================================
// Name        : Distances.cpp
// Created on  : 20.12.2021
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Distances
//============================================================================

#include <iostream>
#include <string_view>
#include <chrono>

// Eigen
#include <Core>

#include "../VtkHeaders.h"
#include "../Types.h"
#include "../Utilities/Utilities.h"
#include "Distances.h"

namespace Tests {

    // TODO: refactor
    Eigen::Vector3d toPoint(std::string_view strPoints) {
        const size_t pos1 = strPoints.find(" ");
        if (std::string::npos == pos1)
            return {};

        const size_t pos2 = strPoints.find(" ", pos1 + 1);
        if (std::string::npos == pos2)
            return {};

        Eigen::Vector3d pt { Eigen::Vector3d::Zero () };
        pt[0] = atof(strPoints.substr(0, pos1).data());
        pt[1] = atof(strPoints.substr(pos1 + 1, strPoints.size() - pos1 - 1).data());
        pt[2] = atof(strPoints.substr(pos2 + 1, strPoints.size() - pos2 - 1).data());
        return pt;
    }

    // TODO: refactor
    std::array<int, 3> toCell(std::string_view strPoints) {
        const size_t pos1 = strPoints.find(" ");
        if (std::string::npos == pos1)
            return {};

        const size_t pos2 = strPoints.find(" ", pos1 + 1);
        if (std::string::npos == pos2)
            return {};

        std::array<int, 3> pt {};
        pt[0] = atoi(strPoints.substr(0, pos1).data());
        pt[1] = atoi(strPoints.substr(pos1 + 1, strPoints.size() - pos1 - 1).data());
        pt[2] = atoi(strPoints.substr(pos2 + 1, strPoints.size() - pos2 - 1).data());
        return pt;
    }

    // TODO: Refactor
    // TODO: Move to Utilities ?????
    // Read .obj file prepared by preprocessing.py module
    std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>>
    ReadTeethObj(std::string_view path) noexcept
    {
        std::ifstream input(path.data());
        std::vector<std::string> lines;
        while(std::getline(input, lines.emplace_back())) { /** Read all file lines. **/ }
        input.close();

        const size_t size {lines.size()};
        constexpr size_t APPROX_DATA_BLOCK_SIZE = 32 * 2;

        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> dataMap;
        std::vector<Eigen::Vector3d> points;
        points.reserve(APPROX_DATA_BLOCK_SIZE);

        for (size_t i {0}; i < size && 'g' == lines[i++][0]; /* skip. */) {
            /** For each tooth: **/
            while (i < size && 'o' == lines[i][0]) {
                const unsigned short toothId = static_cast<unsigned short>(atoi(std::string_view(lines[i]).substr(3, lines[i].size() - 3).data()));
                dataMap.emplace(toothId, vtkPolyData::New());
                ++i;

                for (/* use outer 'i' */; i < size && 'v' == lines[i][0]; ++i) {
                    const std::string_view pointsStr = std::string_view(lines[i]).substr(2, lines[i].size() - 2);
                    points.push_back(toPoint(pointsStr));
                }

                // TODO: Check for MEM LEAK
                vtkSmartPointer<vtkPoints> vtkPoints = vtkPoints::New();
                for (const Eigen::Vector3d& pt: points)
                    vtkPoints->InsertNextPoint(pt[0], pt[1], pt[2]);

                const int pointsCount { static_cast<int>(points.size())};
                vtkSmartPointer<vtkCellArray> vtkCells { vtkSmartPointer<vtkCellArray>::New() };
                for (/* use outer 'i' */; i < size && 'f' == lines[i][0]; ++i) {
                    const std::string_view cellStr = std::string_view(lines[i]).substr(2, lines[i].size() - 2);
                    const auto& [x,y,z] = toCell(cellStr);
                    vtkCells->InsertNextCell({pointsCount + x, pointsCount+ y, pointsCount + z});
                    // vtkCells->InsertNextCell(3, toCellEx(cellStr, pointsCount));
                }

                dataMap[toothId]->SetPoints(vtkPoints);
                dataMap[toothId]->SetPolys(vtkCells);
                points.clear();
            }
        }
        return dataMap;
    }
}

namespace Distances {

    const vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors> ::New() };

    void Display_Single_Tooth() {
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothMap[11]);
        Utilities::DisplayActors({toothActor, toothActor,});
    }

    void Distance_Demo()
    {
        vtkSmartPointer<vtkPolyData> input1, input2;

        vtkSmartPointer<vtkSphereSource> sphereSource1 = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource1->SetCenter(1, 0, 0);
        sphereSource1->SetPhiResolution(21);
        sphereSource1->SetThetaResolution(21);
        sphereSource1->Update();
        input1 = sphereSource1->GetOutput();

        vtkSmartPointer<vtkSphereSource> sphereSource2 = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource2->SetPhiResolution(21);
        sphereSource2->SetThetaResolution(21);
        sphereSource2->Update();
        input2 = sphereSource2->GetOutput();

        vtkSmartPointer<vtkCleanPolyData> clean1 = vtkSmartPointer<vtkCleanPolyData>::New();
        clean1->SetInputData(input1);

        vtkSmartPointer<vtkCleanPolyData> clean2 = vtkSmartPointer<vtkCleanPolyData>::New();
        clean2->SetInputData(input2);

        vtkSmartPointer<vtkDistancePolyDataFilter> distanceFilter = vtkSmartPointer<vtkDistancePolyDataFilter>::New();

        distanceFilter->SetInputConnection( 0, clean1->GetOutputPort() );
        distanceFilter->SetInputConnection( 1, clean2->GetOutputPort() );
        distanceFilter->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection( distanceFilter->GetOutputPort() );
        mapper->SetScalarRange(
                distanceFilter->GetOutput()->GetPointData()->GetScalars()->GetRange()[0],
                distanceFilter->GetOutput()->GetPointData()->GetScalars()->GetRange()[1]);

        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper( mapper );

        vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
        scalarBar->SetLookupTable(mapper->GetLookupTable());
        scalarBar->SetTitle("Distance");
        scalarBar->SetNumberOfLabels(4);

        vtkSmartPointer<vtkRenderer> renderer =vtkSmartPointer<vtkRenderer>::New();
        renderer->SetBackground(colors->GetColor3d("Silver").GetData());//Background color
        renderer->SetBackground2(colors->GetColor3d("Gold").GetData());//Background color
        renderer->GradientBackgroundOn();

        vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
        renWin->AddRenderer( renderer );

        vtkSmartPointer<vtkRenderWindowInteractor> renWinInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
        renWinInteractor->SetRenderWindow( renWin );

        renderer->AddActor( actor );
        renderer->AddActor2D(scalarBar);

        renWin->Render();
        renWinInteractor->Start();
    }

    void DistanceBetween_TwoSpheres_Test()
    {
        vtkSmartPointer<vtkSphereSource> sphereSource1 = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource1->SetCenter(-1.15, 0, 0);
        sphereSource1->SetRadius(1.2f);
        sphereSource1->SetPhiResolution(50);
        sphereSource1->SetThetaResolution(50);
        sphereSource1->Update();
        vtkSmartPointer<vtkPolyData> data1 = sphereSource1->GetOutput();

        vtkSmartPointer<vtkSphereSource> sphereSource2 = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource2->SetCenter(1.25, 0, 0);
        sphereSource2->SetRadius(1.2f);
        sphereSource2->SetPhiResolution(50);
        sphereSource2->SetThetaResolution(50);
        sphereSource2->Update();
        vtkSmartPointer<vtkPolyData> data2 = sphereSource2->GetOutput();

        // TODO: Distance expected ~ 0.1

        const vtkSmartPointer<vtkCleanPolyData> clean1 = vtkSmartPointer<vtkCleanPolyData>::New();
        clean1->SetInputData(data1);

        const vtkSmartPointer<vtkCleanPolyData> clean2 = vtkSmartPointer<vtkCleanPolyData>::New();
        clean2->SetInputData(data2);

        {
            vtkSmartPointer<vtkDistancePolyDataFilter> distanceFilter = vtkSmartPointer<vtkDistancePolyDataFilter>::New();
            distanceFilter->SetInputConnection( 0, clean1->GetOutputPort() );
            distanceFilter->SetInputConnection( 1, clean2->GetOutputPort() );
            distanceFilter->SignedDistanceOff();
            distanceFilter->ComputeSecondDistanceOn();
            distanceFilter->Update();

            double distMin { std::numeric_limits<double>::max() };
            for (vtkDataArray *distArray: {distanceFilter->GetOutput()->GetPointData()->GetScalars(),
                                           distanceFilter->GetOutput()->GetCellData()->GetScalars(),
                                           distanceFilter->GetSecondDistanceOutput()->GetPointData()->GetScalars(),
                                           distanceFilter->GetSecondDistanceOutput()->GetCellData()->GetScalars()})
            {
                const size_t size = distArray->GetNumberOfValues();
                for (size_t i = 0; i < size; ++i) {
                    const double dist = *(distArray->GetTuple(i));
                    distMin = std::min(dist, distMin);
                }
            }
            std::cout << "distMin = " << distMin << std::endl;
        }


        const vtkSmartPointer<vtkActor> tooth1Actor = Utilities::getPolyDataActor(data1);
        const vtkSmartPointer<vtkActor> tooth2Actor = Utilities::getPolyDataActor(data2);
        Utilities::DisplayActors({tooth1Actor, tooth2Actor});
    }


    void DistanceBetweenTwoTooth()
    {
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        const vtkSmartPointer<vtkPolyData> tooth1 { toothMap[11] },  tooth2 { toothMap[21] };


        const vtkSmartPointer<vtkCleanPolyData> clean1 = vtkSmartPointer<vtkCleanPolyData>::New();
        clean1->SetInputData(tooth1);

        const vtkSmartPointer<vtkCleanPolyData> clean2 = vtkSmartPointer<vtkCleanPolyData>::New();
        clean2->SetInputData(tooth2);

        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        vtkSmartPointer<vtkDistancePolyDataFilter> distanceFilter = vtkSmartPointer<vtkDistancePolyDataFilter>::New();
        distanceFilter->SetInputConnection( 0, clean1->GetOutputPort() );
        distanceFilter->SetInputConnection( 1, clean2->GetOutputPort() );
        distanceFilter->SignedDistanceOff();
        distanceFilter->ComputeSecondDistanceOn();
        distanceFilter->Update();

        double distMin { std::numeric_limits<double>::max() };
        for (vtkDataArray *distArray: {distanceFilter->GetOutput()->GetPointData()->GetScalars(),
                                       distanceFilter->GetOutput()->GetCellData()->GetScalars(),
                                       distanceFilter->GetSecondDistanceOutput()->GetPointData()->GetScalars(),
                                       distanceFilter->GetSecondDistanceOutput()->GetCellData()->GetScalars()})
        {
            const size_t size = distArray->GetNumberOfValues();
            for (size_t i = 0; i < size; ++i) {
                const double dist = *(distArray->GetTuple(i));
                distMin = std::min(dist, distMin);
            }
        }

        std::cout << "distMin = " << distMin << std::endl;

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "It took me " << time_span.count() << " seconds.\n";


        const vtkSmartPointer<vtkActor> tooth1Actor = Utilities::getPolyDataActor(tooth1);
        const vtkSmartPointer<vtkActor> tooth2Actor = Utilities::getPolyDataActor(tooth2);
        const vtkSmartPointer<vtkActor> distanceActor = Utilities::getPolyDataActor(distanceFilter->GetOutput());
        Utilities::DisplayActors({tooth1Actor, tooth2Actor, distanceActor});

    }


    double getMinDistance(const vtkSmartPointer<vtkPolyData>& obj1,
                          const vtkSmartPointer<vtkPolyData>& obj2)
    {
        const vtkSmartPointer<vtkCleanPolyData> clean1 { vtkSmartPointer<vtkCleanPolyData>::New() },
                                                clean2 { vtkSmartPointer<vtkCleanPolyData>::New() };
        clean1->SetInputData(obj1);
        clean2->SetInputData(obj2);

        vtkSmartPointer<vtkDistancePolyDataFilter> distanceFilter = vtkSmartPointer<vtkDistancePolyDataFilter>::New();
        distanceFilter->SetInputConnection( 0, clean1->GetOutputPort() );
        distanceFilter->SetInputConnection( 1, clean2->GetOutputPort() );
        distanceFilter->SignedDistanceOff();
        distanceFilter->ComputeSecondDistanceOn();
        distanceFilter->Update();

        double distMin { std::numeric_limits<double>::max() };
        for (vtkDataArray *distArray: {distanceFilter->GetOutput()->GetPointData()->GetScalars(),
                                       distanceFilter->GetOutput()->GetCellData()->GetScalars(),
                                       distanceFilter->GetSecondDistanceOutput()->GetPointData()->GetScalars(),
                                       distanceFilter->GetSecondDistanceOutput()->GetCellData()->GetScalars()})
        {
            const size_t size = distArray->GetNumberOfValues();
            for (size_t i = 0; i < size; ++i) {
                const double dist = *(distArray->GetTuple(i));
                distMin = std::min(dist, distMin);
            }
        }
        return distMin;
    }

    void DistanceBetweenTwoTooth_Decimation()
    {
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::array<vtkSmartPointer<vtkPolyData>, 2> dataOrig { toothMap[11], toothMap[21] };
        std::array<vtkSmartPointer<vtkPolyData>, 2> dataModif {vtkSmartPointer<vtkPolyData>::New(),
                                                               vtkSmartPointer<vtkPolyData>::New()};
        dataModif.front()->DeepCopy(dataOrig.front());
        dataModif.back()->DeepCopy(dataOrig.back());


        for (vtkSmartPointer<vtkPolyData>& pd: dataModif)
        {
            const int numBefore = pd->GetNumberOfPolys();
            const vtkSmartPointer<vtkQuadricDecimation> decimation { vtkSmartPointer<vtkQuadricDecimation>::New() };
            decimation->SetInputData(pd);
            decimation->SetTargetReduction(0.5);
            decimation->Update();
            pd->DeepCopy(decimation->GetOutput());


            /*
            const vtkSmartPointer<vtkPolyDataNormals> normals {vtkSmartPointer<vtkPolyDataNormals>::New()};
            normals->SetInputData(pd);
            normals->ComputePointNormalsOn();
            normals->SplittingOff();
            normals->ConsistencyOn();
            normals->AutoOrientNormalsOff();
            normals->Update();
            pd->DeepCopy(normals->GetOutput());

            const vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter {
                    vtkSmartPointer<vtkSmoothPolyDataFilter>::New()};
            smoothFilter->SetInputData(pd);
            smoothFilter->SetNumberOfIterations(1);
            smoothFilter->SetRelaxationFactor(1.0); // 0.01 ?
            smoothFilter->FeatureEdgeSmoothingOff();
            smoothFilter->BoundarySmoothingOn();
            smoothFilter->Update();
            pd->DeepCopy(smoothFilter->GetOutput());
            */

            const int numAfter = pd->GetNumberOfPolys();
            std::cout << numBefore << " --> " << numAfter << std::endl;
            // Utilities::visualize(pd);
        }

        {
            const vtkSmartPointer<vtkPolyData>& toothOne { dataOrig.front() }, &toothTwo { dataOrig.back()} ;
            const vtkSmartPointer<vtkActor> tooth1Actor = Utilities::getPolyDataActor(toothOne);
            const vtkSmartPointer<vtkActor> tooth2Actor = Utilities::getPolyDataActor(toothTwo);

            const double dist = getMinDistance(toothOne, toothTwo);
            std::cout << dist << std::endl;
            // Utilities::DisplayActors({tooth1Actor, tooth2Actor}, colors);
        }

        {
            const vtkSmartPointer<vtkPolyData>& toothOne { dataModif.front() }, &toothTwo { dataModif.back()} ;
            const vtkSmartPointer<vtkActor> tooth1Actor = Utilities::getPolyDataActor(toothOne);
            const vtkSmartPointer<vtkActor> tooth2Actor = Utilities::getPolyDataActor(toothTwo);

            const double dist = getMinDistance(toothOne, toothTwo);
            std::cout << dist << std::endl;
            // Utilities::DisplayActors({tooth1Actor, tooth2Actor}, colors);
        }
    }

    void DistanceBetweenTwoTooth_Decimation_Performance()
    {
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::array<vtkSmartPointer<vtkPolyData>, 2> dataOrig { toothMap[11], toothMap[21] };
        std::array<vtkSmartPointer<vtkPolyData>, 2> dataModif {vtkSmartPointer<vtkPolyData>::New(),
                                                               vtkSmartPointer<vtkPolyData>::New()};
        dataModif.front()->DeepCopy(dataOrig.front());
        dataModif.back()->DeepCopy(dataOrig.back());


        for (vtkSmartPointer<vtkPolyData>& pd: dataModif)
        {
            const int numBefore = pd->GetNumberOfPolys();
            const vtkSmartPointer<vtkQuadricDecimation> decimation { vtkSmartPointer<vtkQuadricDecimation>::New() };
            decimation->SetInputData(pd);
            decimation->SetTargetReduction(0.5);
            decimation->Update();
            pd->DeepCopy(decimation->GetOutput());

            const int numAfter = pd->GetNumberOfPolys();
            std::cout << numBefore << " --> " << numAfter << std::endl;
        }

        constexpr size_t COUNT {100};

        {
            const vtkSmartPointer<vtkPolyData>& toothOne { dataOrig.front() }, &toothTwo { dataOrig.back()} ;
            const vtkSmartPointer<vtkActor> tooth1Actor = Utilities::getPolyDataActor(toothOne);
            const vtkSmartPointer<vtkActor> tooth2Actor = Utilities::getPolyDataActor(toothTwo);

            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < COUNT; ++i) {
                const double dist = getMinDistance(toothOne, toothTwo);
            }

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }

        {
            const vtkSmartPointer<vtkPolyData>& toothOne { dataModif.front() }, &toothTwo { dataModif.back()} ;
            const vtkSmartPointer<vtkActor> tooth1Actor = Utilities::getPolyDataActor(toothOne);
            const vtkSmartPointer<vtkActor> tooth2Actor = Utilities::getPolyDataActor(toothTwo);

            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < COUNT; ++i) {
                const double dist = getMinDistance(toothOne, toothTwo);
            }

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }
    }
}

void Distances::TestAll()
{
    // Display_Single_Tooth();

    // DistanceBetween_TwoSpheres_Test();

    // Distance_Demo();

    DistanceBetweenTwoTooth();

    // DistanceBetweenTwoTooth_Decimation();

    // DistanceBetweenTwoTooth_Decimation_Performance();
}


