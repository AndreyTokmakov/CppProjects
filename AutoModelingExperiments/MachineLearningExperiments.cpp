//============================================================================
// Name        : MachineLearningExperiments.cpp
// Created on  : 11.10.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Machine Learning experiments
//============================================================================

#include <vtkCutter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkOBBTree.h>
#include <vtkPlaneSource.h>
#include <vtkCenterOfMass.h>
#include <vtkPlane.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkBoundingBox.h>
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
#include <vtkOutlineFilter.h>
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

#include <random>
#include <filesystem>
#include "TreatmentPlan/TreatmentPlan.h"
#include "Utilities.h"
#include "MachineLearningExperiments.h"
#include "Types.h"

namespace {
    constexpr std::string_view TREATMENT_PLAN_JSON_FILE {
        R"(/home/andtokm/Projects/data/cases_ml/10066/Plan.json)" };

    constexpr std::string_view TOOTH_UPPER_STL_10066_BEFORE {
        R"(/home/andtokm/Projects/data/cases_ml/10066/before/crowns/10066_upper.stl)" };

    constexpr std::string_view TOOTH_UPPER_STL_10066_AFTER {
        R"(/home/andtokm/Projects/data/cases_ml/10066/after/crowns/10066_upper.stl)" };

    const vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New() };
}

namespace MachineLearningExperiments::Utils
{
    double getRandom(const double start = 0.0f,
                     const double end = 10.0f) {
        std::random_device rd{};
        auto generator = std::mt19937 { rd() };
        auto distribution = std::uniform_int_distribution<>(start, end);
        return distribution(generator);
    }

    std::map<uint16_t, Point3d> getOriginPointsUpper(const TreatmentPlan::Plan& plan) noexcept {
        std::map<uint16_t, Point3d> points;
        for (const auto& [toothId, toothData]: plan.modellingData.tooth) {
            if (Utilities::isUpperTooth(toothId))
                points.emplace(toothId, toothData.origin);
        }
        return points;
    }

    std::vector<Point3d> getOriginPointsFromFile_Upper(std::string_view filePath) noexcept {
        const TreatmentPlan::Plan& plan = TreatmentPlan::Parser::Parse(filePath);
        std::vector<Point3d> points;
        for (const auto& [toothId, toothData]: plan.modellingData.tooth) {
            if (Utilities::isUpperTooth(toothId))
                points.push_back(toothData.origin);
        }
        return points;
    }


    std::map<uint16_t, Point3d> getFinalPointsUpper(const TreatmentPlan::Plan& plan) noexcept {
        std::map<uint16_t, Point3d> points;
        for (const auto& [toothId, point]: plan.getPointsFinal()) {
            if (Utilities::isUpperTooth(toothId))
                points.emplace(toothId, point);
        }
        return points;
    }

    std::vector<Point3d> getFinalPointsFromFile_Upper(std::string_view filePath) noexcept {
        const TreatmentPlan::Plan& plan = TreatmentPlan::Parser::Parse(filePath);
        std::vector<Point3d> points;
        for (const auto& [toothId, point]: plan.getPointsFinal()) {
            if (Utilities::isUpperTooth(toothId))
                points.push_back(point);
        }
        return points;
    }

    void VisualizePoints(const std::vector<Point<3, double>>& points) {
        const vtkSmartPointer<vtkNamedColors> colors { vtkNamedColors::New()};
        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(points, colors);
        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint}, colors);
    }
}

namespace MachineLearningExperiments::VisualizationTests
{

    void TeethAndOriginPoints(const TreatmentPlan::Plan& plan) {
        const vtkSmartPointer<vtkPolyData> polyData = Utilities::readStl(TOOTH_UPPER_STL_10066_BEFORE);
        std::vector<Point<3, double>> points;
        points.reserve(plan.modellingData.tooth.size());
        for (const auto& tooth: plan.modellingData.tooth)
            points.push_back(tooth.second.origin);

        vtkSmartPointer<vtkActor> actorPoint = Utilities::getPointsActor(points, colors);
        vtkSmartPointer<vtkActor> actorTeeth = Utilities::getPolyDataActor(polyData, colors);

        actorPoint->GetProperty()->SetPointSize(12);
        actorPoint->GetProperty()->RenderPointsAsSpheresOn();

        Utilities::DisplayActors({actorPoint, actorTeeth}, colors);
    }
}

