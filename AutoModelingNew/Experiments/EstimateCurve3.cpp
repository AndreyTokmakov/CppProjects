//============================================================================
// Name        : EstimateCurve3.h
// Created on  : 24.01.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : EstimateCurve3
//============================================================================

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <cmath>
#include <numeric>
#include <ostream>
#include <vector>
#include <algorithm>
#include <array>
#include <span>

#include "../VtkHeaders.h"
#include "../Model/FDI.h"
#include "../Model/Jaw.h"
#include "../Utilities/Utilities.h"
#include "../TreatmentPlan/TreatmentPlan.h"
#include "../Estimators/Estimator.h"
#include "SetUpTorksAndPositions.h"

#include "EstimateCurve3.h"
#include "Utils.h"
#include "Geometry.h"

using namespace std::string_literals;

namespace EstimateCurve3 {
    auto getLine2DActor(const Geometry::Line2D &line, double y = 0,
                        const std::array<double, 3> &color = {1, 0, 0}) -> decltype(auto) {
        return Utilities::getLineActor(
                {line.getPoint1().x(), y, line.getPoint1().y()},
                {line.getPoint2().x(), y, line.getPoint2().y()}, color);
    }

    // TODO: Fix bug [id1, id2] != [id2, id1]
    [[nodiscard]]
    Eigen::Vector3d getContactPoint(const vtkSmartPointer<vtkPolyData> &tooth1,
                                    const vtkSmartPointer<vtkPolyData> &tooth2) noexcept {
        const vtkSmartPointer<vtkIntersectionPolyDataFilter> booleanFilter{
                vtkSmartPointer<vtkIntersectionPolyDataFilter>::New()};
        booleanFilter->GlobalWarningDisplayOff();
        booleanFilter->SetInputData(0, tooth1);
        booleanFilter->SetInputData(1, tooth2);
        booleanFilter->Update();

        Eigen::Vector3d ptContact{};
        if (booleanFilter->GetNumberOfIntersectionPoints() > 0) {
            booleanFilter->GetOutput()->GetCenter(ptContact.data());
            return ptContact;
        }

        vtkSmartPointer<vtkPolyData> toothOne{vtkSmartPointer<vtkPolyData>::New()};
        vtkSmartPointer<vtkPolyData> toothTwo{vtkSmartPointer<vtkPolyData>::New()};
        toothOne->DeepCopy(tooth1);
        toothTwo->DeepCopy(tooth2);

        Eigen::Vector3d tooth1Center{}, tooth2Center{};
        toothOne->GetCenter(tooth1Center.data());
        toothTwo->GetCenter(tooth2Center.data());

        const auto[slopeY, interceptY] =
        Utils::getLineCoefficients({tooth1Center.x(), tooth1Center.y()},
                                   {tooth2Center.x(), tooth2Center.y()});
        const auto[slopeZ, interceptZ] =
        Utils::getLineCoefficients({tooth1Center.x(), tooth1Center.z()},
                                   {tooth2Center.x(), tooth2Center.z()});
        // TODO: Replace to Mid_Points from STD??
        const double xPos1{tooth1Center.x()}, xPos2{tooth2Center.x()};
        const double xMid{xPos2 - (xPos2 - xPos1) / 2};
        constexpr size_t stepsNumber{50};
        const double step = (xMid - xPos1) / stepsNumber;

        for (double pos = xPos1; pos < xMid; pos += step) {
            booleanFilter->SetInputData(0, toothOne);
            booleanFilter->SetInputData(1, toothTwo);
            booleanFilter->Update();
            if (booleanFilter->GetNumberOfIntersectionPoints() > 0)
                break;

            const double x_pos2{xPos2 + xPos1 - pos};
            const double y_pos1 = pos * slopeY + interceptY;
            const double y_pos2 = x_pos2 * slopeY + interceptY;
            const double z_pos1 = pos * slopeZ + interceptZ;
            const double z_pos2 = x_pos2 * slopeZ + interceptZ;

            toothOne = Utilities::setPolyDataCenter(toothOne, pos, y_pos1, z_pos1);
            toothTwo = Utilities::setPolyDataCenter(toothTwo, x_pos2, y_pos2, z_pos2);
        }

        booleanFilter->GetOutput()->GetCenter(ptContact.data());
        return ptContact;
    }

