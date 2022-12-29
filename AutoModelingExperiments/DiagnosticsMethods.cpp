//============================================================================
// Name        : DiagnosticsMethods.cpp
// Created on  : 29.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Diagnostics methods
//============================================================================

#include <vtkCutter.h>
#include <vtkAxes.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkLineSource.h>
#include <vtkTextActor3D.h>
#include <vtkStripper.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkDelaunay3D.h>
#include <vtkAppendFilter.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellCenters.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkCellData.h>
#include <vtkConnectivityFilter.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkDelaunay3D.h>
#include <vtkAppendFilter.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellCenters.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkCellData.h>
#include <vtkConnectivityFilter.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPlane.h>
#include <vtkCenterOfMass.h>
#include <vtkGenericCell.h>
#include <vtkBoxWidget.h>
#include <vtkColor.h>
#include <vtkCellIterator.h>
#include <vtkColorTransferFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkMetaImageReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkLookupTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkPolyDataNormals.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkWarpVector.h>
#include <vtkFillHolesFilter.h>
#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkDataSetMapper.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkLine.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSTLWriter.h>
#include <vtkTriangle.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWarpScalar.h>
#include <vtkProbeFilter.h>
#include <vtkXMLPolyDataWriter.h>

#include <iostream>
#include <iomanip>
#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>

#include "Utilities.h"
#include "Types.h"
#include "Features.h"
#include "DiagnosticsMethods.h"
#include "ContactPoints/ContactPoints.h"
#include "TreatmentPlan/TreatmentPlan.h"

namespace {
    constexpr std::string_view CONTACT_POINTS_JSON_FILE {
            R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/contact_points_2878.json)" };
    constexpr std::string_view upperStl_2878 {
            R"(/home/andtokm/Projects/data/cases/2878/automodeling/crowns/2878_upper.stl)" };
    constexpr std::string_view upperStl_2878_Final {
            R"(/home/andtokm/Projects/data/cases/2878/out_test/teeth/P_2878_upper_01_Template.stl)" };
}

namespace DiagnosticsMethods {

    const vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };

    vtkSmartPointer<vtkActor> getPolyDataActor(std::string_view filePath,
                                               [[maybe_unused]] const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        mapper->SetInputData( Utilities::readStl(filePath));



        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        actor->SetMapper(mapper);
        // actor->GetProperty()->SetDiffuse(0.8);
        // actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
        // actor->GetProperty()->SetSpecular(0.3);
        // actor->GetProperty()->SetSpecularPower(60.0);
        return actor;
    }

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

    vtkSmartPointer<vtkPolyData> transformData(const vtkSmartPointer<vtkPolyData> polyData) {
        vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        transform->RotateX(180);
        transform->RotateY(180);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter { vtkTransformPolyDataFilter::New() };
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();
        return transformFilter->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> rotate_X_90(const vtkSmartPointer<vtkPolyData> polyData) {
        vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        transform->RotateX(90);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter { vtkTransformPolyDataFilter::New() };
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();
        return transformFilter->GetOutput();
    }

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

    std::array<vtkSmartPointer<vtkActor>, 2>
    getLineActorEx(const Types::Line<3>& line, const vtkSmartPointer<vtkNamedColors>& colors)
    {
        vtkSmartPointer<vtkLineSource> lineSource{ vtkLineSource::New() };
        lineSource->SetPoint1(line.getFirstPoint().data());
        lineSource->SetPoint2(line.getSecondPoint().data());
        lineSource->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapperLine{ vtkPolyDataMapper::New() };
        mapperLine->AddInputDataObject(0, lineSource->GetOutput());

        vtkSmartPointer<vtkActor> actorLine{ vtkActor::New() };
        actorLine->SetMapper(mapperLine);
        actorLine->GetProperty()->SetColor(0, 1, 0);
        actorLine->GetProperty()->SetLineWidth(3);



        vtkSmartPointer<vtkPoints> pts{ vtkPoints::New() };
        pts->InsertNextPoint(line.getFirstPoint().data());
        pts->InsertNextPoint(line.getSecondPoint().data());

        vtkSmartPointer<vtkPolyData> pointsPolydata{ vtkPolyData::New() };
        pointsPolydata->SetPoints(pts);

        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter{ vtkVertexGlyphFilter::New() };
        vertexFilter->SetInputData(pointsPolydata);
        vertexFilter->Update();

        vtkSmartPointer<vtkPolyData> polydata{ vtkPolyData::New() };
        polydata->ShallowCopy(vertexFilter->GetOutput());

        vtkSmartPointer<vtkPolyDataMapper> mapperPoints{ vtkPolyDataMapper::New() };
        mapperPoints->SetInputData(polydata);

        vtkSmartPointer<vtkActor> actorPoints{ vtkActor::New() };
        actorPoints->SetMapper(mapperPoints);
        actorPoints->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        actorPoints->GetProperty()->SetPointSize(12);
        actorPoints->GetProperty()->RenderPointsAsSpheresOn();

        return { actorLine, actorPoints};
    }
}

/** Pont - Linder - Hart methods: **/
namespace DiagnosticsMethods::Pont
{
    void ReadTeeth_FindDistances() {
        constexpr std::string_view fileName { R"(/home/andtokm/Projects/teeth_movement/example/out/13181/13181_teeth.obj)" };

        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(fileName);
        const std::vector<vtkSmartPointer<vtkPolyData>> teeth =
                Utilities::separate_connected_components_parallel_test(polyData);
        std::cout << teeth.size() << std::endl;

        Utilities::visualize(teeth);
    }

