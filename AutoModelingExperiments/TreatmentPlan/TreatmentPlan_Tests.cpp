//============================================================================
// Name        : TreatmentPlanTests.cpp
// Created on  : 18.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Automodeling Configuration parser
//============================================================================

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

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
// #include <vtksys/SystemTools.hxx>
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

#include "TreatmentPlan.h"
#include "../Utilities.h"

namespace {
    constexpr std::string_view TREATMENT_PLAN_JSON_FILE {
            R"(/home/andtokm/Projects/data/cases/2333/Treatment plan_01_2021-02-18-22:40:50.json)" };

    constexpr std::string_view TEETH_OBJ_FILE_2333 {
            R"(/home/andtokm/Projects/data/cases/2333/models/f7e0_scan_crown.obj)" };

    const vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };
}


namespace TreatmentPlan_Tests {
    // using TreatmentPlan::Point3d;
    using namespace Types;
}


namespace TreatmentPlan_Tests::Utils {
    // TODO: Move if somewhere else
    constexpr float degToRad(float angle) {
        constexpr float M = M_PI / 180;
        return angle * M;
    };

    vtkSmartPointer<vtkMatrix4x4> GetRotationMatrixX(float angle) {
        const float cosinus = std::cos(degToRad(angle));
        const float sinus   = std::sin(degToRad(angle));

        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        matrix->SetElement(1, 1, cosinus);
        matrix->SetElement(1, 2, -sinus);
        matrix->SetElement(2, 1, sinus);
        matrix->SetElement(2, 2, cosinus);
        return matrix;
    }

    vtkSmartPointer<vtkMatrix4x4> GetRotationMatrixY(float angle) {
        const float cosinus = std::cos(degToRad(angle));
        const float sinus   = std::sin(degToRad(angle));

        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        matrix->SetElement(0, 0, cosinus);
        matrix->SetElement(0, 2, sinus);
        matrix->SetElement(2, 0, -sinus);
        matrix->SetElement(2, 2, cosinus);
        return matrix;
    }

    vtkSmartPointer<vtkMatrix4x4> GetRotationMatrixZ(float angle) {
        const float cosinus = std::cos(degToRad(angle));
        const float sinus   = std::sin(degToRad(angle));

        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        matrix->SetElement(0, 0, cosinus);
        matrix->SetElement(0, 1, -sinus);
        matrix->SetElement(1, 0, sinus);
        matrix->SetElement(1, 1, cosinus);
        return matrix;
    }

    vtkSmartPointer<vtkPolyData> transformData(const vtkSmartPointer<vtkPolyData> polyData) {
        const vtkSmartPointer<vtkMatrix4x4> mX = GetRotationMatrixX(90);
        const vtkSmartPointer<vtkMatrix4x4> mY = GetRotationMatrixY(180);
        const vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        vtkMatrix4x4::Multiply4x4(mX.Get(), mY.Get(), matrix.Get());

        vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        transform->SetMatrix(matrix);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter { vtkTransformPolyDataFilter::New() };
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();
        return transformFilter->GetOutput();
    }
}

namespace TreatmentPlan_Tests::Visualization {

    vtkSmartPointer<vtkActor> getPolyDataActor(std::string_view filePath,
                                               [[maybe_unused]] const vtkSmartPointer<vtkNamedColors>& colors)
    {
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readObj(filePath);

        // const vtkSmartPointer<vtkMatrix4x4> mX = MatrixUtils::GetRotationMatrixX(90);
        // const vtkSmartPointer<vtkMatrix4x4> mY = MatrixUtils::GetRotationMatrixY(180);
        // vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        // vtkMatrix4x4::Multiply4x4(mX.Get(), mY.Get(), matrix.Get());

        vtkSmartPointer<vtkTransform> transform { vtkTransform::New() };
        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter { vtkTransformPolyDataFilter::New() };
        // transform->SetMatrix(matrix);

        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        mapper->SetInputData(transformFilter->GetOutput());

        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        actor->SetMapper(mapper);
        //actor->GetProperty()->SetDiffuse(0.8);
        // actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("LightSteelBlue").GetData());
        // actor->GetProperty()->SetSpecular(0.3);
        // actor->GetProperty()->SetSpecularPower(60.0);

        return actor;
    }

