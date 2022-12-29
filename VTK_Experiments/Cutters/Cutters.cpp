//============================================================================
// Name        : Cutters.cpp
// Created on  : 23.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Cutters
//============================================================================

#include "../VtkHeaders.h"
#include "../Utilities/Utilities.h"
#include "Cutters.h"

#include <iostream>
#include <string_view>

namespace Cutters::Utils
{
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

namespace Cutters
{
    vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };
    Eigen::Vector3d GetCentroid(vtkSmartPointer<vtkPolyData> polyData) {
        // Compute the center of mass
        vtkNew<vtkCenterOfMass> centerOfMassFilter;
        centerOfMassFilter->SetInputData(polyData);
        centerOfMassFilter->SetUseScalarsAsWeights(false);
        centerOfMassFilter->Update();

        Eigen::Vector3d pt {};
        centerOfMassFilter->GetCenter(pt.data());
        return pt;
    }

    void SimpleTest1()
    {
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_6.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);
        const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(polyData);

        Eigen::Vector3d centroid {};
        polyData->GetCenter(centroid.data());

        Eigen::Vector3d normals {0.0, 1.0, 0.0};
        std::array<double, 6> bounds;
        polyData->GetBounds(bounds.data());

        Eigen::Vector3d minBound {bounds[0], bounds[2], bounds[4]};
        Eigen::Vector3d maxBound {bounds[1], bounds[3], bounds[5]};
        const double minDistZ = centroid[1] - minBound[1];
        const double maxDistZ = maxBound[1] - centroid[1];

        vtkNew<vtkPlane> plane;
        plane->SetOrigin(centroid.data());
        plane->SetNormal(normals.data());

        vtkNew<vtkCutter> cutter;
        cutter->SetCutFunction(plane);
        cutter->SetInputData(polyData);
        cutter->GenerateValues(100, -minDistZ, maxDistZ);
        cutter->Update();