    void CheckDistances_Manual() {
        const vtkNew<vtkNamedColors> colors;
        std::vector<Types::Point<3, double>> points {
                { -21.37516780566391,  7.71654283503751,  4.309933591970717  },
                { -18.798992897663187, 7.529394716998286, 11.160850739868003 },
                {  17.90494093201055,  7.636524496510928, 11.015867745896811 },
                {  20.24224301008987,  7.583678280072945,  4.18087576873147  },
                { -14.89378671999506,  7.629965018346851, 18.6534126520938   },
                { -9.258607366094143,  6.54667206508766,  22.204924081636303 },
                { -0.4743870439242235, 5.953008265825731, 26.073406571104993 },
                { 14.230078389548419,  7.187540945604041, 18.545068118183483 },
                {  8.838817327018326,  6.694491116069133, 22.953933250029994 }
        };

        const std::vector<Types::Point<3, double>> molarsPoints {
                { 25.130078389548419,  6.187540945604041, -10.845068118183483 },
                { -25.830078389548419, 6.187540945604041, -10.845068118183483 },
        };

        std::sort(points.begin(), points.end(), [](const auto& pt1, const auto& pt2) {
            return pt1[0] < pt2[0];
        });

        std::vector<Types::Point<3, double>> incisors;
        for (size_t i = 2; i < points.size() - 2; ++i)
            incisors.push_back(points[i]);

        double incisorsDist {0};
        for (size_t i = 1; i < incisors.size(); ++i)
            incisorsDist += incisors[i - 1].distanceTo(incisors[i]);

        const std::vector<Types::Point<3, double>> premolars {
                points[0], points[1], points[7], points[8]
        };

        const double dist1 = std::abs(premolars[0][0] - premolars[3][0]) +
                             std::abs(premolars[1][0] - premolars[2][0]);

        // const double dist2 = molarsPoints.front().distanceTo(molarsPoints.back());
        const double dist2 = std::abs(molarsPoints[0][0] - molarsPoints[1][0]);


        std::cout << "incisorsDist              = " << incisorsDist<< std::endl;
        std::cout << "dist (Premolars)          = " << dist1 / 2 << std::endl;
        std::cout << "dist expected (Premolars) = " << (incisorsDist * 100) / 85 << std::endl;
        std::cout << "dist (Molars)             = " << dist2  << std::endl;
        std::cout << "dist expected (Molars)    = " << (incisorsDist * 100) / 65 << std::endl;

        vtkSmartPointer<vtkActor> actorPoint1 = Utilities::getPointsActor(points, colors);
        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(molarsPoints, colors);
        vtkSmartPointer<vtkActor> actorTeeth = getPolyDataActor(upperStl_2878, colors);
        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        actorPoint1->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

        Utilities::DisplayActors({actorPoint1, actorPoint, actorTeeth}, colors);
    }

    void CheckDistances_Using_CenterOfMasses() {

    }
}

namespace DiagnosticsMethods::Corkhouse
{
    void CheckDistances_Manual() {
        std::vector<Types::Point<3, double>> points {
                { -21.37516780566391,  7.71654283503751,  4.309933591970717  },
                { -18.798992897663187, 7.529394716998286, 11.160850739868003 },
                {  17.90494093201055,  7.636524496510928, 11.015867745896811 },
                {  20.24224301008987,  7.583678280072945,  4.18087576873147  },
                { -14.89378671999506,  7.629965018346851, 18.6534126520938   },
                { -9.258607366094143,  6.54667206508766,  22.204924081636303 },
                { -0.4743870439242235, 5.953008265825731, 26.073406571104993 },
                { 14.230078389548419,  7.187540945604041, 18.545068118183483 },
                {  8.838817327018326,  6.694491116069133, 22.953933250029994 }
        };

        std::sort(points.begin(), points.end(), [](const auto& pt1, const auto& pt2) {
            return pt1[0] < pt2[0];
        });

        std::vector<Types::Point<3, double>> incisors;
        for (size_t i = 2; i < points.size() - 2; ++i)
            incisors.push_back(points[i]);

        double incisorsDist {0};
        for (size_t i = 1; i < incisors.size(); ++i)
            incisorsDist += incisors[i - 1].distanceTo(incisors[i]);

        const auto center  = points[4];
        const auto ptLeft  = (points[0] + points[1]) / 2;
        const auto ptRight = (points[7] + points[8]) / 2;

        const std::vector<Types::Point<3, double>> targetPoints {
                ptLeft, center, ptRight
        };

        double t;
        double closest[3];
        double distSquread = vtkLine::DistanceToLine(center.data(),
                                                     ptLeft.data(),
                                                     ptRight.data(), t, closest);

        std::cout << "incisorsDist              = " << incisorsDist<< std::endl;
        std::cout << "Dist:= " << std::sqrt(distSquread)  << std::endl;
        std::cout << "Closest: [" << closest[0] << "  " << closest[1] << "  " << closest[2] << "]" << std::endl;


        /** Find the squared distance between the points. **/
        double squaredDistance = vtkMath::Distance2BetweenPoints(center.data(), closest);
        /** Take the square root to get the Euclidean distance between the points **/
        double distance = std::sqrt(squaredDistance);

        /** Output the results. **/
        std::cout << "SquaredDistance = " << squaredDistance << std::endl;
        std::cout << "Distance = " << distance << std::endl;

        vtkSmartPointer<vtkActor> actorPoints = Utilities::getPointsActor(targetPoints, colors);
        vtkSmartPointer<vtkActor> actorTeeth = getPolyDataActor(upperStl_2878_Final, colors);
        actorPoints->GetProperty()->SetPointSize(12);
        actorPoints->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoints, actorTeeth}, colors);
    }

    // TODO: Refactor. Make table
    double GetCorkhouseDist(double incisorsLengthSum) {
        if (27 <= incisorsLengthSum && incisorsLengthSum <= 27.5f) return 16.0f;
        else if (27.5f <= incisorsLengthSum && incisorsLengthSum <= 28.0f) return 16.5f;
        else if (28.0f <= incisorsLengthSum && incisorsLengthSum <= 28.5f) return 16.8f;
        else if (28.5f <= incisorsLengthSum && incisorsLengthSum <= 29.0f) return 17.0f;
        else if (29.0f <= incisorsLengthSum && incisorsLengthSum <= 29.5f) return 17.3f;
        else if (29.5f <= incisorsLengthSum && incisorsLengthSum <= 30.0f) return 17.5f;
        else if (30.0f <= incisorsLengthSum && incisorsLengthSum <= 30.5f) return 17.8f;
        else if (30.5f <= incisorsLengthSum && incisorsLengthSum <= 31.0f) return 18.0f;
        else if (31.0f <= incisorsLengthSum && incisorsLengthSum <= 31.5f) return 18.3f;
        else if (31.5f <= incisorsLengthSum && incisorsLengthSum <= 32.0f) return 18.5f;
        else if (32.0f <= incisorsLengthSum && incisorsLengthSum <= 32.5f) return 18.8f;
        else if (32.5f <= incisorsLengthSum && incisorsLengthSum <= 33.0f) return 19.0f;
        else if (33.0f <= incisorsLengthSum && incisorsLengthSum <= 33.5f) return 19.3f;
        else if (33.5f <= incisorsLengthSum && incisorsLengthSum <= 34.0f) return 19.5f;
        else if (34.0f <= incisorsLengthSum && incisorsLengthSum <= 34.5f) return 19.8f;
        else if (34.5f <= incisorsLengthSum && incisorsLengthSum <= 35.0f) return 20.0f;
        return 25;
    }
}

