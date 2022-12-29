//============================================================================
// Name        : Tests.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Tests C++ project
//============================================================================

#include <iostream>
#include <memory>
#include <string>

#include <thread>
#include <future>

#include <mutex>
#include <atomic>

#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>

#include <vector>
#include <list>
#include <algorithm>
#include <array>
#include <set>
#include <span>
// #include <format>
#include <concepts>

#include "VtkHeaders.h"


#include "Model/Common.h"
#include "Model/FDI.h"
#include "Model/Jaw.h"
#include "Model/Tooth.h"
#include "Model/ToothAxis.h"

#include "Utilities/Utilities.h"
#include "TreatmentPlan/TreatmentPlan.h"
#include "Estimators/Estimator.h"
#include "Experiments/SetUpTorksAndPositions.h"
#include "Experiments/TeethAnglesOrientation.h"
#include "Experiments/EstimateCurve.h"
#include "Experiments/EstimateCurve2.h"
#include "Experiments/EstimateCurve3.h"
#include "Experiments/SetUpTeethSegments.h"
#include "Experiments/FindBestCurveTests.h"


namespace Tests {

    vtkSmartPointer<vtkActor> getPolyDataActor(std::string_view filePath)
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
}

namespace Experiments
{
    const vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors>::New() };

    template<typename T>
    struct UnorderedPair: std::pair<T, T> {
        UnorderedPair(T a, T b): std::pair<T, T> {a, b} {
            ///
        }

        template<typename _T>
        friend std::ostream& operator<<(std::ostream& stream, const UnorderedPair<_T>& pair);

        template<typename _T>
        friend bool operator==(const UnorderedPair<_T>& p1, const UnorderedPair<_T>& p2);
    };

    template<typename T>
    struct PairHashUnordered {
        std::size_t operator()(const UnorderedPair<T>& pair) const noexcept {
            std::size_t h1 = std::hash<T>{}(std::min(pair.first, pair.second));
            std::size_t h2 = std::hash<T>{}(std::max(pair.first, pair.second));
            return h1 ^ (h2 << 1);
        }
    };

    template<typename _T>
    std::ostream& operator<<(std::ostream& stream, const UnorderedPair<_T>& pair) {
        stream << "(" << pair.first << ", " << pair.second << ")";
        return stream;
    }

    template<typename _T>
    bool operator==(const UnorderedPair<_T>& p1, const UnorderedPair<_T>& p2) {
        return (p1.first == p2.first && p1.second == p2.second) ||
               (p1.second == p2.first && p1.first == p2.second);
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vect) {
        std::cout << "[" << std::setprecision(12);
        for (const T& entry : vect)
            stream << entry << ' ';
        std::cout << "]";
        return stream;
    }

    template<typename _Ty, size_t _Size>
    std::ostream& operator<<(std::ostream& stream, const std::array<_Ty, _Size>& data) {
        std::cout << "[" << std::setprecision(12);
        for (const _Ty& entry : data)
            stream << entry << ' ';
        std::cout << "]";
        return stream;
    }

    static inline constexpr std::array<std::pair<double, double>, 21> corkHouseTable {{
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
    }};

    // TODO: Use std::lepr for linear interpolation
    [[nodiscard]]
    double GetCorkhouseDist(double dist) noexcept {
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

        // TODO: std::lerp
        const double ratio = (dist - lowerBound.first) / (upperBound.first - lowerBound.first);
        const double offset = (upperBound.second - lowerBound.second) * ratio;

        return lowerBound.second + offset;
    }

    [[nodiscard]]
    double calcIncisorsLength(const Model::Jaw& jaw) {
        double dist { 0.0f };
        for (const auto& toothId: {12, 11, 21, 22})
            dist += jaw.teeth.at(toothId).mesiodistalWidth;
        return dist;
    }

    [[nodiscard]]
    double twoPointDistance(const Eigen::Vector3d& pt1,
                            const Eigen::Vector3d& pt2) noexcept {
        double sqrDist{ 0 };
        for (size_t i { 0 }; i < 3; ++i)
            sqrDist += std::pow(pt1[i] - pt2[i], 2);
        return std::sqrt(sqrDist);
    }

    // NOTE: 2D implementation (y or z == 0)
    [[nodiscard]]
    Eigen::Vector3d findClosetPoint(const Eigen::Vector3d& pt,
                                    const std::function<double(double)>& func) noexcept {
        double step = 0.01;

        // NOTE: Gradient like: Just to define the step direction
        {
            Eigen::Vector3d pt1 { pt.x() - step, 0, 0 }, pt2 { pt.x() + step, 0, 0 };
            pt1.y() = func(pt1.x());
            pt2.y() = func(pt2.x());
            const double d1 = twoPointDistance(pt, pt1);
            const double d2 = twoPointDistance(pt, pt2);
            if (d1 < d2)
                step *= -1;
        }

        double distPrev { std::numeric_limits<double>::max() };
        Eigen::Vector3d ptCloset { pt };
        while (true) {
            ptCloset.z() = func(ptCloset.x());
            const double distCurrent = twoPointDistance(pt, ptCloset);
            if (distCurrent > distPrev)
                break;
            distPrev = std::min(distPrev, distCurrent);
            ptCloset.x() += step;
        }
        return ptCloset;
    }

    [[nodiscard]] std::pair<double, double>
    get_line_coefficients(const Eigen::Vector2d& pt1,
                          const Eigen::Vector2d& pt2) noexcept {
        double slope = (pt2[1] - pt1[1]) / (pt2[0] - pt1[0]);
        double intercept = (pt2[0] * pt1[1] - pt1[0] * pt2[1]) / (pt2[0] - pt1[0]);
        return std::make_pair(slope, intercept);
    };

    // TODO: Fix bug [id1, id2] != [id2, id1]
    [[nodiscard]]
    Eigen::Vector3d getContactPoint(const vtkSmartPointer<vtkPolyData>& tooth1,
                                    const vtkSmartPointer<vtkPolyData>& tooth2) noexcept {
        const vtkSmartPointer<vtkIntersectionPolyDataFilter> booleanFilter {
                vtkSmartPointer<vtkIntersectionPolyDataFilter>::New() };
        booleanFilter->GlobalWarningDisplayOff();
        booleanFilter->SetInputData(0, tooth1);
        booleanFilter->SetInputData(1, tooth2);
        booleanFilter->Update();

        Eigen::Vector3d ptContact {};
        if (booleanFilter->GetNumberOfIntersectionPoints() > 0) {
            booleanFilter->GetOutput()->GetCenter(ptContact.data());
            return ptContact;
        }

        vtkSmartPointer<vtkPolyData> toothOne { vtkSmartPointer<vtkPolyData>::New() };
        vtkSmartPointer<vtkPolyData> toothTwo { vtkSmartPointer<vtkPolyData>::New() };
        toothOne->DeepCopy(tooth1);
        toothTwo->DeepCopy(tooth2);

        Eigen::Vector3d tooth1Center {}, tooth2Center {};
        toothOne->GetCenter(tooth1Center.data());
        toothTwo->GetCenter(tooth2Center.data());

        const auto [slopeY, interceptY] = get_line_coefficients({tooth1Center.x(), tooth1Center.y()},
                                                                {tooth2Center.x(), tooth2Center.y()});
        const auto [slopeZ, interceptZ] = get_line_coefficients({tooth1Center.x(), tooth1Center.z()},
                                                                {tooth2Center.x(), tooth2Center.z()});
        // TODO: Replace to Mid_Points from STD??
        const double xPos1 {tooth1Center.x() }, xPos2 {tooth2Center.x() };
        const double xMid { xPos2 - (xPos2 - xPos1) / 2 };
        constexpr size_t stepsNumber { 50 };
        const double step = (xMid - xPos1) / stepsNumber;

        for (double pos = xPos1; pos < xMid; pos += step) {
            booleanFilter->SetInputData(0, toothOne);
            booleanFilter->SetInputData(1, toothTwo);
            booleanFilter->Update();
            if (booleanFilter->GetNumberOfIntersectionPoints() > 0)
                break;

            const double x_pos2 { xPos2 + xPos1 - pos };
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

    auto CalcTeethContactPoints(const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>>& toothMap)
    -> decltype(auto)
    {
        constexpr std::array<int, 14> lowerIDs { 47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37 };
        constexpr std::array<int, 14> upperIDs { 17, 16, 15, 14, 13, 12, 11, 21, 22, 23, 24, 25, 26, 27 };

        const int bucketSize = std::thread::hardware_concurrency();
        std::vector<std::vector<std::pair<int, int>>> toothPairs(bucketSize);
        for (int bucket = 0; const auto& ids: { lowerIDs, upperIDs }) {
            for (size_t size = ids.size(), i = 1; i < size; ++i) {
                toothPairs[bucket++].emplace_back(ids[i - 1], ids[i]);
                bucket = (bucketSize == bucket) ? 0 : bucket;
            }
        }

        std::mutex mtx;
        std::unordered_map<UnorderedPair<int>, Eigen::Vector3d, PairHashUnordered<int>> contactPoints;
        auto handler = [&] (const std::vector<std::pair<int, int>>& bucket) {
            for (const auto &[id1, id2]: bucket) {
                const vtkSmartPointer<vtkPolyData> tooth1 = toothMap.at(id1), tooth2 = toothMap.at(id2);
                const Eigen::Vector3d ptContact = getContactPoint(tooth1, tooth2);
                {
                    std::lock_guard<std::mutex> lock {mtx};
                    contactPoints.emplace(UnorderedPair<int>{id1, id2}, ptContact);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
        };

        std::vector<std::future<void>> workers;
        for (const auto& bucket: toothPairs)
            workers.emplace_back(std::async(std::launch::async, handler, bucket));

        std::for_each(workers.cbegin(), workers.cend(), [](const auto& task) {
            task.wait();
        });

        return contactPoints;
    }

    //-----------------------------------------------------------------------------------------------//
    //                                                                                               //
    //-----------------------------------------------------------------------------------------------//

    void Visualize_Tooth_OBJ_Data() {
        using namespace Model;

        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/2878_teeth.obj)" };
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap = Tests::ReadTeethObj(filePath);

        Jaw lower(toothMap, TeethType::Lower);
        Jaw upper(toothMap, TeethType::Upper);

        for (auto &jaw: {lower, upper}){
            const vtkSmartPointer<vtkAppendPolyData> dataAppender {vtkSmartPointer<vtkAppendPolyData>::New()};
            for (const auto &[id, tooth]: jaw.teeth)
                dataAppender->AddInputData(tooth.toothPolyData);
            dataAppender->Update();
            Utilities::visualize(dataAppender->GetOutput());
        }
    }

    [[nodiscard("Make sure to handle return value")]]
    double getAngle(const Eigen::Vector3d& vect1,
                    const Eigen::Vector3d& vect2) noexcept {
        const double a { vect1.dot(vect2) };
        const double b { std::sqrt(vect1.dot(vect1)) * std::sqrt(vect2.dot(vect2)) };
        return (acos(a / b) * 180) / std::numbers::pi;
    }

    void Visualize_Tooth_and_Axis_GOOD()
    {
        constexpr std::string_view planJsonFile {
                R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        // for (unsigned short toothId: FDI::teethIds ) {
        for (unsigned short toothId: {33, 47}) {
            if (18 == toothId || 28 == toothId || 38 == toothId || 48 == toothId )
                continue;

            Model::Tooth tooth(toothMap[toothId], toothId);
            Model::ToothAxis& axes = tooth.setAxis(teethAxes[toothId]);

            const short signH = tooth.axes.horizontal.x() >= 0 ? 1 : -1;
            const short signV = tooth.axes.vertical.y() >= 0 ? 1 : -1;
            const Eigen::Vector3d verticalOrig {0, static_cast<double>(signV), 0};
            const Eigen::Vector3d horizontalOrig {static_cast<double>(signH), 0, 0};

            vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);

            {
                const Eigen::Vector3d forRotationAngle {axes.horizontal.x(), 0, axes.horizontal.z()};
                double rotationAngle = getAngle(horizontalOrig, forRotationAngle);
                rotationAngle *= (forRotationAngle.z() > 0 ? 1 : -1) * signH;

                {
                    vtkSmartPointer<vtkTransform> transform {vtkSmartPointer<vtkTransform>::New()};
                    transform->RotateY(rotationAngle);
                    vtkSmartPointer<vtkMatrix3x3> mat = Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
                    Eigen::Matrix3d rotationMatrix{mat->GetData()};

                    axes.horizontal = rotationMatrix.transpose() * axes.horizontal;
                    axes.vertical = rotationMatrix.transpose() * axes.vertical;
                    axes.angulation = rotationMatrix.transpose() * axes.angulation;
                }
                toothData = Utilities::rotatePolyData(toothData, 0, rotationAngle, 0);
            }

            {
                Eigen::Vector3d forTorkAngle {0, axes.vertical.y(), axes.vertical.z()};
                double xRotationAngle = getAngle(verticalOrig, forTorkAngle);
                xRotationAngle *= -signV * (tooth.axes.vertical.z() >= 0 ? 1 : -1);

                {
                    vtkSmartPointer<vtkTransform> transform{vtkSmartPointer<vtkTransform> ::New()};
                    transform->RotateX(xRotationAngle);
                    vtkSmartPointer<vtkMatrix3x3> mat = Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
                    Eigen::Matrix3d rotationMatrix{mat->GetData()};

                    auto T = rotationMatrix.transpose();
                    axes.horizontal = T * axes.horizontal;
                    axes.vertical = T * axes.vertical;
                    axes.angulation = T * axes.angulation;
                }
                toothData = Utilities::rotatePolyData(toothData, xRotationAngle, 0, 0);
            }

            {
                double zRotationAngle = getAngle(horizontalOrig, axes.horizontal);
                if (FDI::isUpperTooth(toothId) && axes.horizontal.y() > 0) {
                    zRotationAngle *= -1;
                }
                if (FDI::isLowerTooth(toothId) && axes.horizontal.y() < 0) {
                    zRotationAngle *= -1;
                }

                {
                    vtkSmartPointer<vtkTransform> transform{vtkSmartPointer<vtkTransform> ::New()};
                    transform->RotateZ(zRotationAngle);
                    vtkSmartPointer<vtkMatrix3x3> mat = Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
                    Eigen::Matrix3d rotationMatrix{mat->GetData()};

                    auto T = rotationMatrix.transpose();
                    axes.horizontal = T * axes.horizontal;
                    axes.vertical = T * axes.vertical;
                    axes.angulation = T * axes.angulation;
                }
                toothData = Utilities::rotatePolyData(toothData, 0, 0, zRotationAngle);
            }

            std::array<double, 6> bounds {};

            toothData->GetBounds(bounds.data());
            std::cout << tooth.getToothId() << " = " << bounds[1] - bounds[0] << std::endl;

            vtkSmartPointer<vtkActor> verticalAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.vertical * 7, {1, 0, 0});
            vtkSmartPointer<vtkActor> frontAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.angulation * 7, {1, 0, 0});
            vtkSmartPointer<vtkActor> horizontalAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.horizontal * 7, {1, 0, 0});
            horizontalAxeActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Red").GetData());

            vtkSmartPointer<vtkActor> horizontalOrigActor =
                    Utilities::getLineActor({0, 0, 0}, horizontalOrig * 7, {1, 0, 0});
            horizontalOrigActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());


            const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothData);
            const vtkSmartPointer<vtkActor> outlineActor = Utilities::getOutlineActor(toothData);

            Utilities::DisplayActors({toothActor, outlineActor,
                                      verticalAxeActor, horizontalAxeActor, frontAxeActor
                                     });
        }
    }

    // TODO: Check for performance
    void Visualize_Tooth_and_Axis_EigenQuaterniond()
    {
        constexpr std::string_view planJsonFile {
                R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        for (unsigned short toothId: FDI::teethIds ) {
        // for (unsigned short toothId: { 11}) {
            if (18 == toothId || 28 == toothId || 38 == toothId || 48 == toothId )
                continue;

            Model::Tooth tooth(toothMap[toothId], toothId);
            Model::ToothAxis& axes = tooth.setAxis(teethAxes[toothId]);

            const short signH = tooth.axes.horizontal.x() >= 0 ? 1 : -1;
            const short signV = tooth.axes.vertical.y() >= 0 ? 1 : -1;
            const Eigen::Vector3d verticalOrig {0, static_cast<double>(signV), 0};
            const Eigen::Vector3d horizontalOrig {static_cast<double>(signH), 0, 0};

            vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);

            {
                Eigen::Matrix3d matrix {Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

                axes.horizontal = matrix * axes.horizontal;
                axes.vertical = matrix * axes.vertical;
                axes.angulation = matrix * axes.angulation;

                vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                toothData = Utilities::rotatePolyData(toothData, vtkMatrix);

                matrix = Eigen::Quaterniond::FromTwoVectors(axes.vertical, verticalOrig).toRotationMatrix();

                axes.horizontal = matrix * axes.horizontal;
                axes.vertical = matrix * axes.vertical;
                axes.angulation = matrix * axes.angulation;

                vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                toothData = Utilities::rotatePolyData(toothData, vtkMatrix);
            }


            vtkSmartPointer<vtkActor> verticalAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.vertical * 7, {1, 0, 0});
            vtkSmartPointer<vtkActor> frontAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.angulation * 7, {1, 0, 0});
            vtkSmartPointer<vtkActor> horizontalAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.horizontal * 7, {1, 0, 0});
            horizontalAxeActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Red").GetData());

            vtkSmartPointer<vtkActor> horizontalOrigActor =
                    Utilities::getLineActor({0, 0, 0}, horizontalOrig * 7, {1, 0, 0});
            horizontalOrigActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());


            const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothData);
            const vtkSmartPointer<vtkActor> outlineActor = Utilities::getOutlineActor(toothData);

            Utilities::DisplayActors({toothActor, outlineActor,
                                      verticalAxeActor, horizontalAxeActor, frontAxeActor
                                     });
        }
    }

    // TODO: Check for performance
    void Visualize_Tooth_and_Axis_EigenQuaterniond_Estimator()
    {
        constexpr std::string_view planJsonFile {
                R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (unsigned short toothId: FDI::teethIds ) {
            if (18 == toothId || 28 == toothId || 38 == toothId || 48 == toothId )
                continue;

            Model::Tooth tooth(toothMap[toothId], toothId);
            tooth.setAxis(teethAxes[toothId]);
            estimator->estimate(tooth);
        }
    }


    // TODO: Check for performance
    void Visualize_Tooth_and_Axis_EigenQuaterniond_Debug()
    {
        constexpr std::string_view planJsonFile {
                R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        for (unsigned short toothId: FDI::teethIds ) {
            // for (unsigned short toothId: { 11}) {
            if (18 == toothId || 28 == toothId || 38 == toothId || 48 == toothId )
                continue;

            Model::Tooth tooth(toothMap[toothId], toothId);
            Model::ToothAxis& axes = tooth.setAxis(teethAxes[toothId]);

            { // ---------------------------- FOR debug ---------------------------------------------
                std::cout << toothId << std::endl;
                const Eigen::Array<double, 9, 1> axesVector (teethAxes[toothId].data());

                vtkSmartPointer<vtkTransform> transform{vtkTransform::New()};
                transform->RotateX(90);
                transform->RotateY(180);

                auto X = Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
                const Eigen::Matrix3d rotationMatrix {X->GetData()};
                const auto matrix = rotationMatrix.transpose() ;

                Eigen::Vector3d A1 = matrix * static_cast<Eigen::VectorXd>(axesVector.segment(0, 3));
                Eigen::Vector3d A2 = matrix * static_cast<Eigen::VectorXd>(axesVector.segment(3, 3));
                Eigen::Vector3d A3 = matrix * static_cast<Eigen::VectorXd>(axesVector.segment(6, 3));

                std::cout << A1 << std::endl;
                std::cout << A2 << std::endl;
                std::cout << A3 << std::endl;

                std::cout << axes.horizontal << std::endl;
                std::cout << axes.angulation << std::endl;
                std::cout << axes.vertical << std::endl;

            } //-------------------------------------------------------------------------------------------

            const short signH = tooth.axes.horizontal.x() >= 0 ? 1 : -1;
            const short signV = tooth.axes.vertical.y() >= 0 ? 1 : -1;
            const Eigen::Vector3d verticalOrig {0, static_cast<double>(signV), 0};
            const Eigen::Vector3d horizontalOrig {static_cast<double>(signH), 0, 0};

            vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);

            {
                Eigen::Matrix3d matrix {Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

                axes.horizontal = matrix * axes.horizontal;
                axes.vertical = matrix * axes.vertical;
                axes.angulation = matrix * axes.angulation;

                vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                toothData = Utilities::rotatePolyData(toothData, vtkMatrix);

                matrix = Eigen::Quaterniond::FromTwoVectors(axes.vertical, verticalOrig).toRotationMatrix();

                axes.horizontal = matrix * axes.horizontal;
                axes.vertical = matrix * axes.vertical;
                axes.angulation = matrix * axes.angulation;

                vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                toothData = Utilities::rotatePolyData(toothData, vtkMatrix);
            }


            vtkSmartPointer<vtkActor> verticalAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.vertical * 7, {1, 0, 0});
            vtkSmartPointer<vtkActor> frontAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.angulation * 7, {1, 0, 0});
            vtkSmartPointer<vtkActor> horizontalAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.horizontal * 7, {1, 0, 0});
            horizontalAxeActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Red").GetData());

            vtkSmartPointer<vtkActor> horizontalOrigActor =
                    Utilities::getLineActor({0, 0, 0}, horizontalOrig * 7, {1, 0, 0});
            horizontalOrigActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());


            const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothData);
            const vtkSmartPointer<vtkActor> outlineActor = Utilities::getOutlineActor(toothData);

            /*
            Utilities::DisplayActors({toothActor, outlineActor,
                                      verticalAxeActor, horizontalAxeActor, frontAxeActor
                                     }, colors);
            */
        }
    }



    // TODO: Check for Memory Leak!!! We have here a lot !!!!
    void Visualize_Tooth_and_Axis_PERFORMANCE_SINGLE_THREAD()
    {
        constexpr std::string_view planJsonFile {
                R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        //------------------------------------------------------------------------------------------------------

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 2'000; ++i) {
                for (unsigned short toothId: FDI::teethIds) {
                    if (18 == toothId || 28 == toothId || 38 == toothId || 48 == toothId)
                        continue;

                    Model::Tooth tooth(toothMap[toothId], toothId);
                    Model::ToothAxis &axes = tooth.setAxis(teethAxes[toothId]);

                    const short signH = tooth.axes.horizontal.x() >= 0 ? 1 : -1;
                    const short signV = tooth.axes.vertical.y() >= 0 ? 1 : -1;
                    const Eigen::Vector3d verticalOrig{0, static_cast<double>(signV), 0};
                    const Eigen::Vector3d horizontalOrig{static_cast<double>(signH), 0, 0};

                    vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);
                    {
                        Eigen::Matrix3d matrix{Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

                        axes.horizontal = matrix * axes.horizontal;
                        axes.vertical = matrix * axes.vertical;
                        axes.angulation = matrix * axes.angulation;

                        vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                        toothData = Utilities::rotatePolyData(toothData, vtkMatrix);

                        matrix = Eigen::Quaterniond::FromTwoVectors(axes.vertical, verticalOrig).toRotationMatrix();

                        axes.horizontal = matrix * axes.horizontal;
                        axes.vertical = matrix * axes.vertical;
                        axes.angulation = matrix * axes.angulation;

                        vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                        toothData = Utilities::rotatePolyData(toothData, vtkMatrix);
                    }
                }
            }
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 2'000; ++i) {
                for (unsigned short toothId: FDI::teethIds) {
                    if (18 == toothId || 28 == toothId || 38 == toothId || 48 == toothId)
                        continue;

                    Model::Tooth tooth(toothMap[toothId], toothId);
                    Model::ToothAxis &axes = tooth.setAxis(teethAxes[toothId]);

                    const short signH = tooth.axes.horizontal.x() >= 0 ? 1 : -1;
                    const short signV = tooth.axes.vertical.y() >= 0 ? 1 : -1;
                    const Eigen::Vector3d verticalOrig{0, static_cast<double>(signV), 0};
                    const Eigen::Vector3d horizontalOrig{static_cast<double>(signH), 0, 0};

                    vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);

                    {
                        const Eigen::Vector3d forRotationAngle{axes.horizontal.x(), 0, axes.horizontal.z()};
                        double rotationAngle = getAngle(horizontalOrig, forRotationAngle);
                        rotationAngle *= (forRotationAngle.z() > 0 ? 1 : -1) * signH;

                        {
                            vtkSmartPointer<vtkTransform> transform{vtkSmartPointer<vtkTransform>::New()};
                            transform->RotateY(rotationAngle);
                            vtkSmartPointer<vtkMatrix3x3> mat = Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
                            Eigen::Matrix3d rotationMatrix{mat->GetData()};

                            axes.horizontal = rotationMatrix.transpose() * axes.horizontal;
                            axes.vertical = rotationMatrix.transpose() * axes.vertical;
                            axes.angulation = rotationMatrix.transpose() * axes.angulation;
                        }
                        toothData = Utilities::rotatePolyData(toothData, 0, rotationAngle, 0);
                    }

                    {
                        Eigen::Vector3d forTorkAngle{0, axes.vertical.y(), axes.vertical.z()};
                        double xRotationAngle = getAngle(verticalOrig, forTorkAngle);
                        xRotationAngle *= -signV * (tooth.axes.vertical.z() >= 0 ? 1 : -1);

                        {
                            vtkSmartPointer<vtkTransform> transform{vtkSmartPointer<vtkTransform>::New()};
                            transform->RotateX(xRotationAngle);
                            vtkSmartPointer<vtkMatrix3x3> mat = Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
                            Eigen::Matrix3d rotationMatrix{mat->GetData()};

                            auto T = rotationMatrix.transpose();
                            axes.horizontal = T * axes.horizontal;
                            axes.vertical = T * axes.vertical;
                            axes.angulation = T * axes.angulation;
                        }
                        toothData = Utilities::rotatePolyData(toothData, xRotationAngle, 0, 0);
                    }

                    {
                        double zRotationAngle = getAngle(horizontalOrig, axes.horizontal);
                        if (FDI::isUpperTooth(toothId) && axes.horizontal.y() > 0) {
                            zRotationAngle *= -1;
                        }
                        if (FDI::isLowerTooth(toothId) && axes.horizontal.y() < 0) {
                            zRotationAngle *= -1;
                        }

                        {
                            vtkSmartPointer<vtkTransform> transform{vtkSmartPointer<vtkTransform>::New()};
                            transform->RotateZ(zRotationAngle);
                            vtkSmartPointer<vtkMatrix3x3> mat = Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
                            Eigen::Matrix3d rotationMatrix{mat->GetData()};

                            auto T = rotationMatrix.transpose();
                            axes.horizontal = T * axes.horizontal;
                            axes.vertical = T * axes.vertical;
                            axes.angulation = T * axes.angulation;
                        }
                        toothData = Utilities::rotatePolyData(toothData, 0, 0, zRotationAngle);
                    }
                }
            }
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }
    }

    void Visualize_Tooth_and_Axis_MEM_LEAK()
    {
        constexpr std::string_view planJsonFile {
                R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        //------------------------------------------------------------------------------------------------------

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 200'000; ++i) {
                for (unsigned short toothId: FDI::teethIds) {
                    if (18 == toothId || 28 == toothId || 38 == toothId || 48 == toothId)
                        continue;

                    Model::Tooth tooth(toothMap[toothId], toothId);
                    Model::ToothAxis &axes = tooth.setAxis(teethAxes[toothId]);

                    const short signH = tooth.axes.horizontal.x() >= 0 ? 1 : -1;
                    const short signV = tooth.axes.vertical.y() >= 0 ? 1 : -1;
                    const Eigen::Vector3d verticalOrig{0, static_cast<double>(signV), 0};
                    const Eigen::Vector3d horizontalOrig{static_cast<double>(signH), 0, 0};

                    vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);
                    {
                        Eigen::Matrix3d matrix{Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

                        axes.horizontal = matrix * axes.horizontal;
                        axes.vertical = matrix * axes.vertical;
                        axes.angulation = matrix * axes.angulation;

                        vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                        toothData = Utilities::rotatePolyData(toothData, vtkMatrix);

                        matrix = Eigen::Quaterniond::FromTwoVectors(axes.vertical, verticalOrig).toRotationMatrix();

                        axes.horizontal = matrix * axes.horizontal;
                        axes.vertical = matrix * axes.vertical;
                        axes.angulation = matrix * axes.angulation;

                        vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                        toothData = Utilities::rotatePolyData(toothData, vtkMatrix);
                    }
                }
            }
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds.\n";
        }
    }

    void Visualize_Tooth_and_Axis()
    {
        constexpr std::string_view planJsonFile {
                R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        for (unsigned short toothId: FDI::teethIds ) {
            // for (unsigned short toothId: { 11,12,13,14,15,16,17}) {
            if (18 == toothId || 28 == toothId || 38 == toothId || 48 == toothId )
                continue;

            std::cout << "toothId = " << toothId << std::endl;

            Model::Tooth tooth(toothMap[toothId], toothId);
            Model::ToothAxis& axes = tooth.setAxis(teethAxes[toothId]);

            const short signH = tooth.axes.horizontal.x() >= 0 ? 1 : -1;
            const short signV = tooth.axes.vertical.y() >= 0 ? 1 : -1;
            const Eigen::Vector3d verticalOrig {0, static_cast<double>(signV), 0};
            const Eigen::Vector3d horizontalOrig {static_cast<double>(signH), 0, 0};

            vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);

            {
                const Eigen::Vector3d forRotationAngle {axes.horizontal.x(), 0, axes.horizontal.z()};
                double rotationAngle = getAngle(horizontalOrig, forRotationAngle);
                rotationAngle *= (forRotationAngle.z() > 0 ? 1 : -1) * signH;

                {
                    vtkSmartPointer<vtkTransform> transform {vtkSmartPointer<vtkTransform>::New()};
                    transform->RotateY(rotationAngle);
                    vtkSmartPointer<vtkMatrix3x3> mat = Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
                    Eigen::Matrix3d rotationMatrix{mat->GetData()};

                    axes.horizontal = rotationMatrix.transpose() * axes.horizontal;
                    axes.vertical = rotationMatrix.transpose() * axes.vertical;
                    axes.angulation = rotationMatrix.transpose() * axes.angulation;
                }
                toothData = Utilities::rotatePolyData(toothData, 0, rotationAngle, 0);
            }

            {
                Eigen::Vector3d forTorkAngle {0, axes.vertical.y(), axes.vertical.z()};
                double xRotationAngle = getAngle(verticalOrig, forTorkAngle);
                xRotationAngle *= -signV * (tooth.axes.vertical.z() >= 0 ? 1 : -1);

                {
                    vtkSmartPointer<vtkTransform> transform{vtkSmartPointer<vtkTransform>::New()};
                    transform->RotateX(xRotationAngle);
                    vtkSmartPointer<vtkMatrix3x3> mat = Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
                    Eigen::Matrix3d rotationMatrix{mat->GetData()};

                    auto T = rotationMatrix.transpose();
                    axes.horizontal = T * axes.horizontal;
                    axes.vertical = T * axes.vertical;
                    axes.angulation = T * axes.angulation;
                }
                toothData = Utilities::rotatePolyData(toothData, xRotationAngle, 0, 0);
            }

            {
                double zRotationAngle = getAngle(horizontalOrig, axes.horizontal);
                if (FDI::isUpperTooth(toothId) && axes.horizontal.y() > 0)
                    zRotationAngle *= -1;
                if (FDI::isLowerTooth(toothId) && axes.horizontal.y() < 0) {
                    zRotationAngle *= -1;
                }

                {
                    vtkSmartPointer<vtkTransform> transform{vtkSmartPointer<vtkTransform>::New()};
                    transform->RotateZ(zRotationAngle);
                    vtkSmartPointer<vtkMatrix3x3> mat = Utilities::cast4x4MatrixTo3x3(transform->GetMatrix());
                    Eigen::Matrix3d rotationMatrix{mat->GetData()};

                    auto T = rotationMatrix.transpose();
                    axes.horizontal = T * axes.horizontal;
                    axes.vertical = T * axes.vertical;
                    axes.angulation = T * axes.angulation;
                }
                toothData = Utilities::rotatePolyData(toothData, 0, 0, zRotationAngle);

                vtkSmartPointer<vtkActor> verticalAxeActor =
                        Utilities::getLineActor({0, 0, 0}, axes.vertical * 7, {1, 0, 0});
                vtkSmartPointer<vtkActor> frontAxeActor =
                        Utilities::getLineActor({0, 0, 0}, axes.angulation * 7, {1, 0, 0});
                vtkSmartPointer<vtkActor> horizontalAxeActor =
                        Utilities::getLineActor({0, 0, 0}, axes.horizontal * 7, {1, 0, 0});
                horizontalAxeActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Red").GetData());

                vtkSmartPointer<vtkActor> horizontalOrigActor =
                        Utilities::getLineActor({0, 0, 0}, horizontalOrig * 7, {1, 0, 0});
                horizontalOrigActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());


                const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothData);
                const vtkSmartPointer<vtkActor> outlineActor = Utilities::getOutlineActor(toothData);

                Utilities::DisplayActors({toothActor, outlineActor,
                                          verticalAxeActor, horizontalAxeActor, frontAxeActor
                                         });
            }
        }
    }

    void Display_Tooth_Axis()
    {
        constexpr std::string_view planJsonFile {
                R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        for (auto toothId: {11,12,13,14,15,16,17,21,22,23,24,25,26,37,31,32,33,34,35,36,37,41,42,43,44,45,46,47})
        {
            Model::Tooth tooth(toothMap[toothId], toothId);
            Model::ToothAxis& axes = tooth.setAxis(teethAxes[toothId]);

            const vtkSmartPointer<vtkActor> lineActor1 =
                    Utilities::getLineActor(axes.vertical * -7, axes.vertical * 7, {1, 0, 0});
            const vtkSmartPointer<vtkActor> lineActor2 =
                    Utilities::getLineActor(axes.angulation * -7, axes.angulation * 7, {1, 0, 0});
            const vtkSmartPointer<vtkActor> lineActor3 =
                    Utilities::getLineActor(axes.horizontal * -7, axes.horizontal * 7, {1, 0, 0});


            vtkSmartPointer<vtkPolyData> toothData = Utilities::setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);
            vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothData);
            Utilities::DisplayActors({toothActor,lineActor1, lineActor2, lineActor3});
        }
    }
}

