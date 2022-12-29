//============================================================================
// Name        : Utilities.h
// Created on  : 27.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utilities
//============================================================================

#include <iostream>

#include "../VtkHeaders.h"

#include "../Utilities/Utilities.h"
#include "../TreatmentPlan/TreatmentPlan.h"
#include "../Model/Common.h"
#include "../Model/FDI.h"
#include "../Model/Jaw.h"
#include "../Model/Tooth.h"
#include "../Model/ToothAxis.h"

#include "SpeeСurve.h"

namespace SpeeCurve
{
    const vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors>::New() };
}

namespace SpeeCurve::Utils {

    vtkSmartPointer<vtkActor> getPolyDataActor(std::string_view filePath,
                                               [[maybe_unused]] const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkSmartPointer<vtkPolyDataMapper>::New() };
        mapper->SetInputData( Utilities::readStl(filePath));



        vtkSmartPointer<vtkActor> actor { vtkSmartPointer<vtkActor>::New() };
        actor->SetMapper(mapper);
        // actor->GetProperty()->SetDiffuse(0.8);
        // actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
        // actor->GetProperty()->SetSpecular(0.3);
        // actor->GetProperty()->SetSpecularPower(60.0);
        return actor;
    }

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
};

namespace SpeeCurve::Tests
{
    void Display_Tooths()
    {
        using namespace Model;
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/2878_teeth.obj)" };
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        const Jaw lower(toothMap, TeethType::Lower);
        const Jaw upper(toothMap, TeethType::Upper);

        vtkSmartPointer<vtkPolyData> lowerTeehsData;
        for (auto &jaw: {lower}){
            const vtkSmartPointer<vtkAppendPolyData> dataAppender {vtkSmartPointer<vtkAppendPolyData>::New()};
            for (const auto &tooth: jaw.teeth)
                dataAppender->AddInputData(tooth.toothPolyData);
            dataAppender->Update();
            lowerTeehsData = dataAppender->GetOutput();
        }

        Eigen::Vector3d center {0.0, 7.0, -45.0};
        Eigen::Vector3d pt1 {45.0, 7.0, 0.0};
        Eigen::Vector3d pt2 {-45.0, 6.0, 0.0};

        const vtkSmartPointer<vtkPlaneSource> planeSource { vtkSmartPointer<vtkPlaneSource>::New() };
        planeSource->SetCenter(center.data()); // Coordinates of back angle
        planeSource->SetPoint1(pt1.data());
        planeSource->SetPoint2(pt2.data());
        planeSource->Update();
        vtkSmartPointer<vtkPolyData> planeData { planeSource->GetOutput() };

        std::vector<vtkSmartPointer<vtkActor>> actors;

        vtkSmartPointer<vtkActor> centerActor = Utilities::getPointsActor({center}, colors);
        vtkSmartPointer<vtkActor> ptsActor = Utilities::getPointsActor({pt1, pt2}, colors);

        actors.push_back(Utilities::getPolyDataActor(planeData, colors));
        actors.push_back(Utilities::getPolyDataActor(lowerTeehsData, colors));
        actors.push_back(centerActor);
        actors.push_back(ptsActor);

        Utilities::DisplayActors(actors, colors);
    }
}

void SpeeCurve::TestAll()
{
    Tests::Display_Tooths();
};