namespace DiagnosticsMethods::VisualizeDiagnosticsResults
{
    void DrawLineWithTextSizes()
    {
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

        Utilities::DisplayActorsEx({lineActor, textActor1, textActor2, textActor3}, colors);
    }


    vtkSmartPointer<vtkTextActor> buildTextActor(std::string_view text,
                                                 const Types::Point<2, double>& position,
                                                 std::string colorName = "Gray",
                                                 const size_t fontSize = 24) {
        vtkSmartPointer<vtkTextActor> actor { vtkTextActor::New() };
        actor->SetInput(text.data());
        actor->SetPosition(position[0], position[1]);
        actor->GetTextProperty()->SetFontSize(fontSize);
        actor->GetTextProperty()->SetColor(colors->GetColor3d(colorName).GetData());
        return actor;
    }


    void Visualize_Estimation_Pont()
    {
        std::vector<Types::Point<3, double>> points {
                { -21.37516780566391,  7.71654283503751,  4.309933591970717  },
                { -18.798992897663187, 7.529394716998286, 11.160850739868003 },
                {  17.90494093201055,  7.636524496510928, 11.015867745896811 },
                {  20.24224301008987,  7.583678280072945,  4.18087576873147  },
                { -14.89378671999506,  7.629965018346851, 18.6534126520938   },
                { -9.258607366094143,  6.54667206508766,  22.204924081636303 },
                { -0.4743870439242235, 5.953008265825731, 26.073406571104993 },
                { 14.230078389548419,  7.187540945604041, 18.545068118183483 },
                {  8.838817327018326,  6.694491116069133, 22.953933250029994 }
        };

        std::sort(points.begin(), points.end(), [](const auto& pt1, const auto& pt2) {
            return pt1[0] < pt2[0];
        });

        std::vector<Types::Point<3, double>> incisors;
        for (size_t i = 2; i < points.size() - 2; ++i)
            incisors.push_back(points[i]);

        constexpr size_t fontSize{24};
        double incisorsLengthSum {0};
        for (size_t i = 1; i < incisors.size(); ++i)
            incisorsLengthSum += incisors[i - 1].distanceTo(incisors[i]);
        std::cout << "Incisors Length sum  = " << incisorsLengthSum<< std::endl;

        const auto center  = points[4];
        const auto ptLeft  = (points[0] + points[1]) / 2;
        const auto ptRight = (points[7] + points[8]) / 2;

        const std::vector<Types::Point<3, double>> targetPoints {
                ptLeft, center, ptRight
        };

        vtkSmartPointer<vtkActor> actorPoints = Utilities::getPointsActor(targetPoints, colors);
        vtkSmartPointer<vtkActor> actorTeeth = getPolyDataActor(upperStl_2878_Final, colors);
        actorPoints->GetProperty()->SetPointSize(12);
        actorPoints->GetProperty()->RenderPointsAsSpheresOn();

        vtkSmartPointer<vtkRenderer> renderer { vtkRenderer::New() };
        renderer->SetBackground(colors->GetColor3d("DarkBlue").GetData());

        // Display actual / expected distance between premolars
        {
            const double distExpected = (incisorsLengthSum * 100) / 85;
            std::cout << "Dist expected (Premolars) = " << distExpected << std::endl;

            const vtkSmartPointer<vtkActor> lineActor = getLineActor({ptLeft, ptRight}, colors);
            lineActor->GetProperty()->SetColor(colors->GetColor3d("Gray").GetData());

            const double premolarDistActual = ptLeft.distanceTo(ptRight);

            Types::Point<2, double> pt{5, 60};
            std::string text{"First premolar distance: "};
            vtkSmartPointer<vtkTextActor> metricNameActor = buildTextActor(text, pt, "Gray", fontSize);

            pt[0] += text.length() * fontSize / 2.15;
            vtkSmartPointer<vtkTextActor> textActorActual =
                    buildTextActor(std::to_string(premolarDistActual), pt, "Red", fontSize);

            pt[0] += 110;
            vtkSmartPointer<vtkTextActor> textActorDelimiter = buildTextActor(" / ", pt, "Gray", fontSize);

            pt[0] += 30;
            vtkSmartPointer<vtkTextActor> textActorExpected =
                    buildTextActor(std::to_string(distExpected), pt, "Green", fontSize);

            renderer->AddActor(lineActor);
            renderer->AddActor2D(metricNameActor);
            renderer->AddActor2D(textActorActual);
            renderer->AddActor2D(textActorDelimiter);
            renderer->AddActor2D(textActorExpected);
        }

        {
            double t;
            Types::Point<3, double> closest;
            double distSquread = vtkLine::DistanceToLine(center.data(),
                                                         ptLeft.data(),
                                                         ptRight.data(), t, closest.data());
            // std::cout << "Dist = " << std::sqrt(distSquread)  << std::endl;
            // std::cout << "Closest: [" << closest[0] << "  " << closest[1] << "  " << closest[2] << "]" << std::endl;

            /** Find the squared distance between the points. **/
            const double squaredDistance = vtkMath::Distance2BetweenPoints(center.data(), closest.data());

            /** Take the square root to get the Euclidean distance between the points **/
            const double distance = std::sqrt(squaredDistance);

            /** Output the results. **/
            std::cout << "SquaredDistance = " << squaredDistance << std::endl;
            std::cout << "Distance = " << distance << std::endl;

            const vtkSmartPointer<vtkActor> lineActor = getLineActor({closest, center}, colors);
            lineActor->GetProperty()->SetColor(colors->GetColor3d("DarkGray").GetData());

            Types::Point<2, double> pt { 5, 20 };
            std::string text{"Distance to incisors   : "};
            vtkSmartPointer<vtkTextActor> metricNameActor = buildTextActor(text, pt, "Gray", fontSize);

            pt[0] += text.length() * fontSize / 2.15;
            vtkSmartPointer<vtkTextActor> textActorActual =
                    buildTextActor(std::to_string(distance), pt, "Red", fontSize);

            pt[0] += 110;
            vtkSmartPointer<vtkTextActor> textActorDelimiter = buildTextActor(" / ", pt, "Gray", fontSize);

            pt[0] += 30;
            const double distExpected = Corkhouse::GetCorkhouseDist(incisorsLengthSum);
            vtkSmartPointer<vtkTextActor> textActorExpected =
                    buildTextActor(std::to_string(distExpected), pt, "Green", fontSize);

            renderer->AddActor(lineActor);
            renderer->AddActor2D(metricNameActor);
            renderer->AddActor2D(textActorActual);
            renderer->AddActor2D(textActorDelimiter);
            renderer->AddActor2D(textActorExpected);
        }

        {   // Try to visualize 3D Texts - Failed
            /*
            const vtkSmartPointer<vtkTextActor3D> textActor1 { vtkTextActor3D::New() };
            textActor1->SetPosition(textPosPt[0], textPosPt[1], textPosPt[2]);
            textActor1->SetInput(lineText.c_str());
            textActor1->GetTextProperty()->SetFontSize(fontSize);
            textActor1->GetTextProperty()->SetColor(colors->GetColor3d("Red").GetData());
            textActor1->RotateX(90);

            textPosPt[0] += lineText.size()  * fontSize / 2 + 2;

            const vtkSmartPointer<vtkTextActor3D> textActor2 { vtkTextActor3D::New() };
            textActor2->SetPosition(textPosPt[0], textPosPt[1], textPosPt[2]);
            textActor2->SetInput("/");
            textActor2->GetTextProperty()->SetFontSize(fontSize);
            textActor2->GetTextProperty()->SetColor(colors->GetColor3d("Red").GetData());
            textActor2->RotateX(90);

            textPosPt[0] += 5;

            const vtkSmartPointer<vtkTextActor3D> textActor3 { vtkTextActor3D::New() };
            textActor3->SetPosition(textPosPt[0], textPosPt[1], textPosPt[2]);
            textActor3->SetInput(std::to_string(distExpected).c_str());
            textActor3->GetTextProperty()->SetFontSize(fontSize);
            textActor3->GetTextProperty()->SetColor(colors->GetColor3d("Green").GetData());
            textActor3->RotateX(90);*/
        }

        renderer->AddActor(actorPoints);
        renderer->AddActor(actorTeeth);

        vtkSmartPointer<vtkRenderWindow> window { vtkRenderWindow::New() };
        window->SetSize(1200, 800);
        window->SetPosition(250, 100);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK Window");

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor { vtkRenderWindowInteractor::New() };
        renderWindowInteractor->SetRenderWindow(window);

        window->Render();
        renderWindowInteractor->Start();
    }

