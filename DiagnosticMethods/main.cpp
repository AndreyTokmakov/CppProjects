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

#include "Methods/SpeeСurve.h"

#include "Model/Common.h"
#include "Model/FDI.h"
#include "Model/Jaw.h"
#include "Model/Tooth.h"
#include "Model/ToothAxis.h"

#include "Utilities/Utilities.h"
#include "TreatmentPlan/TreatmentPlan.h"


namespace Tests {
    const vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors>::New() };

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
}

namespace Experiments
{
    vtkSmartPointer<vtkNamedColors> colors { vtkSmartPointer<vtkNamedColors>::New() };

    vtkSmartPointer<vtkPolyData> rotatePolyData(const vtkSmartPointer<vtkPolyData> polyData,
                                                double x, double y, double z) noexcept {
        const vtkSmartPointer<vtkTransform> transform { vtkSmartPointer<vtkTransform>::New() };
        transform->RotateX(x);
        transform->RotateY(y);
        transform->RotateZ(z);

        const vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter {
            vtkSmartPointer<vtkTransformPolyDataFilter>::New() };
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> rotatePolyData(const vtkSmartPointer<vtkPolyData> polyData,
                                                const vtkSmartPointer<vtkMatrix4x4>& matrix) noexcept {
        const vtkSmartPointer<vtkTransform> transform { vtkSmartPointer<vtkTransform>::New() };
        transform->SetMatrix(matrix);

        const vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter {
                vtkSmartPointer<vtkTransformPolyDataFilter>::New() };
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> moveTooth(const vtkSmartPointer<vtkPolyData>& polyData,
                                           double x, double y, double z) {
        const vtkSmartPointer<vtkTransform> transform { vtkSmartPointer<vtkTransform>::New() };
        transform->Translate(x, y, z);

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter
            { vtkSmartPointer<vtkTransformPolyDataFilter>::New()};
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> setPolyDataCenter(const vtkSmartPointer<vtkPolyData>& polyData,
                                                   double x, double y, double z) {
        Eigen::Vector3d center;
        polyData->GetCenter(center.data());
        return moveTooth(polyData, x - center[0], y - center[1], z - center[2]);
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



    void Visualize_Tooth_OBJ_Data() {
        using namespace Model;

        constexpr std::string_view filePath {
                R"(/home/andtokm/Projects/data/cases/2878/automodeling/out/2878_teeth.obj)" };
        std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>> toothMap = Tests::ReadTeethObj(filePath);

        Jaw lower(toothMap, TeethType::Lower);
        Jaw upper(toothMap, TeethType::Upper);

        for (auto &jaw: {lower, upper}){
            const vtkSmartPointer<vtkAppendPolyData> dataAppender {vtkSmartPointer<vtkAppendPolyData>::New()};
            for (const auto &tooth: jaw.teeth)
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

            vtkSmartPointer<vtkPolyData> toothData = setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);

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
                toothData = rotatePolyData(toothData, 0, rotationAngle, 0);
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
                toothData = rotatePolyData(toothData, xRotationAngle, 0, 0);
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
                toothData = rotatePolyData(toothData, 0, 0, zRotationAngle);

                vtkSmartPointer<vtkActor> verticalAxeActor =
                        Utilities::getLineActor({0, 0, 0}, axes.vertical * 7, colors);
                vtkSmartPointer<vtkActor> frontAxeActor =
                        Utilities::getLineActor({0, 0, 0}, axes.angulation * 7, colors);
                vtkSmartPointer<vtkActor> horizontalAxeActor =
                        Utilities::getLineActor({0, 0, 0}, axes.horizontal * 7, colors);
                horizontalAxeActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Red").GetData());

                vtkSmartPointer<vtkActor> horizontalOrigActor =
                        Utilities::getLineActor({0, 0, 0}, horizontalOrig * 7, colors);
                horizontalOrigActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());


                const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothData, colors);
                const vtkSmartPointer<vtkActor> outlineActor = Utilities::getOutlineActor(toothData);

                Utilities::DisplayActors({toothActor, outlineActor,
                                          verticalAxeActor, horizontalAxeActor, frontAxeActor
                                          }, colors);
            }
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

            vtkSmartPointer<vtkPolyData> toothData = setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);

            {
                Eigen::Matrix3d matrix {Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

                axes.horizontal = matrix * axes.horizontal;
                axes.vertical = matrix * axes.vertical;
                axes.angulation = matrix * axes.angulation;

                vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                toothData = rotatePolyData(toothData, vtkMatrix);

                matrix = Eigen::Quaterniond::FromTwoVectors(axes.vertical, verticalOrig).toRotationMatrix();

                axes.horizontal = matrix * axes.horizontal;
                axes.vertical = matrix * axes.vertical;
                axes.angulation = matrix * axes.angulation;

                vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                toothData = rotatePolyData(toothData, vtkMatrix);
            }


            vtkSmartPointer<vtkActor> verticalAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.vertical * 7, colors);
            vtkSmartPointer<vtkActor> frontAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.angulation * 7, colors);
            vtkSmartPointer<vtkActor> horizontalAxeActor =
                    Utilities::getLineActor({0, 0, 0}, axes.horizontal * 7, colors);
            horizontalAxeActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Red").GetData());