namespace MachineLearningExperiments {

    constexpr std::string_view ML_CASES_FOLDER {R"(/home/andtokm/Projects/data/cases_ml)" };


    template<typename _Ty>
    std::ostream& operator<<(std::ostream& stream, const std::vector<_Ty>& vect) {
        for (const auto& v: vect)
            stream << v << ' ';
        return stream;
    }

    std::vector<Point3d> GetAllPoints_Upper()
    {
        std::vector<Point3d> points;
        points.reserve(1000);
        for(auto const& dir_entry: std::filesystem::directory_iterator{ML_CASES_FOLDER}) {
            if (dir_entry.is_directory()) {
                for(auto const& entry: std::filesystem::directory_iterator{dir_entry}) {
                    if (entry.is_regular_file() && entry.path().has_extension() &&
                        0 == entry.path().extension().compare(".json")) {
                        const std::vector<Point3d>& pts =
                                Utils::getFinalPointsFromFile_Upper(entry.path().c_str());
                        points.insert(points.end(), pts.begin(), pts.end());
                    }
                }
            }
        }
        return points;
    }

    // Return std::vector of Points
    std::vector<std::vector<Point3d>> GetPointsVectors_Upper() {
        std::vector<std::vector<Point3d>> pointsVectors;
        for(auto const& dir_entry: std::filesystem::directory_iterator{ML_CASES_FOLDER}) {
            if (dir_entry.is_directory()) {
                for(auto const& entry: std::filesystem::directory_iterator{dir_entry}) {
                    if (entry.is_regular_file() && entry.path().has_extension() &&
                        0 == entry.path().extension().compare(".json")) {
                        pointsVectors.emplace_back(Utils::getFinalPointsFromFile_Upper(entry.path().c_str()));
                    }
                }
            }
        }
        return pointsVectors;
    }

    struct Coefficient {
        double a {};
        double b {};
        double c {};
    };

    std::ostream& operator<<(std::ostream& stream, const Coefficient& coef) {
        stream << "A: " << std::setprecision(12) << coef.a << std::endl;
        stream << "B: " << std::setprecision(12) << coef.b << std::endl;
        stream << "C: " << std::setprecision(12) << coef.c;
        return stream;
    }

    void Predict_Parabola_Fixed_Points() {
        constexpr size_t pointsCount = 100;
        const Coefficient coef { Utils::getRandom(), Utils::getRandom(), Utils::getRandom()};
        auto equationParabola = [](double x, const Coefficient& coef) {
            return coef.a * (x * x) + coef.b * x + coef.c;
        };

        const std::vector<double> x = [pointsCount]() {
            std::vector<double> data(pointsCount);
            std::iota(data.begin(), data.end(), 1);
            const auto xMax = *std::max_element(data.cbegin(), data.cend());
            VectorUtilities::Devide(data, xMax);
            return data;
        }();

        const std::vector<double> y = [&]() {
            std::vector<double> data;
            for (const auto v: x)
                data.push_back(equationParabola(v, coef));
            return data;
        }();

        Coefficient coef_predicted {Utils::getRandom(),Utils::getRandom(),Utils::getRandom()};
        std::vector<double> diff(x.size()), grad_y_pred(x.size());
        std::vector<double> tmp(x.size()), tmp2(x.size());
        constexpr double learning_rate = 1e-5;
        constexpr size_t epochs { 900'000'000};

        std::cout << coef << std::endl;

        for (size_t i = 0; i < epochs; ++i)
        {
            for (size_t i = 0; i < pointsCount; ++i) {
                diff[i] = equationParabola(x[i], coef_predicted) - y[i];
            }

            // Compute loss: Mean Squared Error
            // The sum of the squares of the difference between the assumed value and the actual
            const double loss = VectorUtilities::SquareSum(diff);
            // if (0 == i % 1000)
            //    std::cout << loss << std::endl;

            VectorUtilities::Multiply(diff, 4.0, grad_y_pred); // [ diff * 4 ]        --> grad_y_pred

            // Gradient for 'C' coefficient: Sum of 'grad_y_pred'
            const double grad_c = std::accumulate(grad_y_pred.cbegin(), grad_y_pred.cend(), 0.0);

            // [ grad_y_pred * x ] --> tmp
            VectorUtilities::Multiply(grad_y_pred, x, tmp);
            const double grad_b = std::accumulate(tmp.cbegin(), tmp.cend(), 0.0);

            // [ grad_y_pred * x * x] --> [ tmp * x] --> tm2
            VectorUtilities::Multiply(tmp, x, tmp2);
            const double grad_a = std::accumulate(tmp2.cbegin(), tmp2.cend(), 0.0);


            coef_predicted.a -= learning_rate * grad_a;
            coef_predicted.b -= learning_rate * grad_b;
            coef_predicted.c -= learning_rate * grad_c;
            if (0.001 > loss) {
                std::cout << "Iter counnt = " << i << std::endl;
                break;
            }
        }

        std::cout << coef_predicted << std::endl;
    }

