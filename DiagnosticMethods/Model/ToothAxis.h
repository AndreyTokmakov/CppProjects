//============================================================================
// Name        : Model.h
// Created on  : 10.12.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Model
//============================================================================

#ifndef CPPPROJECTS_TOOTHAXIS_H
#define CPPPROJECTS_TOOTHAXIS_H

#include <Core>
#include <Dense>

namespace Model {

    // NOTE: The axes and their values are set based on the assumption that the origin
    //       point from where they originate is located at coordinates (0, 0, 0) in space
    struct ToothAxis {
        // The horizontal (X) axis of the tooth.
        // Rotation around this axis is called tooth 'TORK'
        Eigen::Vector3d horizontal {Eigen::Vector3d::Zero () };

        // The frontal (z) axis of the tooth. (front <-> back axis)
        // Rotation around this axis is called tooth 'ANGULATION'
        Eigen::Vector3d angulation {Eigen::Vector3d::Zero () };

        // The vertical (Y) axis of the tooth.
        // Rotation around this axis is called tooth 'ROTATION'
        Eigen::Vector3d vertical {Eigen::Vector3d::Zero () };

        void rotate(double x, double y, double z);

        // TODO: Add multiply operation ???
        //       - Use Eigen::Matrix3d ???
        // TODO: Add Quaternion cast operation
    };
};

#endif //CPPPROJECTS_TOOTHAXIS_H