    void Visualize_Estimation_Pont_FromContactPoints()
    {
        constexpr std::string_view teethObjFile {
            R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/2878_teeth.obj)"};
        constexpr std::string_view teethUpperStl {
                R"(/home/andtokm/Projects/data/cases/2878/automodeling/crowns/2878_upper.stl)"};
        constexpr std::string_view teethLowerStl {
                R"(/home/andtokm/Projects/data/cases/2878/automodeling/crowns/2878_lower.stl)"};

        const ContactPoints::Points& contactPoints = ContactPoints::Parse(
           R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/contact_points_2878.json)");
        const TreatmentPlan::Plan& plan = TreatmentPlan::Parser::Parse(
                R"(/home/andtokm/Projects/data/cases/2878/Treatment plan_01_2021-07-15-15:43:00.json)");

        std::vector<Types::Point<3, double>> pointsOrigin;
        for (const auto& [id, data] : plan.modellingData.tooth) {
            if (Utilities::isUpperTooth(id))
                pointsOrigin.push_back(data.origin);
        }

        const std::vector<Types::Point<3, double>> pointsContact = contactPoints.getUpperPoints();
        vtkSmartPointer<vtkPolyData> contactPointsData = Utilities::pointsToPolyData(pointsContact);
        contactPointsData = rotate_X_90(contactPointsData);

        // const vtkSmartPointer<vtkActor> actorTeeth =
        //        Utilities::getPolyDataActor(Utilities::readObj(teethObjFile), colors);

        vtkSmartPointer<vtkPolyData> actorTeethDataUpper = rotate_X_90(Utilities::readStl(teethUpperStl));
        const vtkSmartPointer<vtkActor> actorTeethUpper = Utilities::getPolyDataActor(actorTeethDataUpper, colors);

        // vtkSmartPointer<vtkPolyData> actorTeethDataLower = rotate_X_90(Utilities::readStl(teethLowerStl));
        // const vtkSmartPointer<vtkActor> actorTeethLower = Utilities::getPolyDataActor(actorTeethDataLower, colors);

        vtkSmartPointer<vtkPolyData> originPointsData = Utilities::pointsToPolyData(pointsOrigin);
        originPointsData = transformData(originPointsData);
        // originPointsData = rotate_X_90(originPointsData);


        const vtkSmartPointer<vtkActor> actorContactPoint = Utilities::getPolyDataActor(contactPointsData, colors);
        actorContactPoint->GetProperty()->SetPointSize(12);
        actorContactPoint->GetProperty()->RenderPointsAsSpheresOn();
        actorContactPoint->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

        const vtkSmartPointer<vtkActor> actorOriginPoint = Utilities::getPolyDataActor(originPointsData, colors);
        actorOriginPoint->GetProperty()->SetPointSize(12);
        actorOriginPoint->GetProperty()->RenderPointsAsSpheresOn();
        actorOriginPoint->GetProperty()->SetColor(colors->GetColor3d("Green").GetData());

        Utilities::DisplayActors({actorContactPoint, actorOriginPoint, actorTeethUpper}, colors);
    }

    void Visualize_Tooth_OBJ_Data() {
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                ReadTeethObj(R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/2878_teeth.obj)");

        const vtkSmartPointer<vtkAppendPolyData> dataAppender { vtkAppendPolyData::New() };
        for (const auto& [id, data] : toothMap)
            if (Utilities::isUpperTooth(id))
                dataAppender->AddInputData(data);
        dataAppender->Update();

        Utilities::visualize(dataAppender->GetOutput());
    }

