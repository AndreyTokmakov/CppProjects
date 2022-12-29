//============================================================================
// Name        : EstimateCurve.h
// Created on  : 24.01.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : SetUpTorksAndPositions
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
#include "../Model/Common.h"
#include "../Model/FDI.h"
#include "../Model/Jaw.h"
#include "../Utilities/Utilities.h"
#include "../TreatmentPlan/TreatmentPlan.h"
#include "../Estimators/Estimator.h"
#include "SetUpTorksAndPositions.h"

#include "EstimateCurve.h"
#include "Utils.h"
#include "Geometry.h"

using namespace std::string_literals;

namespace EstimateCurve {
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

    //======================================================================================================

    void DrawLine() {
        Geometry::Line2D line{{1, 0},
                              {5, 0}};
        const auto &origin = line.getMidPoint();

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (int i = 0; i < 18 * 4; ++i) {
            line.rotateAroundPoint(origin, 2.5f);
            actors.push_back(getLine2DActor(line));
        }

        Utilities::DisplayActors(actors);
    }

    vtkSmartPointer<vtkActor> getGumsActor(std::string_view gumsPath,
                                           double xStep, double yStep, double ztep) {
        auto gumsData = Utilities::readStl(gumsPath);

        {
            auto parts = Utilities::separate_connected_components_parallel_test(gumsData);
            std::cout << parts.size() << std::endl;
            int maxMetricsValues{0};
            for (auto &pd: parts) {
                const auto value = pd->GetNumberOfCells();
                if (value > maxMetricsValues) {
                    gumsData = pd;
                    maxMetricsValues = value;
                }
            }
            gumsData = Utilities::rotatePolyData(gumsData, 90, 180, 0);
            gumsData = Utilities::moveTooth(gumsData, xStep, yStep, ztep);
        }

        vtkNew<vtkPlane> plane;
        plane->SetNormal(0.0, 1.0, 0.0);

        Eigen::Vector3d center {};
        std::array<double, 6> bounds {};
        gumsData->GetBounds(bounds.data());
        gumsData->GetCenter(center.data());

        vtkNew<vtkCutter> cutter;
        cutter->SetCutFunction(plane);
        cutter->SetInputData(gumsData);

        const Eigen::Vector3d minBound {bounds[0], bounds[2], bounds[4]},
                maxBound {bounds[1], bounds[3], bounds[5]};

        double from {minBound[1] - center[1]}, until { center[1] };

        cutter->GenerateValues(40, from, until);
        vtkNew<vtkPolyDataMapper> cutterMapper;
        cutterMapper->SetInputConnection(cutter->GetOutputPort());
        cutterMapper->ScalarVisibilityOff();

        vtkSmartPointer<vtkActor> planeActor { vtkSmartPointer<vtkActor>::New() };
        planeActor->GetProperty()->SetColor(std::array<double, 3>{1, 0 ,0}.data());
        planeActor->GetProperty()->SetLineWidth(1);
        planeActor->SetMapper(cutterMapper);

        return planeActor;

#if 0
        {
            for (const auto&[toothId, data]: toothMap) {
                if (FDI::isLowerTooth(toothId)) {
                    auto toothData = Utilities::rotatePolyData(data, 0, 0, 0);
                    actors.push_back(Utilities::getPolyDataActor(toothData));
                }
            }
        }
#endif

#if 0
        vtkNew<vtkFeatureEdges> featureEdges;
        featureEdges->SetInputData(gumsData);
        // featureEdges->BoundaryEdgesOn();
        // featureEdges->FeatureEdgesOn();
        // featureEdges->ManifoldEdgesOn();
        // featureEdges->NonManifoldEdgesOn();
        featureEdges->Update();

        const vtkSmartPointer<vtkActor> featureEdgesActor =
                Utilities::getPolyDataActor(featureEdges->GetOutput());
        featureEdgesActor->GetProperty()->SetLineWidth(1);
        actors.push_back(featureEdgesActor);

        Utilities::DisplayActors(actors);
#endif
    }


    void DrawEllipseTest() {

        const auto actor = getEllipseActor(24, 38);
        Utilities::DisplayActors({actor});
    }

