//============================================================================
// Name        : Utilities.h
// Created on  : 27.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utilities
//============================================================================

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#ifndef UTILITIES_INCLUDE_GUARD
#define UTILITIES_INCLUDE_GUARD

#include <string>
#include <array>
#include <string_view>

#include <vtkBoxWidget.h>
#include <vtkColor.h>
#include <vtkNamedColors.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkMatrix3x3.h>
#include <vtkMatrix4x4.h>

// Eigen includes:
#include <Core>
#include <Dense>

namespace Utilities
{
    void WriteSTL(vtkAlgorithmOutput* data, std::string_view fileName) ;
    void WriteSTL(vtkSmartPointer<vtkPolyData> data, std::string_view fileName);

    vtkAlgorithmOutput* readPolyData(std::string_view fileName);
    vtkSmartPointer<vtkPolyData> readStl(std::string_view fileName);
    vtkSmartPointer<vtkPolyData> readObj(std::string_view fileName);



    vtkSmartPointer<vtkActor> getPolyDataActor(const vtkSmartPointer<vtkPolyData>& polyData,
                                               [[maybe_unused]] const vtkSmartPointer<vtkNamedColors>& colors);

    vtkSmartPointer<vtkActor> getPolyDataActor(const std::vector<vtkSmartPointer<vtkPolyData>>& data,
                                               [[maybe_unused]] const vtkSmartPointer<vtkNamedColors>& colors);

    vtkSmartPointer<vtkPolyData> pointsToPolyData(const std::vector<Eigen::Vector3d>& points);

    vtkSmartPointer<vtkActor> getPointsActor(const std::vector<Eigen::Vector3d>& points,
                                             const vtkSmartPointer<vtkNamedColors>& colors);

    vtkSmartPointer<vtkActor> getLinesActor_FromPoints(const std::vector<Eigen::Vector3d>& points,
                                                       const vtkSmartPointer<vtkNamedColors>& colors);

    vtkSmartPointer<vtkActor> getLineActor(const Eigen::Vector3d& pt1,
                                           const Eigen::Vector3d& pt2,
                                           [[maybe_unused]] const vtkSmartPointer<vtkNamedColors>& colors);

    [[nodiscard("Make sure to handle return value")]]
    vtkSmartPointer<vtkActor> getOutlineActor(const vtkSmartPointer<vtkPolyData>& toothData);

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkMatrix3x3> cast4x4MatrixTo3x3(const vtkSmartPointer<vtkMatrix4x4>& matrix);

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkMatrix4x4> cast3x3MatrixToVTK4x4(const Eigen::Matrix3d& matrix);

    void DisplayActors(const std::vector<vtkSmartPointer<vtkActor>>& actors,
                       const vtkSmartPointer<vtkNamedColors>& colors);

    // TODO: Remove ??? Refactor
    void DisplayActorsEx(const std::vector<vtkSmartPointer<vtkProp3D>>& actors,
                         const vtkSmartPointer<vtkNamedColors>& colors);


    Eigen::Vector3d GetCentroid(vtkSmartPointer<vtkPolyData> polyData);
    Eigen::Vector3d GetCentroid(vtkAlgorithmOutput* polyData);


    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkPolyData> rotatePolyData(const vtkSmartPointer<vtkPolyData> polyData,
                                                double x, double y, double z) noexcept;

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkPolyData> rotatePolyData(const vtkSmartPointer<vtkPolyData> polyData,
                                                const vtkSmartPointer<vtkMatrix4x4>& matrix) noexcept;

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkPolyData> moveTooth(const vtkSmartPointer<vtkPolyData>& polyData,
                                           double x, double y, double z);

    [[nodiscard("Do not discard the function return value. It's not free...")]]
    vtkSmartPointer<vtkPolyData> setPolyDataCenter(const vtkSmartPointer<vtkPolyData>& polyData,
                                                   double x, double y, double z);



    void visualize(vtkAlgorithmOutput* data);
    void visualize(vtkSmartPointer<vtkPolyData> data,
                   bool drawBoundingBox = false);

    void visualize(const std::vector<vtkSmartPointer<vtkPolyData>>& data,
                   bool drawBoundingBox = false);
    void visualize(const std::vector<vtkAlgorithmOutput*>& data);

    void separate_connected_components(const vtkSmartPointer<vtkPolyData>& dataPtr);
    void separate_connected_components_ex(const vtkSmartPointer<vtkPolyData>& dataPtr);
    void separate_connected_components_parallel_BAD(const vtkSmartPointer<vtkPolyData>& dataPtr);

    std::vector<vtkSmartPointer<vtkPolyData>>
    separate_connected_components_parallel_test(const vtkSmartPointer<vtkPolyData>& dataPtr);
}

#endif //!UTILITIES_INCLUDE_GUARD