    void Test_2333() {
        //  2333 modellingData -> origins
        std::vector<Point<3, double>> points {
                { 4.240545, 23.378765, 3.91348 },
                { 11.825355, 21.398654999999999, 4.511089999999999},
                { 17.44469, 16.351195, 5.148149999999999 },
                { 19.90663, 8.46239, 4.456305},
                { 23.08021, -0.547425, 5.92237 },
                { 26.171505000000005, -10.670755, 5.686515 },
                { 26.29981, -21.06691, 7.239615},
                { -4.508735, 23.038915, 3.9823199999999998},
                { -11.731365, 20.482745, 4.528605},
                { -16.53941, 14.682765, 5.1518500000000009},
                { -19.09662, 6.991885, 5.1114500000000009},
                { -22.637755, -2.360515, 5.9980850000000009 },
                { -26.00975, -12.402925, 6.553205},
                { -27.2658, -22.19812, 8.464034999999999},
                { -2.4193, 19.58705, -1.8008399999999998},
                { -7.833255, 18.017585, -1.304735},
                { -13.366905000000001, 15.147120000000001, -2.08092},
                { -16.45844, 7.7731650000000009, -0.5639949999999998},
                { -20.03951, -1.0079149999999997, 0.6784399999999999 },
                { -23.72306, -12.42989, 1.67645},
                { -25.95383, -24.656275, 3.944705},
                { 2.819625, 18.99276, -0.7857400000000001},
                { 8.285695, 18.569545, -1.23414},
                { 13.160810000000002, 14.739325000000001, -2.1419300000000005},
                { 16.81943, 7.633, -0.9731599999999998 },
                { 20.927465, -0.8684000000000003, 0.3722549999999998},
                { 23.69798, -12.83745, 1.7548499999999999},
                { 24.286385, -25.229155, 4.038515}
        };

        for (const auto& pt: points)
            std::cout << pt << std::endl;


        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(points, colors);
        vtkSmartPointer<vtkActor> actorTeeth = getPolyDataActor(TEETH_OBJ_FILE_2333, colors);

        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeeth}, colors);
    }

    void TeethAndOriginPoints(const TreatmentPlan::Plan& plan) {
        std::vector<Point<3, double>> points;
        points.reserve(plan.modellingData.tooth.size());
        for (const auto& tooth: plan.modellingData.tooth)
            points.push_back(tooth.second.origin);

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(points, colors);
        vtkSmartPointer<vtkActor> actorTeeth = getPolyDataActor(TEETH_OBJ_FILE_2333, colors);

        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeeth}, colors);
    }

    void TeethAndPoints_Test(const TreatmentPlan::Plan& plan) {
        std::vector<Point<3, double>> points;
        points.reserve(plan.modellingData.tooth.size());
        for (const auto& tooth: plan.modellingData.tooth)
            points.push_back(tooth.second.anchor_point_1);

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(points, colors);
        vtkSmartPointer<vtkActor> actorTeeth = getPolyDataActor(TEETH_OBJ_FILE_2333, colors);
        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeeth}, colors);
    }

    void _visualyzePoints(const std::vector<Types::Point<3, double>>& pts)
    {
        vtkSmartPointer<vtkPoints> points { vtkPoints::New() };
        for (const auto& pt : pts)
            points->InsertNextPoint(pt.data());

        vtkSmartPointer<vtkPolyData> linesPolyData { vtkPolyData::New() };
        linesPolyData->SetPoints(points);

        std::vector<vtkSmartPointer<vtkLine>> lines;
        for (size_t i = 0; i < pts.size(); i += 2) {
            const auto line = lines.emplace_back(vtkNew<vtkLine>());
            line->GetPointIds()->SetId(0, i);
            line->GetPointIds()->SetId(1, i + 1);
        }

        const vtkSmartPointer<vtkCellArray> cellArray { vtkCellArray::New() };
        for (const auto& line : lines)
            cellArray->InsertNextCell(line);

        linesPolyData->SetLines(cellArray);

        //=============================================================


        vtkSmartPointer<vtkPolyDataMapper> mapper { vtkPolyDataMapper::New() };
        mapper->SetInputData(linesPolyData);

        vtkSmartPointer<vtkActor> actor { vtkActor::New() };
        actor->SetMapper(mapper);
        actor->GetProperty()->SetLineWidth(4);
        actor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

        Utilities::DisplayActors({actor}, colors);
    }

    void DrawToothSegmets(const TreatmentPlan::Plan& plan)
    {
        std::vector<Point<3, double>> points;
        points.reserve(plan.modellingData.tooth.size());
        for (const auto& tooth : plan.modellingData.tooth)
            points.push_back(tooth.second.origin);

        _visualyzePoints(points);
    }
}

