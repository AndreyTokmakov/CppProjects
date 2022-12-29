//============================================================================
// Name        : Quaternion.h
// Created on  : 11.01.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Quaternion
//============================================================================

#include "Quaternion.h"


#include <iostream>
#include <string>
#include <string_view>
#include <memory>


#include <iomanip>

#include <Core>
#include <Geometry>

namespace Quaternions {

    void Create_Quaternion() {
        // Eigen::Quaternion<double> q {1,2,3,4};
        Eigen::Quaternion<double> q {0, 0, 0, 0} ;

        q.x() = 0.1;
        q.y() = 0.2;
        q.z() = 0.3;
        q.w() = 0.4;

        // q = AngleAxis<float>(angle_in_radian, axis);
        std::cout << q << std::endl;
    }

    void Create_Quaternion2() {
        Eigen::Quaternion<double> q ;

        q.coeffs() = {1,2,3, 4};

        std::cout << q << std::endl;
    }

    void Quaternion_From_Matrix() {
        Eigen::Matrix3f matrix;

        matrix << 0.9988237836840151, 0.044008217428877484, -0.020354998056510223,
                -0.04536624999068998, 0.9963775959122178,   -0.07192767009970338,
                0.017115855483443205, 0.07276649753095299,  0.9972021281205471;

        Eigen::Quaternion<float> quaternion(matrix);

        std::cout << std::setprecision(12) << quaternion << std::endl;

        // Expected:
        // -0.0362079404294
        // 0.00937662180513
        // 0.0223648641258
        // 0.999049961567
    }
}
void Quaternion::TestAll()
{
    // Quaternions::Create_Quaternion();
    // Quaternions::Create_Quaternion2();
    Quaternions::Quaternion_From_Matrix();
};
