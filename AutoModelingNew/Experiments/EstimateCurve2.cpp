//============================================================================
// Name        : EstimateCurve2.h
// Created on  : 24.01.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : EstimateCurve2
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

#include "EstimateCurve2.h"
#include "Utils.h"
#include "Geometry.h"

using namespace std::string_literals;

namespace EstimateCurve2 {
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
            const double yLine = line.getPoint2().y(), yCurve = func(line.getPoint2().x(), toothID % 10, dest);
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
        smoothFilter->SetNumberOfIterations(4);
        smoothFilter->SetRelaxationFactor(0.7);
        smoothFilter->FeatureEdgeSmoothingOff();
        smoothFilter->BoundarySmoothingOn();
        smoothFilter->Update();
        toothData = smoothFilter->GetOutput();

        while (toothData->GetNumberOfPoints() > 50) {
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

        {   // INFO: Orient and SetUp a Torks and etc:
            Utils::orientTeethToPlane(allTeeth);
            SetUpTorksAndPositions::alignTeethTorks(allTeeth);
            SetUpTorksAndPositions::alignTeethVertical(allTeeth);
        }

        std::array<double, 6> leftUpperIncisor { allTeeth.at(31).GetBounds() };
        std::array<double, 6> rightUpperIncisor { allTeeth.at(41).GetBounds() };
        std::array<double, 6> leftPreMolarBounds { allTeeth.at(34).GetBounds() };
        std::array<double, 6> leftMolarBounds { allTeeth.at(37).GetBounds() };
        std::array<double, 6> rightPreMolarBounds { allTeeth.at(44).GetBounds() };
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
            std::swap(leftPreMolarBounds, rightPreMolarBounds);
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

        const Eigen::Vector3d leftPreMolarLeftPt {
                leftPreMolarCenter.x() - (std::abs(leftPreMolarBounds[0] - leftPreMolarBounds[1])/2),
                leftPreMolarCenter.y(), leftPreMolarCenter.z() };
        const Eigen::Vector3d leftPreMolarRightPt {
                leftPreMolarCenter.x() + (std::abs(leftPreMolarBounds[0] - leftPreMolarBounds[1])/2),
                leftPreMolarCenter.y(), leftPreMolarCenter.z() };
        const Eigen::Vector3d leftMolarLeftPt {
                leftMolarCenter.x() - (std::abs(leftMolarBounds[0] - leftMolarBounds[1])/2),
                leftMolarCenter.y(), leftMolarCenter.z()  };
        const Eigen::Vector3d leftMolarRightPt {
                leftMolarCenter.x() + (std::abs(leftMolarBounds[0] - leftMolarBounds[1])/2),
                leftMolarCenter.y(), leftMolarCenter.z()  };
        const Eigen::Vector3d rightPreMolarLeftPt {
                rightPreMolarCenter.x() - (std::abs(rightPreMolarBounds[0] - rightPreMolarBounds[1])/2),
                rightPreMolarCenter.y(), rightPreMolarCenter.z() };
        const Eigen::Vector3d rightPreMolarRightPt {
                rightPreMolarCenter.x() + (std::abs(rightPreMolarBounds[0] - rightPreMolarBounds[1])/2),
                rightPreMolarCenter.y(), rightPreMolarCenter.z() };
        const Eigen::Vector3d rightMolarLeftPt {
                rightMolarCenter.x() - (std::abs(rightMolarBounds[0] - rightMolarBounds[1])/2),
                rightMolarCenter.y(), rightMolarCenter.z() };
        const Eigen::Vector3d rightMolarRightPt {
                rightMolarCenter.x() + (std::abs(rightMolarBounds[0] - rightMolarBounds[1])/2),
                rightMolarCenter.y(), rightMolarCenter.z() };



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
            toothData->DeepCopy(Utilities::moveTooth(toothData, - xShiftCenter, 0, - zBack));
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
            Utilities::DisplayActors(actors);
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

        // const auto gumsActor = getGumsActor(gumsPath, -xShiftCenter, 0, -zBack);
        double A = std::abs(xRight - xLeft) / 2, B = std::abs(zFront - zBack);
        double slopeLeft {0}, interceptLeft {0}, slopeRight {0}, interceptRight{0};

        /*
        std::tie(slopeLeft, interceptLeft) = Geometry::Line2D::getLineCoefficients(
                {-A, leftPreMolarCenter.z() - zBack},
                {leftMolarCenter.x() - xShiftCenter, leftMolarCenter.z() -zBack});
        std::tie(slopeRight, interceptRight) = Geometry::Line2D::getLineCoefficients(
                {rightPreMolarCenter.x() - xShiftCenter, rightPreMolarCenter.z() - zBack},
                {rightMolarCenter.x() - xShiftCenter, rightMolarCenter.z() - zBack});
        */

        // INFO: The intersection with the X-axis of the straight lines should not be greater modulo A
        auto curveFunction = [&](double x) -> double {
            if (std::abs(x) > A || 0 == A|| 0 == B)
                return 0;

            const double zEllipse = std::sqrt((B * B) * (1 - (x * x) / (A * A))); // Ellipse
            const double zLine = x > 0 ?x * slopeRight + interceptRight : x * slopeLeft + interceptLeft;
            return std::min(zEllipse, zLine);
        };

        auto getCurveActor = [&](double y = 0) {
            std::vector<Eigen::Vector3d> points;
            for (double x = -A; x <= A; x += 0.01)
            {
                const double zEllipse = std::sqrt((B * B) * (1 - (x * x) / (A * A))); // Ellipse
                const double zLine = x > 0 ?x * slopeRight + interceptRight : x * slopeLeft + interceptLeft;
                points.emplace_back(x, y, std::min(zEllipse, zLine));
            }
            return Utilities::getPointsActor(points, {1, 1, 0}, 5);
        };

#if 1
        {   // INFO: Visualize teeth and CURVE initial positions
            std::vector<vtkSmartPointer<vtkActor>> actors;
            for (int i: {30, 40}) {
                for (int n = 1; n <= 7; ++n) {
                    const int toothID = i + n;
                    vtkSmartPointer<vtkPolyData> toothData = allTeeth.at(toothID).toothPolyData;
                    toothData = Utilities::moveTooth(toothData, -xShiftCenter, 0, 0);
                    actors.push_back(Utilities::getPolyDataActor(toothData));
                }
            }

            const auto [slope1Left, intercept1Left] = Utils::getLineCoefficients(
                    {leftPreMolarLeftPt.x(), leftPreMolarLeftPt.z()}, {leftMolarLeftPt.x(), leftMolarLeftPt.z()});

            const auto [slope2Left, intercept2Left] = Utils::getLineCoefficients(
                    {leftPreMolarRightPt.x(), leftPreMolarRightPt.z()}, {leftMolarRightPt.x(), leftMolarRightPt.z()});

            const auto [slope1Right, intercept1Right] = Utils::getLineCoefficients(
                    {rightMolarLeftPt.x(), rightMolarLeftPt.z()}, {rightPreMolarLeftPt.x(), rightPreMolarLeftPt.z()});

            const auto [slope2Right, intercept2Right] = Utils::getLineCoefficients(
                    {rightMolarRightPt.x(), rightMolarRightPt.z()}, {rightPreMolarRightPt.x(), rightPreMolarRightPt.z()});


            std::cout << "[" << slope1Left << ", " << intercept1Left << "] ["
                             << slope2Left << ", " << intercept2Left << "]\n";
            std::cout << "[" << slope1Right << ", " << intercept1Right << "] ["
                             << slope2Right << ", " << intercept2Right << "]\n";

            {
                actors.push_back(Utilities::getPointsActor({leftPreMolarLeftPt, leftMolarLeftPt  }, {1, 0, 0}));
                actors.push_back(Utilities::getPointsActor({leftPreMolarRightPt, leftMolarRightPt}, {0, 1, 0}));

                actors.push_back(Utilities::getPointsActor({rightMolarLeftPt, rightPreMolarLeftPt  }, {0, 1, 0}));
                actors.push_back(Utilities::getPointsActor({rightMolarRightPt, rightPreMolarRightPt}, {1, 0, 0}));

                /*
                actors.push_back(Utilities::getPointsActor({leftMolarCenter}, {1, 0, 0}));
                actors.push_back(Utilities::getPointsActor({leftPreMolarCenter}, {1, 0, 0}));
                actors.push_back(Utilities::getPointsActor({rightMolarCenter}, {0, 1, 0}));
                actors.push_back(Utilities::getPointsActor({rightPreMolarCenter}, {0, 1, 0}));
                */

                // actors.push_back(Utilities::getPointsActor({{-A, yOffset, 0}}, {1, 0, 0}));
                // actors.push_back(Utilities::getPointsActor({{A, yOffset, 0}}, {0, 1, 0}));
                // actors.push_back(Utilities::getPointsActor({{-xLimit, yOffset, 0}, {xLimit, yOffset, 0}}, {0,1,0}));
            }


            std::vector<Eigen::Vector3d> leftLines1, leftLines2;
            /*
            for (double x = -A; x <= A; x += 0.01) {
                const double z1L = slope1Left * x + intercept1Left;
                if (z1L >= 0 && z1L < (B + 3))
                    leftLinesPoints.emplace_back(x, 5, z1L);

                const double z2L = slope2Left * x + intercept2Left;
                if (z2L >= 0 && z2L < (B + 3))
                    leftLinesPoints.emplace_back(x, 5, z2L);

                const double z1R = slope1Right * x + intercept1Right;
                if (z1R >= 0 && z1R < (B + 3))
                    leftLinesPoints.emplace_back(x, 5, z1R);

                const double z2R = slope2Right * x + intercept2Right;
                if (z2R >= 0 && z2R < (B + 3))
                    leftLinesPoints.emplace_back(x, 5, z2R);
            }
            */

            for (double x = -A; x <= A; x += 0.01)
            {

                {
                    const double slope = slope1Left; // min slope
                    const double intercept = intercept1Right; // max intercept
                    {
                        const double z = -slope * x + intercept;
                        if (z >= 0 && z < (B + 3))
                            leftLines1.emplace_back(x, 5, z);
                    }
                    {
                        const double z = slope * x + intercept;
                        if (z >= 0 && z < (B + 3))
                            leftLines1.emplace_back(x, 5, z);
                    }
                }

                {
                    const double slope = slope1Right; // max slope
                    const double intercept = intercept2Right; // max intercept
                    {
                        const double z = -slope * x + intercept;
                        if (z >= 0 && z < (B + 3))
                            leftLines2.emplace_back(x, 5, z);
                    }
                    {
                        const double z = slope * x + intercept;
                        if (z >= 0 && z < (B + 3))
                            leftLines2.emplace_back(x, 5, z);
                    }
                }
            }


            actors.push_back(Utilities::getPointsActor(leftLines1, {0, 1, 0}, 4));
            actors.push_back(Utilities::getPointsActor(leftLines2, {1, 0, 1}, 4));


            /*
            std::vector<Eigen::Vector3d> points;
            for (double x = -A; x <= A; x += 0.01) {
                const double z = curveFunction(x);
                if (z >= 0)
                    points.emplace_back(x, 5, z);
            }
            actors.push_back(Utilities::getPointsActor(points, {1, 1, 0}, 4));
            */
            Utilities::DisplayActors(actors);
        }
        return;
#endif

#if 0
        // INFO: Estimate curve
        std::array<std::pair<Geometry::Point2D, double>, 50> transformFinal;
        std::array<Geometry::Line2D, 50> segmentsFinal;
        int epoch = 1000;
        constexpr double step = 0.01;
        while (epoch--) {
            double xMove = 0, yMove = 0;
            double xMoveSigned = 0, yMoveSigned = 0;
            bool epochFailed = false;
            for (int i: {30, 40})
            {
                Geometry::Point2D dest{0, CURVE_ESTIMATE(0, 0, {})};
                for (int n = 1; !epochFailed && n <= 7; ++n) {
                    const int toothID = i + n;
                    segmentsFinal[toothID] = segments[toothID]; // TODO: do we need 'segmentsFinal' ??
                    Geometry::Line2D &line = segmentsFinal[toothID];
                    const Geometry::Point2D &centerOrigin = line.getMidPoint();
                    [[maybe_unused]] const auto &metrics =
                            placeSegmentOnCurveMetricsEx(line, toothID, dest, CURVE_ESTIMATE);
                    dest = line.getPoint2();

                    auto &[pt, angle] = transformFinal[toothID];
                    const Geometry::Point2D &centerNew = line.getMidPoint();

                    // INFO: Calc oll X move and Y move for all teeth

                    if (5 == n || 6 == n || 7 == n)
                        xMoveSigned += std::abs(centerNew.x()) - std::abs(centerOrigin.x());
                    if (1 == n || 2 == n || 3 == n || 4 == n)
                        yMoveSigned += std::abs(centerNew.y()) - std::abs(centerOrigin.y());

                    xMove += std::abs(std::abs(centerNew.x()) - std::abs(centerOrigin.x()));
                    yMove += std::abs(std::abs(centerNew.y()) - std::abs(centerOrigin.y()));

                    // FIXME: Failed to place segments on curve --> enlarge x or Y radius
                    if (!metrics.first) {
                        std::cout << "ERROR\n";
                        B += step;
                        if (A > xLimit) {
                            A -= std::abs(xMoveSigned / yMoveSigned) * step;
                        }
                        epochFailed = true;
                        std::cout << xMoveSigned << "   " << yMoveSigned << "  [" << A << ", " << B << "]\n";
                        break;
                    }

                    // INFO: Save calcs
                    pt = line.getMidPoint();
                    angle = metrics.second;
                }
            }
            if (epochFailed)
                continue;

            if (xMove > 2 || yMove > 2) { // FIXME: 2 - magic
                if (xMove > yMove)
                    A -= step;
                else
                    B -= step;

            } else break;
            std::cout << xMoveSigned << "   " << yMoveSigned << "  [" << A << ", " << B << "]\n";
        }

        std::cout << "====================================================================================\n";
        std::cout << "A: " << A << ", B: " << B << std::endl;
        std::cout << "(" << slopeLeft << ", " << interceptLeft << ") (" << slopeRight << "," << interceptRight << ")\n";
        std::cout << "yMax = " << curveFunctionFinalEx(0, {}, {}) << std::endl;
        std::cout << zFront - zBack<< std::endl;
        std::cout << "====================================================================================\n";
        return;
#endif

    }
}

void EstimateCurve2::TestAll(const std::vector<std::string_view>& params)
{
    FitTheCurve_AlignedSegments(params);
};