namespace TreatmentPlan_Tests::FormatsConvertation
{
    constexpr std::string_view TEETH_STL_FILE_LOWER_2333 {
            R"(/home/andtokm/Projects/data/cases/2333/automodeling/crowns/2333_lower.stl)"
    };

    constexpr std::string_view TEETH_STL_FILE_UPPER_2333 {
            R"(/home/andtokm/Projects/data/cases/2333/automodeling/crowns/2333_upper.stl)"
    };

    //-----------------------------------------------------------------------------------------------------

    void OriginPoints_TransformTeethData(const TreatmentPlan::Plan& plan) {
        std::vector<Point<3, double>> points;
        points.reserve(plan.modellingData.tooth.size());
        for (const auto& tooth: plan.modellingData.tooth)
            points.push_back(tooth.second.origin);

        const vtkSmartPointer<vtkPolyData> lowerTeethData =
                Utils::transformData(Utilities::readStl(TEETH_STL_FILE_LOWER_2333));
        const vtkSmartPointer<vtkPolyData> upperTeethData =
                Utils::transformData(Utilities::readStl(TEETH_STL_FILE_UPPER_2333));

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(points, colors);
        vtkSmartPointer<vtkActor> actorTeethLower = Utilities::getPolyDataActor(lowerTeethData, colors);
        vtkSmartPointer<vtkActor> actorTeethUpper = Utilities::getPolyDataActor(upperTeethData, colors);

        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeethLower, actorTeethUpper}, colors);
    }

    void OriginPoints_TransformPoints(const TreatmentPlan::Plan& plan) {
        std::vector<Point<3, double>> points;
        points.reserve(plan.modellingData.tooth.size());
        for (const auto& tooth: plan.modellingData.tooth)
            points.push_back(tooth.second.origin);

        const vtkSmartPointer<vtkPolyData> pointsData = Utilities::pointsToPolyData(points);
        const vtkSmartPointer<vtkPolyData> pointsDataTransformed = Utils::transformData(pointsData);
        const vtkSmartPointer<vtkPolyData> lowerTeethData = Utilities::readStl(TEETH_STL_FILE_LOWER_2333);
        const vtkSmartPointer<vtkPolyData> upperTeethData = Utilities::readStl(TEETH_STL_FILE_UPPER_2333);

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPolyDataActor(pointsDataTransformed, colors);
        vtkSmartPointer<vtkActor> actorTeethLower = Utilities::getPolyDataActor(lowerTeethData, colors);
        vtkSmartPointer<vtkActor> actorTeethUpper = Utilities::getPolyDataActor(upperTeethData, colors);

        actorPoint->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeethLower, actorTeethUpper}, colors);
    }
}

namespace TreatmentPlan_Tests::DisplayPointsFinal
{