    void Visualize_Estimation_Pont_FromContactPoints_And_OBJ_File()
    {
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                ReadTeethObj(R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/2878_teeth.obj)");
        const ContactPoints::Points& contactPoints = ContactPoints::Parse(
                R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/contact_points_2878.json)");
        const TreatmentPlan::Plan& plan = TreatmentPlan::Parser::Parse(
                R"(/home/andtokm/Projects/data/cases/2878/Treatment plan_01_2021-07-15-15:43:00.json)");

        // Get Upper tooth poly data:
        const vtkSmartPointer<vtkAppendPolyData> dataAppender { vtkAppendPolyData::New() };
        for (const auto& [id, data] : toothMap)
            if (Utilities::isUpperTooth(id))
                dataAppender->AddInputData(data);
        dataAppender->Update();

        // Contact points:
        const std::vector<Types::Point<3, double>> pointsContact = contactPoints.getUpperPoints();
        vtkSmartPointer<vtkPolyData> contactPointsData = Utilities::pointsToPolyData(pointsContact);
        contactPointsData = rotate_X_90(contactPointsData);

        // Origin points:
        std::vector<Types::Point<3, double>> pointsOrigin;
        for (const auto& [id, data] : plan.modellingData.tooth) {
            if (Utilities::isUpperTooth(id))
                pointsOrigin.push_back(data.origin);
        }

        // Center points:
        std::vector<Types::Point<3, double>> centerPoints;
        for (const auto& [id, data] : toothMap) {
            if (Utilities::isUpperTooth(id))
                data->GetCenter(centerPoints.emplace_back().data());
        }

        const vtkSmartPointer<vtkPolyData> teethDataUpper = rotate_X_90(dataAppender->GetOutput());
        const vtkSmartPointer<vtkActor> actorTeeth =
                Utilities::getPolyDataActor(teethDataUpper, colors);

        // Center points actor:
        const vtkSmartPointer<vtkPolyData> centerPointsData =
                rotate_X_90(Utilities::pointsToPolyData(centerPoints));
        const vtkSmartPointer<vtkActor> centerPointsActor =
                Utilities::getPolyDataActor(centerPointsData, colors);
        centerPointsActor->GetProperty()->SetPointSize(12);
        centerPointsActor->GetProperty()->RenderPointsAsSpheresOn();
        centerPointsActor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

        // Contact points actor:
        const vtkSmartPointer<vtkActor> actorContactPoint = Utilities::getPolyDataActor(contactPointsData, colors);
        actorContactPoint->GetProperty()->SetPointSize(12);
        actorContactPoint->GetProperty()->RenderPointsAsSpheresOn();
        actorContactPoint->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

        // Origin points actor:
        const vtkSmartPointer<vtkPolyData> originPointsData =
                transformData(Utilities::pointsToPolyData(pointsOrigin));
        const vtkSmartPointer<vtkActor> actorOriginPoints = Utilities::getPolyDataActor(originPointsData, colors);
        actorOriginPoints->GetProperty()->SetPointSize(12);
        actorOriginPoints->GetProperty()->RenderPointsAsSpheresOn();
        actorOriginPoints->GetProperty()->SetColor(colors->GetColor3d("Green").GetData());

        Utilities::DisplayActors({
            actorTeeth, centerPointsActor, actorContactPoint, actorOriginPoints}, colors);
    }
}

namespace DiagnosticsMethods::Demo
{
    using Types::Coordinate;


    static inline constexpr std::array<int16_t, 6> upperIncisorsIDs { 13,  12,  11, 21,  22,  23 };

    static inline constexpr std::array<std::pair<double, double>, 21> corkHouseTable { {
        { 26.5f, 15.8f},
        { 27.0f, 16.0f},
        { 27.5f, 16.3f},
        { 28.0f, 16.5f},
        { 28.5f, 16.8f},
        { 29.0f, 17.0f},
        { 29.5f, 17.3f},
        { 30.0f, 17.5f},
        { 30.5f, 17.8f},
        { 31.0f, 18.0f},
        { 31.5f, 18.3f},
        { 32.0f, 18.5f},
        { 32.5f, 18.8f},
        { 33.0f, 19.0f},
        { 33.5f, 19.3f},
        { 34.0f, 19.5f},
        { 34.5f, 19.8f},
        { 35.0f, 20.0f},
        { 35.5f, 20.5f},
        { 36.0f, 21.0f},
        { 36.5f, 21.5f}
    } };

    double GetCorkhouseDist(double dist) {
        if (dist <= corkHouseTable.front().first)
            return corkHouseTable.front().second;
        else if (dist >= corkHouseTable.back().first)
            return corkHouseTable.back().second;

        const decltype(corkHouseTable)::const_iterator iter =
                std::find_if(corkHouseTable.cbegin(), corkHouseTable.cend(), [&](const auto val) {
                    return val.first >= dist;
                });

        const decltype(corkHouseTable)::value_type& lowerBound{ *std::prev(iter) };
        const decltype(corkHouseTable)::value_type& upperBound{ *iter };
        const double ratio = (dist - lowerBound.first) / (upperBound.first - lowerBound.first);
        const double offset = (upperBound.second - lowerBound.second) * ratio;

        return lowerBound.second + offset;
    }

    vtkSmartPointer<vtkTextActor> buildTextActor(std::string_view text,
                                                 Types::Point<2, double> position,
                                                 std::string colorName = "Gray",
                                                 const size_t fontSize = 24) {
        vtkSmartPointer<vtkTextActor> actor{ vtkTextActor::New() };
        actor->SetInput(text.data());
        actor->SetPosition(position[0], position[1]);
        actor->GetTextProperty()->SetFontSize(static_cast<int>(fontSize));
        actor->GetTextProperty()->SetColor(colors->GetColor3d(colorName).GetData());
        return actor;
    }

    vtkSmartPointer<vtkPolyData>
    getUpperTeethFromOBJTeethMap(const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>>& toothMap) {
        const vtkSmartPointer<vtkAppendPolyData> dataAppender{ vtkAppendPolyData::New() };
        for (const auto& [id, data] : toothMap) {
            if (Utilities::isUpperTooth(id))
                dataAppender->AddInputData(data);
        }
        dataAppender->Update();
        return dataAppender->GetOutput();
    }

