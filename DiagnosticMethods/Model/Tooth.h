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

namespace Model
{
    class Tooth final {
    public:
        /** Tooth VTK poly data smart ptr object: **/
        const vtkSmartPointer<vtkPolyData> toothPolyData { nullptr };

        /** Tooth FDI id number: **/
        const FDI::IdType toothId { 0 };

        /** Tooth axes: **/
        ToothAxis axes;

        /** Tooth mesiodistal width: **/
        double mesiodistalWidth { 0.0f };

    public:
        Tooth(const vtkSmartPointer<vtkPolyData>& toothData, FDI::IdType id) noexcept;

        [[nodiscard("Make sure to handle return value")]]
        inline FDI::IdType getToothId() const noexcept {
            return toothId;
        }

        // TODO: Can we afford to make it noexcept ???
        ToothAxis& setAxis(const std::vector<double>& planAxes);

    private:

        [[nodiscard("Make sure to handle return value")]]
        ToothAxis prepareToothAxes(const std::vector<double>& planAxes) const noexcept;

        // Rotate??
        // Move ??
        // Get center ??? (cache center)
        // Update Axis when transforming tooth data

        // TODO: Add 'mesiodistalWidth' calc
        // TODO: Add Fissures
    };

}

#endif //CPPPROJECTS_TOOTH_H
