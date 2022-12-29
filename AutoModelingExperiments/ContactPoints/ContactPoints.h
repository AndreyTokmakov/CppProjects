//============================================================================
// Name        : ContactPoints.h
// Created on  : 30.09.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ContactPoints features
//============================================================================

#ifndef AUTOMODELINGEXPERIMENTS_CONTACTPOINTS_H
#define AUTOMODELINGEXPERIMENTS_CONTACTPOINTS_H

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <map>

#include "../Types.h"

namespace ContactPoints {
    using Point3d = Types::Point<3, double>;

    struct ToothContactPoints {
        Point3d pt1;
        Point3d pt2;
    };

    struct Points {
        std::map<int16_t, ToothContactPoints> teethContactPoints;

        std::vector<Point3d> getUpperPoints() const noexcept;
        std::vector<Point3d> getLowerPoints() const noexcept;
    };

    // TODO: check for RNVO ???? Performance reasons
    Points Parse(std::string_view filePath);
}

namespace ContactPoints
{
    void TestAll([[maybe_unused]] const std::vector<std::string_view>& params);
}

#endif //AUTOMODELINGEXPERIMENTS_CONTACTPOINTS_H