    const std::string TEETH_STL_FILE_LOWER {
            R"(/home/andtokm/Projects/data/cases_ml/10174/before/crowns/10174_lower.stl)"
    };
    const std::string TEETH_STL_FILE_UPPER {
            R"(/home/andtokm/Projects/data/cases_ml/10174/before/crowns/10174_upper.stl)"
    };
    const std::string TEETH_STL_FILE_LOWER_AFTER {
            R"(/home/andtokm/Projects/data/cases_ml/10174/after/crowns/10174_lower.stl)"
    };
    const std::string TEETH_STL_FILE_UPPER_AFTER  {
            R"(/home/andtokm/Projects/data/cases_ml/10174/after/crowns/10174_upper.stl)"
    };

    const std::string JSON_FILE_PATH {
            R"(/home/andtokm/Projects/data/cases_ml/10174/Plan.json)"
    };


    void ShowPointsBefore() {
        const TreatmentPlan::Plan& plan = TreatmentPlan::Parser::Parse(JSON_FILE_PATH);
        std::vector<Point<3, double>> points;
        points.reserve(plan.modellingData.tooth.size());
        for (const auto& tooth: plan.modellingData.tooth)
            points.push_back(tooth.second.origin);

        const vtkSmartPointer<vtkPolyData> pointsData = Utilities::pointsToPolyData(points);
        const vtkSmartPointer<vtkPolyData> pointsDataTransformed = Utils::transformData(pointsData);
        const vtkSmartPointer<vtkPolyData> lowerTeethData = Utilities::readStl(TEETH_STL_FILE_LOWER);
        const vtkSmartPointer<vtkPolyData> upperTeethData = Utilities::readStl(TEETH_STL_FILE_UPPER);

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPolyDataActor(pointsDataTransformed, colors);
        vtkSmartPointer<vtkActor> actorTeethLower = Utilities::getPolyDataActor(lowerTeethData, colors);
        vtkSmartPointer<vtkActor> actorTeethUpper = Utilities::getPolyDataActor(upperTeethData, colors);

        actorPoint->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeethLower, actorTeethUpper}, colors);
    }

    void ShowPointsAfter() {
        const TreatmentPlan::Plan& plan = TreatmentPlan::Parser::Parse(JSON_FILE_PATH);
        std::vector<Point<3, double>> points;
        for (const auto& [k, v]: plan.getPointsFinal())
            points.push_back(v);

        const vtkSmartPointer<vtkPolyData> pointsData = Utilities::pointsToPolyData(points);
        const vtkSmartPointer<vtkPolyData> pointsDataTransformed = Utils::transformData(pointsData);
        const vtkSmartPointer<vtkPolyData> lowerTeethData = Utilities::readStl(TEETH_STL_FILE_LOWER_AFTER);
        const vtkSmartPointer<vtkPolyData> upperTeethData = Utilities::readStl(TEETH_STL_FILE_UPPER_AFTER);

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPolyDataActor(pointsDataTransformed, colors);
        vtkSmartPointer<vtkActor> actorTeethLower = Utilities::getPolyDataActor(lowerTeethData, colors);
        vtkSmartPointer<vtkActor> actorTeethUpper = Utilities::getPolyDataActor(upperTeethData, colors);

        actorPoint->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeethLower, actorTeethUpper}, colors);
    }
}

void TreatmentPlan_Tests::TestAll()
{
    const TreatmentPlan::Plan& plan = TreatmentPlan::Parser::Parse(TREATMENT_PLAN_JSON_FILE);

    // Visualization::Test_2333();
    // Visualization::TeethAndOriginPoints(plan);
    // Visualization::TeethAndPoints_Test(plan);
    // Visualization::DrawToothSegmets(plan);

    // FormatsConvertation::OriginPoints_TransformTeethData(plan);
    FormatsConvertation::OriginPoints_TransformPoints(plan);

    // DisplayPointsFinal::ShowPointsBefore();
    // DisplayPointsFinal::ShowPointsAfter();
}