    // Returns sorted (by X) vector of contact points:
    std::vector<Types::Point<3, double>>
    getUpperContactPointFromFile(std::string_view filePath) noexcept {
        const ContactPoints::Points& contactPoints = ContactPoints::Parse(filePath);
        std::vector<Types::Point<3, double>> points;
        for (const auto& [id, pts] : contactPoints.teethContactPoints) {
            if (Utilities::isUpperTooth(id) &&
                upperIncisorsIDs.end() != std::find(upperIncisorsIDs.cbegin(), upperIncisorsIDs.cend(), id)) {
                points.push_back(pts.pt1);
                points.push_back(pts.pt2);
            }
        }

        // Sort by X Coordinate:
        std::sort(points.begin(), points.end(), [](const auto& pt1, const auto& pt2) {
            return pt1[0] < pt2[0];
        });

        const auto xAvg = std::abs(points.front()[0] - points.back()[0]) / 2;
        const auto xTemp = xAvg / 6;  // 1/4 part of the tooth

        // TODO: Skip points located too close on the X axe
        std::vector<Types::Point<3, double>> pointsUnique;
        for (const auto& pt : points) {
            const double x = pt[0];
            const bool b = std::any_of(pointsUnique.cbegin(), pointsUnique.cend(), [&](const auto& p) {
                return (p[0] > (x - xTemp)) && (p[0] < (x + xTemp));
            });
            if (false == b)
                pointsUnique.push_back(pt);
        }
        return pointsUnique;
    }

    double calcIncisorsLength(const std::vector<Types::Point<3, double>>& incisors) noexcept {
        double incisorsDist{ 0 };
        for (size_t i = 1; i < incisors.size(); ++i) {
            const double x = incisors[i - 1][0] - incisors[i][0];
            // const double y = incisors[i - 1][1] - incisors[i][1];
            const double z = incisors[i - 1][2] - incisors[i][2];
            incisorsDist += std::hypot(x, z);
        }
        return incisorsDist;
    }

    std::unordered_map<unsigned short, Types::Point<3, double>>
    getCenterPoints(const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>>& toothMap) noexcept {
        std::unordered_map<unsigned short, Types::Point<3, double>> centersMap;
        Types::Point<3, double> ptCenter;
        for (const auto& [id, data] : toothMap) {
            if (Utilities::isUpperTooth(id)) {
                data->GetCenter(ptCenter.data());
                centersMap.emplace(id, ptCenter);
            }
        }
        return centersMap;
    }

    Types::Line<3>
    getLineByToothIDs(const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>>& toothMap,
                      const unsigned short id1,
                      const unsigned short id2) noexcept {
        Types::Point<3, double> pt1, pt2;
        if (const auto iter = toothMap.find(id1); toothMap.end() != iter) {
            iter->second->GetCenter(pt1.data());
        }
        if (const auto iter = toothMap.find(id2); toothMap.end() != iter) {
            iter->second->GetCenter(pt2.data());
        }
        return { pt1, pt2 };
    }

    // FIXME: there is a problem in the preprocessing module: it sometimes incorrectly classifies teeth.
    // For example , in the case of 2878 , it is being formed .obj file missed
    // teeth 15 and 25 (instead of them 16 and 26 at once)
    void PresentationDemo([[maybe_unused]] const std::vector<std::string_view>& params)
    {
        std::string caseId = "2280";
        if (!params.empty())
            caseId = params[0];

        std::string automodelingDir {R"(/automodeling/out/)"};
        if (params.size() > 1)
            automodelingDir = {R"(/automodeling2/out/)"};

        // std::string rootFolderOrig { R"(/home/andtokm/Projects/data/cases/)" };
        // rootFolderOrig.append(caseId).append(R"(/automodeling/out/)");

        std::string rootFolder { R"(/home/andtokm/Projects/data/cases/)" };
        rootFolder.append(caseId).append(automodelingDir);

        const std::string toothObjFile{ rootFolder + caseId + "_teeth.obj" };

        const std::string contactPointsFile { rootFolder + "contact_points_" + caseId + ".json" };
        // const std::string contactPointsFileOrig { rootFolderOrig + "contact_points_" + caseId + ".json" };


        //========================================================================================================//


        // Get Upper tooth poly data:
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap = ReadTeethObj(toothObjFile);
        const vtkSmartPointer<vtkPolyData> teethDataUpper = getUpperTeethFromOBJTeethMap(toothMap);

        // Contact points:
        const std::vector<Types::Point<3, double>> contactPoints = getUpperContactPointFromFile(contactPointsFile);
        // const std::vector<Types::Point<3, double>> contactPointsOrig = getUpperContactPointFromFile(contactPointsFileOrig);

        // Get incisors contact points:
        const std::vector<Types::Point<3, double>> incisors = [&]() {
            std::vector<Types::Point<3, double>> pts;
            for (size_t i = 1; i < contactPoints.size() - 1; ++i)
                pts.push_back(contactPoints[i]);
            return pts;
        }();

        // Get incisors contact points (Original):
        /*
        const std::vector<Types::Point<3, double>> incisorsOrig = [&]() {
            std::vector<Types::Point<3, double>> pts;
            for (size_t i = 1; i < contactPointsOrig.size() - 1; ++i)
                pts.push_back(contactPointsOrig[i]);
            return pts;
        }();
        */

        // Center points:
        // const std::unordered_map<unsigned short, Types::Point<3, double>> centerPointsMap = getCenterPoints(toothMap);

        // Calculate the incisors length:
        const double incisorsLength = calcIncisorsLength(incisors);

        // Premolar and molars lines points:
        // const Types::Line<3> premolarLine = getLineByToothIDs(toothMap, 14, 24);
        // const Types::Line<3> molarLine = getLineByToothIDs(toothMap, 16, 26);

        Types::Point<3, double> tooth14, tooth24;
        if (const auto iter = toothMap.find(14); toothMap.end() != iter) {
            iter->second->GetCenter(tooth14.data());
        }
        if (const auto iter = toothMap.find(24); toothMap.end() != iter) {
            iter->second->GetCenter(tooth24.data());
        }

        Types::Point<3, double> tooth16, tooth26;
        if (const auto iter = toothMap.find(16); toothMap.end() != iter) {
            iter->second->GetCenter(tooth16.data());
        }
        if (const auto iter = toothMap.find(26); toothMap.end() != iter) {
            iter->second->GetCenter(tooth26.data());
        }

        Types::Point<3, double> center { contactPoints [3]};

        // TODO: Use tooth Features / Fissures to get the points right
        tooth16[Coordinate::X] -= 0.5; tooth16[Coordinate::Y] -= 2; tooth16[Coordinate::Z] += 1.5;
        tooth26[Coordinate::X] += 0.5; tooth26[Coordinate::Y] -= 2; tooth26[Coordinate::Z] += 1.5;
        tooth14[Coordinate::Y] -= 2.0; tooth24[Coordinate::Y] -= 2;
        center[Coordinate::Y] -= 4.0;

        const Types::Line<3> premolarLine {tooth14, tooth24};
        const Types::Line<3> molarLine {tooth16, tooth26};


        const double premolarDistanceActual { premolarLine.getLength() };
        const double premolarDistanceExpected { incisorsLength * 100 / 85 };
        const double molarDistanceActual { molarLine.getLength() };
        const double molarDistanceExpected { incisorsLength * 100 / 65 };

        std::cout << "Incisors length: " << incisorsLength << std::endl;
        std::cout << "Premolar length: " << premolarDistanceActual
                  << ". Exepected: " << premolarDistanceExpected << std::endl;
        std::cout << "Molar length   : " << molarDistanceActual
                  << ". Exepected: " << molarDistanceExpected << std::endl;

        double t;



        Types::Point<3, double> closest;
        double distSquread = vtkLine::DistanceToLine(center.data(), tooth14.data(), tooth24.data(), t, closest.data());
        /** Find the squared distance between the points. **/
        const double distance = center.distanceTo(closest);
        std::cout << "Corkhouse distances: " << distance << std::endl
                  << "   Actual   : " << distance
                  << "   Expected : " << GetCorkhouseDist(incisorsLength) << std::endl;


        // -------------------------------- Visualization part ---------------------------------------//

        constexpr size_t fontSize{ 16 };
        const vtkSmartPointer<vtkRenderer> renderer{ vtkRenderer::New() };
        renderer->SetBackground(colors->GetColor3d("DarkBlue").GetData());



        // Contact points:
        {
            std::vector<Types::Point<3, double> > pts {incisors};
            for (size_t i = 0; i < pts.size(); ++i)
                pts[i][Coordinate::Y] = pts[0][Coordinate::Y];

            auto contactPointsActor = Utilities::getPointsActor(pts, colors);
            contactPointsActor->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());
            renderer->AddActor(contactPointsActor);

            for (size_t i = 1; i < pts.size(); ++i) {
                const auto actor = getLineActor({pts[i-1], pts[i]}, colors);
                renderer->AddActor(actor);
            }
        }

