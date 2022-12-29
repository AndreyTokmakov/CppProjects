//============================================================================
// Name        : Preprocessing.h
// Created on  : 07.12.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Preprocessing
//============================================================================

#include "VtkHeaders.h"
#include "Types.h"
#include "Utilities.h"

#include <iostream>
#include <algorithm>
#include <string_view>
#include <numeric>
#include <random>

#include "Preprocessing.h"

namespace Preprocessing {

    // TODO: refactor
    Types::Point<3, double> toPoint(std::string_view strPoints) {
        const size_t pos1 = strPoints.find(" ");
        if (std::string::npos == pos1)
            return {};

        const size_t pos2 = strPoints.find(" ", pos1 + 1);
        if (std::string::npos == pos2)
            return {};

        Types::Point<3, double> pt {};
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
        std::vector<Types::Point<3, double>> points;
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

                vtkSmartPointer<vtkPoints> vtkPoints = vtkPoints::New();
                for (const Types::Point<3, double>& pt: points)
                    vtkPoints->InsertNextPoint(pt[0], pt[1], pt[2]);

                const int pointsCount { static_cast<int>(points.size())};
                vtkSmartPointer<vtkCellArray> vtkCells = vtkCellArray::New();
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

    void Visualize_Tooth_OBJ_Data()
    {
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                ReadTeethObj(R"(/home/andtokm/Projects/data/cases/2280/models/crowns.obj)");

        const vtkSmartPointer<vtkAppendPolyData> dataAppender { vtkAppendPolyData::New() };
        for (const auto& [id, data] : toothMap)
            if (Utilities::isLowerTooth(id))
                dataAppender->AddInputData(data);
        dataAppender->Update();

        Utilities::visualize(dataAppender->GetOutput());
    }

    void PrintDataMetrics(vtkSmartPointer<vtkPolyData> data, std::string_view text) {
        const auto featureEdges = vtkSmartPointer<vtkFeatureEdges>::New();
        featureEdges->SetInputData(data);
        featureEdges->BoundaryEdgesOn();
        featureEdges->FeatureEdgesOff();
        featureEdges->NonManifoldEdgesOff();
        featureEdges->ManifoldEdgesOff();
        featureEdges->Update();

        std::cout << "============================== " << text << " =====================" << std::endl;
        std::cout << "Data NumberOfPolys: " << data->GetNumberOfPolys()<< std::endl;
        std::cout << "Data NumberOfLines: " << data->GetNumberOfLines()<< std::endl;
        std::cout << "Data NumberOfCells: " << data->GetNumberOfCells()<< std::endl;
        std::cout << "Data NumberOfVerts: " << data->GetNumberOfVerts()<< std::endl;
        std::cout << "Data NumberOfPoints: " << data->GetNumberOfPoints()<< std::endl;
        std::cout << "NumberOfCells : " << featureEdges->GetOutput()->GetNumberOfCells() << std::endl;
        std::cout << "NumberOfPoints: " << featureEdges->GetOutput()->GetNumberOfPoints() << std::endl;
        std::cout << "NumberOfPolys : " << featureEdges->GetOutput()->GetNumberOfPolys() << std::endl;
        std::cout << "NumberOfLines : " << featureEdges->GetOutput()->GetNumberOfLines() << std::endl;
    }

    void FillHoles_ExtractEdge_Decimate()
    {
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                ReadTeethObj(R"(/home/andtokm/Projects/data/cases/2280/models/crowns.obj)");

        constexpr unsigned short toothId { 32 };
        if (auto iter = toothMap.find(toothId); toothMap.end() != iter) {
            vtkSmartPointer<vtkPolyData> toothData = iter->second;
            size_t numberOfOpenEdges { std::numeric_limits<size_t>::max()};

            const auto fillHolesFilter = vtkSmartPointer<vtkFillHolesFilter>::New();
            fillHolesFilter->SetInputData(toothData);
            fillHolesFilter->SetHoleSize(1000.0);//1000.0
            fillHolesFilter->Update();
            toothData = fillHolesFilter->GetOutput();

            PrintDataMetrics(toothData, "original");
            Utilities::visualize(toothData);

            {
                const vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter {vtkSmoothPolyDataFilter::New()};
                smoothFilter->SetInputData(toothData);
                smoothFilter->SetNumberOfIterations(1);
                smoothFilter->SetRelaxationFactor(0.4); // 0.01 ?
                smoothFilter->FeatureEdgeSmoothingOff();
                smoothFilter->BoundarySmoothingOn();
                smoothFilter->Update();
                toothData = smoothFilter->GetOutput();

                PrintDataMetrics(toothData, "after vtkSmoothPolyDataFilter");
                Utilities::visualize(toothData);
            }

            {
                const vtkSmartPointer<vtkTriangleFilter> triangleFilter{vtkTriangleFilter::New()};
                triangleFilter->SetInputData(toothData);
                triangleFilter->Update();
                toothData = triangleFilter->GetOutput();

                PrintDataMetrics(toothData, "after vtkTriangleFilter");
                Utilities::visualize(toothData);
            }

            {
                const vtkSmartPointer<vtkQuadricDecimation> decimation {vtkQuadricDecimation::New()};
                decimation->SetInputData(toothData);
                decimation->SetTargetReduction(0.9);
                decimation->Update();
                toothData = decimation->GetOutput();

                PrintDataMetrics(toothData, "after vtkQuadricDecimation");
                Utilities::visualize(toothData);
            }

            {
                const vtkSmartPointer<vtkPolyDataNormals> normals{vtkPolyDataNormals::New()};
                normals->SetInputData(toothData);
                normals->ComputePointNormalsOn();
                normals->SplittingOff();
                normals->ConsistencyOn();
                normals->AutoOrientNormalsOff();
                normals->Update();
                toothData = normals->GetOutput();

                PrintDataMetrics(toothData, "after vtkPolyDataNormals");
                Utilities::visualize(toothData);
            }



            {
                const vtkSmartPointer<vtkQuadricDecimation> decimation {vtkQuadricDecimation::New()};
                decimation->SetInputData(toothData);
                decimation->SetTargetReduction(0.5);
                decimation->Update();
                toothData = decimation->GetOutput();

                PrintDataMetrics(toothData, "after vtkQuadricDecimation");
                Utilities::visualize(toothData);
            }

            {
                const vtkSmartPointer<vtkPolyDataNormals> normals{vtkPolyDataNormals::New()};
                normals->SetInputData(toothData);
                normals->ComputePointNormalsOn();
                normals->SplittingOff();
                normals->ConsistencyOn();
                normals->AutoOrientNormalsOff();
                normals->Update();
                toothData = normals->GetOutput();

                PrintDataMetrics(toothData, "after vtkPolyDataNormals");
                Utilities::visualize(toothData);
            }
        }
    }

    void Preprocess_Tooth_ForMove()
    {
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                ReadTeethObj(R"(/home/andtokm/Projects/data/cases/2280/models/crowns.obj)");

        constexpr unsigned short toothId { 32 };
        auto iter = toothMap.find(toothId);
        if (toothMap.end() == iter)
            return;
        vtkSmartPointer<vtkPolyData> toothData = iter->second;

        const vtkSmartPointer<vtkFillHolesFilter> fillHolesFilter {vtkFillHolesFilter::New()};
        fillHolesFilter->SetInputData(toothData);
        fillHolesFilter->SetHoleSize(1000.0);//1000.0
        fillHolesFilter->Update();
        toothData = fillHolesFilter->GetOutput();

        const vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter {vtkSmoothPolyDataFilter::New()};
        smoothFilter->SetInputData(toothData);
        smoothFilter->SetNumberOfIterations(4);
        smoothFilter->SetRelaxationFactor(0.7);
        smoothFilter->FeatureEdgeSmoothingOff();
        smoothFilter->BoundarySmoothingOn();
        smoothFilter->Update();
        toothData = smoothFilter->GetOutput();

        Utilities::visualize(toothData);

        while (toothData->GetNumberOfPoints() > 100) {
            const vtkSmartPointer<vtkQuadricDecimation> decimation {vtkQuadricDecimation::New()};
            decimation->SetInputData(toothData);
            decimation->SetTargetReduction(0.2);
            decimation->Update();
            toothData = decimation->GetOutput();

            std::cout << "Data NumberOfPoints: " << toothData->GetNumberOfPoints()<< std::endl;
        }

        const vtkSmartPointer<vtkPolyDataNormals> normals{vtkPolyDataNormals::New()};
        normals->SetInputData(toothData);
        normals->ComputePointNormalsOn();
        normals->SplittingOff();
        normals->ConsistencyOn();
        normals->AutoOrientNormalsOff();
        normals->Update();
        toothData = normals->GetOutput();

        Utilities::visualize(toothData);
    }
};


void Preprocessing::TestAll()
{
    // FillHoles_ExtractEdge_Decimate();
    Preprocess_Tooth_ForMove();
};