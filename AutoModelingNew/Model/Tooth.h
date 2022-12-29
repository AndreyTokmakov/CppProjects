//============================================================================
// Name        : Tooth.h
// Created on  : 10.12.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Tooth
//============================================================================

#ifndef CPPPROJECTS_TOOTH_H
#define CPPPROJECTS_TOOTH_H

#include <vtkPolyData.h>
#include "FDI.h"
#include "ToothAxis.h"
// #include "../Estimators/Estimator.h"

namespace Model
{
    class Tooth final {
    public: // TODO: --> Private (its set public temporary now)
        /** Tooth VTK poly data smart ptr object: **/
        const vtkSmartPointer<vtkPolyData> toothPolyData { nullptr };

        /** Tooth FDI id number: **/
        const FDI::IdType toothId { 0 };

        /** Tooth axes: **/
        ToothAxis axes;

        /** Tooth mesiodistal width: **/
        // TODO: add Getter
        double mesiodistalWidth { 0.0f };

    public:
        Tooth(const vtkSmartPointer<vtkPolyData>& toothData, FDI::IdType id) noexcept;

        [[nodiscard("Make sure to handle return value")]]
        inline FDI::IdType getToothId() const noexcept {
            return toothId;
        }

        // TODO: Can we afford to make it noexcept ???
        ToothAxis& setAxis(const std::vector<double>& planAxes);

        // TODO: Performance ??? Return ref??
        [[nodiscard("Make sure to handle return value")]]
        inline ToothAxis getAxis() const noexcept {
            return axes;
        }

        inline void setMesiodistalWidth(double value) noexcept {
            mesiodistalWidth = value;
        }

        [[nodiscard("Make sure to handle return value")]]
        Eigen::Vector3d GetCenter() const noexcept;

        [[nodiscard("Make sure to handle return value")]]
        std::array<double, 6> GetBounds() const noexcept;

    private:

        [[nodiscard("Make sure to handle return value")]]
        ToothAxis prepareToothAxes(const std::vector<double>& planAxes) const noexcept;

        // GetBorders() --> std::array<double, 6> ?? Eigen::Array
        // Rotate?? with axis
        // Move ??
        // Update Axis when transforming tooth data

        // TODO: Add 'mesiodistalWidth' calc
        // TODO: Add Fissures

        // TODO: Add ToothType ---> enum class ToothType {}

        // TODO: Upper??? Lower?? Left ??? Right ???
    };
}

#endif //CPPPROJECTS_TOOTH_H