        std::cout << "GetNumberOfContours = " << cutter->GetNumberOfContours() << std::endl;
        Utilities::visualize(cutter->GetOutput());
    }


    void CutToothWithPlane()
    {
        constexpr size_t numContours {50};
        constexpr std::string_view filePath {R"(/home/andtokm/Projects/data/out/Tooths/tooth_6.stl)"};
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);
        const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(polyData);

        Eigen::Vector3d centroid {};
        polyData->GetCenter(centroid.data());

        // auto pointActor = Utilities::getPointsActor({centroid}, colors);
        // Utilities::DisplayActors({pointActor, toothActor}, colors);

        Eigen::Vector3d normals {0.0, 1.0, 0.0};
        std::array<double, 6> bounds;
        polyData->GetBounds(bounds.data());

        Eigen::Vector3d minBound {bounds[0], bounds[2], bounds[4]};
        Eigen::Vector3d maxBound {bounds[1], bounds[3], bounds[5]};
        const double minDistZ = centroid[1] - minBound[1];
        const double maxDistZ = maxBound[1] - centroid[1];

        vtkNew<vtkPlane> plane;
        plane->SetOrigin(centroid.data());
        plane->SetNormal(normals.data());

        vtkNew<vtkCutter> cutter;
        cutter->SetCutFunction(plane);
        cutter->SetInputData(polyData);
        cutter->GenerateValues(numContours, /* -minDistZ */ maxDistZ/ 2, maxDistZ);
        cutter->Update();

        vtkNew<vtkStripper> contourStripper;
        contourStripper->SetInputConnection(cutter->GetOutputPort());
        contourStripper->JoinContiguousSegmentsOn();
        // contourStripper->JoinContiguousSegmentsOff();
        contourStripper->Update();


        vtkNew<vtkPolyDataMapper> cutterMapper;
        cutterMapper->SetInputConnection(contourStripper->GetOutputPort());
        cutterMapper->ScalarVisibilityOff();

        vtkNew<vtkActor> planeActor;
        planeActor->GetProperty()->SetColor(colors->GetColor3d("Deep_pink").GetData());
        planeActor->GetProperty()->SetLineWidth(2);
        planeActor->SetMapper(cutterMapper);

        // Utilities::visualize(contourStripper->GetOutput());
        Utilities::DisplayActors({planeActor});
    }

    void Cut_Tooth_Vertical() {
        constexpr std::string_view filePath { R"(/home/andtokm/Projects/data/out/Tooths/tooth_1.stl)" };
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(filePath);

        // Create a plane to cut
        vtkNew<vtkPlane> plane;
        plane->SetOrigin(polyData->GetCenter());

        // TODO: Angle of the plane
        plane->SetNormal(0, 1, 0.1);

        std::array<double, 6> bounds;
        polyData->GetBounds(bounds.data());

        Eigen::Vector3d center {};
        polyData->GetCenter(center.data());

        Eigen::Vector3d minBound {bounds[0], bounds[2], bounds[4]};
        Eigen::Vector3d maxBound {bounds[1], bounds[3], bounds[5]};
        const double minDistZ = center[1] - minBound[1];
        const double maxDistZ = maxBound[1] - center[1];
        const double beginDistZ = maxDistZ / 2; // Middle between center and upper point

        std::cout << "minBound: " << minBound << "\n"
                  << "maxBound: " << maxBound << "\n"
                  << "center: " << center << std::endl;
        std::cout << "distanceMin = " << minDistZ << "\n"
                  << "distanceMax = " << maxDistZ << std::endl;

        // Create cutter
        vtkNew<vtkCutter> cutter;
        cutter->SetCutFunction(plane);
        cutter->SetInputData(polyData);

        cutter->GenerateValues(30, beginDistZ - 2, maxDistZ + 2);
        vtkNew<vtkPolyDataMapper> cutterMapper;
        cutterMapper->SetInputConnection(cutter->GetOutputPort());
        cutterMapper->ScalarVisibilityOff();

        vtkNew<vtkActor> planeActor;
        planeActor->GetProperty()->SetColor(colors->GetColor3d("Deep_pink").GetData());
        planeActor->GetProperty()->SetLineWidth(5);
        planeActor->SetMapper(cutterMapper);

        const vtkSmartPointer<vtkActor> centerActor = Utilities::getPointsActor({center}, {1, 0, 0});
        const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(polyData);

        Utilities::DisplayActors({planeActor, centerActor, toothActor});
    }

    // TODO: it is necessary to consider separately the lower and separately the upper jaw
    void Split_And_CutTooths() {
        constexpr std::string_view fileName {
                R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> separatedData =
                Utils::ReadTeethObj(fileName);

        for (const auto& [id, polyData]: separatedData)
        {
            vtkNew<vtkPlane> plane;
            plane->SetOrigin(polyData->GetCenter());

            // TODO: Angle of the plane
            plane->SetNormal(0, 1, 0.1);

            Eigen::Vector3d center {};
            std::array<double, 6> bounds;
            polyData->GetBounds(bounds.data());
            polyData->GetCenter(center.data());

            const Eigen::Vector3d minBound {bounds[0], bounds[2], bounds[4]};
            const Eigen::Vector3d maxBound {bounds[1], bounds[3], bounds[5]};

            double from {0}, until {0};
            if (Utilities::isLowerTooth(id)) {
                from = 0;
                until = maxBound[1] - center[1];
            }
            else {
                from = minBound[1] - center[1];
                until = 0;
            }

            // Create cutter
            vtkNew<vtkCutter> cutter;
            cutter->SetCutFunction(plane);
            cutter->SetInputData(polyData);

            cutter->GenerateValues(60, from, until);
            vtkNew<vtkPolyDataMapper> cutterMapper;
            cutterMapper->SetInputConnection(cutter->GetOutputPort());
            cutterMapper->ScalarVisibilityOff();

            vtkNew<vtkActor> planeActor;
            planeActor->GetProperty()->SetColor(colors->GetColor3d("Green").GetData());
            planeActor->GetProperty()->SetLineWidth(3);
            planeActor->SetMapper(cutterMapper);

            const vtkSmartPointer<vtkActor> centerActor = Utilities::getPointsActor({center}, {1, 0, 0});
            const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(polyData);

            Utilities::DisplayActors({planeActor, centerActor, toothActor});
        }
    }
}

void Cutters::TestAll()
{
    // Cutters::SimpleTest1();
    // Cutters::CutToothWithPlane();
    // Cutters::Cut_Tooth_Vertical();
    Cutters::Split_And_CutTooths();

};




