//============================================================================
// Name        : TreatmentPlan.h
// Created on  : 18.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Automodeling Treatment Plan
//============================================================================

#ifndef AUTOMODELING_TREATMENT_PLAN__INCLUDE_GUARD
#define AUTOMODELING_TREATMENT_PLAN__INCLUDE_GUARD

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <map>

#include "../Types.h"

// TODO: 1. Add std::optional as wrappers for entities that could be missing
// TODO: 2. Check std::arrays<T, N> size when assigning values
// TODO: 3. Add 'getMissingTeeth()' method
// TODO: 4. Add points transfomation -> another orientation


namespace TreatmentPlan {
    using namespace Types;

    using Point3d = Types::Point<3, double>;
    using Axes = std::vector<double>;
    using Origin = Point3d;
    using Quaternion = std::array<double, 4>;

    struct ToothModellingData {
        Axes axes;
        Origin origin;
        Quaternion quaternion;
        Point3d anchor_point_1;
        Point3d anchor_point_2;
        Point3d anchor_point_3;
        Point3d anchor_point_local_1;
        Point3d anchor_point_local_2;
        Point3d anchor_point_local_3;
    };

    struct ToothFrame {
        std::vector<Quaternion> quaternions;
        Point3d translation;
        size_t quaternion_index;
    };

    struct Keyframe {
        // TODO: Refactor --> std::map??
        std::map<uint16_t, ToothFrame> toothFrames;
    };


    struct ModellingData {
        // TODO: Refactor --> std::map??
        std::map<uint16_t, ToothModellingData> tooth;
    };

    struct Keyframes {
        std::vector<Keyframe> lower;
        std::vector<Keyframe> upper;
    };

    /** Treatment plan: **/
    struct Plan {
        /** Contains setup modeling data: **/
        ModellingData modellingData;

        /** Contains teeth movement keyframes list: **/
        Keyframes keyframes;

    public:
        [[nodiscard]]
        std::map<uint16_t, Point3d> getPointsFinal() const noexcept;
    };
};

namespace TreatmentPlan::Parser {

    [[nodiscard]]
    TreatmentPlan::Plan Parse(std::string_view filePath);
}

/** Tests **/
namespace TreatmentPlan_Tests {
    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vect);

    template<typename _Ty, size_t _Size>
    std::ostream& operator<<(std::ostream& stream, const std::array<_Ty, _Size>& data);

    void TestAll();
}

/** UnitTests **/
namespace TreatmentPlan_UnitTests
{
    using namespace TreatmentPlan;
    
    void TestAll();
}

#endif // !AUTOMODELING_TREATMENT_PLAN__INCLUDE_GUARD