        // First premolar distance text data:
        {
            Types::Point<2, double> pt{ 5, 160 };
            auto methodActor = buildTextActor("Upper incisors length:     ", pt, "Gray", fontSize);
            methodActor->GetTextProperty()->ItalicOn();

            pt[0] += 230;
            auto valueActor = buildTextActor(std::to_string(incisorsLength), pt, "Gray", fontSize);
            valueActor->GetTextProperty()->BoldOn();
            valueActor->GetTextProperty()->SetColor(0.7, 0.7, 0.3);

            renderer->AddActor2D(methodActor);
            renderer->AddActor2D(valueActor);
        }


        // First premolar distance text data:
        {
            Types::Point<2, double> pt{ 5, 125 };
            vtkSmartPointer<vtkTextActor> methodActor = buildTextActor("Pont method: ", pt, "Gray", fontSize);
            methodActor->GetTextProperty()->SetColor(0.8, 0.8, 0.1);
            methodActor->GetTextProperty()->BoldOn();

            pt[1] -= 25;
            std::string text{ "      First premolar distance: " };
            vtkSmartPointer<vtkTextActor> metricNameActor = buildTextActor(text, pt, "Gray", fontSize);

            pt[0] += text.length() * fontSize / 2.15;
            vtkSmartPointer<vtkTextActor> textActorActual =
                    buildTextActor(std::to_string(premolarDistanceActual), pt, "Red", fontSize);

            pt[0] += 80;
            vtkSmartPointer<vtkTextActor> textActorDelimiter = buildTextActor(" / ", pt, "Gray", fontSize);

            pt[0] += 20;
            vtkSmartPointer<vtkTextActor> textActorExpected =
                    buildTextActor(std::to_string(premolarDistanceExpected), pt, "Green", fontSize);

            renderer->AddActor2D(methodActor);
            renderer->AddActor2D(metricNameActor);
            renderer->AddActor2D(textActorActual);
            renderer->AddActor2D(textActorDelimiter);
            renderer->AddActor2D(textActorExpected);
        }

        // Molar distance text data:
        {

            Types::Point<2, double> pt{ 5, 75 };
            std::string text{ "      Molar distance: " };
            vtkSmartPointer<vtkTextActor> metricNameActor = buildTextActor(text, pt, "Gray", fontSize);

            pt[0] += text.length() * fontSize / 2.15 + 67;
            vtkSmartPointer<vtkTextActor> textActorActual =
                    buildTextActor(std::to_string(molarDistanceActual), pt, "Red", fontSize);

            pt[0] += 80;
            vtkSmartPointer<vtkTextActor> textActorDelimiter = buildTextActor(" / ", pt, "Gray", fontSize);

            pt[0] += 20;
            vtkSmartPointer<vtkTextActor> textActorExpected =
                    buildTextActor(std::to_string(molarDistanceExpected), pt, "Green", fontSize);

            renderer->AddActor2D(metricNameActor);
            renderer->AddActor2D(textActorActual);
            renderer->AddActor2D(textActorDelimiter);
            renderer->AddActor2D(textActorExpected);
        }