    void FindClosetPoint_Test() {
        constexpr double A = 24.0, B = 38.0;
        const Eigen::Vector3d pt1{14, 14, 0};

        [[maybe_unused]]
        auto equationEllipse = [](double x, double a, double b) -> double {
            if (std::abs(x) >= a || 0 == a || 0 == b)
                return 0;
            return std::sqrt((b * b) * (1 - (x * x) / (a * a)));
        };

        auto ellipse = [=](double x) {
            return equationEllipse(x, A, B);
        };
        const auto closest = findClosetPoint({pt1.x(), pt1.y()}, ellipse);

        std::vector<vtkSmartPointer<vtkActor>> actors;
        {
            actors.push_back(Utilities::getPointsActor({pt1}, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({{closest.x(), closest.y(), 0}}, {1, 0, 0}));
            actors.push_back(getEllipseActor(24, 38));
        }
        Utilities::DisplayActors(actors);
    }

    void VisualizeTooths_LineSections_ContactPoints() {
        constexpr std::string_view planJsonFile{R"(/home/andtokm/Projects/data/cases/2280/Plan.json)"};
        constexpr std::string_view filePath{
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)"};

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto&[toothId, toothData]: plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw upper{Model::TeethType::Upper};
        std::unique_ptr<Estimators::Estimator> estimator{std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto&[toothId, data]: toothMap) {
            if (FDI::isUpperTooth(toothId)) {
                if (auto[iter, ok] = upper.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        std::unordered_map<Utils::UnorderedPair<int>, Eigen::Vector3d, Utils::PairHashUnordered<int>> contactPoints;
        std::array<int, 14> teethIds{47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};
        for (size_t i = 1; i < teethIds.size(); ++i) {
            const int id1 = teethIds[i - 1], id2 = teethIds[i];
            const vtkSmartPointer<vtkPolyData> tooth1 = toothMap.at(id1);
            const vtkSmartPointer<vtkPolyData> tooth2 = toothMap.at(id2);

            Eigen::Vector3d ptContact = getContactPoint(tooth1, tooth2);
            contactPoints.emplace(Utils::UnorderedPair<int>{id1, id2}, ptContact);
        }

        std::vector<vtkSmartPointer<vtkActor>> actors{
                Utilities::getPolyDataActor(toothMap.at(teethIds[0])),
                Utilities::getPolyDataActor(toothMap.at(teethIds[1]))
        };
        for (size_t i = 2; i < teethIds.size(); ++i) {
            const int id1 = teethIds[i - 2], id2 = teethIds[i - 1], id3 = teethIds[i];
            Eigen::Vector3d pt1 = contactPoints.at(Utils::UnorderedPair<int>{id1, id2});
            Eigen::Vector3d pt2 = contactPoints.at(Utils::UnorderedPair<int>{id2, id3});

            actors.push_back(Utilities::getPolyDataActor(toothMap.at(id3)));
            actors.push_back(Utilities::getLineActor(pt1, pt2, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({pt1, pt2}, {1, 0, 0}));
        }
        Utilities::DisplayActors(actors);
    }

    void VisualizeTooths_LineSections() {
        constexpr std::string_view planJsonFile{R"(/home/andtokm/Projects/data/cases/2280/Plan.json)"};
        constexpr std::string_view filePath{
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)"};

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto&[toothId, toothData]: plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw upper{Model::TeethType::Upper};
        std::unique_ptr<Estimators::Estimator> estimator{std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto&[toothId, data]: toothMap) {
            if (FDI::isUpperTooth(toothId)) {
                if (auto[iter, ok] = upper.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (const auto&[id, tooth]: upper.teeth) {
            vtkSmartPointer<vtkPolyData> toothData = tooth.toothPolyData;

            Eigen::Vector3d center{};
            toothData->GetCenter(center.data());

            const Model::ToothAxis axes = tooth.getAxis();
            Eigen::Vector3d start = center - axes.horizontal;
            Eigen::Vector3d end = center + axes.horizontal;

            const auto[slopeY, interceptY] =
            Utils::getLineCoefficients({start.x(), start.y()}, {end.x(), end.y()});
            const auto[slopeZ, interceptZ] =
            Utils::getLineCoefficients({start.x(), start.z()}, {end.x(), end.z()});

            // the ratio of the current length of the segment to the length of the tooth
            const double X = tooth.mesiodistalWidth / Utils::twoPointDistance(start, end);;

            start.x() -= std::abs((start.x() - center.x())) * (X - 1);
            end.x() += std::abs((end.x() - center.x())) * (X - 1);
            start.y() = start.x() * slopeY + interceptY;
            end.y() = end.x() * slopeY + interceptY;
            start.z() = start.x() * slopeZ + interceptZ;
            end.z() = end.x() * slopeZ + interceptZ;

            actors.push_back(Utilities::getPolyDataActor(toothData));
            actors.push_back(Utilities::getLineActor(start, end, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({start, end}, {1, 0, 0}));
        }
        Utilities::DisplayActors(actors);
    }


    void FindMoveFitting_LineSegment_ForTooth() {
        constexpr std::string_view planJsonFile{R"(/home/andtokm/Projects/data/cases/2280/Plan.json)"};
        constexpr std::string_view filePath{
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)"};

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        // TODO: Check for performance: pass polyData&& (as RValue ref) ???
        std::unique_ptr<Estimators::Estimator> estimator{std::make_unique<Estimators::DistanceEstimator>()};
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&&[toothId, polyData]: toothMap) {
            if (auto[iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
                estimator->estimate(iter->second);
            }
        }

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (const auto&[id, tooth]: allTeeth) {
            if (
                //33 != id && 31 != id && 32 != id &&
                //43 != id && 41 != id && 42 != id &&
                // 44 != id &&
                    34 != id
                    )
                // if (33 != id )
                continue;

            const vtkSmartPointer<vtkPolyData> toothData = tooth.toothPolyData;
            const Model::ToothAxis &axes = tooth.getAxis();
            const Eigen::Vector3d &center{tooth.GetCenter()};
            const std::array<double, 6> &bounds{tooth.GetBounds()};

            Eigen::Vector3d centerNew{center.x(), bounds[3] - 0.5, center.z()};
            {
                vtkNew<vtkPlane> plane;
                plane->SetOrigin(centerNew.data());
                plane->SetNormal(axes.vertical.data());

                // Create cutter
                vtkNew<vtkCutter> cutter;
                cutter->SetCutFunction(plane);
                cutter->SetInputData(toothData);
                cutter->Update();
                vtkSmartPointer<vtkPolyData> cutterData = cutter->GetOutput();


                cutterData->GetCenter(centerNew.data());
                centerNew.y() += 1;

                /*
                vtkNew<vtkCenterOfMass> centerFilter;
                centerFilter->SetInputData(cutterData);
                centerFilter->SetUseScalarsAsWeights(false);
                centerFilter->Update();

                Eigen::Vector3d centerX2 {};
                centerFilter->GetCenter(centerX2.data());
                centerX2.y() += 0.8;
                */

                vtkNew<vtkPolyDataMapper> cutterMapper;
                cutterMapper->SetInputConnection(cutter->GetOutputPort());
                cutterMapper->ScalarVisibilityOff();


                vtkNew<vtkActor> planeActor;
                planeActor->GetProperty()->SetColor(std::array<double, 3>{0, 0, 1}.data());
                planeActor->GetProperty()->SetLineWidth(3);
                planeActor->SetMapper(cutterMapper);

                actors.emplace_back(planeActor);
                actors.push_back(Utilities::getPointsActor({centerNew}, {0, 1, 0}));
            }


            Eigen::Vector3d start = center - axes.horizontal, end = center + axes.horizontal;
            // Eigen::Vector3d start = centerNew - axes.horizontal, end = centerNew + axes.horizontal;

            const auto[slopeY, interceptY] =
            Utils::getLineCoefficients({start.x(), start.y()}, {end.x(), end.y()});
            const auto[slopeZ, interceptZ] =
            Utils::getLineCoefficients({start.x(), start.z()}, {end.x(), end.z()});

            // the ratio of the current length of the segment to the length of the tooth
            const double segLength = Utils::twoPointDistance(start, end);
            const double ratio = tooth.mesiodistalWidth / segLength;
            const double xDist = std::abs(start.x() - end.x());

            const double yDiff = 4;

            start.x() = center.x() - ratio * xDist / 2;
            end.x() = center.x() + ratio * xDist / 2;
            start.y() = start.x() * slopeY + interceptY + yDiff;
            end.y() = end.x() * slopeY + interceptY + yDiff;
            start.z() = start.x() * slopeZ + interceptZ;
            end.z() = end.x() * slopeZ + interceptZ;

            actors.push_back(Utilities::getPolyDataActor(toothData));
            actors.push_back(Utilities::getLineActor(start, end, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({center}));
            actors.push_back(Utilities::getPointsActor({start, end}, {1, 0, 0}));
        }
        Utilities::DisplayActors(actors);
    }

    void Visulize_ContactPoints_Ellipse() {
        constexpr std::string_view planJsonFile{R"(/home/andtokm/Projects/data/cases/2280/Plan.json)"};
        constexpr std::string_view filePath{
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)"};

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        // TODO: Check for performance: pass polyData&& (as RValue ref) ???
        std::unique_ptr<Estimators::Estimator> estimator{std::make_unique<Estimators::DistanceEstimator>()};
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&&[toothId, polyData]: toothMap) {
            if (auto[iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
                estimator->estimate(iter->second);
            }
        }

        // INFO: Get centroids:
        std::array<Eigen::Vector3d, 50> centroids;
        for (const auto &[id, teethData]: allTeeth) {
            const Eigen::Vector3d &center = teethData.GetCenter();
            // INFO: we should swap Y and Z and then set new Z == 0
            centroids[id] = {center.x(), center.z(), 0};
        }

        const auto contactPoints = CalcTeethContactPoints(allTeeth);
        const Eigen::Vector3d &centerContactPoints = contactPoints.at({41, 31}),
                &centerRightMolar = centroids.at(37), &centerLeftMolar = centroids.at(47);
        const double xShiftCenter = centerContactPoints.x(),
                yBottom = std::min(centerLeftMolar.y(), centerRightMolar.y()),
                yTop = centerContactPoints.z();

        // INFO: Get line-segments from contact points:
        std::array<Geometry::Line2D, 50> segments;
        constexpr std::array<int, 14> teethIds{47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};
        for (size_t i = 2; i < teethIds.size(); ++i) {
            const int toothId = teethIds[i - 1], id2 = teethIds[i - 2], id1 = teethIds[i];
            const Utils::UnorderedPair<int> pair2{id2, toothId}, pair1{id1, toothId};
            const auto ptIter2 = contactPoints.find(pair2),
                    ptIter1 = contactPoints.find(pair1);
            if (contactPoints.end() != ptIter2 && contactPoints.end() != ptIter1) {
                const Eigen::Vector3d &pt1 = ptIter1->second, &pt2 = ptIter2->second;
                segments[toothId] = Geometry::Line2D{{pt1.x() - xShiftCenter, pt1.z() - yBottom},
                                                     {pt2.x() - xShiftCenter, pt2.z() - yBottom}};
            }
        }

        // TODO: Need to ensure that segment points are sorted along the X axis
        //       [47].x() --> [46].x() -> .... -> [41].x() -> [31].x() --> ...
        // TODO: Place segments center to X [0, 0]

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (int i: {46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36}) {
            const Geometry::Line2D &line = segments[i];
            actors.push_back(getLine2DActor(line));
            actors.push_back(Utilities::getPointsActor({
                                                               {line.getPoint1().x(), line.getPoint1().y(), 0},
                                                               {line.getPoint2().x(), line.getPoint2().y(), 0}
                                                       }));
        }

        const double xLeft = centerLeftMolar.x(), xRight = centerRightMolar.x();
        const double A = std::abs(xRight - xLeft) / 2,
                B = std::abs(yTop - yBottom);
        actors.push_back(getEllipseActor(A, B));

        Utilities::DisplayActors(actors);
    }

    void placeSegmentOnCurve(Geometry::Line2D &line,
                             const Geometry::Point2D &dest,
                             const std::function<double(double)> &func) noexcept {
        line.setPoint1(dest);
        auto closest = findClosetPoint(line.getPoint2(), func);
        double dist = closest.distanceTo(line.getPoint2());

        // INFO: Determine rotation direction
        line.rotateAroundPoint(line.getPoint1(), 0.01);
        closest = findClosetPoint(line.getPoint2(), func);
        const double direction = closest.distanceTo(line.getPoint2()) < dist ? 1 : -1;
        line.rotateAroundPoint(line.getPoint1(), -0.01);

        const bool under = func(line.getPoint2().x()) > line.getPoint2().y() ? true : false;
        for (int angle = 0; angle < 45; ++angle) {
            line.rotateAroundPoint(line.getPoint1(), direction);
            const double yLine = line.getPoint2().y(), yCurve = func(line.getPoint2().x());
            if ((under && yLine >= yCurve) || (!under && yCurve >= yLine))
                break;
        }
    }

    double placeSegmentOnCurveMetrics(Geometry::Line2D &line,
                                      const Geometry::Point2D &dest,
                                      const std::function<double(double)> &func) noexcept {
        line.setPoint1(dest);
        auto closest = findClosetPoint(line.getPoint2(), func);
        double dist = closest.distanceTo(line.getPoint2());

        // INFO: Determine rotation direction
        line.rotateAroundPoint(line.getPoint1(), 0.01);
        closest = findClosetPoint(line.getPoint2(), func);
        const double direction = closest.distanceTo(line.getPoint2()) < dist ? 1 : -1;
        line.rotateAroundPoint(line.getPoint1(), -0.01);

        double angle = 0;
        const bool under = func(line.getPoint2().x()) > line.getPoint2().y() ? true : false;
        for (int i = 0; i < 45; ++i) {
            line.rotateAroundPoint(line.getPoint1(), direction);
            const double yLine = line.getPoint2().y(), yCurve = func(line.getPoint2().x());
            if ((under && yLine >= yCurve) || (!under && yCurve >= yLine))
                break;
            angle = i;
        }
        return angle;
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


    void PlaceSegmentsToTheCurve() {
        // 2280 2880 2425
        constexpr std::string_view planJsonFile{R"(/home/andtokm/Projects/data/cases/2280/Plan.json)"};
        constexpr std::string_view filePath{
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)"};

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        // TODO: Check for performance: pass polyData&& (as RValue ref) ???
        std::unique_ptr<Estimators::Estimator> estimator{std::make_unique<Estimators::DistanceEstimator>()};
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&&[toothId, polyData]: toothMap) {
            if (auto[iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
                estimator->estimate(iter->second);
            }
        }

        // INFO: Get centroids:
        std::array<Eigen::Vector3d, 50> centroids;
        for (const auto &[id, teethData]: allTeeth) {
            const Eigen::Vector3d &center = teethData.GetCenter();
            // INFO: we should swap Y and Z and then set new Z == 0
            centroids[id] = {center.x(), center.z(), 0};
        }

        const auto contactPoints = CalcTeethContactPoints(allTeeth);
        const Eigen::Vector3d &centerContactPoints = contactPoints.at({41, 31}),
                &centerRightMolar = centroids.at(37), &centerLeftMolar = centroids.at(47);
        const double xShiftCenter = centerContactPoints.x(),
                yBottom = std::min(centerLeftMolar.y(), centerRightMolar.y()),
                yTop = centerContactPoints.z();

        // INFO: Get line-segments from contact points:
        std::array<Geometry::Line2D, 50> segments;
        constexpr std::array<int, 14> teethIds{47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};
        for (size_t i = 2; i < teethIds.size(); ++i) {
            const int toothId = teethIds[i - 1], id2 = teethIds[i - 2], id1 = teethIds[i];
            const Utils::UnorderedPair<int> pair2{id2, toothId}, pair1{id1, toothId};
            const auto ptIter2 = contactPoints.find(pair2),
                    ptIter1 = contactPoints.find(pair1);
            if (contactPoints.end() != ptIter2 && contactPoints.end() != ptIter1) {
                const Eigen::Vector3d &pt1 = ptIter1->second, &pt2 = ptIter2->second;
                segments[toothId] = Geometry::Line2D{{pt1.x() - xShiftCenter, pt1.z() - yBottom},
                                                     {pt2.x() - xShiftCenter, pt2.z() - yBottom}};
            }
        }

        // TODO: Place segments center to X [0, 0]
        // FIXME: It should be reversed!! I've messed up right and left
        for (auto toothID: FDI::lowerTeeth) {
            Geometry::Line2D &line = segments[toothID];
            if (FDI::isLeftSideTooth(toothID) && line.getPoint1().x() > line.getPoint2().x())
                line.swapPoints();
            else if (FDI::isRightSideTooth(toothID) && line.getPoint2().x() > line.getPoint1().x())
                line.swapPoints();
        }

        const double xLeft = centerLeftMolar.x(), xRight = centerRightMolar.x();
        double A = std::abs(xRight - xLeft) / 2, B = std::abs(yTop - yBottom);

        A += 5;

        for (int X = 0; X < 30; X++)
        {
            A -= 0.3;

            auto ellipseOld = [A, B](double x) -> double {
                if (std::abs(x) >= A || 0 == B || 0 == B)
                    return 0;
                return std::sqrt((B * B) * (1 - (x * x) / (A * A)));
            };

            auto equationEllipse = [](double x, double A, double B) -> double {
                return std::sqrt((B * B) * (1 - (x * x) / (A * A)));
            };

            // INFO: Using the previous coordinate to calculate the parameters of the line
            // INFO: previous - there must be a lateral point of the previous segment
            auto curveFunction = [=](double x, int id, const Geometry::Point2D& previous) -> double {
                // constexpr double xLinePos = 17; // FIXME: random
                if (std::abs(x) > A || 0 == A|| 0 == B)
                    return 0;

                // INFO: Only for last premolar and molars
                if (id >= 5) {
                    const auto[slope, intercept] =
                    Geometry::Line2D::getLineCoefficients({x > 0 ? A : -A, 0}, previous);
                    return x * slope + intercept;
                } else {
                    return equationEllipse(x, A, B);
                }
            };

            std::vector<vtkSmartPointer<vtkActor>> actors;

            double xMove = 0, yMove = 0;
            for (int i: {30, 40}) {
                Geometry::Point2D dest{0, curveFunction(0, 0, {0, 0})};
                for (int n = 1; n <= 6; ++n) {
                    const int toothID = i + n;
                    Geometry::Line2D line = segments[toothID];
                    const Geometry::Point2D &centerOrigin = line.getMidPoint();

                    placeSegmentOnCurveMetricsEx(line, toothID, dest, curveFunction);
                    dest = line.getPoint2();
                    const Geometry::Point2D &centerNew = line.getMidPoint();

                    xMove += std::abs(centerNew.x()) - std::abs(centerOrigin.x());
                    yMove += std::abs(centerNew.y()) - std::abs(centerOrigin.y());

                    actors.push_back(getLine2DActor(line, 0, {0, 1, 0}));
                    actors.push_back(Utilities::getPointsActor({
                        {line.getPoint1().x(), line.getPoint1().y(), 0},
                        {line.getPoint2().x(), line.getPoint2().y(), 0}}));
                }
            }
            std::cout << xMove << "   " << yMove << std::endl;
            if (0 > yMove)
                B += 0.1;


            {
                for (int i: {30, 40}) {
                    Geometry::Point2D dest{0, curveFunction(0, 0, {0, 0})};
                    for (int n = 1; n <= 6; ++n) {
                        const int toothID = i + n;
                        Geometry::Line2D &line = segments[toothID];
                        actors.push_back(getLine2DActor(line));
                        actors.push_back(Utilities::getPointsActor({
                            {line.getPoint1().x(), line.getPoint1().y(), 0},
                            {line.getPoint2().x(), line.getPoint2().y(), 0}}));
                    }
                }
            }

            actors.push_back(getEllipseActor(A, B));
            Utilities::DisplayActors(actors);
        }
    }

    void FitTheCurve() {
        // 2280 2880 2425
        constexpr std::string_view planJsonFile{R"(/home/andtokm/Projects/data/cases/2280/Plan.json)"};
        constexpr std::string_view filePath{
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)"};

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        // TODO: Check for performance: pass polyData&& (as RValue ref) ???
        std::unique_ptr<Estimators::Estimator> estimator{std::make_unique<Estimators::DistanceEstimator>()};
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&&[toothId, polyData]: toothMap) {
            if (auto[iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
                estimator->estimate(iter->second);
            }
        }

        // INFO: Get centroids:
        std::array<Eigen::Vector3d, 50> centroids;
        for (const auto &[id, teethData]: allTeeth) {
            const Eigen::Vector3d &center = teethData.GetCenter();
            // INFO: we should swap Y and Z and then set new Z == 0
            centroids[id] = {center.x(), center.z(), 0};
        }

        const auto contactPoints = CalcTeethContactPoints(allTeeth);
        const Eigen::Vector3d &centerContactPoints = contactPoints.at({41, 31}),
                &centerRightMolar = centroids.at(37), &centerLeftMolar = centroids.at(47);
        const double xShiftCenter = centerContactPoints.x(),
                yBottom = std::min(centerLeftMolar.y(), centerRightMolar.y()),
                yTop = centerContactPoints.z();

        // INFO: Get line-segments from contact points:
        std::array<Geometry::Line2D, 50> segments;
        constexpr std::array<int, 14> teethIds{47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};
        for (size_t i = 2; i < teethIds.size(); ++i) {
            const int toothId = teethIds[i - 1], id2 = teethIds[i - 2], id1 = teethIds[i];
            const Utils::UnorderedPair<int> pair2{id2, toothId}, pair1{id1, toothId};
            const auto ptIter2 = contactPoints.find(pair2),
                    ptIter1 = contactPoints.find(pair1);
            if (contactPoints.end() != ptIter2 && contactPoints.end() != ptIter1) {
                const Eigen::Vector3d &pt1 = ptIter1->second, &pt2 = ptIter2->second;
                segments[toothId] = Geometry::Line2D{{pt1.x() - xShiftCenter, pt1.z() - yBottom},
                                                     {pt2.x() - xShiftCenter, pt2.z() - yBottom}};
            }
        }

        // TODO: Place segments center to X [0, 0]
        for (auto toothID: FDI::lowerTeeth) {
            Geometry::Line2D &line = segments[toothID];
            if (FDI::isLeftSideTooth(toothID) && line.getPoint1().x() > line.getPoint2().x())
                line.swapPoints();
            else if (FDI::isRightSideTooth(toothID) && line.getPoint2().x() > line.getPoint1().x())
                line.swapPoints();
        }

        const double xLeft = centerLeftMolar.x(), xRight = centerRightMolar.x();
        double A = std::abs(xRight - xLeft) / 2, B = std::abs(yTop - yBottom);
        std::cout << "A = " << A << ", B = " << B << std::endl;

        // TODO: Check performance to pass std::array<Geometry::Line2D, 50
        //       to the estimation / minimization method
        auto ellipse = [&A, &B](double x, [[maybe_unused]] int id, const Geometry::Point2D& previous) -> double {
            if (std::abs(x) >= A || 0 == B || 0 == B)
                return 0;
            return std::sqrt((B * B) * (1 - (x * x) / (A * A)));
        };

        A += 2;
        B += 3;

        double xMovePrev = 100500, yMovePrev = 100500;
        int epoch = 1000;
        const double step = 0.05;
        while (epoch--) {
            double xMove = 0, yMove = 0;
            for (int i: {30, 40}) {
                Geometry::Point2D dest{0, ellipse(0, 0, {})};
                for (int n = 1; n <= 6; ++n) {
                    const int toothID = i + n;
                    Geometry::Line2D line = segments[toothID];
                    const Geometry::Point2D &centerOrigin = line.getMidPoint();

                    placeSegmentOnCurveMetricsEx(line, toothID, dest, ellipse);
                    dest = line.getPoint2();

                    const Geometry::Point2D &centerNew = line.getMidPoint();
                    const double moveDist = centerNew.distanceTo(centerOrigin);

                    // TODO: Calc oll X move and Y move for all teeth ,,
                    if (1 == n || 2 == n || 3 == n)
                        yMove += moveDist;
                    if (4 == n || 5 == n || 6 == n)
                        xMove += moveDist;
                }
            }

            double xStep = xMovePrev - xMove, yStep = yMovePrev - yMove;
            if (xStep > 0) {
                A -= step;
                xMovePrev = xMove;
            }
            if (yStep > 0) {
                B -= step;
                yMovePrev = yMove;
            }

            // TODO: Add SIGN to distance returned from placeSegmentOnCurveMetricsEx
            //       in case when points within the ellipse
            // TODO: Add condition to stop
        }

        std::cout << "A = " << A << ", B = " << B << std::endl;


        std::vector<vtkSmartPointer<vtkActor>> actors;
        {
            for (int i: {30, 40}) {
                Geometry::Point2D dest{0, ellipse(0, 0, {})};
                for (int n = 1; n <= 6; ++n) {
                    const int toothID = i + n;
                    Geometry::Line2D &line = segments[toothID];
                    actors.push_back(getLine2DActor(line));
                    actors.push_back(Utilities::getPointsActor({
                                                                       {line.getPoint1().x(), line.getPoint1().y(), 0},
                                                                       {line.getPoint2().x(), line.getPoint2().y(), 0}}));
                }
            }
            actors.push_back(getEllipseActor(A, B));
        }
        Utilities::DisplayActors(actors);
    }

    void FitTheCurve2() {
        // 2280 2880 2425 2457 2494 2630 2929
        constexpr std::string_view planJsonFile{R"(/home/andtokm/Projects/data/cases/2280/Plan.json)"};
        constexpr std::string_view filePath{
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)"};

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Utils::ReadTeethObj(filePath);

        // TODO: Check for performance: pass polyData&& (as RValue ref) ???
        std::unique_ptr<Estimators::Estimator> estimator{std::make_unique<Estimators::DistanceEstimator>()};
        std::unordered_map<unsigned short, Model::Tooth> allTeeth;
        for (auto&&[toothId, polyData]: toothMap) {
            if (auto[iter, ok] = allTeeth.emplace(toothId, Model::Tooth{polyData, toothId}); ok) {
                iter->second.setAxis(plan.modellingData.tooth.at(toothId).axes); // FIXME ??
                estimator->estimate(iter->second);
            }
        }

        // INFO: Get centroids:
        std::array<Eigen::Vector3d, 50> centroids;
        for (const auto &[id, teethData]: allTeeth) {
            const Eigen::Vector3d &center = teethData.GetCenter();
            // INFO: we should swap Y and Z and then set new Z == 0
            centroids[id] = {center.x(), center.z(), 0};
        }

        const auto contactPoints = CalcTeethContactPoints(allTeeth);
        const Eigen::Vector3d &centerContactPoints = contactPoints.at({41, 31}),
                &centerRightMolar = centroids.at(37), &centerLeftMolar = centroids.at(47);
        const double xShiftCenter = centerContactPoints.x(),
                yBottom = std::min(centerLeftMolar.y(), centerRightMolar.y()),
                yTop = centerContactPoints.z();

        // INFO: Get line-segments from contact points:
        std::array<Geometry::Line2D, 50> segments;
        constexpr std::array<int, 14> teethIds{47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};
        for (size_t i = 2; i < teethIds.size(); ++i) {
            const int toothId = teethIds[i - 1], id2 = teethIds[i - 2], id1 = teethIds[i];
            const Utils::UnorderedPair<int> pair2{id2, toothId}, pair1{id1, toothId};
            const auto ptIter2 = contactPoints.find(pair2),
                    ptIter1 = contactPoints.find(pair1);
            if (contactPoints.end() != ptIter2 && contactPoints.end() != ptIter1) {
                const Eigen::Vector3d &pt1 = ptIter1->second, &pt2 = ptIter2->second;
                segments[toothId] = Geometry::Line2D{{pt1.x() - xShiftCenter, pt1.z() - yBottom},
                                                     {pt2.x() - xShiftCenter, pt2.z() - yBottom}};
            }
        }

        // TODO: Place segments center to X [0, 0]
        for (auto toothID: FDI::lowerTeeth) {
            Geometry::Line2D &line = segments[toothID];
            if (FDI::isLeftSideTooth(toothID) && line.getPoint1().x() > line.getPoint2().x())
                line.swapPoints();
            else if (FDI::isRightSideTooth(toothID) && line.getPoint2().x() > line.getPoint1().x())
                line.swapPoints();
        }

        const double xLeft = centerLeftMolar.x(), xRight = centerRightMolar.x();
        double A = std::abs(xRight - xLeft) / 2, B = std::abs(yTop - yBottom);
        std::cout << "A = " << A << ", B = " << B << std::endl;

        // TODO: Check performance to pass std::array<Geometry::Line2D, 50
        //       to the estimation / minimization method
        auto ellipse = [&A, &B](double x, [[maybe_unused]] int id, const Geometry::Point2D& previous) -> double {
            if (std::abs(x) >= A || 0 == B || 0 == B)
                return 0;
            return std::sqrt((B * B) * (1 - (x * x) / (A * A)));
        };

        A += 2;
        B += 3;

        int epoch = 10000;
        constexpr double step = 0.01;
        while (epoch--) {
            double xMove = 0, yMove = 0;
            for (int i: {30, 40}) {
                Geometry::Point2D dest{0, ellipse(0, 0, {})};
                for (int n = 1; n <= 6; ++n) {
                    const int toothID = i + n;
                    Geometry::Line2D line = segments[toothID];
                    const Geometry::Point2D &centerOrigin = line.getMidPoint();

                    placeSegmentOnCurveMetricsEx(line, toothID, dest, ellipse);
                    dest = line.getPoint2();

                    const Geometry::Point2D &centerNew = line.getMidPoint();

                    // INFO: Calc oll X move and Y move for all teeth ,,
                    xMove += std::abs(centerNew.x()) - std::abs(centerOrigin.x());
                    yMove += std::abs(centerNew.y()) - std::abs(centerOrigin.y());
                }
            }

            if (xMove > 2 || yMove > 2) {
                if (xMove > yMove)
                    A -= step;
                else
                    B -= step;

                if (2 > yMove)
                    B += step;
                else if (2 > xMove)
                    A += step;
            } else break;
        }

        // INFO: Get final positions
        std::array<std::pair<Geometry::Point2D, double>, 50> finalPositions;
        for (int i: {30, 40}) {
            Geometry::Point2D dest{0, ellipse(0, 0, {})};
            for (int n = 1; n <= 6; ++n) {
                const int toothID = i + n;
                Geometry::Line2D line = segments[toothID];

                auto &[pt, angle] = finalPositions[toothID];
                [[maybe_unused]] const auto &[ok, an] =
                placeSegmentOnCurveMetricsEx(line, toothID, dest, ellipse);

                angle = an;
                pt = line.getMidPoint();
                dest = line.getPoint2();
            }
        }


        std::vector<vtkSmartPointer<vtkActor>> actors;
        {
            for (int i: {30, 40}) {
                Geometry::Point2D dest{0, ellipse(0, 0, {})};
                for (int n = 1; n <= 6; ++n) {
                    const int toothID = i + n;
                    Geometry::Line2D line = segments[toothID];
                    const Geometry::Point2D &centerOrigin = line.getMidPoint();
                    const Geometry::Point2D &centerDest = finalPositions[toothID].first;
                    const Geometry::Point2D &move = centerDest - centerOrigin;
                    double angle = finalPositions[toothID].second;

                    vtkSmartPointer<vtkPolyData> data = allTeeth.at(toothID).toothPolyData;

                    std::cout << toothID << "   " << centerOrigin.distanceTo(centerDest) << "   " << angle << "\n";

                    data = Utilities::rotatePolyData(data, 90, 180, 0);
                    data = Utilities::moveTooth(data, xShiftCenter + move.x(), -yBottom + move.y(), 0);
                    {
                        Eigen::Vector3d pos{};
                        data->GetCenter(pos.data());
                        data = Utilities::setPolyDataCenter(data, 0, 0, 0);
                        data = Utilities::rotatePolyData(data, 0, 0, angle);
                        data = Utilities::setPolyDataCenter(data, pos.x(), pos.y(), pos.z());
                    }

                    {
                        line.getPoint1() += move;
                        line.getPoint2() += move;
                        line.rotateAroundPoint(line.getMidPoint(), angle);

                        actors.push_back(getLine2DActor(line));
                        actors.push_back(Utilities::getPointsActor({
                                                                           {line.getPoint1().x(), line.getPoint1().y(), 0},
                                                                           {line.getPoint2().x(), line.getPoint2().y(), 0}}));
                    }

                    const auto toothActor = Utilities::getPolyDataActor(data);
                    actors.push_back(toothActor);
                }
            }

            for (int toothID: {37, 47}) {
                vtkSmartPointer<vtkPolyData> data = allTeeth.at(toothID).toothPolyData;
                data = Utilities::rotatePolyData(data, 90, 180, 0);
                data = Utilities::moveTooth(data, xShiftCenter, -yBottom, 0);
                const auto toothActor = Utilities::getPolyDataActor(data);
                actors.push_back(toothActor);
            }


            auto actor = getEllipseActor(A, B);
            actor->GetProperty()->SetLineWidth(4);
            actor->GetProperty()->SetColor(std::array<double, 3>{1, 0, 0}.data());
            actors.push_back(actor);
        }
        Utilities::DisplayActors(actors);


        /*
        std::vector<vtkSmartPointer<vtkActor>> actors;
        {
            for (int i: {30, 40}) {
                Geometry::Point2D dest{0, ellipse(0)};
                for (int n = 1; n <= 6; ++n) {
                    const int toothID = i + n;
                    Geometry::Line2D line = segments[toothID];
                    const Geometry::Point2D& centerOrigin = line.getMidPoint();
                    const Geometry::Point2D& centerDest = finalPositions[toothID].first;
                    const Geometry::Point2D& move = centerDest - centerOrigin;
                    double angel = finalPositions[toothID].second;

                    line.getPoint1() += move;
                    line.getPoint2() += move;
                    line.rotateAroundPoint(line.getMidPoint(), angel);

                    actors.push_back(getLine2DActor(line));
                    actors.push_back(Utilities::getPointsActor({
                        {line.getPoint1().x(), line.getPoint1().y(), 0},
                        {line.getPoint2().x(), line.getPoint2().y(), 0}}));
                }
            }
            {
                auto actor = getEllipseActor(A, B);
                actor->GetProperty()->SetLineWidth(4);
                actor->GetProperty()->SetColor(std::array<double, 3>{1, 1, 0}.data());
                actors.push_back(actor);
            }
        }
        Utilities::DisplayActors(actors);
             */
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

    void FitTheCurve_AlignedSegments(const std::vector<std::string_view>& params) {
        // 2280 2880 2425 2457 2494 2630 2929 2622
        const std::string caseId = params.empty() ? "2280" : std::string(params.front());
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

        const std::pair<double, double> orientAngles = Utils::orientTeethToPlane(allTeeth);
        SetUpTorksAndPositions::alignTeethTorks(allTeeth);
        SetUpTorksAndPositions::alignTeethVertical(allTeeth);


        const std::array<double, 6> &leftUpperIncisor{allTeeth.at(31).GetBounds()},
                &rightUpperIncisor{allTeeth.at(41).GetBounds()},
                &leftMolarBounds{allTeeth.at(37).GetBounds()},
                &rightMolarBounds{allTeeth.at(47).GetBounds()};

        const double xLeft = std::min({leftMolarBounds[0], leftMolarBounds[1], rightMolarBounds[0], rightMolarBounds[1]});
        const double xRight = std::max({leftMolarBounds[0], leftMolarBounds[1], rightMolarBounds[0], rightMolarBounds[1]});
        const double zBack = std::min({leftMolarBounds[4], leftMolarBounds[5],
                                       rightMolarBounds[4], rightMolarBounds[5]});
        const double zFront = std::max({leftUpperIncisor[4], leftUpperIncisor[5],
                                       rightUpperIncisor[4], rightUpperIncisor[5]});
        Eigen::Vector3d leftMolarCenter {allTeeth.at(37).GetCenter()};
        Eigen::Vector3d leftPreMolarCenter {allTeeth.at(35).GetCenter()};
        Eigen::Vector3d rightMolarCenter {allTeeth.at(47).GetCenter()};
        Eigen::Vector3d rightPreMolarCenter {allTeeth.at(45).GetCenter()};

        if (leftMolarCenter.x() > rightMolarCenter.x())
            std::swap(rightMolarCenter, leftMolarCenter);
        if (leftPreMolarCenter.x() > rightPreMolarCenter.x())
            std::swap(rightPreMolarCenter, leftPreMolarCenter);

        // FIXME: We should use the Incisor contact point instead of (xLeft + xRight)/2
        const double xShiftCenter { std::midpoint(xLeft, xRight) };
        const double xLimit { (std::abs(leftMolarCenter.x()) + std::abs(rightMolarCenter.x()))/2};

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


        {   // INFO: Visualize segments VS teeth
            std::vector<vtkSmartPointer<vtkActor>> actors;
            for (int i: {30, 40}) {
                for (int n = 1; n <= 7; ++n) {
                    const int toothID = i + n;
                    Geometry::Line2D& line = segments[toothID];
                    constexpr double yOffset = 5;

                    const auto lineActor = Utilities::getLineActor(
                            {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                            {line.getPoint2().x(), yOffset, line.getPoint2().y()}, {1, 0, 0});
                    actors.push_back(lineActor);

                    const auto ptsActor = Utilities::getPointsActor({
                            {line.getPoint1().x(), yOffset, line.getPoint1().y()},
                            {line.getPoint2().x(), yOffset, line.getPoint2().y()}});
                    actors.push_back(ptsActor);

                    vtkSmartPointer<vtkPolyData> toothData = allTeeth.at(toothID).toothPolyData;
                    const auto toothActor = Utilities::getPolyDataActor(toothData);

                    actors.push_back(toothActor);
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

        const auto gumsActor = getGumsActor(gumsPath, -xShiftCenter, 0, -zBack);
        double A = std::abs(xRight - xLeft) / 2, B = std::abs(zFront - zBack);
        double slopeLeft {0}, interceptLeft {0}, slopeRight {0}, interceptRight{0};

        std::tie(slopeLeft, interceptLeft) = Geometry::Line2D::getLineCoefficients(
                {leftPreMolarCenter.x() - xShiftCenter, leftPreMolarCenter.z() - zBack},
                {leftMolarCenter.x() - xShiftCenter, leftMolarCenter.z() -zBack});
        std::tie(slopeRight, interceptRight) = Geometry::Line2D::getLineCoefficients(
                {rightPreMolarCenter.x() - xShiftCenter, rightPreMolarCenter.z() - zBack},
                {rightMolarCenter.x() - xShiftCenter, rightMolarCenter.z() - zBack});

        auto equationEllipse = [](double x, double A, double B) -> double {
            if (std::abs(x) >= A)
                return 0;
            return std::sqrt((B * B) * (1 - (x * x) / (A * A)));
        };

        // INFO: Using the previous coordinate to calculate the parameters of the line
        // INFO: previous - there must be a lateral point of the previous segment
        auto curveEstimateFunction = [&](double x, int id, const Geometry::Point2D& previous) -> double {
            if (std::abs(x) > A || 0 == A|| 0 == B)
                return 0;

            // INFO: Only for last premolar and molars
            if (id > 4) {
                // TODO: use std::tie ????
                const auto [slope, intercept] = Geometry::Line2D::getLineCoefficients({x > 0 ? A : -A, 0}, previous);
                if (x > 0) {
                    slopeRight = slope;
                    interceptRight = intercept;
                } else {
                    slopeLeft = slope;
                    interceptLeft = intercept;
                }

                return x * slope + intercept;
            } else {
                return equationEllipse(x, A, B);
            }
        };

        auto curveFunctionFinalEx = [&](double x, int, const Geometry::Point2D&) -> double {
            if (std::abs(x) > A || 0 == A|| 0 == B)
                return 0;

            const double zEllipse = equationEllipse(x, A, B);
            const double zLine = x > 0 ?x * slopeRight + interceptRight : x * slopeLeft + interceptLeft;
            return std::min(zEllipse, zLine);
        };

        auto getCurveActor = [&](double y = 0) {
            std::vector<Eigen::Vector3d> points;
            for (double x = -A; x <= A; x += 0.01)
            {
                const double zEllipse = equationEllipse(x, A, B);
                const double zLine = x > 0 ?x * slopeRight + interceptRight : x * slopeLeft + interceptLeft;
                points.emplace_back(x, y, std::min(zEllipse, zLine));
            }
            return Utilities::getPointsActor(points, {1, 1, 0}, 5);
        };

        const auto CURVE_ESTIMATE = curveEstimateFunction;

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
                        if (A > (xLimit )) {
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

        /*
        std::cout << "====================================================================================\n";
        std::cout << "A: " << A << ", B: " << B << std::endl;
        std::cout << "(" << slopeLeft << ", " << interceptLeft << ") (" << slopeRight << "," << interceptRight << ")\n";
        std::cout << "yMax = " << curveFunctionFinalEx(0, {}, {}) << std::endl;
        std::cout << zFront - zBack<< std::endl;
        std::cout << "====================================================================================\n";
        return;
        */

#if 0
        {   // INFO: Visualize final segments vs original
            std::vector<vtkSmartPointer<vtkActor>> actors;
            for (int i: {30, 40}) {
                Geometry::Point2D dest{0, curveFunctionFinal(0, 0, {})};
                for (int n = 1; n <= 7; ++n)
                {
                    const int toothID = i + n;
                    const Geometry::Line2D& line = segmentsFinal[toothID];
                    actors.push_back(getLine2DActor(line));
                    actors.push_back(Utilities::getPointsActor({
                        {line.getPoint1().x(), 0, line.getPoint1().y()},
                        {line.getPoint2().x(), 0, line.getPoint2().y()}}));

                    const Geometry::Line2D& lineOrig = segments[toothID];
                    const auto lineActor = Utilities::getLineActor(
                            {lineOrig.getPoint1().x(), 0, lineOrig.getPoint1().y()},
                            {lineOrig.getPoint2().x(), 0, lineOrig.getPoint2().y()}, {0, 1, 1});
                    actors.push_back(lineActor);

                    const auto ptsActor = Utilities::getPointsActor({
                        {lineOrig.getPoint1().x(), 0, lineOrig.getPoint1().y()},
                        {lineOrig.getPoint2().x(), 0, lineOrig.getPoint2().y()}},  {0, 1, 0});
                    actors.push_back(ptsActor);
                }
            }
            actors.push_back(getEllipseActor(A, B));
            Utilities::DisplayActors(actors);
        }
#endif


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
            Utilities::DisplayActors(actors);
        }
#endif


#if 1
        // TODO: This is BAD! tmp --> refactor
        // INFO: Move teeth to segments positions
        std::unordered_map<int, vtkSmartPointer<vtkPolyData>> placedOnCurveTeeth;
        {
            for (int i: {30, 40}) {
                Geometry::Point2D dest{0, curveFunctionFinalEx(0, 0, {})};
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
            Utilities::DisplayActors(actors, {1950, 50}, caseId);
        }
#endif

#if 0
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        using PairDist = std::pair<std::pair<unsigned short,unsigned short>, double>;
        std::vector<std::future<PairDist>> jobs;
        constexpr std::array<unsigned short , 14> ids {47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37 };
        for (int i = 1; i < ids.size(); i++){
            jobs.emplace_back(std::async(std::launch::async,[&](int id1, int id2) -> PairDist {
                const double dist = getDistance(placedOnCurveTeeth[id1], placedOnCurveTeeth[id2]);
                return PairDist {{id1, id2}, dist};
            }, ids[i -1], ids[i]));
        }

        std::unordered_map<Utils::UnorderedPair<int>, double, Utils::PairHashUnordered<int>> dists;
        std::for_each(jobs.begin(), jobs.end(), [&dists] (auto &F) {
            const PairDist result = F.get();
            dists.emplace(Utils::UnorderedPair<int>{result.first.first, result.first.second}, result.second);
        });

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
        std::cout << "It took me " << time_span.count() << " seconds.\n";

        for (const auto& entry: dists)
            std::cout << "[" << entry.first.first << ", " << entry.first.second << "] = "
                      << entry.second << std::endl;
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
            data = Utilities::rotatePolyData(data, 0, -angle, 0);
            data = Utilities::setPolyDataCenter(data, origin);
            return Utilities::moveTooth(data,  move.x(), 0, move.y());
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
        };

        auto moveSegment = [&](int toothId, double xStep,
                               std::vector<vtkSmartPointer<vtkActor>>& actorsLocal) {
            Geometry::Line2D line = segmentsFinal[toothId];
            Geometry::Point2D dest {line.getPoint1().x() + xStep,
                                    curveFunctionFinalEx(line.getPoint1().x() + xStep, toothId % 10, {})};
            const auto &metrics = placeSegmentOnCurveMetricsEx(line, toothId, dest, curveFunctionFinalEx);
#if 0
            {
                constexpr double yOffset = 5;
                const auto lineActor = Utilities::getLineActor(
                        {line.getPoint1().x(), 0, line.getPoint1().y()},
                        {line.getPoint2().x(), 0, line.getPoint2().y()}, {1, 0, 0});
                actorsLocal.push_back(lineActor);

                const auto ptsActor = Utilities::getPointsActor({
                    {line.getPoint1().x(), 0, line.getPoint1().y()},
                    {line.getPoint2().x(), 0, line.getPoint2().y()}});
                actorsLocal.push_back(ptsActor);

                actorsLocal.push_back(Utilities::getPointsActor({{dest.x(), 0, dest.y()}}, {0,1,0}, 14));
            }
#endif
            auto &[pt, angle] = transformFinal[toothId];
            pt = line.getMidPoint();
            angle = metrics.second;
        };

#if 0
        {
            constexpr double stepSize = { 0.01 };
            constexpr std::array<int, 7> leftIds {31, 32, 33, 34, 35, 36 ,37};
            std::vector<vtkSmartPointer<vtkActor>> actors {
                    Utilities::getPolyDataActor(allTeethLight.at(leftIds.front()))  };

            for (size_t i = 1; i < leftIds.size(); ++i)
            {
                const int id1 = leftIds[i - 1], id2 = leftIds[i];
                const short direction = segmentsFinal[id2].getPoint1().x() > segmentsFinal[id1].getPoint1().x() ? 1 : -1;
                const vtkSmartPointer<vtkPolyData> toothDest = allTeethLight.at(id1);
                vtkSmartPointer<vtkPolyData> toothToMove = allTeethLight.at(id2);

                double stepTotal = 0.0;
                const bool isIntersectedBefore = isTeethIntersect(toothDest, toothToMove);
                const double deltaStep = stepSize * direction;
                bool wasMoved {false};

                std::cout << "[" << id1 << "-" << id2 << "]. Intersected = "
                          << std::boolalpha << isIntersectedBefore << std::endl;

                while (true && isIntersectedBefore) {
                    wasMoved = true;
                    stepTotal += deltaStep;
                    moveSegment(id2, stepTotal, actors);
                    toothToMove = applyTransformation(id2, allTeethLight.at(id2));


                    // const bool isIntersectedNow = isTeethIntersect(toothDest, toothToMove);
                    // std::cout << "          Intersected now = " << std::boolalpha << isIntersectedNow << std::endl;

                    const double distance = getDistance(toothDest, toothToMove);
                    std::cout << "          Distance: " << std::boolalpha << distance << std::endl;

                    // if (false == isIntersectedNow) {
                    if (distance >= 0.5) {

                        /*
                        std::cout << getDistance(toothDest,toothToMove ) << std::endl;
                        std::vector<vtkSmartPointer<vtkActor>> actorsTmp {
                                Utilities::getPolyDataActor(toothDest) ,  Utilities::getPolyDataActor(toothToMove)
                        };
                        Utilities::DisplayActors(actorsTmp);*/

                        /*
                        if (false == isTeethIntersect(toothDest, toothToMove)) {
                            std::vector<vtkSmartPointer<vtkActor>> actorsTmp1{
                                    Utilities::getPolyDataActor(toothDest), Utilities::getPolyDataActor(toothToMove)
                            };
                            Utilities::DisplayActors(actorsTmp1);
                        }
                         */

                        break;
                    }
                }
                if (wasMoved)
                    allTeethLight.at(id2) = toothToMove;
                actors.push_back(Utilities::getPolyDataActor(allTeethLight.at(id2)));
            }

            {
                actors.push_back(getCurveActor(5));
                Utilities::DisplayActors(actors);
                actors.clear();
                actors.push_back(Utilities::getPolyDataActor(allTeethLight.at(leftIds.front())));
            }

            for (size_t i = 1; i < leftIds.size(); ++i)
            {
                const int id1 = leftIds[i - 1], id2 = leftIds[i];
                const short direction = segmentsFinal[id2].getPoint1().x() > segmentsFinal[id1].getPoint1().x() ? 1 : -1;
                const vtkSmartPointer<vtkPolyData> toothDest = allTeethLight.at(id1);
                vtkSmartPointer<vtkPolyData> toothToMove = allTeethLight.at(id2);

                double stepTotal = 0.0;
                const double deltaStep = stepSize * direction;
                while (true) {
                    stepTotal -= deltaStep;
                    moveSegment(id2, stepTotal, actors);
                    toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                    if (true == isTeethIntersect(toothDest, toothToMove)) {
                        stepTotal += deltaStep;
                        moveSegment(id2, stepTotal, actors);
                        toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                        break;
                    }
                }
                allTeethLight.at(id2) = toothToMove;
                actors.push_back(Utilities::getPolyDataActor(allTeethLight.at(id2)));
            }

            actors.push_back(getCurveActor(5));
            Utilities::DisplayActors(actors);
        }
#endif


#if 1
        // TODO: ****** Aling 31 and 41 ***********

        std::mutex mtx;
        constexpr double stepSize = { 0.01 }, fallbackFactor {5.0};
        std::vector<vtkSmartPointer<vtkActor>> actors { Utilities::getPolyDataActor(allTeethLight.at(31)) };

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
                bool wasMoved {false};

                while (true && isIntersectedBefore) {
                    wasMoved = true;
                    stepTotal += deltaStep;
                    moveSegment(id2, stepTotal, actors);
                    toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                    toothToMoveNormal = applyTransformation(id2, placedOnCurveTeeth.at(id2));

                    // if (false ==isTeethIntersect(toothDest, toothToMove)) break;
                    if (getDistance(toothDest, toothToMove) > 0.5) break;
                }
                if (wasMoved) {
                    allTeethLight.at(id2) = toothToMove;
                    placedOnCurveTeeth.at(id2) = toothToMoveNormal;
                }
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
                    moveSegment(id2, stepTotal, actors);
                    toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                    toothToMoveNormal = applyTransformation(id2, placedOnCurveTeeth.at(id2));
                    if (true == isTeethIntersect(toothDest, toothToMove)) {
                        moveSegment(id2, stepTotal + fallbackFactor * deltaStep, actors);
                        toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                        break;
                    }
                }
                allTeethLight.at(id2) = toothToMove;
                placedOnCurveTeeth.at(id2) = toothToMoveNormal;
            }
        });

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
                    moveSegment(id2, stepTotal, actors);
                    toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                    toothToMoveNormal = applyTransformation(id2, placedOnCurveTeeth.at(id2));

                    // if (false ==isTeethIntersect(toothDest, toothToMove)) break;
                    if (getDistance(toothDest, toothToMove) > 0.5) break;
                }
                if (wasMoved) {
                    allTeethLight.at(id2) = toothToMove;
                    placedOnCurveTeeth.at(id2) = toothToMoveNormal;
                }
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
                    moveSegment(id2, stepTotal, actors);
                    toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                    toothToMoveNormal = applyTransformation(id2, placedOnCurveTeeth.at(id2));
                    if (true == isTeethIntersect(toothDest, toothToMove)) {
                        moveSegment(id2, stepTotal + fallbackFactor * deltaStep, actors);
                        toothToMove = applyTransformation(id2, allTeethLight.at(id2));
                        break;
                    }
                }
                allTeethLight.at(id2) = toothToMove;
                placedOnCurveTeeth.at(id2) = toothToMoveNormal;
            }
        });

        processRight.wait();
        processLeft.wait();

        vtkSmartPointer<vtkAppendPolyData> dataAppender { vtkAppendPolyData::New() };
        for (int toothID: {47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36 ,37}) {
            // auto toothFinalData = allTeethLight.at(toothID);
            auto toothFinalData = placedOnCurveTeeth.at(toothID);
            toothFinalData = Utilities::moveTooth(toothFinalData, xShiftCenter, 0, zBack);
            dataAppender->AddInputData(toothFinalData);
        }
        dataAppender->Update();
        const auto teethData = dataAppender->GetOutput();
        auto teethDataNew = Utilities::rotatePolyData(teethData, -orientAngles.first, 0, -orientAngles.second);
        teethDataNew = Utilities::moveTooth(teethDataNew, 0, 0, -3);


        actors.clear();
        actors.push_back(Utilities::getPolyDataActor(teethDataNew));

        // actors.push_back(getCurveActor(5));
        actors.push_back(gumsActor);
        Utilities::DisplayActors(actors, {1950, 50}, caseId);

        const std::string stlFileName {R"(/home/andtokm/tmp/15/)" + caseId + ".stl"};
        Utilities::WriteSTL(teethDataNew, stlFileName);
#endif
    }
}

void EstimateCurve::TestAll(const std::vector<std::string_view>& params)
{
    // DrawLine();
    // DrawEllipseTest();
    // FindClosetPoint_Test();

    // VisualizeTooths_LineSections();
    // FindMoveFitting_LineSegment_ForTooth();
    // VisualizeTooths_LineSections_ContactPoints();

    // Visulize_ContactPoints_Ellipse();
    // PlaceSegmentsToTheCurve();

    // FitTheCurve();
    // FitTheCurve2();    // INFO: X and Y ABS movements are used

    FitTheCurve_AlignedSegments(params);

};
