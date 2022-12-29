//============================================================================
// Name        : Jaw.h
// Created on  : 10.12.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Jaw
//============================================================================

#ifndef CPPPROJECTS_JAW_H
#define CPPPROJECTS_JAW_H

#include "Common.h"
#include <vector>
#include <unordered_map>

#include "Tooth.h"

namespace Model {
    class Jaw final {
    public:
        /** Upper/Lower jaw side: **/
        TeethType side { TeethType::Lower };

        /** Teeth belonging to this jaw: **/
        std::vector<Tooth> teeth;

        Jaw(const std::unordered_map<unsigned short, vtkSmartPointer<vtkPolyData>>& toothMap,
            TeethType jawSide);
    };
};

#endif //CPPPROJECTS_TOOTH_H