        {

            Types::Point<2, double> pt{ 5, 40 };
            vtkSmartPointer<vtkTextActor> methodActor = buildTextActor("Korkhause method: ", pt, "Gray", fontSize);
            methodActor->GetTextProperty()->SetColor(0.8, 0.8, 0.1);
            methodActor->GetTextProperty()->BoldOn();

            pt[1] -= 25;
            std::string text{ "      Distance to incisors   : " };
            vtkSmartPointer<vtkTextActor> metricNameActor = buildTextActor(text, pt, "Gray", fontSize);

            pt[0] += text.length() * fontSize / 2.15;
            vtkSmartPointer<vtkTextActor> textActorActual =
                    buildTextActor(std::to_string(distance), pt, "Red", fontSize);

            pt[0] += 80;
            vtkSmartPointer<vtkTextActor> textActorDelimiter = buildTextActor(" / ", pt, "Gray", fontSize);

            pt[0] += 20;
            const double distExpected = GetCorkhouseDist(incisorsLength);
            vtkSmartPointer<vtkTextActor> textActorExpected =
                    buildTextActor(std::to_string(distExpected), pt, "Green", fontSize);

            renderer->AddActor2D(methodActor);
            renderer->AddActor2D(metricNameActor);
            renderer->AddActor2D(textActorActual);
            renderer->AddActor2D(textActorDelimiter);
            renderer->AddActor2D(textActorExpected);
        }


        // Teeth actor:
        const vtkSmartPointer<vtkActor> actorTeeth = Utilities::getPolyDataActor(teethDataUpper, colors);

        // Contact points poly data:
        const vtkSmartPointer<vtkPolyData> contactPointsData = Utilities::pointsToPolyData(contactPoints);

        // Contact points actor:
        const vtkSmartPointer<vtkActor> actorContactPoint = Utilities::getPolyDataActor(contactPointsData, colors);
        actorContactPoint->GetProperty()->SetPointSize(12);
        actorContactPoint->GetProperty()->RenderPointsAsSpheresOn();
        actorContactPoint->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

        const auto premolarsLineActors = getLineActorEx(premolarLine, colors);
        premolarsLineActors.front()->GetProperty()->SetColor(colors->GetColor3d("Purple").GetData()); //Line color

        const auto molarsLineActors = getLineActorEx(molarLine, colors);
        molarsLineActors.front()->GetProperty()->SetColor(colors->GetColor3d("Purple").GetData());

        const auto corkHauseLineActors = getLineActorEx({ center , closest }, colors);
        corkHauseLineActors.front()->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());



        renderer->AddActor(actorTeeth);
        // renderer->AddActor(actorContactPoint);
        renderer->AddActor(premolarsLineActors[0]);
        renderer->AddActor(premolarsLineActors[1]);
        renderer->AddActor(molarsLineActors[0]);
        renderer->AddActor(molarsLineActors[1]);
        renderer->AddActor(corkHauseLineActors[0]);
        renderer->AddActor(corkHauseLineActors[1]);

        vtkSmartPointer<vtkRenderWindow> window{ vtkRenderWindow::New() };
        window->SetSize(1200, 800);
        window->SetPosition(250, 100);
        window->AddRenderer(renderer);
        window->SetWindowName("VTK Window");

        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor{ vtkRenderWindowInteractor::New() };
        renderWindowInteractor->SetRenderWindow(window);

        window->Render();
        renderWindowInteractor->Start();

        /*
        Utilities::DisplayActors({actorTeeth,
                                  // actorContactPoint,
                                  premolarsLineActors[0], premolarsLineActors[1],
                                  molarsLineActors[0], molarsLineActors[1],
                                  corkHauseLineActors[0], corkHauseLineActors[1]}, colors);
        */
    }
}

namespace DiagnosticsMethods::Measurements {

    void MeasureToothLength()
    {
        constexpr std::string_view toothFilePath { R"(/home/andtokm/Projects/data/out/Tooths/tooth_8.stl)" };
        constexpr std::string_view teetsObjFilePath {
            R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/2878_teeth.obj)" };

        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> teethPolyData =
                ReadTeethObj(teetsObjFilePath);

        // const vtkSmartPointer<vtkPolyData> toothPolyData = Utilities::readStl(toothFilePath);
        vtkSmartPointer<vtkPolyData> toothPolyData = teethPolyData.find(21)->second;

        vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        // transform->RotateX(-25);
        // transform->RotateY(180);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter { vtkTransformPolyDataFilter::New() };
        transformFilter->SetInputData(toothPolyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();
        toothPolyData = transformFilter->GetOutput();

        Types::Point<3, double> center;
        toothPolyData->GetCenter(center.data());

        vtkSmartPointer<vtkAxes> axes { vtkAxes::New() };
        axes->SetScaleFactor(20);
        axes->SetOrigin(center.data());
        axes->Update();


        vtkSmartPointer<vtkTransform> transformAxes { vtkTransform::New() };
        transformAxes->RotateX(0.08728908249141087 * 180);
        transformAxes->RotateY(-0.9159658857497649 * 180);
        transformAxes->RotateZ(0.39164666757226013 * 180);
        // transformAxes->RotateY(180);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilterAxes { vtkTransformPolyDataFilter::New() };
        transformFilterAxes->SetInputData(axes->GetOutput());
        transformFilterAxes->SetTransform(transformAxes);
        transformFilterAxes->Update();



        const auto axisActor = Utilities::getPolyDataActor(transformFilterAxes->GetOutput(), colors);
        axisActor->GetProperty()->SetLineWidth(4);
        axisActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

        const auto toothActor = Utilities::getPolyDataActor(toothPolyData, colors);
        Utilities::DisplayActors({axisActor, toothActor}, colors);


        // [0.08728908249141087, -0.9159658857497649, 0.39164666757226013]
    }

}

void DiagnosticsMethods::TestAll([[maybe_unused]] const std::vector<std::string_view>& params)
{
    // Pont::ReadTeeth_FindDistances();
    // Pont::CheckDistances_Manual();

    // Corkhouse::CheckDistances_Manual();

    // VisualizeDiagnosticsResults::DrawLineWithTextSizes();
    // VisualizeDiagnosticsResults::Visualize_Estimation_Pont();
    // VisualizeDiagnosticsResults::Visualize_Estimation_Pont_FromContactPoints();
    // VisualizeDiagnosticsResults::Visualize_Tooth_OBJ_Data();
    // VisualizeDiagnosticsResults::Visualize_Estimation_Pont_FromContactPoints_And_OBJ_File();

    Demo::PresentationDemo(params);

    // Measurements::MeasureToothLength();
}