namespace Setup {

    const vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors>::New() };

    void SetupTeethAndVisualize()
    {
        constexpr std::string_view planJsonFile {
                R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw lower { Model::TeethType::Lower };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isLowerTooth(toothId)) {
                // Model::Tooth tooth(data, toothId);
                if (auto [iter, ok] = lower.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (const auto& [id, tooth]: lower.teeth) {
            std::cout << id << std::endl;
            actors.push_back(Utilities::getPolyDataActor(tooth.toothPolyData));
        }
        Utilities::DisplayActors(actors);
    }
}

namespace Experiments::EstimateCurve {

#if 0
    void polyfit(const std::vector<double> &x,
                 const std::vector<double> &y,
                 std::vector<double> &coeff,
                 const size_t order)
    {
        // Create Matrix Placeholder of size n x k, n= number of data points,
        // k = order of polynomial, for example k = 3 for cubic polynomial
        Eigen::MatrixXd T(x.size(), order + 1);
        Eigen::VectorXd V = Eigen::VectorXd::Map(&y.front(), y.size());

        // Populate the matrix
        for(size_t i = 0; i < x.size(); ++i) {
            for(size_t j = 0; j < order + 1; ++j) {
                T(i, j) = pow(x[i], j);
            }
        }

        // Solve for linear least square fit
        const Eigen::VectorXd result  = T.householderQr().solve(V);
        coeff.resize(order + 1);
        for (size_t k = 0; k < order + 1; k++)
            coeff[k] = result[k];
    }
#endif

    //-------------------------------------------------------------------------------------------------

    void CalculateContactPointTest()
    {
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<UnorderedPair<int>, Eigen::Vector3d, PairHashUnordered<int>> contactPoints;
        constexpr std::array<int, 14> teethIds {47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};

        // NOTE: Performance measurements
        const auto startTime = std::chrono::high_resolution_clock::now();

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (size_t i = 1; i< teethIds.size(); ++i) {
            const int id1 = teethIds[i - 1], id2 = teethIds[i];
            const vtkSmartPointer<vtkPolyData> tooth1 = toothMap.at(id1), tooth2 = toothMap.at(id2);
            const Eigen::Vector3d ptContact = getContactPoint(tooth1, tooth2);
            contactPoints.emplace(UnorderedPair<int>{id1, id2}, ptContact);
        }
        // NOTE: Performance measurements
        const auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "Get contact points: "
                  << duration_cast<std::chrono::duration<double>>(endTime - startTime).count() << " seconds\n";


        actors.push_back(Utilities::getPolyDataActor(toothMap.at(teethIds[0])));
        for (size_t i = 1; i< teethIds.size(); ++i) {
            const int id1 = teethIds[i - 1], id2 = teethIds[i];
            const vtkSmartPointer<vtkPolyData> tooth = toothMap.at(id2);
            const Eigen::Vector3d pt = contactPoints.at(UnorderedPair<int>{id1, id2});

            actors.push_back(Utilities::getPointsActor({pt}, {1, 0, 0}));
            actors.push_back(Utilities::getPolyDataActor(tooth));
        }
        Utilities::DisplayActors(actors);
    }

    void CalculateContactPointTest_Parallel()
    {
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        constexpr std::array<int, 14> teethIdsLower {47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};
        constexpr std::array<int, 14> teethIdsUpper {17, 16, 15, 14, 13, 12, 11, 21, 22, 23, 24, 25, 26, 27};

        // NOTE: Performance measurements
        const auto startTime = std::chrono::high_resolution_clock::now();
        const std::unordered_map<UnorderedPair<int>, Eigen::Vector3d, PairHashUnordered<int>> contactPoints =
                CalcTeethContactPoints(toothMap);

        // NOTE: Performance measurements
        const auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "Get contact points: "
                  << duration_cast<std::chrono::duration<double>>(endTime - startTime).count() << " seconds\n";

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (const auto& ids: {teethIdsLower, teethIdsUpper}) {
            actors.push_back(Utilities::getPolyDataActor(toothMap.at(ids[0])));
            for (size_t i = 1; i < ids.size(); ++i) {
                const int id1 = ids[i - 1], id2 = ids[i];
                const vtkSmartPointer<vtkPolyData> tooth = toothMap.at(id2);
                const Eigen::Vector3d pt = contactPoints.at(UnorderedPair<int>{id1, id2});

                actors.push_back(Utilities::getPointsActor({pt}, {1, 0, 0}));
                actors.push_back(Utilities::getPolyDataActor(tooth));
            }
        }

        Utilities::DisplayActors(actors);
    }

    void CalculateAndVisualize()
    {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw upper { Model::TeethType::Upper };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isUpperTooth(toothId)) {
                // Model::Tooth tooth(data, toothId);
                if (auto [iter, ok] = upper.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        const double incisorsLength = calcIncisorsLength(upper);
        const double premolarDistanceExpected { incisorsLength * 100 / 85 };
        const double molarDistanceExpected { incisorsLength * 100 / 65 };

        Eigen::Vector3d tooth14Center {}, tooth24Center {}, tooth16Center {}, tooth26Center {};
        {
            upper.teeth.at(14).toothPolyData->GetCenter(tooth14Center.data());
            upper.teeth.at(24).toothPolyData->GetCenter(tooth24Center.data());
            upper.teeth.at(16).toothPolyData->GetCenter(tooth16Center.data());
            upper.teeth.at(26).toothPolyData->GetCenter(tooth26Center.data());
        }

        const double premolarDistanceActual_Upper = twoPointDistance(tooth14Center, tooth24Center);
        const double molarDistanceActual_Upper = twoPointDistance(tooth16Center, tooth26Center);

        double t = 0;
        const Eigen::Vector3d ptContact = getContactPoint(upper.teeth.at(11).toothPolyData,
                                                          upper.teeth.at(21).toothPolyData);
        Eigen::Vector3d closest {};
        const double distSquared = vtkLine::DistanceToLine(ptContact.data(),
                tooth14Center.data(), tooth24Center.data(), t, closest.data());

        const double distance = twoPointDistance(ptContact, closest);
        const double corkhouseDist = GetCorkhouseDist(incisorsLength);

        {
            std::cout << "Incisors length: " << incisorsLength << std::endl;
            std::cout << "Premolar distance (expected): " << premolarDistanceExpected << std::endl;
            std::cout << "Molar distance (expected): " << molarDistanceExpected << std::endl;
            std::cout << "Premolar distance (actual): " << premolarDistanceActual_Upper << std::endl;
            std::cout << "Molar distance (actual): " << molarDistanceActual_Upper << std::endl << std::endl;
            std::cout << "Distance (actual): " << distance << std::endl;
            std::cout << "Distance (actual): " << std::sqrt(distSquared) << std::endl;
            std::cout << "Distance (expected): " << corkhouseDist << std::endl;
        }

        std::vector<vtkSmartPointer<vtkActor>> actors;
        {
            for (const auto&[id, tooth]: upper.teeth) {
                actors.push_back(Utilities::getPolyDataActor(tooth.toothPolyData));
            }
            actors.push_back(Utilities::getLineActor(tooth14Center, tooth24Center, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({tooth14Center, tooth24Center}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(tooth16Center, tooth26Center, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({tooth16Center, tooth26Center}, {1, 0, 0}));

            actors.push_back(Utilities::getPointsActor({ptContact, closest}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(ptContact, closest, {1, 0, 0}));
        }
        Utilities::DisplayActors(actors);
    }

    void EstimateCurveTest()
    {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw upper { Model::TeethType::Upper };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isUpperTooth(toothId)) {
                if (auto [iter, ok] = upper.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        const double incisorsLength = calcIncisorsLength(upper);
        const double premolarDistanceExpected { incisorsLength * 100 / 85 };
        const double molarDistanceExpected { incisorsLength * 100 / 65 };

        Eigen::Vector3d tooth14Center {}, tooth24Center {}, tooth16Center {}, tooth26Center {};
        {
            upper.teeth.at(14).toothPolyData->GetCenter(tooth14Center.data());
            upper.teeth.at(24).toothPolyData->GetCenter(tooth24Center.data());
            upper.teeth.at(16).toothPolyData->GetCenter(tooth16Center.data());
            upper.teeth.at(26).toothPolyData->GetCenter(tooth26Center.data());
        }

        const double premolarDistanceActual_Upper = twoPointDistance(tooth14Center, tooth24Center);
        const double molarDistanceActual_Upper = twoPointDistance(tooth16Center, tooth26Center);

        auto tooth11Dummy = Utilities::moveTooth(upper.teeth.at(11).toothPolyData, 0, 0, 2);
        auto tooth21Dummy = Utilities::moveTooth(upper.teeth.at(21).toothPolyData, 0, 0, 2);


        double t = 0;
        /*
        const Eigen::Vector3d ptContact = getContactPoint(upper.teeth.at(11).toothPolyData,
                                                          upper.teeth.at(21).toothPolyData);
        */
        const Eigen::Vector3d ptContact = getContactPoint(tooth11Dummy, tooth21Dummy);

        Eigen::Vector3d closest {};
        const double distSquared = vtkLine::DistanceToLine(ptContact.data(),
                                                           tooth14Center.data(), tooth24Center.data(), t, closest.data());

        const double distance = twoPointDistance(ptContact, closest);
        const double corkhouseDist = GetCorkhouseDist(incisorsLength);

        {
            std::cout << "Incisors length: " << incisorsLength << std::endl;
            std::cout << "Premolar distance (expected): " << premolarDistanceExpected << std::endl;
            std::cout << "Molar distance (expected): " << molarDistanceExpected << std::endl;
            std::cout << "Premolar distance (actual): " << premolarDistanceActual_Upper << std::endl;
            std::cout << "Molar distance (actual): " << molarDistanceActual_Upper << std::endl << std::endl;
            std::cout << "Distance (actual): " << distance << std::endl;
            std::cout << "Distance (actual): " << std::sqrt(distSquared) << std::endl;
            std::cout << "Distance (expected): " << corkhouseDist << std::endl;
        }

        std::vector<vtkSmartPointer<vtkActor>> actors;
        {
            for (const auto&[id, tooth]: upper.teeth) {
                if (11 == id || 21 == id)
                    continue;
                actors.push_back(Utilities::getPolyDataActor(tooth.toothPolyData));
            }

            actors.push_back(Utilities::getPolyDataActor(tooth11Dummy));
            actors.push_back(Utilities::getPolyDataActor(tooth21Dummy));

            actors.push_back(Utilities::getLineActor(tooth14Center, tooth24Center, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({tooth14Center, tooth24Center}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(tooth16Center, tooth26Center, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({tooth16Center, tooth26Center}, {1, 0, 0}));

            actors.push_back(Utilities::getPointsActor({ptContact, closest}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(ptContact, closest, {1, 0, 0}));
        }
        Utilities::DisplayActors(actors);
    }

    void VisualizeTooths_LineSections()
    {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw upper { Model::TeethType::Upper };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isUpperTooth(toothId)) {
                if (auto [iter, ok] = upper.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (const auto&[id, tooth]: upper.teeth)
        {
            vtkSmartPointer<vtkPolyData> toothData = tooth.toothPolyData;

            Eigen::Vector3d center {};
            toothData->GetCenter(center.data());

            const Model::ToothAxis axes = tooth.getAxis();
            Eigen::Vector3d start = center - axes.horizontal;
            Eigen::Vector3d end = center + axes.horizontal;

            const auto [slopeY, interceptY] = get_line_coefficients({start.x(), start.y()}, {end.x(), end.y()});
            const auto [slopeZ, interceptZ] = get_line_coefficients({start.x(), start.z()}, {end.x(), end.z()});

            // the ratio of the current length of the segment to the length of the tooth
            const double X = tooth.mesiodistalWidth / twoPointDistance(start, end);;

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

    void VisualizeTooths_LineSections_ContactPoints()
    {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw upper { Model::TeethType::Upper };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isUpperTooth(toothId)) {
                if (auto [iter, ok] = upper.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        std::unordered_map<UnorderedPair<int>, Eigen::Vector3d, PairHashUnordered<int>> contactPoints;
        std::array<int, 14> teethIds {47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};
        for (size_t i = 1; i< teethIds.size(); ++i) {
            const int id1 = teethIds[i - 1], id2 = teethIds[i];
            const vtkSmartPointer<vtkPolyData> tooth1 = toothMap.at(id1);
            const vtkSmartPointer<vtkPolyData> tooth2 = toothMap.at(id2);

            Eigen::Vector3d ptContact = getContactPoint(tooth1, tooth2);
            contactPoints.emplace(UnorderedPair<int>{id1, id2}, ptContact);
        }

        std::vector<vtkSmartPointer<vtkActor>> actors {
                Utilities::getPolyDataActor(toothMap.at(teethIds[0])),
                Utilities::getPolyDataActor(toothMap.at(teethIds[1]))
        };
        for (size_t i = 2; i< teethIds.size(); ++i) {
            const int id1 = teethIds[i - 2], id2 = teethIds[i - 1], id3 = teethIds[i];
            Eigen::Vector3d pt1 = contactPoints.at(UnorderedPair<int>{id1, id2});
            Eigen::Vector3d pt2 = contactPoints.at(UnorderedPair<int>{id2, id3});

            actors.push_back(Utilities::getPolyDataActor(toothMap.at(id3)));
            actors.push_back(Utilities::getLineActor(pt1, pt2, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({pt1, pt2}, {1, 0, 0}));
        }
        Utilities::DisplayActors(actors);
    }

    void VisualizeTooths_LineSections_And_ContactPoints()
    {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw upper { Model::TeethType::Upper };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isUpperTooth(toothId)) {
                if (auto [iter, ok] = upper.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        std::unordered_map<UnorderedPair<int>, Eigen::Vector3d, PairHashUnordered<int>> contactPoints;
        std::array<int, 14> teethIds {17, 16, 15, 14, 13, 12, 11, 21, 22, 23, 24, 25, 26, 27};
        for (size_t i = 1; i< teethIds.size(); ++i) {
            const int id1 = teethIds[i - 1], id2 = teethIds[i];
            const vtkSmartPointer<vtkPolyData> tooth1 = toothMap.at(id1);
            const vtkSmartPointer<vtkPolyData> tooth2 = toothMap.at(id2);

            Eigen::Vector3d ptContact = getContactPoint(tooth1, tooth2);
            contactPoints.emplace(UnorderedPair<int>{id1, id2}, ptContact);
        }

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (const auto&[id, tooth]: upper.teeth)
        {
            vtkSmartPointer<vtkPolyData> toothData = tooth.toothPolyData;

            Eigen::Vector3d center {};
            toothData->GetCenter(center.data());

            const Model::ToothAxis axes = tooth.getAxis();
            Eigen::Vector3d start = center - axes.horizontal;
            Eigen::Vector3d end = center + axes.horizontal;

            const auto [slopeY, interceptY] = get_line_coefficients({start.x(), start.y()}, {end.x(), end.y()});
            const auto [slopeZ, interceptZ] = get_line_coefficients({start.x(), start.z()}, {end.x(), end.z()});

            // the ratio of the current length of the segment to the length of the tooth
            const double X = tooth.mesiodistalWidth / twoPointDistance(start, end);;

            start.x() -= std::abs((start.x() - center.x())) * (X - 1);
            end.x() += std::abs((end.x() - center.x())) * (X - 1);
            start.y() = start.x() * slopeY + interceptY;
            end.y() = end.x() * slopeY + interceptY;
            start.z() = start.x() * slopeZ + interceptZ;
            end.z() = end.x() * slopeZ + interceptZ;

            actors.push_back(Utilities::getPolyDataActor(toothData));
            actors.push_back(Utilities::getLineActor(start, end, {1, 0, 1}));
            actors.push_back(Utilities::getPointsActor({start, end}, {1, 0, 1}));
        }

        // Add contact points lines
        for (size_t i = 2; i< teethIds.size(); ++i) {
            const int id1 = teethIds[i - 2], id2 = teethIds[i - 1], id3 = teethIds[i];
            Eigen::Vector3d pt1 = contactPoints.at(UnorderedPair<int>{id1, id2});
            Eigen::Vector3d pt2 = contactPoints.at(UnorderedPair<int>{id2, id3});

            actors.push_back(Utilities::getLineActor(pt1, pt2, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({pt1, pt2}, {1, 0, 0}));
        }
        Utilities::DisplayActors(actors);
    }

    /*
    void EstimateCurveTest_VisualizePoints_AndCurve_Parabola()
    {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw upper { Model::TeethType::Upper };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isUpperTooth(toothId)) {
                if (auto [iter, ok] = upper.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        std::unordered_map<UnorderedPair<int>, Eigen::Vector3d, PairHashUnordered<int>> contactPoints;
        std::array<int, 14> teethIds {17, 16, 15, 14, 13, 12, 11, 21, 22, 23, 24, 25, 26, 27};
        for (size_t i = 1; i< teethIds.size(); ++i) {
            const int id1 = teethIds[i - 1], id2 = teethIds[i];
            const vtkSmartPointer<vtkPolyData> tooth1 = toothMap.at(id1);
            const vtkSmartPointer<vtkPolyData> tooth2 = toothMap.at(id2);

            Eigen::Vector3d ptContact = getContactPoint(tooth1, tooth2);
            contactPoints.emplace(UnorderedPair<int>{id1, id2}, ptContact);
        }

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (const auto&[id, tooth]: upper.teeth)
        {
            vtkSmartPointer<vtkPolyData> toothData = tooth.toothPolyData;

            Eigen::Vector3d center {};
            toothData->GetCenter(center.data());

            const Model::ToothAxis axes = tooth.getAxis();
            Eigen::Vector3d start = center - axes.horizontal;
            Eigen::Vector3d end = center + axes.horizontal;

            const auto [slopeY, interceptY] = get_line_coefficients({start.x(), start.y()}, {end.x(), end.y()});
            const auto [slopeZ, interceptZ] = get_line_coefficients({start.x(), start.z()}, {end.x(), end.z()});

            // the ratio of the current length of the segment to the length of the tooth
            const double X = tooth.mesiodistalWidth / twoPointDistance(start, end);;

            start.x() -= std::abs((start.x() - center.x())) * (X - 1);
            end.x() += std::abs((end.x() - center.x())) * (X - 1);
            start.y() = start.x() * slopeY + interceptY;
            end.y() = end.x() * slopeY + interceptY;
            start.z() = start.x() * slopeZ + interceptZ;
            end.z() = end.x() * slopeZ + interceptZ;

            actors.push_back(Utilities::getPolyDataActor(toothData, colors));
            // actors.push_back(Utilities::getLineActor(start, end, {1, 0, 1}));
            // actors.push_back(Utilities::getPointsActor({start, end}, {1, 0, 1}));
        }



        const double incisorsLength = calcIncisorsLength(upper);
        const double premolarDistanceExpected { incisorsLength * 100 / 85 };
        const double molarDistanceExpected { incisorsLength * 100 / 65 };

        Eigen::Vector3d tooth14Center {}, tooth24Center {}, tooth16Center {}, tooth26Center {};
        {
            upper.teeth.at(14).toothPolyData->GetCenter(tooth14Center.data());
            upper.teeth.at(24).toothPolyData->GetCenter(tooth24Center.data());
            upper.teeth.at(16).toothPolyData->GetCenter(tooth16Center.data());
            upper.teeth.at(26).toothPolyData->GetCenter(tooth26Center.data());
        }

        const double premolarDistanceActual_Upper = twoPointDistance(tooth14Center, tooth24Center);
        const double molarDistanceActual_Upper = twoPointDistance(tooth16Center, tooth26Center);

        const Eigen::Vector3d ptContact = getContactPoint(upper.teeth.at(11).toothPolyData,
                                                          upper.teeth.at(21).toothPolyData);
        double t = 0;
        Eigen::Vector3d closest {};
        const double distSquared = vtkLine::DistanceToLine(ptContact.data(),
                            tooth14Center.data(), tooth24Center.data(), t, closest.data());

        const double distance = twoPointDistance(ptContact, closest);
        const double corkhouseDist = GetCorkhouseDist(incisorsLength);

        {
            std::cout << "Incisors length: " << incisorsLength << std::endl;
            std::cout << "Premolar distance: \n" <<  "   Actual: "
                      << premolarDistanceActual_Upper << ". Expected: " << premolarDistanceExpected << std::endl;
            std::cout << "Molar distance: \n" << "   Actual: "
                      << molarDistanceActual_Upper << ". Expected: " <<  molarDistanceExpected << std::endl;
            std::cout << "Korkhouse: \n" << "   Actual: "
                      << distance << ". Expected: " << corkhouseDist << std::endl;
        }

        {
            actors.push_back(Utilities::getLineActor(tooth14Center, tooth24Center, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({tooth14Center, tooth24Center}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(tooth16Center, tooth26Center, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({tooth16Center, tooth26Center}, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({ptContact, closest}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(ptContact, closest, {1, 0, 0}));
        }

        { // Calc points for 'Ideal Curve':
            const double premolarDiff = premolarDistanceExpected - premolarDistanceActual_Upper;
            const double molarDiff = molarDistanceExpected - molarDistanceActual_Upper;
            const double corkDiff = corkhouseDist - distance;

            Eigen::Vector3d pt14 {tooth14Center}, pt24 {tooth24Center},
                            pt16 {tooth16Center}, pt26 {tooth26Center}, center {ptContact};
            pt14.x() += (pt14.x() > 0 ? 1: -1) * premolarDiff / 2;
            pt24.x() += (pt24.x() > 0 ? 1: -1) * premolarDiff / 2;
            pt16.x() += (pt16.x() > 0 ? 1: -1) * molarDiff / 2;
            pt26.x() += (pt26.x() > 0 ? 1: -1) * molarDiff / 2;
            center.z() += (center.z() > 0 ? 1: -1) * corkDiff;

            std::cout << pt16 << std::endl;
            std::cout << pt14 << std::endl;
            std::cout << center << std::endl;
            std::cout << pt24 << std::endl;
            std::cout << pt26 << std::endl;

            actors.push_back(Utilities::getPointsActor({ pt14, pt24, pt16, pt26, center }, {1, 0, 1}));

            const std::vector<double> X { pt16.x(), pt14.x(),  center.x(), pt24.x(), pt26.x() };
            const std::vector<double> Z { pt16.z(), pt14.z(), center.z(), pt24.z(), pt26.z() };

            std::vector<Eigen::Vector3d> curvePoints;
            std::vector<double> coefficients;
            polyfit(X, Z, coefficients, 3);
            const double stepsCount = 1000;
            const double step = (X.back() - X.front()) / stepsCount;
            for(double i = X.front(); i < X.back(); i += step) {
                const double zPredicted = coefficients[0] + coefficients[1] * i +
                        coefficients[2] * (pow(i, 2)) + coefficients[3] * (pow(i, 3)) ;
                curvePoints.emplace_back(i, pt14.y(), zPredicted);
            }
            actors.push_back(Utilities::getPointsActor(curvePoints, {0, 0, 0}, 8));
        }

        // Add contact points lines
        for (size_t i = 2; i< teethIds.size(); ++i) {
            const int id1 = teethIds[i - 2], id2 = teethIds[i - 1], id3 = teethIds[i];
            Eigen::Vector3d pt1 = contactPoints.at(UnorderedPair<int>{id1, id2});
            Eigen::Vector3d pt2 = contactPoints.at(UnorderedPair<int>{id2, id3});

            // actors.push_back(Utilities::getLineActor(pt1, pt2, {1, 0, 0}));
            // actors.push_back(Utilities::getPointsActor({pt1, pt2}, {1, 0, 0}));
        }
        Utilities::DisplayActors(actors, colors);
    }
    */

    void EstimateCurveTest_VisualizePoints_AndCurve_EllipseUpper()
    {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw upper { Model::TeethType::Upper };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isUpperTooth(toothId)) {
                if (auto [iter, ok] = upper.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        std::unordered_map<UnorderedPair<int>, Eigen::Vector3d, PairHashUnordered<int>> contactPoints;
        std::array<int, 14> teethIds {17, 16, 15, 14, 13, 12, 11, 21, 22, 23, 24, 25, 26, 27};
        for (size_t i = 1; i< teethIds.size(); ++i) {
            const int id1 = teethIds[i - 1], id2 = teethIds[i];
            const vtkSmartPointer<vtkPolyData> tooth1 = toothMap.at(id1);
            const vtkSmartPointer<vtkPolyData> tooth2 = toothMap.at(id2);

            Eigen::Vector3d ptContact = getContactPoint(tooth1, tooth2);
            contactPoints.emplace(UnorderedPair<int>{id1, id2}, ptContact);
        }

        std::vector<vtkSmartPointer<vtkActor>> actors;
        for (const auto&[id, tooth]: upper.teeth)
        {
            vtkSmartPointer<vtkPolyData> toothData = tooth.toothPolyData;

            Eigen::Vector3d center {};
            toothData->GetCenter(center.data());

            const Model::ToothAxis axes = tooth.getAxis();
            Eigen::Vector3d start = center - axes.horizontal;
            Eigen::Vector3d end = center + axes.horizontal;

            const auto [slopeY, interceptY] = get_line_coefficients({start.x(), start.y()}, {end.x(), end.y()});
            const auto [slopeZ, interceptZ] = get_line_coefficients({start.x(), start.z()}, {end.x(), end.z()});

            // the ratio of the current length of the segment to the length of the tooth
            const double X = tooth.mesiodistalWidth / twoPointDistance(start, end);;

            start.x() -= std::abs((start.x() - center.x())) * (X - 1);
            end.x() += std::abs((end.x() - center.x())) * (X - 1);
            start.y() = start.x() * slopeY + interceptY;
            end.y() = end.x() * slopeY + interceptY;
            start.z() = start.x() * slopeZ + interceptZ;
            end.z() = end.x() * slopeZ + interceptZ;

            actors.push_back(Utilities::getPolyDataActor(toothData));
            // actors.push_back(Utilities::getLineActor(start, end, {1, 0, 1}));
            // actors.push_back(Utilities::getPointsActor({start, end}, {1, 0, 1}));
        }

        //***********************************************************************************************//
        //                                 Calc Pont and Corkhause values                                //
        //***********************************************************************************************//

        const double incisorsLength = calcIncisorsLength(upper);
        const double premolarDistanceExpected { incisorsLength * 100 / 85 };
        const double molarDistanceExpected { incisorsLength * 100 / 65 };

        Eigen::Vector3d tooth14Center {}, tooth24Center {}, tooth16Center {}, tooth26Center {};
        {
            upper.teeth.at(14).toothPolyData->GetCenter(tooth14Center.data());
            upper.teeth.at(24).toothPolyData->GetCenter(tooth24Center.data());
            upper.teeth.at(16).toothPolyData->GetCenter(tooth16Center.data());
            upper.teeth.at(26).toothPolyData->GetCenter(tooth26Center.data());
        }

        const double premolarDistanceActual_Upper = twoPointDistance(tooth14Center, tooth24Center);
        const double molarDistanceActual_Upper = twoPointDistance(tooth16Center, tooth26Center);

        const Eigen::Vector3d ptContact = getContactPoint(upper.teeth.at(11).toothPolyData,
                                                          upper.teeth.at(21).toothPolyData);
        double t = 0;
        Eigen::Vector3d closest {};
        [[maybe_unused]] const double distSquared = vtkLine::DistanceToLine(ptContact.data(),
                     tooth14Center.data(), tooth24Center.data(), t, closest.data());

        const double distance = twoPointDistance(ptContact, closest);
        const double corkhouseDist = GetCorkhouseDist(incisorsLength);

        {
            std::cout << "Incisors length: " << incisorsLength << std::endl;
            std::cout << "Premolar distance: \n" <<  "   Actual: "
                      << premolarDistanceActual_Upper << ". Expected: " << premolarDistanceExpected << std::endl;
            std::cout << "Molar distance: \n" << "   Actual: "
                      << molarDistanceActual_Upper << ". Expected: " <<  molarDistanceExpected << std::endl;
            std::cout << "Korkhouse: \n" << "   Actual: "
                      << distance << ". Expected: " << corkhouseDist << std::endl;
        }

        {
            actors.push_back(Utilities::getLineActor(tooth14Center, tooth24Center, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({tooth14Center, tooth24Center}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(tooth16Center, tooth26Center, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({tooth16Center, tooth26Center}, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({ptContact, closest}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(ptContact, closest, {1, 0, 0}));
        }

        { // Calc points for 'Ideal Curve':
            const double premolarDiff = premolarDistanceExpected - premolarDistanceActual_Upper;
            const double molarDiff = molarDistanceExpected - molarDistanceActual_Upper;
            const double corkDiff = corkhouseDist - distance;

            auto equationEllipse = [](double x, double a, double b) -> double  {
                if (x == a || 0 == a || 0 == b)
                    return 0;
                return std::sqrt((b * b) * (1 - (x * x) / (a * a)));
            };

            Eigen::Vector3d pt14 {tooth14Center}, pt24 {tooth24Center},
                    pt16 {tooth16Center}, pt26 {tooth26Center}, center {ptContact};
            pt14.x() += (pt14.x() > 0 ? 1: -1) * premolarDiff / 2;
            pt24.x() += (pt24.x() > 0 ? 1: -1) * premolarDiff / 2;
            pt16.x() += (pt16.x() > 0 ? 1: -1) * molarDiff / 2;
            pt26.x() += (pt26.x() > 0 ? 1: -1) * molarDiff / 2;
            center.z() += (center.z() > 0 ? 1: -1) * corkDiff;

            const double A = (pt26.x() - pt16.x()) / 2;
            const double B = center.z() - pt16.z() - 1;
            const double zOffset = pt26.z();

            std::cout << A << ", " << B << std::endl;
            std::cout << "Z = " << zOffset<< std::endl;

            actors.push_back(Utilities::getPointsActor({ pt14, pt24, pt16, pt26, center }, {1, 0, 1}));


            std::vector<Eigen::Vector3d> curvePoints;
            const double stepsCount = 1000;
            const double step = (2 * A) / stepsCount;

            const double xBegin = -A, xEnd = A;
            std::cout << "[" << xBegin << " - " << xEnd << "]\n";

            for(double x = xBegin; x <= xEnd; x += step) {
                const double y = equationEllipse(x, A, B);
                std::cout << "(" << x << " - " << y << ")\n";
                curvePoints.emplace_back(x, 0, y + zOffset);
            }
            actors.push_back(Utilities::getPointsActor(curvePoints, {0, 0, 0}, 8));
        }

        //***********************************************************************************************//
        //                                 Visualize contact points part                                 //
        //***********************************************************************************************//

        // Add contact points lines
        for (size_t i = 2; i< teethIds.size(); ++i) {
            const int id1 = teethIds[i - 2], id2 = teethIds[i - 1], id3 = teethIds[i];
            [[maybe_unused]] Eigen::Vector3d pt1 = contactPoints.at(UnorderedPair<int>{id1, id2});
            [[maybe_unused]] Eigen::Vector3d pt2 = contactPoints.at(UnorderedPair<int>{id2, id3});

            // actors.push_back(Utilities::getLineActor(pt1, pt2, {1, 0, 0}));
            // actors.push_back(Utilities::getPointsActor({pt1, pt2}, {1, 0, 0}));
        }
        Utilities::DisplayActors(actors);
    }

    void EstimateCurveTest_VisualizePoints_AndCurve_EllipseLower()
    {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw lowerJaw { Model::TeethType::Lower }, upperJaw { Model::TeethType::Lower };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isLowerTooth(toothId)) {
                if (auto [iter, ok] = lowerJaw.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
            if (FDI::isUpperTooth(toothId)) {
                if (auto [iter, ok] = upperJaw.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        constexpr unsigned short incisorId1 = 31, incisorId2 = 41;
        constexpr unsigned short left1 = 46, left2 = 44;
        constexpr unsigned short right1 = 36, right2 = 34;

        std::vector<vtkSmartPointer<vtkActor>> actors;
        constexpr std::array<int, 14> teethIds {47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};
        const std::unordered_map<UnorderedPair<int>, Eigen::Vector3d, PairHashUnordered<int>> contactPoints
                = CalcTeethContactPoints(toothMap);

        for (const auto&[id, tooth]: lowerJaw.teeth)
        {
            vtkSmartPointer<vtkPolyData> toothData = tooth.toothPolyData;

            Eigen::Vector3d center {};
            toothData->GetCenter(center.data());

            const Model::ToothAxis axes = tooth.getAxis();
            Eigen::Vector3d start = center - axes.horizontal;
            Eigen::Vector3d end = center + axes.horizontal;

            const auto [slopeY, interceptY] = get_line_coefficients({start.x(), start.y()}, {end.x(), end.y()});
            const auto [slopeZ, interceptZ] = get_line_coefficients({start.x(), start.z()}, {end.x(), end.z()});

            // the ratio of the current length of the segment to the length of the tooth
            const double X = tooth.mesiodistalWidth / twoPointDistance(start, end);;

            start.x() -= std::abs((start.x() - center.x())) * (X - 1);
            end.x() += std::abs((end.x() - center.x())) * (X - 1);
            start.y() = start.x() * slopeY + interceptY;
            end.y() = end.x() * slopeY + interceptY;
            start.z() = start.x() * slopeZ + interceptZ;
            end.z() = end.x() * slopeZ + interceptZ;

            // actors.push_back(Utilities::getPolyDataActor(toothData, colors));

            // actors.push_back(Utilities::getLineActor(start, end, {1, 0, 1}));
            // actors.push_back(Utilities::getPointsActor({start, end}, {1, 0, 1}));
        }


        //***********************************************************************************************//
        //                                 Calc Pont and Corkhause values                                //
        //***********************************************************************************************//

        const double incisorsLength = calcIncisorsLength(upperJaw);
        const double premolarDistanceExpected { incisorsLength * 100 / 85 };
        const double molarDistanceExpected { incisorsLength * 100 / 65 };

        Eigen::Vector3d ptRightLower {}, ptRightUpper {}, ptLeftLower {}, ptLeftUpper {};
        {
            ptLeftLower = contactPoints.at(UnorderedPair<int>{left1, left1 + 1});
            ptLeftUpper = contactPoints.at(UnorderedPair<int>{left2, left2 + 1});
            ptRightLower = contactPoints.at(UnorderedPair<int>{right1, right1 + 1});
            ptRightUpper = contactPoints.at(UnorderedPair<int>{right2, right2 + 1});

            ptRightLower.x() += 1; ptRightLower.z() += 4.5; // ptRightLower.y() += 4;
            ptLeftLower.x() -= 1; ptLeftLower.z() += 4.5; // ptLeftLower.y() += 4;
        }

        const double premolarDistanceActual_Upper = twoPointDistance(ptLeftUpper, ptRightUpper);
        const double molarDistanceActual_Upper = twoPointDistance(ptRightLower, ptLeftLower);
        const Eigen::Vector3d ptContact = contactPoints.at(UnorderedPair<int>{incisorId2, incisorId1});
        double t = 0;
        Eigen::Vector3d closest {};
        [[maybe_unused]] const double distSquared = vtkLine::DistanceToLine(ptContact.data(),
                               ptLeftUpper.data(), ptRightUpper.data(), t, closest.data());
        std::cout << ptContact << std::endl;
        const double distance = twoPointDistance(ptContact, closest);
        const double corkhouseDist = GetCorkhouseDist(incisorsLength);

        {
            std::cout << "Incisors length: " << incisorsLength << std::endl;
            std::cout << "Premolar distance: \n" <<  "   Actual: "
                      << premolarDistanceActual_Upper << ". Expected: " << premolarDistanceExpected << std::endl;
            std::cout << "Molar distance: \n" << "   Actual: "
                      << molarDistanceActual_Upper << ". Expected: " <<  molarDistanceExpected << std::endl;
            std::cout << "Korkhouse: \n" << "   Actual: "
                      << distance << ". Expected: " << corkhouseDist << std::endl;
        }

        {
            actors.push_back(Utilities::getLineActor(ptLeftUpper, ptRightUpper, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({ptLeftUpper, ptRightUpper}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(ptLeftLower, ptRightLower, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({ptLeftLower, ptRightLower}, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({ptContact, closest}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(ptContact, closest, {1, 0, 0}));
        }


        { // Parabola:
            const double premolarDiff = premolarDistanceExpected - premolarDistanceActual_Upper;
            const double molarDiff = molarDistanceExpected - molarDistanceActual_Upper;
            const double corkDiff = corkhouseDist - distance;

            [[maybe_unused]]
            auto equationEllipse = [](double x, double a, double b) -> double  {
                if (x == a || 0 == a || 0 == b)
                    return 0;
                return std::sqrt((b * b) * (1 - (x * x) / (a * a)));
            };

            Eigen::Vector3d rightUpper {ptRightUpper}, rightLower {ptRightLower},
                    leftUpper {ptLeftUpper}, leftLower {ptLeftLower}, center {ptContact};
            rightUpper.x() += (rightUpper.x() > 0 ? 1: -1) * premolarDiff / 2;
            leftUpper.x() += (leftUpper.x() > 0 ? 1: -1) * premolarDiff / 2;
            rightLower.x() += (rightLower.x() > 0 ? 1: -1) * molarDiff / 2;
            leftLower.x() += (leftLower.x() > 0 ? 1: -1) * molarDiff / 2;
            center.z() += (center.z() > 0 ? 1: -1) * corkDiff;

            actors.push_back(Utilities::getPointsActor(
                    { rightUpper, leftUpper, rightLower, leftLower, center }, {1, 0, 1}));

            double A = std::abs(rightLower.x() - leftLower.x()) / 2;
            double B = center.z() - rightLower.z();
            const double xMid = std::midpoint(rightLower.x(), leftLower.x());
            const double zOffset = rightLower.z();

            std::cout << "xMid = " << xMid << std::endl;
            auto ellipse = [=](double x) {
                return equationEllipse(x, A, B) + zOffset;
            };

            const double stepsCount = 10000;
            const double step = (2 * A) / stepsCount;

            std::vector<Eigen::Vector3d> curvePoints;
            for(double x = -A; x <= A; x += step) {
                curvePoints.emplace_back(x + xMid, 0, ellipse(x));
            }
            actors.push_back(Utilities::getPointsActor(curvePoints, {0, 0, 0}, 6));

            // ---------------------- Tooth points -------------------------------

            Eigen::Vector3d tooth32 {}, tooth42 {};
            lowerJaw.teeth.at(32).toothPolyData->GetCenter(tooth32.data());
            lowerJaw.teeth.at(42).toothPolyData->GetCenter(tooth42.data());

            tooth32.y() = 0; tooth42.y() = 0;

            Eigen::Vector3d closes32 = findClosetPoint(tooth32, ellipse);
            Eigen::Vector3d closes42 = findClosetPoint(tooth42, ellipse);

            // TODO: Do something with xMid
            closes32.x() += xMid; closes42.x() += xMid;

            // actors.push_back(Utilities::getPointsActor({tooth32, closes32, tooth42, closes42}, {0, 1, 0}));
            // actors.push_back(Utilities::getLineActor(tooth32, closes32, {0, 1, 0}));
            // actors.push_back(Utilities::getLineActor(tooth42, closes42, {0, 1, 0}));


            //----------------------  Move teeth -------------------------------------

            Eigen::Vector3d ptFrom{}, ptTo {};
            double ratio = 0.0;
            {
                const double dist32 = twoPointDistance(tooth32, closes32);
                const double dist42 = twoPointDistance(tooth42, closes42);
                if (dist32 > dist42) {
                    ptFrom = tooth32;
                    ptTo = closes32;
                    ratio = 1 - dist42/ dist32;
                } else {
                    ptFrom = tooth42;
                    ptTo = closes42;
                    ratio = 1 - dist32/ dist42;
                }
                actors.push_back(Utilities::getLineActor(ptFrom, ptTo, {0, 1, 0}));
            }

            {
                const auto[slope, intercept] = get_line_coefficients({ptFrom.x(), ptFrom.z()}, {ptTo.x(), ptTo.z()});
                double xStep = (ptTo.x() - ptFrom.x()) * ratio;

                Eigen::Vector3d toothCenter {};
                auto& tooth32 = lowerJaw.teeth.at(32);
                tooth32.toothPolyData->GetCenter(toothCenter.data());

                toothCenter.x() += xStep;
                toothCenter.z() = toothCenter.x() * slope + intercept;

                //tooth32.toothPolyData->DeepCopy(Utilities::setPolyDataCenter(
                //        tooth32.toothPolyData, toothCenter.x(), toothCenter.y(), toothCenter.z()));
            }

        }



        /*
        {  // Ellipse:
            const double premolarDiff = premolarDistanceExpected - premolarDistanceActual_Upper;
            const double molarDiff = molarDistanceExpected - molarDistanceActual_Upper;
            const double corkDiff = corkhouseDist - distance;

            Eigen::Vector3d rightUpper {ptRightUpper}, rightLower {ptRightLower},
                    leftUpper {ptLeftUpper}, leftLower {ptLeftLower}, center {ptContact};
            rightUpper.x() += (rightUpper.x() > 0 ? 1: -1) * premolarDiff / 2;
            leftUpper.x() += (leftUpper.x() > 0 ? 1: -1) * premolarDiff / 2;
            rightLower.x() += (rightLower.x() > 0 ? 1: -1) * molarDiff / 2;
            leftLower.x() += (leftLower.x() > 0 ? 1: -1) * molarDiff / 2;
            center.z() += (center.z() > 0 ? 1: -1) * corkDiff - 3; // TODO: -3 for test

            actors.push_back(Utilities::getPointsActor({
                leftLower, leftUpper, center, rightUpper, rightLower }, {1, 0, 1}));

            const std::vector<double> X { leftLower.x(), leftUpper.x(),  center.x(), rightUpper.x(), rightLower.x() };
            const std::vector<double> Z { leftLower.z(), leftUpper.z(), center.z(), rightUpper.z(), rightLower.z() };

            std::vector<Eigen::Vector3d> curvePoints;
            std::vector<double> coefficients;
            polyfit(X, Z, coefficients, 3);
            const double stepsCount = 1000;
            const double step = (X.back() - X.front()) / stepsCount;
            for(double i = X.front(); i < X.back(); i += step) {
                const double zPredicted = coefficients[0] + coefficients[1] * i +
                                          coefficients[2] * (pow(i, 2)) + coefficients[3] * (pow(i, 3)) ;
                curvePoints.emplace_back(i, leftLower.y() + 2, zPredicted);
            }
            actors.push_back(Utilities::getPointsActor(curvePoints, {0, 0, 0}, 6));
        }
        */

        for (const auto&[id, tooth]: lowerJaw.teeth) {
            actors.push_back(Utilities::getPolyDataActor(tooth.toothPolyData));
        }

        //***********************************************************************************************//
        //                                 Visualize contact points part                                 //
        //***********************************************************************************************//

        // Add contact points lines
        for (size_t i = 2; i< teethIds.size(); ++i) {
            const int id1 = teethIds[i - 2], id2 = teethIds[i - 1], id3 = teethIds[i];
            [[maybe_unused]] Eigen::Vector3d pt1 = contactPoints.at(UnorderedPair<int>{id1, id2});
            [[maybe_unused]] Eigen::Vector3d pt2 = contactPoints.at(UnorderedPair<int>{id2, id3});

            // actors.push_back(Utilities::getLineActor(pt1, pt2, {1, 0, 0}));
            // actors.push_back(Utilities::getPointsActor({pt1, pt2}, {1, 0, 0}));
        }
        Utilities::DisplayActors(actors);
    }
}


namespace Experiments::MoveTeeth
{

    void EstimateCurveTest_VisualizePoints_AndCurve_EllipseLower()
    {
        constexpr std::string_view planJsonFile { R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };
        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2280/automodeling/out/2280_teeth.obj)" };

        const TreatmentPlan::Plan plan = TreatmentPlan::Parser::Parse(planJsonFile);
        const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap =
                Tests::ReadTeethObj(filePath);

        std::unordered_map<unsigned short, std::vector<double>> teethAxes;
        for (const auto& [toothId, toothData] : plan.modellingData.tooth)
            teethAxes.emplace(toothId, toothData.axes);

        Model::Jaw lowerJaw { Model::TeethType::Lower }, upperJaw { Model::TeethType::Lower };
        std::unique_ptr<Estimators::Estimator> estimator { std::make_unique<Estimators::DistanceEstimator>()};
        for (const auto& [toothId, data]: toothMap) {
            if (FDI::isLowerTooth(toothId)) {
                if (auto [iter, ok] = lowerJaw.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
            if (FDI::isUpperTooth(toothId)) {
                if (auto [iter, ok] = upperJaw.emplace(toothId, Model::Tooth{data, toothId}); true == ok) {
                    iter->second.setAxis(teethAxes[toothId]);
                    estimator->estimate(iter->second);
                }
            }
        }

        constexpr unsigned short incisorId1 = 31, incisorId2 = 41;
        constexpr unsigned short left1 = 46, left2 = 44;
        constexpr unsigned short right1 = 36, right2 = 34;

        std::vector<vtkSmartPointer<vtkActor>> actors;
        // constexpr std::array<int, 14> teethIds {47, 46, 45, 44, 43, 42, 41, 31, 32, 33, 34, 35, 36, 37};
        const auto contactPoints = CalcTeethContactPoints(toothMap);

#if 0
        for (const auto&[id, tooth]: lowerJaw.teeth)
        {
            vtkSmartPointer<vtkPolyData> toothData = tooth.toothPolyData;

            Eigen::Vector3d center {};
            toothData->GetCenter(center.data());

            const Model::ToothAxis axes = tooth.getAxis();
            Eigen::Vector3d start = center - axes.horizontal;
            Eigen::Vector3d end = center + axes.horizontal;

            const auto [slopeY, interceptY] = get_line_coefficients({start.x(), start.y()}, {end.x(), end.y()});
            const auto [slopeZ, interceptZ] = get_line_coefficients({start.x(), start.z()}, {end.x(), end.z()});

            // the ratio of the current length of the segment to the length of the tooth
            const double X = tooth.mesiodistalWidth / twoPointDistance(start, end);;

            start.x() -= std::abs((start.x() - center.x())) * (X - 1);
            end.x() += std::abs((end.x() - center.x())) * (X - 1);
            start.y() = start.x() * slopeY + interceptY;
            end.y() = end.x() * slopeY + interceptY;
            start.z() = start.x() * slopeZ + interceptZ;
            end.z() = end.x() * slopeZ + interceptZ;

            // actors.push_back(Utilities::getPolyDataActor(toothData, colors));
            // actors.push_back(Utilities::getLineActor(start, end, {1, 0, 1}));
            // actors.push_back(Utilities::getPointsActor({start, end}, {1, 0, 1}));
        }
#endif

        //***********************************************************************************************//
        //                                 Calc Pont and Corkhause values                                //
        //***********************************************************************************************//

        const double incisorsLength = calcIncisorsLength(upperJaw);
        const double premolarDistanceExpected { incisorsLength * 100 / 85 };
        const double molarDistanceExpected { incisorsLength * 100 / 65 };

        Eigen::Vector3d ptRightLower {}, ptRightUpper {}, ptLeftLower {}, ptLeftUpper {};
        {
            ptLeftLower = contactPoints.at(UnorderedPair<int>{left1, left1 + 1});
            ptLeftUpper = contactPoints.at(UnorderedPair<int>{left2, left2 + 1});
            ptRightLower = contactPoints.at(UnorderedPair<int>{right1, right1 + 1});
            ptRightUpper = contactPoints.at(UnorderedPair<int>{right2, right2 + 1});

            ptRightLower.x() += 1; ptRightLower.z() += 4.5; // ptRightLower.y() += 4;
            ptLeftLower.x() -= 1; ptLeftLower.z() += 4.5; // ptLeftLower.y() += 4;
        }

        const double premolarDistanceActual_Upper = twoPointDistance(ptLeftUpper, ptRightUpper);
        const double molarDistanceActual_Upper = twoPointDistance(ptRightLower, ptLeftLower);
        const Eigen::Vector3d ptContact = contactPoints.at(UnorderedPair<int>{incisorId2, incisorId1});
        double t = 0;

        Eigen::Vector3d closest {};
        [[maybe_unused]] const double distSquared = vtkLine::DistanceToLine(ptContact.data(),
                    ptLeftUpper.data(), ptRightUpper.data(), t, closest.data());
        const double distance = twoPointDistance(ptContact, closest);
        const double corkhouseDist = GetCorkhouseDist(incisorsLength);

        {
            actors.push_back(Utilities::getLineActor(ptLeftUpper, ptRightUpper, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({ptLeftUpper, ptRightUpper}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(ptLeftLower, ptRightLower, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({ptLeftLower, ptRightLower}, {1, 0, 0}));
            actors.push_back(Utilities::getPointsActor({ptContact, closest}, {1, 0, 0}));
            actors.push_back(Utilities::getLineActor(ptContact, closest, {1, 0, 0}));
        }


        { // Ellipse:
            const double premolarDiff = premolarDistanceExpected - premolarDistanceActual_Upper;
            const double molarDiff = molarDistanceExpected - molarDistanceActual_Upper;
            const double corkDiff = corkhouseDist - distance;

            [[maybe_unused]]
            auto equationEllipse = [](double x, double a, double b) -> double  {
                if (x == a || 0 == a || 0 == b)
                    return 0;
                return std::sqrt((b * b) * (1 - (x * x) / (a * a)));
            };

            Eigen::Vector3d rightUpper {ptRightUpper}, rightLower {ptRightLower},
                    leftUpper {ptLeftUpper}, leftLower {ptLeftLower}, center {ptContact};
            rightUpper.x() += (rightUpper.x() > 0 ? 1: -1) * premolarDiff / 2;
            leftUpper.x() += (leftUpper.x() > 0 ? 1: -1) * premolarDiff / 2;
            rightLower.x() += (rightLower.x() > 0 ? 1: -1) * molarDiff / 2;
            leftLower.x() += (leftLower.x() > 0 ? 1: -1) * molarDiff / 2;
            center.z() += (center.z() > 0 ? 1: -1) * corkDiff;

            actors.push_back(Utilities::getPointsActor(
                    { rightUpper, leftUpper, rightLower, leftLower, center }, {1, 0, 1}));

            double A = std::abs(rightLower.x() - leftLower.x()) / 2;
            double B = center.z() - rightLower.z();
            const double xMid = std::midpoint(rightLower.x(), leftLower.x());
            const double zOffset = rightLower.z();

            std::cout << "xMid = " << xMid << std::endl;
            auto ellipse = [=](double x) {
                return equationEllipse(x, A, B) + zOffset;
            };

            const double stepsCount = 10000;
            const double step = (2 * A) / stepsCount;

            std::vector<Eigen::Vector3d> curvePoints;
            for(double x = -A; x <= A; x += step) {
                curvePoints.emplace_back(x + xMid, 0, ellipse(x));
            }
            actors.push_back(Utilities::getPointsActor(curvePoints, {0, 0, 0}, 6));


            // ---------------------- Tooth points -------------------------------

            constexpr std::array<std::pair<unsigned short, unsigned short>, 5> toothPairs {{
                {31, 41},
                {32, 42},
                {33, 43},
                {34, 44},
                {35, 45},
                // {36, 46}
            }};
            for (const auto& [id1, id2]: toothPairs) {

                Eigen::Vector3d firstTooth{}, secondTooth{};
                lowerJaw.teeth.at(id1).toothPolyData->GetCenter(firstTooth.data());
                lowerJaw.teeth.at(id2).toothPolyData->GetCenter(secondTooth.data());

                firstTooth.y() = 0;
                secondTooth.y() = 0;

                Eigen::Vector3d closesToFirst = findClosetPoint(firstTooth, ellipse);
                Eigen::Vector3d closesToSecond = findClosetPoint(secondTooth, ellipse);

                // TODO: Do something with xMid
                closesToFirst.x() += xMid; closesToSecond.x() += xMid;

                // actors.push_back(Utilities::getPointsActor({closesToFirst, closesToSecond}, {0, 1, 0}));
                // actors.push_back(Utilities::getLineActor(tooth32, closes32, {0, 1, 0}));
                // actors.push_back(Utilities::getLineActor(tooth42, closes42, {0, 1, 0}));

                //----------------------  Move teeth -------------------------------------

                Eigen::Vector3d ptFrom{}, ptTo{};
                double ratio = 0.0;
                int toothToMoveID = -1;
                {
                    const double fistDist = twoPointDistance(firstTooth, closesToFirst);
                    const double secondDist = twoPointDistance(secondTooth, closesToSecond);
                    if (fistDist > secondDist) {
                        ptFrom = firstTooth;
                        ptTo = closesToFirst;
                        ratio = 1 - secondDist / fistDist;
                        toothToMoveID = id1;
                    } else {
                        ptFrom = secondTooth;
                        ptTo = closesToSecond;
                        ratio = 1 - fistDist / secondDist;
                        toothToMoveID = id2;
                    }
                    // actors.push_back(Utilities::getLineActor(ptFrom, ptTo, {0, 1, 0}));
                }

                {
                    // TODO: bad!!! Refactor it!!!!
                    const auto[slope, intercept] = get_line_coefficients({ptFrom.x(), ptFrom.z()}, {ptTo.x(), ptTo.z()});
                    double xStep = (ptTo.x() - ptFrom.x()) * ratio;

                    Eigen::Vector3d toothCenter{};
                    const auto &toothToMove = lowerJaw.teeth.at(toothToMoveID);
                    toothToMove.toothPolyData->GetCenter(toothCenter.data());

                    toothCenter.x() += xStep;
                    toothCenter.z() = toothCenter.x() * slope + intercept;

                    toothToMove.toothPolyData->DeepCopy(Utilities::setPolyDataCenter(
                            toothToMove.toothPolyData, toothCenter.x(), toothCenter.y(), toothCenter.z()));
                }
            }

        }
        //***********************************************************************************************//
        //                                     Visualize teeth                                           //
        //***********************************************************************************************//

        for (const auto&[id, tooth]: lowerJaw.teeth)
            actors.push_back(Utilities::getPolyDataActor(tooth.toothPolyData));

        Utilities::DisplayActors(actors);
    }
}

// TODO: it is necessary to visualize the calculated segments of the
//       mesiodistane length of the teeth in accordance with the X axis

// TODO: Добавить анализ кости и кортикальной пласты что бы понимать
//      на сколько мы модем и можем ли вообще двигать зубы

// TODO: Add calculation of torc angles and angulation of teeth --> To JAW Class

// TODO: Доделать модуль устанвливающий положение горизонтальной оси зуба вдоль режущегося края
//       (в зависимости от типа зуба)

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Experiments::Display_Tooth_Axis();

    // Experiments::Visualize_Tooth_and_Axis();

    // Experiments::Visualize_Tooth_and_Axis_EigenQuaterniond();
    // Experiments::Visualize_Tooth_and_Axis_EigenQuaterniond_Estimator();
    // Experiments::Visualize_Tooth_and_Axis_EigenQuaterniond_Debug();
    // Experiments::Visualize_Tooth_and_Axis_GOOD();
    // Experiments::Visualize_Tooth_and_Axis_PERFORMANCE_SINGLE_THREAD();
    // Experiments::Visualize_Tooth_and_Axis_MEM_LEAK();

    // TreatmentPlan_Tests::TestAll();
    // TreatmentPlan_UnitTests::TestAll();

    // Setup::SetupTeethAndVisualize();

    // Experiments::EstimateCurve::EstimateCurveTest();
    // Experiments::EstimateCurve::EstimateCurveTest_VisualizePoints_AndCurve_Parabola(); // PolynomialFit
    // Experiments::EstimateCurve::EstimateCurveTest_VisualizePoints_AndCurve_EllipseUpper();
    // Experiments::EstimateCurve::EstimateCurveTest_VisualizePoints_AndCurve_EllipseLower();

    // Experiments::EstimateCurve::CalculateContactPointTest();
    // Experiments::EstimateCurve::CalculateContactPointTest_Parallel();

    // Experiments::EstimateCurve::CalculateAndVisualize();
    // Experiments::EstimateCurve::VisualizeTooths_LineSections();
    // Experiments::EstimateCurve::VisualizeTooths_LineSections_ContactPoints();
    // Experiments::EstimateCurve::VisualizeTooths_LineSections_And_ContactPoints();

    // INFO: Move to to Ellipse curve
    // Experiments::MoveTeeth::EstimateCurveTest_VisualizePoints_AndCurve_EllipseLower();

    // EstimateCurve::TestAll(args);
    // EstimateCurve2::TestAll(args);
    // EstimateCurve3::TestAll(args);
    // FindBestCurveTests::TestAll(args);
    // SetUpTeethSegments::TestAll();
    // TeethAnglesOrientation::TestAll();
    // SetUpTorksAndPositions::TestAll(args);

    return EXIT_SUCCESS;
}