            vtkSmartPointer<vtkActor> horizontalOrigActor =
                    Utilities::getLineActor({0, 0, 0}, horizontalOrig * 7, colors);
            horizontalOrigActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());


            const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothData, colors);
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

                    vtkSmartPointer<vtkPolyData> toothData = setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);
                    {
                        Eigen::Matrix3d matrix{Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

                        axes.horizontal = matrix * axes.horizontal;
                        axes.vertical = matrix * axes.vertical;
                        axes.angulation = matrix * axes.angulation;

                        vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                        toothData = rotatePolyData(toothData, vtkMatrix);

                        matrix = Eigen::Quaterniond::FromTwoVectors(axes.vertical, verticalOrig).toRotationMatrix();

                        axes.horizontal = matrix * axes.horizontal;
                        axes.vertical = matrix * axes.vertical;
                        axes.angulation = matrix * axes.angulation;

                        vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                        toothData = rotatePolyData(toothData, vtkMatrix);
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

                    vtkSmartPointer<vtkPolyData> toothData = setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);

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
                        toothData = rotatePolyData(toothData, 0, rotationAngle, 0);
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
                        toothData = rotatePolyData(toothData, xRotationAngle, 0, 0);
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
                        toothData = rotatePolyData(toothData, 0, 0, zRotationAngle);
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

                    vtkSmartPointer<vtkPolyData> toothData = setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);
                    {
                        Eigen::Matrix3d matrix{Eigen::Quaterniond::FromTwoVectors(axes.horizontal, horizontalOrig)};

                        axes.horizontal = matrix * axes.horizontal;
                        axes.vertical = matrix * axes.vertical;
                        axes.angulation = matrix * axes.angulation;

                        vtkSmartPointer<vtkMatrix4x4> vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                        toothData = rotatePolyData(toothData, vtkMatrix);

                        matrix = Eigen::Quaterniond::FromTwoVectors(axes.vertical, verticalOrig).toRotationMatrix();

                        axes.horizontal = matrix * axes.horizontal;
                        axes.vertical = matrix * axes.vertical;
                        axes.angulation = matrix * axes.angulation;

                        vtkMatrix = Utilities::cast3x3MatrixToVTK4x4(matrix);
                        toothData = rotatePolyData(toothData, vtkMatrix);
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

            vtkSmartPointer<vtkPolyData> toothData = setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);

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
                toothData = rotatePolyData(toothData, 0, rotationAngle, 0);
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
                toothData = rotatePolyData(toothData, xRotationAngle, 0, 0);
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
                toothData = rotatePolyData(toothData, 0, 0, zRotationAngle);

                vtkSmartPointer<vtkActor> verticalAxeActor =
                        Utilities::getLineActor({0, 0, 0}, axes.vertical * 7, colors);
                vtkSmartPointer<vtkActor> frontAxeActor =
                        Utilities::getLineActor({0, 0, 0}, axes.angulation * 7, colors);
                vtkSmartPointer<vtkActor> horizontalAxeActor =
                        Utilities::getLineActor({0, 0, 0}, axes.horizontal * 7, colors);
                horizontalAxeActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Red").GetData());

                vtkSmartPointer<vtkActor> horizontalOrigActor =
                        Utilities::getLineActor({0, 0, 0}, horizontalOrig * 7, colors);
                horizontalOrigActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Yellow").GetData());


                const vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothData, colors);
                const vtkSmartPointer<vtkActor> outlineActor = Utilities::getOutlineActor(toothData);

                Utilities::DisplayActors({toothActor, outlineActor,
                                          verticalAxeActor, horizontalAxeActor, frontAxeActor
                                         }, colors);
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
                    Utilities::getLineActor(axes.vertical * -7, axes.vertical * 7, colors);
            const vtkSmartPointer<vtkActor> lineActor2 =
                    Utilities::getLineActor(axes.angulation * -7, axes.angulation * 7, colors);
            const vtkSmartPointer<vtkActor> lineActor3 =
                    Utilities::getLineActor(axes.horizontal * -7, axes.horizontal * 7, colors);


            vtkSmartPointer<vtkPolyData> toothData = setPolyDataCenter(tooth.toothPolyData, 0, 0, 0);
            vtkSmartPointer<vtkActor> toothActor = Utilities::getPolyDataActor(toothData, colors);
            Utilities::DisplayActors({toothActor,lineActor1, lineActor2, lineActor3}, colors);
        }
    }
}



int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    // Experiments::Display_Tooth_Axis();

    // Experiments::Visualize_Tooth_and_Axis();

    // Experiments::Visualize_Tooth_and_Axis_EigenQuaterniond();
    // Experiments::Visualize_Tooth_and_Axis_GOOD();
    // Experiments::Visualize_Tooth_and_Axis_PERFORMANCE_SINGLE_THREAD();
    // Experiments::Visualize_Tooth_and_Axis_MEM_LEAK();

    // TreatmentPlan_Tests::TestAll();
    // TreatmentPlan_UnitTests::TestAll();

    // SpeeCurve::TestAll();

    return EXIT_SUCCESS;
}