    void PredictTest() {
        /** Points from Treatment plan:  **/

        /*
        const std::vector<Point<3, double>> pointsAfter =
                Utils::getFinalPointsFromFile_Upper(TREATMENT_PLAN_JSON_FILE);
        const size_t pointsCount = pointsAfter.size();
        */

        const std::vector<std::vector<Point<3, double>>> pointsAfter = GetPointsVectors_Upper();
        for (const auto& pts: pointsAfter)
             Utils::VisualizePoints(pts);

        //Utils::VisualizePoints(pointsAfter);
        return;

        /*
        auto equationParabola = [](double x, const Coefficient& coef) {
            return coef.a * (x * x) + coef.b * x + coef.c;
        };

        const std::vector<double> x = [&]() {
            std::vector<double> data;
            data.reserve(pointsAfter.size());
            for (const auto& pt: pointsAfter)
                data.push_back(pt[0]);
            const auto xMax = *std::max_element(data.cbegin(), data.cend());
            VectorUtilities::Devide(data, xMax);
            return data;
        }();

        const std::vector<double> y = [&]() {
            std::vector<double> data;
            for (const auto& pt: pointsAfter)
                data.push_back(pt[2]);
            return data;
        }();

        Coefficient coef_predicted {Utils::getRandom(),Utils::getRandom(),Utils::getRandom()};
        std::vector<double> diff(x.size()), grad_y_pred(x.size());
        std::vector<double> tmp(x.size()), tmp2(x.size());
        constexpr double learning_rate = 1e-4;
        constexpr size_t epochs { 900'000'000};


        for (size_t i = 0; i < epochs; ++i)
        {
            for (size_t i = 0; i < pointsCount; ++i) {
                diff[i] = equationParabola(x[i], coef_predicted) - y[i];
            }

            // Compute loss: Mean Squared Error
            // The sum of the squares of the difference between the assumed value and the actual
            const double loss = VectorUtilities::SquareSum(diff);
            if (0 == i % 1000)
                std::cout << loss << std::endl;

            VectorUtilities::Multiply(diff, 4.0, grad_y_pred); // [ diff * 4 ]        --> grad_y_pred

            // Gradient for 'C' coefficient: Sum of 'grad_y_pred'
            const double grad_c = std::accumulate(grad_y_pred.cbegin(), grad_y_pred.cend(), 0.0f);

            // [ grad_y_pred * x ] --> tmp
            VectorUtilities::Multiply(grad_y_pred, x, tmp);
            const double grad_b = std::accumulate(tmp.cbegin(), tmp.cend(), 0.0f);

            // [ grad_y_pred * x * x] --> [ tmp * x] --> tm2
            VectorUtilities::Multiply(tmp, x, tmp2);
            const double grad_a = std::accumulate(tmp2.cbegin(), tmp2.cend(), 0.0f);

            coef_predicted.a -= learning_rate * grad_a;
            coef_predicted.b -= learning_rate * grad_b;
            coef_predicted.c -= learning_rate * grad_c;
            if (0.001 > loss) {
                std::cout << "Iter counnt = " << i << std::endl;
                break;
            }
        }
        std::cout << coef_predicted << std::endl;

        */
    }
};

void MachineLearningExperiments::TestAll([[maybe_unused]] const std::vector<std::string_view>& params)
{
    const TreatmentPlan::Plan& plan = TreatmentPlan::Parser::Parse(TREATMENT_PLAN_JSON_FILE);

    // MachineLearningExperiments::GetPoints();
    MachineLearningExperiments::Predict_Parabola_Fixed_Points();
    // MachineLearningExperiments::PredictTest();

   // VisualizationTests::TeethAndOriginPoints(plan);
}