    std::unordered_map<Utils::UnorderedPair<int>, Eigen::Vector3d, Utils::PairHashUnordered<int>>
    CalcTeethContactPoints(const std::unordered_map<unsigned short, Model::Tooth> &toothMap) {
        constexpr std::array<int, 14> lowerIDs{47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};
        constexpr std::array<int, 14> upperIDs{17, 16, 15, 14, 13, 12, 11, 21, 22, 23, 24, 25, 26, 27};

        const int bucketSize = std::thread::hardware_concurrency();
        std::vector<std::vector<std::pair<int, int>>> toothPairs(bucketSize);
        for (int bucket = 0; const auto &ids: {lowerIDs, upperIDs}) {
            for (size_t size = ids.size(), i = 1; i < size; ++i) {
                toothPairs[bucket++].emplace_back(ids[i - 1], ids[i]);
                bucket = (bucketSize == bucket) ? 0 : bucket;
            }
        }

        std::mutex mtx;
        std::unordered_map<Utils::UnorderedPair<int>, Eigen::Vector3d, Utils::PairHashUnordered<int>> contactPoints;
        auto handler = [&](const std::vector<std::pair<int, int>> &bucket) {
            for (const auto &[id1, id2]: bucket) {
                const vtkSmartPointer<vtkPolyData> tooth1 = toothMap.at(id1).toothPolyData,
                        tooth2 = toothMap.at(id2).toothPolyData;
                const Eigen::Vector3d ptContact = getContactPoint(tooth1, tooth2);
                {
                    std::lock_guard<std::mutex> lock{mtx};
                    contactPoints.emplace(Utils::UnorderedPair<int>{id1, id2}, ptContact);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
        };

        std::vector<std::future<void>> workers;
        for (const auto &bucket: toothPairs)
            workers.emplace_back(std::async(std::launch::async, handler, bucket));
        std::for_each(workers.cbegin(), workers.cend(), [](const auto &task) {
            task.wait();
        });
        return contactPoints;
    }

    [[nodiscard]]
    Geometry::Point2D findClosetPoint(const Geometry::Point2D pt,
                                      const std::function<double(double)> &func) noexcept {
        // NOTE: Gradient like: Just to define the step direction
        constexpr double STEP_DEFAULT{0.01};
        const Geometry::Point2D pt1{pt.x() - 1, func(pt.x() - 1)},
                pt2{pt.x() + 1, func(pt.x() + 1)};
        const double d1 = pt.distanceTo(pt1), d2 = pt.distanceTo(pt2);
        const double step = d1 > d2 ? STEP_DEFAULT : -STEP_DEFAULT;

        double distPrev{std::numeric_limits<double>::max()};
        Geometry::Point2D ptCloset{pt};
        while (true) {
            ptCloset.y() = func(ptCloset.x());
            const double distCurrent = pt.distanceTo(ptCloset);
            if (distCurrent > distPrev)
                break;
            distPrev = std::min(distPrev, distCurrent);
            ptCloset.x() += step;
        }
        return ptCloset;
    }

    vtkSmartPointer<vtkActor> getEllipseActor(const double A, const double B) {
        auto equationEllipse = [](double x, double a, double b) -> double {
            if (std::abs(x) > a || 0 == a || 0 == b)
                return 0;
            return std::sqrt((b * b) * (1 - (x * x) / (a * a)));
        };

        vtkSmartPointer<vtkPoints> points{vtkSmartPointer<vtkPoints>::New()};
        for (double x = -A; x <= A; x += 0.5)
            points->InsertNextPoint(x, 0, equationEllipse(x, A, B));
        points->InsertNextPoint(A, 0, equationEllipse(A, A, B));

        vtkSmartPointer<vtkParametricSpline> spline{vtkSmartPointer<vtkParametricSpline>::New()};
        spline->SetPoints(points);

        vtkSmartPointer<vtkParametricFunctionSource> functionSource{
                vtkSmartPointer<vtkParametricFunctionSource>::New()};
        functionSource->SetParametricFunction(spline);
        functionSource->Update();

        return Utilities::getPolyDataActor(functionSource->GetOutput());
    }


    // TODO: Refactor
    //       1. Skip use of toothID and isRightSideTooth
    //       2. Test for performance against placeSegmentOnCurveMetrics -> 27x times faster
    std::pair<bool, double>
    placeSegmentOnCurveMetricsEx(Geometry::Line2D &line,
                                 const int toothID,
                                 const Geometry::Point2D &dest,
                                 const std::function<double(double, int, const Geometry::Point2D&)> &func) noexcept {
        line.setPoint1(dest);
        const bool outer = func(line.getPoint2().x(), toothID % 10, dest) > line.getPoint2().y() ? true : false;
        double direction = outer ? -1.0 : 1.0;
        direction *= FDI::isRightSideTooth(toothID) ? 1 : -1;

        double angle = 0;
        for (int i = 0; i < 45; ++i) { // FIXME: Magic constant 45
            line.rotateAroundPoint(line.getPoint1(), direction);
            if (0 > line.getPoint2().y())
                return std::make_pair(false, 0);
            const double yLine = line.getPoint2().y(), yCurve = func(line.getPoint2().x(), toothID, dest);
            if ((outer && yLine >= yCurve) || (!outer && yCurve >= yLine))
                break;
            angle = i;
        }
        return std::make_pair(true, direction * angle);
    }

    vtkSmartPointer<vtkPolyData> decimate(const vtkSmartPointer<vtkPolyData>& data) {

        vtkSmartPointer<vtkPolyData> toothData {vtkSmartPointer<vtkPolyData>::New()};
        toothData->DeepCopy(data);

        const vtkSmartPointer<vtkFillHolesFilter> fillHolesFilter {vtkSmartPointer<vtkFillHolesFilter> ::New()};
        fillHolesFilter->SetInputData(toothData);
        fillHolesFilter->SetHoleSize(1000.0);//1000.0
        fillHolesFilter->Update();
        toothData = fillHolesFilter->GetOutput();

        const vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter {vtkSmartPointer<vtkSmoothPolyDataFilter>::New()};
        smoothFilter->SetInputData(toothData);
        smoothFilter->SetNumberOfIterations(1);
        smoothFilter->SetRelaxationFactor(0.7);
        smoothFilter->FeatureEdgeSmoothingOff();
        smoothFilter->BoundarySmoothingOn();
        smoothFilter->Update();
        toothData = smoothFilter->GetOutput();

        while (toothData->GetNumberOfPoints() > 30) {
            const vtkSmartPointer<vtkQuadricDecimation> decimation {vtkSmartPointer<vtkQuadricDecimation>::New()};
            decimation->SetInputData(toothData);
            decimation->SetTargetReduction(0.2);
            decimation->Update();
            toothData = decimation->GetOutput();
        }

        const vtkSmartPointer<vtkPolyDataNormals> normals{vtkSmartPointer<vtkPolyDataNormals>::New()};
        normals->SetInputData(toothData);
        normals->ComputePointNormalsOn();
        normals->SplittingOff();
        normals->ConsistencyOn();
        normals->AutoOrientNormalsOff();
        normals->Update();

        return normals->GetOutput();
    }

    double getDistance(const vtkSmartPointer<vtkPolyData> &tooth1,
                       const vtkSmartPointer<vtkPolyData> &tooth2) {
        const vtkSmartPointer<vtkCleanPolyData> clean1{vtkSmartPointer<vtkCleanPolyData>::New()},
                clean2{vtkSmartPointer<vtkCleanPolyData>::New()};
        clean1->SetInputData(tooth1);
        clean2->SetInputData(tooth2);

        vtkSmartPointer<vtkDistancePolyDataFilter> distanceFilter = vtkSmartPointer<vtkDistancePolyDataFilter>::New();
        distanceFilter->SetInputConnection(0, clean1->GetOutputPort());
        distanceFilter->SetInputConnection(1, clean2->GetOutputPort());
        distanceFilter->SignedDistanceOff();
        distanceFilter->ComputeSecondDistanceOn();
        distanceFilter->Update();

        double distMin{std::numeric_limits<double>::max()};
        for (vtkDataArray *distArray: {distanceFilter->GetOutput()->GetPointData()->GetScalars(),
                                       distanceFilter->GetOutput()->GetCellData()->GetScalars(),
                                       distanceFilter->GetSecondDistanceOutput()->GetPointData()->GetScalars(),
                                       distanceFilter->GetSecondDistanceOutput()->GetCellData()->GetScalars()}) {
            const size_t size = distArray->GetNumberOfValues();
            for (size_t i = 0; i < size; ++i) {
                const double dist = *(distArray->GetTuple(i));
                distMin = std::min(dist, distMin);
            }
        }
        return distMin;
    }

    //============================================================================================

    void FitTheCurve_AlignedSegments(const std::vector<std::string_view>& params) {
        // 2280 2880 2425 2457 2494 2630 2929 2622
        const std::string caseId = params.empty() ? "2622" : std::string(params.front());
        const std::string planJsonFile = R"(/home/andtokm/Projects/data/cases/)" + caseId + R"(/Plan.json)";
        const std::string filePath = R"(/home/andtokm/Projects/data/cases/)" + caseId
                                     + R"(/automodeling/out/)" + caseId + "_teeth.obj"s;
        const std::string gumsPath = R"(/home/andtokm/Projects/data/cases/)" + caseId +
                                     + R"(/models/dc16_scan_l_orient.stl)";

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        std::unique_ptr<Estimators::Estimator> estimator{std::make_unique<Estimators::DistanceEstimator>()};
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&&[toothId, polyData]: toothMap) {
            if (auto[iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
                estimator->estimate(iter->second);
            }
        }

        // INFO: Orient and SetUp a Torks and etc:
        const std::pair<double, double> orientAngles = Utils::orientTeethToPlane(allTeeth);
        SetUpTorksAndPositions::alignTeethTorks(allTeeth);
        SetUpTorksAndPositions::alignTeethVertical(allTeeth);

        std::array<double, 6> leftUpperIncisor { allTeeth.at(31).GetBounds() };
        std::array<double, 6> rightUpperIncisor { allTeeth.at(41).GetBounds() };
        std::array<double, 6> leftMolarBounds { allTeeth.at(37).GetBounds() };
        std::array<double, 6> rightMolarBounds { allTeeth.at(47).GetBounds() };

        Eigen::Vector3d leftPreMolarCenter { allTeeth.at(34).GetCenter() };
        Eigen::Vector3d leftMolarCenter { allTeeth.at(37).GetCenter() };
        Eigen::Vector3d rightPreMolarCenter { allTeeth.at(44).GetCenter() };
        Eigen::Vector3d rightMolarCenter { allTeeth.at(47).GetCenter() };

        // INFO: Invalidate right and left centers and bounds:
        if (leftMolarCenter.x() > rightMolarCenter.x()) {
            std::swap(rightMolarCenter, leftMolarCenter);
            std::swap(rightPreMolarCenter, leftPreMolarCenter);
            std::swap(leftUpperIncisor, rightUpperIncisor);
            std::swap(leftMolarBounds, rightMolarBounds);
        }

        const double xLeft = std::min({leftMolarBounds[0], leftMolarBounds[1],
                                       rightMolarBounds[0], rightMolarBounds[1]});
        const double xRight = std::max({leftMolarBounds[0], leftMolarBounds[1],
                                        rightMolarBounds[0], rightMolarBounds[1]});
        const double zBack = std::min({leftMolarBounds[4], leftMolarBounds[5],
                                       rightMolarBounds[4], rightMolarBounds[5]});
        const double zFront = std::max({leftUpperIncisor[4], leftUpperIncisor[5],
                                        rightUpperIncisor[4], rightUpperIncisor[5]});

        // FIXME: We should use the Incisor contact point instead of (xLeft + xRight)/2
        const double xShiftCenter { std::midpoint(xLeft, xRight) };
        const double xLimit { (std::abs(leftMolarCenter.x()) + std::abs(rightMolarCenter.x()))/2};

        {
            leftPreMolarCenter += Eigen::Vector3d{-xShiftCenter, 1,  -zBack};
            leftMolarCenter += Eigen::Vector3d{-xShiftCenter, 1,  -zBack};
            rightPreMolarCenter += Eigen::Vector3d{-xShiftCenter, 1,  -zBack};
            rightMolarCenter += Eigen::Vector3d{-xShiftCenter, 1,  -zBack};
        }

        // INFO: Get line-segments from contact points:
        // INFO: + move teeth to right positions
        // TODO: 50 - size is to much! Create something else
        std::array<Geometry::Line2D, 50> segments;
        for (const auto&[toothID, tooth]: allTeeth) {
            if (FDI::isUpperTooth(toothID))
                continue;

            const vtkSmartPointer<vtkPolyData> toothData = tooth.toothPolyData;
            const Eigen::Vector3d &center{tooth.GetCenter()};
            const std::array<double, 6> &bounds{tooth.GetBounds()};

            const Model::ToothAxis &axes = tooth.axes;
            Eigen::Vector3d start = center - axes.horizontal,
                    end = center + axes.horizontal;

            const auto[slopeY, interceptY] = Utils::getLineCoefficients({start.x(), start.y()}, {end.x(), end.y()});
            const auto[slopeZ, interceptZ] = Utils::getLineCoefficients({start.x(), start.z()}, {end.x(), end.z()});

            const double segLength = Utils::twoPointDistance(start, end);
            const double ratio = tooth.mesiodistalWidth / segLength;
            const double xDist = std::abs(start.x() - end.x());

            start.x() = center.x() - ratio * xDist / 2;
            end.x() = center.x() + ratio * xDist / 2;
            start.y() = start.x() * slopeY + interceptY;
            end.y() = end.x() * slopeY + interceptY;
            start.z() = start.x() * slopeZ + interceptZ;
            end.z() = end.x() * slopeZ + interceptZ;

            Eigen::Vector3d midPoint{std::midpoint(start.x(), end.x()),
                                     std::midpoint(start.y(), end.y()),
                                     std::midpoint(start.z(), end.z())};

            if ((toothID % 10) < 4) {
                constexpr double cutSize{1.0};
                Eigen::Vector3d centerNew{center.x(), center.y(), center.z()};
                centerNew.y() = FDI::isLowerTooth(tooth.getToothId()) ? bounds[3] - cutSize : bounds[2] + cutSize;

                const vtkNew<vtkPlane> plane;
                plane->SetOrigin(centerNew.data());
                plane->SetNormal(axes.vertical.data());

                const vtkNew<vtkCutter> cutter;
                cutter->SetCutFunction(plane);
                cutter->SetInputData(toothData);
                cutter->Update();
                vtkSmartPointer<vtkPolyData> cutterData = cutter->GetOutput();

                std::array<double, 6> cuttedBounds {};
                cutterData->GetBounds(cuttedBounds.data());
                const Eigen::Vector3d cuttingEdge{std::midpoint(cuttedBounds[0], cuttedBounds[1]),
                                                  std::midpoint(cuttedBounds[2], cuttedBounds[3]) + 1.6,
                                                  std::midpoint(cuttedBounds[4], cuttedBounds[5])};

                const Eigen::Vector3d diff = cuttingEdge - midPoint;
                start += diff;
                end += diff;
            }

            segments[toothID] = Geometry::Line2D{{start.x() - xShiftCenter, start.z() - zBack},
                                                 {end.x() - xShiftCenter,   end.z() - zBack}};
            toothData->DeepCopy(Utilities::moveTooth(toothData, -xShiftCenter, 0, -zBack));
        }

        constexpr double yOffset = 5;
        {   // INFO: Visualize segments VS teeth
            std::vector<vtkSmartPointer<vtkActor>> actors;
            for (int i: {30, 40}) {
                for (int n = 1; n <= 7; ++n) {
                    const int toothID = i + n;
                    Geometry::Line2D& line = segments[toothID];

                    const auto lineActor = Utilities::getLineActor(
                            {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                            {line.getPoint2().x(), yOffset, line.getPoint2().y()}, {1, 0, 0});
                    actors.push_back(lineActor);

                    const auto ptsActor = Utilities::getPointsActor({
                        {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                        {line.getPoint2().x(), yOffset, line.getPoint2().y()}});
                    actors.push_back(ptsActor);

                    vtkSmartPointer<vtkPolyData> toothData = allTeeth.at(toothID).toothPolyData;
                    actors.push_back(Utilities::getPolyDataActor(toothData));
                }
            }
            // actors.push_back(gumsActor);
            std::for_each(actors.begin(), actors.end(), [](auto& a) {a->RotateX(90); a->RotateY(180); });
            Utilities::DisplayActors(actors, {1950, 50}, caseId);
        }

        // TODO: Place segments center to X [0, 0]
        for (auto toothID: FDI::lowerTeeth) {
            Geometry::Line2D &line = segments[toothID];
            if (FDI::isLeftSideTooth(toothID) && line.getPoint1().x() > line.getPoint2().x())
                line.swapPoints();
            else if (FDI::isRightSideTooth(toothID) && line.getPoint2().x() > line.getPoint1().x())
                line.swapPoints();
        }

        //=========================================================================================

        double A = std::abs(xRight - xLeft) / 2, B = std::abs(zFront - zBack);
        double slopeLeft {0}, interceptLeft {0}, slopeRight {0}, interceptRight{0};
        double lineA = A;

        // INFO: Using the previous coordinate to calculate the parameters of the line
        // INFO: previous - there must be a lateral point of the previous segment
        auto curveEstimateFunction = [&](double x, int id, const Geometry::Point2D& previous) -> double {
            // INFO: Only for last premolar and molars
            if ((id % 10) > 4) {
                // TODO: use std::tie ????
                const auto [slope, intercept] =
                        Geometry::Line2D::getLineCoefficients({x > 0 ? lineA : -lineA, 0}, previous);
                if (x > 0)
                    std::tie(slopeRight, interceptRight) = std::make_pair(slope, intercept);
                else
                    std::tie(slopeLeft, interceptLeft) = std::make_pair(slope, intercept);
                return x * slope + intercept; // Line
            } else {
                return std::sqrt((B * B) * (1 - (x * x) / (A * A))); // Ellipse
            }
        };

        auto curveFunctionFinal = [&](double x, int id, const Geometry::Point2D&) -> double {
            if (std::abs(x) > A || 0 == A|| 0 == B)
                return 0;
            else if ((id % 10) > 4) {
                return x > 0 ? x * slopeRight + interceptRight : x * slopeLeft + interceptLeft;
            } else {
                return std::sqrt((B * B) * (1 - (x * x) / (A * A))); // Ellipse
            }
        };

        auto getCurveActor = [&](double yOffset = 0) {
            std::vector<Eigen::Vector3d> points;
            for (double x = -A; x <= A; x += 0.01)
            {
                const double zEllipse = std::sqrt((B * B) * (1 - (x * x) / (A * A))); // Ellipse
                const double zLine = x > 0 ?x * slopeRight + interceptRight : x * slopeLeft + interceptLeft;
                // points.emplace_back(x, yOffset, std::min(zEllipse, zLine));
                if (B * 0.8 > zLine && zLine > 0)
                    points.emplace_back(x, yOffset, zLine);
                points.emplace_back(x, yOffset, zEllipse);
            }
            return Utilities::getPointsActor(points, {1, 1, 0}, 5);
        };

        //==============================================================================================

        const auto CURVE_ESTIMATE = curveEstimateFunction;


        const double incisorsMoveYMax { 2.0 };
        const double xLineMin {xLimit}, xLineMax {xLimit + 3};
        const double ellipseAMin {xLimit - 2}, ellipseAMax {xLimit + 2};

        A += 10;

        // INFO: Estimate curve
        std::array<std::pair<Geometry::Point2D, double>, 50> transformFinal;
        std::array<Geometry::Line2D, 50> segmentsFinal;
        int epoch = 10000;
        constexpr double step = 0.01;
        while (epoch--)
        {
            std::vector<vtkSmartPointer<vtkActor>> actors;

            double xMove = 0, yMove = 0;
            bool epochFailed = false;
            double molarsMoveX { 0.0 }, preMolarsMoveX { 0.0 }, yStepMax { 0.0 };
            for (int i: {30, 40})
            {
                Geometry::Point2D dest {0, CURVE_ESTIMATE(0, 0, {})};
                for (int n = 1; n <= 7; ++n) {
                    const int toothID = i + n;
                    segmentsFinal[toothID] = segments[toothID]; // TODO: do we need 'segmentsFinal' ??
                    Geometry::Line2D &line = segmentsFinal[toothID];
                    const Geometry::Point2D &centerOrigin = line.getMidPoint();
                    const auto &metrics = placeSegmentOnCurveMetricsEx(line, toothID, dest, CURVE_ESTIMATE);
                    dest = line.getPoint2();

                    auto &[pt, angle] = transformFinal[toothID];
                    const Geometry::Point2D &centerNew = line.getMidPoint();

                    xMove += std::abs(std::abs(centerNew.x()) - std::abs(centerOrigin.x()));
                    yMove += std::abs(std::abs(centerNew.y()) - std::abs(centerOrigin.y()));

                    // FIXME: Failed to place segments on curve --> enlarge x or Y radius
                    if (!metrics.first) {
                        std::cout << "ERROR!\n";
                        epochFailed = true;
                        break;
                    } else {
                        // INFO: Save calcs
                        pt = line.getMidPoint();
                        angle = metrics.second;
                    }

                    if (1 == n || 2 == n || 3 == n) {  // Incisors signed total movement Y (Z)
                        yStepMax = std::max(std::abs(centerNew.y()) - std::abs(centerOrigin.y()), yStepMax);
                    } else if (4 == n) {     // Upper PreMolars signed total X movement
                        preMolarsMoveX += std::abs(centerNew.x()) - std::abs(centerOrigin.x());
                    } else if (7 == n) {     // Lower Molars signed total X movement
                        molarsMoveX += std::abs(centerNew.x()) - std::abs(centerOrigin.x());
                    }
#if 0
                    {   // Visualization actors:
                        const auto lineActor = Utilities::getLineActor(
                                {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                                {line.getPoint2().x(), yOffset, line.getPoint2().y()}, {1, 0, 0});
                        actors.push_back(lineActor);

                        const auto ptsActor = Utilities::getPointsActor({
                            {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                            {line.getPoint2().x(), yOffset, line.getPoint2().y()}});
                        actors.push_back(ptsActor);

                        const Geometry::Line2D &lineOrig = segments[toothID];
                        const auto lineOrigActor = Utilities::getLineActor(
                                {lineOrig.getPoint1().x(), yOffset, lineOrig.getPoint1().y()},
                                {lineOrig.getPoint2().x(), yOffset, lineOrig.getPoint2().y()}, {0, 1, 1});
                        actors.push_back(lineOrigActor);

                        const auto ptsOringActor = Utilities::getPointsActor({
                            {lineOrig.getPoint1().x(), yOffset, lineOrig.getPoint1().y()},
                            {lineOrig.getPoint2().x(), yOffset, lineOrig.getPoint2().y()}}, {0, 1, 0});
                        actors.push_back(ptsOringActor);

                        actors.push_back(Utilities::getPolyDataActor(allTeeth.at(toothID).toothPolyData));
                    }
#endif
                }
            }

#if 0
            std::cout << "\nStepMax       ==> " << yStepMax << std::endl;
            std::cout << "preMolarsMoveX  ==> " << preMolarsMoveX << std::endl;
            std::cout << "molarsMoveX     ==> " << molarsMoveX << std::endl;
            std::cout << "epoch: " << epoch << std::endl;
#endif

            // TODO: Add STOP conditions
            if (epochFailed) {
                if (incisorsMoveYMax >= (yStepMax + step)) {
                    B += 2 * step;
                } else {
                    std::cout << "Can not increase B\n";
                    A += 2 * step;
                    std::cout << "A = " << A << std::endl;
                }
                continue;
            }

            if (preMolarsMoveX > (1 * molarsMoveX)) { // INFO: make curve wider - OK, narrowing - NO
                A -= step;
            }
            else {
                if (molarsMoveX > 0.2) { // FIXME: 0.1
                    lineA -= step;
                }
            }
            if (lineA > xLineMin)
                lineA -= step;

        }

        //=================================================================================================

#if 1
        {   // INFO: Visualize final segments VS teeth vs Original segments
            std::vector<vtkSmartPointer<vtkActor>> actors;
            for (int i: {30, 40}) {
                for (int n = 1; n <= 7; ++n) {
                    const int toothID = i + n;
                    const Geometry::Line2D& line = segmentsFinal[toothID];
                    constexpr double yOffset = 5;

                    const auto lineActor = Utilities::getLineActor(
                            {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                            {line.getPoint2().x(), yOffset, line.getPoint2().y()}, {1, 0, 0});
                    actors.push_back(lineActor);

                    const auto ptsActor = Utilities::getPointsActor({
                        {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                        {line.getPoint2().x(), yOffset, line.getPoint2().y()}});
                    actors.push_back(ptsActor);


                    const Geometry::Line2D& lineOrig = segments[toothID];
                    const auto lineOrigActor = Utilities::getLineActor(
                            {lineOrig.getPoint1().x(), yOffset, lineOrig.getPoint1().y()},
                            {lineOrig.getPoint2().x(), yOffset, lineOrig.getPoint2().y()}, {0, 1, 1});
                    actors.push_back(lineOrigActor);

                    const auto ptsOringActor = Utilities::getPointsActor({
                        {lineOrig.getPoint1().x(), yOffset, lineOrig.getPoint1().y()},
                        {lineOrig.getPoint2().x(), yOffset, lineOrig.getPoint2().y()}},  {0, 1, 0});
                    actors.push_back(ptsOringActor);

                    vtkSmartPointer<vtkPolyData> toothData = allTeeth.at(toothID).toothPolyData;
                    const auto toothActor = Utilities::getPolyDataActor(toothData);

                    actors.push_back(toothActor);
                }
            }
            actors.push_back(getCurveActor(6));
            // actors.push_back(gumsActor);
            std::for_each(actors.begin(), actors.end(), [](auto& a) {a->RotateX(90); a->RotateY(180); });
            Utilities::DisplayActors(actors, {1950, 50}, caseId);
        }
#endif


#if 1
        // TODO: This is BAD! tmp --> refactor
        // INFO: Move teeth to segments positions
        std::unordered_map<int, vtkSmartPointer<vtkPolyData>> placedOnCurveTeeth;
        {
            for (int i: {30, 40}) {
                for (int n = 1; n <= 7; ++n) {
                    const int toothID = i + n;
                    Geometry::Line2D line = segments[toothID];
                    const Geometry::Point2D &centerOrigin = line.getMidPoint();
                    const Geometry::Point2D &centerDest = transformFinal[toothID].first;
                    const Geometry::Point2D &move = centerDest - centerOrigin;
                    double angle = transformFinal[toothID].second;

                    placedOnCurveTeeth[toothID] = allTeeth.at(toothID).toothPolyData;
                    placedOnCurveTeeth[toothID] = Utilities::moveTooth(placedOnCurveTeeth[toothID], move.x(), 0, move.y());
                    {
                        Eigen::Vector3d pos{};
                        placedOnCurveTeeth[toothID]->GetCenter(pos.data());
                        placedOnCurveTeeth[toothID] = Utilities::setPolyDataCenter(placedOnCurveTeeth[toothID], 0, 0, 0);
                        placedOnCurveTeeth[toothID] = Utilities::rotatePolyData(placedOnCurveTeeth[toothID], 0, -angle, 0);
                        placedOnCurveTeeth[toothID] = Utilities::setPolyDataCenter(placedOnCurveTeeth[toothID], pos.x(), pos.y(), pos.z());
                    }
                }
            }
        }

        {   // INFO: Visualize placed on curve teeth:
            std::vector<vtkSmartPointer<vtkActor>> actors;
            for (int i: {30, 40}) {
                for (int n = 1; n <= 7; ++n) {
                    const int toothID = i + n;
                    const auto toothActor = Utilities::getPolyDataActor(placedOnCurveTeeth[toothID]);

                    const Geometry::Line2D& line = segmentsFinal[toothID];
                    constexpr double yOffset = 5;

                    const auto lineActor = Utilities::getLineActor(
                            {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                            {line.getPoint2().x(), yOffset, line.getPoint2().y()}, {1, 0, 0});
                    actors.push_back(lineActor);

                    const auto ptsActor = Utilities::getPointsActor({
                        {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                        {line.getPoint2().x(), yOffset, line.getPoint2().y()}});
                    actors.push_back(ptsActor);

                    actors.push_back(toothActor);
                }
            }
            actors.push_back(getCurveActor(0));
            // actors.push_back(gumsActor);
            std::for_each(actors.begin(), actors.end(), [](auto& a) {a->RotateX(90); a->RotateY(180); });
            Utilities::DisplayActors(actors, {1950, 50}, caseId);
        }
#endif


        //====================================================================================================
        //====================================================================================================
        //====================================================================================================

        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> allTeethLight;
        {   // INFO: Decimate and visualize
            std::vector<vtkSmartPointer<vtkActor>> actors;
            for (const auto&[id, data]: placedOnCurveTeeth) {
                allTeethLight[id] = decimate(data);
                actors.push_back(Utilities::getPolyDataActor(allTeethLight[id]));
            }
            std::for_each(actors.begin(), actors.end(), [](auto& a) {a->RotateX(90); a->RotateY(180); });
            Utilities::DisplayActors(actors, {1950, 50}, caseId);
        }

        auto applyTransformation = [&](const int toothID, const vtkSmartPointer<vtkPolyData>& toothData) {
            const std::pair<Geometry::Point2D, double>& toothTransform = transformFinal[toothID];
            const Geometry::Point2D &centerOrigin = segmentsFinal[toothID].getMidPoint(),
                    &move = toothTransform.first - centerOrigin;
            const double angle = toothTransform.second;

            Eigen::Vector3d origin {};
            toothData->GetCenter(origin.data());

            auto data = Utilities::setPolyDataCenter(toothData, {});
            data->ComputeBounds();
            data = Utilities::rotatePolyData(data, 0, -angle, 0);
            data = Utilities::setPolyDataCenter(data, origin);
            data = Utilities::moveTooth(data,  move.x(), 0, move.y());
            return data;
        };

        auto isTeethIntersect = [](const vtkSmartPointer<vtkPolyData> &tooth1,
                                   const vtkSmartPointer<vtkPolyData> &tooth2) noexcept {
            const vtkSmartPointer<vtkIntersectionPolyDataFilter> booleanFilter {
                    vtkSmartPointer<vtkIntersectionPolyDataFilter>::New()};
            booleanFilter->GlobalWarningDisplayOff();
            booleanFilter->SetInputData(0, tooth1);
            booleanFilter->SetInputData(1, tooth2);
            booleanFilter->Update();
            return booleanFilter->GetNumberOfIntersectionPoints() > 0;
            return true;
        };

        auto moveSegment = [&](int toothId, double xStep) {
            Geometry::Line2D line = segmentsFinal[toothId];
            Geometry::Point2D dest {line.getPoint1().x() + xStep,
                                    curveFunctionFinal(line.getPoint1().x() + xStep, toothId, {})};
            const auto &metrics = placeSegmentOnCurveMetricsEx(line, toothId, dest, curveFunctionFinal);
            auto &[pt, angle] = transformFinal[toothId];
            pt = line.getMidPoint();
            angle = metrics.second;
        };

#if 0
        {
            constexpr std::array<int, 7> teethIDs{31, 32, 33, 34, 35, 36, 37};
            {
                constexpr double stepSize = { 0.01 };
                for (size_t i = 1; i < teethIDs.size(); ++i) {
                    const int id1 = teethIDs[i - 1], id2 = teethIDs[i];
                    const short direction =
                            segmentsFinal[id2].getPoint1().x() > segmentsFinal[id1].getPoint1().x() ? 1 : -1;
                    const vtkSmartPointer<vtkPolyData> toothDest = allTeethLight.at(id1);
                    vtkSmartPointer<vtkPolyData> toothToMove = allTeethLight.at(id2);
                    vtkSmartPointer<vtkPolyData> toothToMoveNormal = placedOnCurveTeeth.at(id2);

                    double stepTotal = 0.0;
                    const bool isIntersectedBefore = isTeethIntersect(toothDest, toothToMove);
                    const double deltaStep = stepSize * direction;
                    while (true && isIntersectedBefore) {
                        stepTotal += deltaStep;
                        moveSegment(id2, stepTotal);
                        toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                        toothToMoveNormal = applyTransformation(id2, placedOnCurveTeeth.at(id2));
                        if (getDistance(toothDest, toothToMove) > 0.5) break;
                    }

                    allTeethLight.at(id2) = toothToMove;
                    placedOnCurveTeeth.at(id2) = toothToMoveNormal;
                }
            }

            {
                std::vector<vtkSmartPointer<vtkActor>> actorsTMP {};
                actorsTMP.push_back(getCurveActor(5));
                for (int ID: teethIDs)
                    actorsTMP.push_back(Utilities::getPolyDataActor(placedOnCurveTeeth[ID]));
                std::for_each(actorsTMP.begin(), actorsTMP.end(), [](auto& a) {a->RotateX(90); a->RotateY(180); });
                Utilities::DisplayActors(actorsTMP, {1950, 50}, caseId);
            }

            {
                constexpr double stepSize = { 0.1 }, fallbackFactor {2.0};
                std::vector<vtkSmartPointer<vtkActor>> actors{Utilities::getPolyDataActor(allTeethLight.at(31))};
                for (size_t i = 1; i < teethIDs.size(); ++i)
                {
                    const int id1 = teethIDs[i - 1], id2 = teethIDs[i];
                    const short direction =
                            segmentsFinal[id2].getPoint1().x() > segmentsFinal[id1].getPoint1().x() ? 1 : -1;
                    const vtkSmartPointer<vtkPolyData> toothDest = allTeethLight.at(id1);
                    vtkSmartPointer<vtkPolyData> toothToMove = allTeethLight.at(id2);
                    vtkSmartPointer<vtkPolyData> toothToMoveNormal = placedOnCurveTeeth.at(id2);

                    double stepTotal = 0.0;
                    const double deltaStep = stepSize * direction;
                    while (true)
                    {
                        stepTotal -= deltaStep;
                        moveSegment(id2, stepTotal);
                        toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                        toothToMoveNormal = applyTransformation(id2, placedOnCurveTeeth.at(id2));
                        if (true == isTeethIntersect(toothDest, toothToMove)) {
                            moveSegment(id2, stepTotal + fallbackFactor * deltaStep);
                            toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                            break;
                        }
                    }

                    allTeethLight.at(id2)->DeepCopy(toothToMove);
                    placedOnCurveTeeth.at(id2) = toothToMoveNormal;
                }
            }

            {
                std::vector<vtkSmartPointer<vtkActor>> actorsTMP {};
                actorsTMP.push_back(getCurveActor(5));
                for (int ID: teethIDs)
                    actorsTMP.push_back(Utilities::getPolyDataActor(placedOnCurveTeeth[ID]));
                std::for_each(actorsTMP.begin(), actorsTMP.end(), [](auto& a) {a->RotateX(90); a->RotateY(180); });
                Utilities::DisplayActors(actorsTMP, {1950, 50}, caseId);
            }
        }
#endif


#if 1
        // TODO: ****** Aling 31 and 41 ***********

        std::mutex mtx;
        constexpr double stepSize = { 0.01 }, fallbackFactor {5.0};
        std::future<void> processLeft = std::async(std::launch::async, [&] {
            constexpr std::array<int, 7> leftIds {31, 32, 33, 34, 35, 36 ,37};
            for (size_t i = 1; i < leftIds.size(); ++i)
            {
                const int id1 = leftIds[i - 1], id2 = leftIds[i];
                const short direction = segmentsFinal[id2].getPoint1().x() > segmentsFinal[id1].getPoint1().x() ? 1 : -1;
                const vtkSmartPointer<vtkPolyData> toothDest = allTeethLight.at(id1);
                vtkSmartPointer<vtkPolyData> toothToMove = allTeethLight.at(id2);
                vtkSmartPointer<vtkPolyData> toothToMoveNormal = placedOnCurveTeeth.at(id2);

                double stepTotal = 0.0;
                const bool isIntersectedBefore = isTeethIntersect(toothDest, toothToMove);
                const double deltaStep = stepSize * direction;
                while (true && isIntersectedBefore) {
                    stepTotal += deltaStep;
                    moveSegment(id2, stepTotal);
                    toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                    toothToMoveNormal = applyTransformation(id2, placedOnCurveTeeth.at(id2));

                    // if (false ==isTeethIntersect(toothDest, toothToMove)) break;
                    if (getDistance(toothDest, toothToMove) > 0.5) break;
                }

                std::lock_guard<std::mutex> lock {mtx };
                allTeethLight.at(id2) = toothToMove;
                placedOnCurveTeeth.at(id2) = toothToMoveNormal;
            }
            // return;
            for (size_t i = 1; i < leftIds.size(); ++i)
            {
                const int id1 = leftIds[i - 1], id2 = leftIds[i];
                const short direction = segmentsFinal[id2].getPoint1().x() > segmentsFinal[id1].getPoint1().x() ? 1 : -1;
                const vtkSmartPointer<vtkPolyData> toothDest = allTeethLight.at(id1);
                vtkSmartPointer<vtkPolyData> toothToMove = allTeethLight.at(id2);
                vtkSmartPointer<vtkPolyData> toothToMoveNormal = placedOnCurveTeeth.at(id2);

                double stepTotal = 0.0;
                const double deltaStep = stepSize * direction;
                while (true) {
                    stepTotal -= deltaStep;
                    moveSegment(id2, stepTotal);
                    toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                    toothToMoveNormal = applyTransformation(id2, placedOnCurveTeeth.at(id2));
                    if (true == isTeethIntersect(toothDest, toothToMove)) {
                        moveSegment(id2, stepTotal + fallbackFactor * deltaStep);
                        toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                        break;
                    }
                }

                std::lock_guard<std::mutex> lock {mtx };
                allTeethLight.at(id2) = toothToMove;
                placedOnCurveTeeth.at(id2) = toothToMoveNormal;
            }
        });

        processLeft.wait();

        std::future<void> processRight = std::async(std::launch::async, [&] {
            constexpr std::array<int, 7> leftIds {41, 42, 43, 44, 45, 46, 47};
            for (size_t i = 1; i < leftIds.size(); ++i)
            {
                const int id1 = leftIds[i - 1], id2 = leftIds[i];
                const short direction = segmentsFinal[id2].getPoint1().x() > segmentsFinal[id1].getPoint1().x() ? 1 : -1;
                const vtkSmartPointer<vtkPolyData> toothDest = allTeethLight.at(id1);
                vtkSmartPointer<vtkPolyData> toothToMove = allTeethLight.at(id2);
                vtkSmartPointer<vtkPolyData> toothToMoveNormal = placedOnCurveTeeth.at(id2);

                double stepTotal = 0.0;
                const bool isIntersectedBefore = isTeethIntersect(toothDest, toothToMove);
                const double deltaStep = stepSize * direction;
                bool wasMoved {false};

                while (true && isIntersectedBefore) {
                    wasMoved = true;
                    stepTotal += deltaStep;
                    moveSegment(id2, stepTotal);
                    toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                    toothToMoveNormal = applyTransformation(id2, placedOnCurveTeeth.at(id2));
                    if (getDistance(toothDest, toothToMove) > 0.5) break;
                }

                std::lock_guard<std::mutex> lock {mtx };
                allTeethLight.at(id2) = toothToMove;
                placedOnCurveTeeth.at(id2) = toothToMoveNormal;
            }

            // return;
            for (size_t i = 1; i < leftIds.size(); ++i)
            {
                const int id1 = leftIds[i - 1], id2 = leftIds[i];
                const short direction = segmentsFinal[id2].getPoint1().x() > segmentsFinal[id1].getPoint1().x() ? 1 : -1;
                const vtkSmartPointer<vtkPolyData> toothDest = allTeethLight.at(id1);
                vtkSmartPointer<vtkPolyData> toothToMove = allTeethLight.at(id2);
                vtkSmartPointer<vtkPolyData> toothToMoveNormal = placedOnCurveTeeth.at(id2);

                double stepTotal = 0.0;
                const double deltaStep = stepSize * direction;
                while (true) {
                    stepTotal -= deltaStep;
                    moveSegment(id2, stepTotal);
                    toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                    toothToMoveNormal = applyTransformation(id2, placedOnCurveTeeth.at(id2));
                    if (true == isTeethIntersect(toothDest, toothToMove)) {
                        moveSegment(id2, stepTotal + fallbackFactor * deltaStep);
                        toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                        break;
                    }
                }

                std::lock_guard<std::mutex> lock { mtx };
                allTeethLight.at(id2) = toothToMove;
                placedOnCurveTeeth.at(id2) = toothToMoveNormal;
            }
        });

        processRight.wait();


        std::vector<vtkSmartPointer<vtkActor>> actors { };
        //vtkSmartPointer<vtkAppendPolyData> dataAppender { vtkAppendPolyData::New() };
        for (int toothID: {47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36 ,37}) {
            ///auto toothFinalData = allTeethLight.at(toothID);
            auto toothFinalData = placedOnCurveTeeth.at(toothID);
            //toothFinalData = Utilities::moveTooth(toothFinalData, xShiftCenter, 0, zBack);
            toothFinalData = Utilities::rotatePolyData(toothFinalData, -orientAngles.first, 0, -orientAngles.second);
            //dataAppender->AddInputData(toothFinalData);

            const Geometry::Line2D& line = segmentsFinal[toothID];
            constexpr double yOffset = 5;

            const auto lineActor = Utilities::getLineActor(
                    {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                    {line.getPoint2().x(), yOffset, line.getPoint2().y()}, {1, 0, 0});
            actors.push_back(lineActor);

            const auto ptsActor = Utilities::getPointsActor({
                {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                {line.getPoint2().x(), yOffset, line.getPoint2().y()}});
            actors.push_back(ptsActor);

            actors.push_back(Utilities::getPolyDataActor(toothFinalData));
        }

       // dataAppender->Update();
        //const auto teethData = dataAppender->GetOutput();
        //auto teethDataNew = Utilities::rotatePolyData(teethData, -orientAngles.first, 0, -orientAngles.second);
        //teethDataNew = Utilities::moveTooth(teethDataNew, 0, 0, -3);


        //actors.push_back(Utilities::getPolyDataActor(teethDataNew));
        // actors.push_back(getCurveActor(5));
        // actors.push_back(gumsActor);
        std::for_each(actors.begin(), actors.end(), [](auto& a) {a->RotateX(90); a->RotateY(180); });
        Utilities::DisplayActors(actors, {1950, 50}, caseId);

        // const std::string stlFileName {R"(/home/andtokm/tmp/15/)" + caseId + ".stl"};
        // Utilities::WriteSTL(teethDataNew, stlFileName);
#endif

    }
}

void EstimateCurve3::TestAll(const std::vector<std::string_view>& params)
{
    FitTheCurve_AlignedSegments(params);
};
