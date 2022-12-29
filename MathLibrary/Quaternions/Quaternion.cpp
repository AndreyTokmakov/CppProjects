//============================================================================
// Name        : Quaternion.cpp
// Created on  : 08.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Quaternion src
//============================================================================

#include <iostream>
#include <memory>
#include <math.h>
#include <algorithm>
#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <string>
#include <mutex>
#include <atomic>
#include <numeric>
#include <iomanip>

#include "../Types/Types.h"
#include "Quaternion.h"

namespace {
    constexpr size_t FLOAT_NUM_PRECISION {12};
}

namespace Quaternions {

    struct Quaternion {
        /** Vector: **/
        float x;
        float y;
        float z; 

        /** Scalar: **/
        float w;

        inline float norm() const noexcept {
            return x*x + y*y + z*z + w*w;
        };
    };

    std::ostream& operator<<(std::ostream& stream,
                             const Quaternion& quant) {
        stream << std::setprecision(FLOAT_NUM_PRECISION) << quant.x << std::endl;
        stream << std::setprecision(FLOAT_NUM_PRECISION) << quant.y << std::endl;
        stream << std::setprecision(FLOAT_NUM_PRECISION) << quant.z << std::endl;
        stream << std::setprecision(FLOAT_NUM_PRECISION) << quant.w << std::endl;
        return stream;
    }
}

namespace Quaternions {

    // Converting spherical coordinates to a quaternion
    void SphericalToQuaternion(Quaternion * q, 
                               float latitude, 
                               float longitude, 
                               float angle)
    {
        float sin_a = sin( angle / 2 );
        float cos_a = cos( angle / 2 );

        float sin_lat = sin( latitude );
        float cos_lat = cos( latitude );

        float sin_long = sin( longitude );
        float cos_long = cos( longitude );

        q->x = sin_a * cos_lat * sin_long;
        q->y = sin_a * sin_lat;
        q->z = sin_a * sin_lat * cos_long;
        q->w = cos_a;
    }

    // Converting a matrix to a quaternion
    void MatrixToQuaternion(const float matrix[4][4],
                            Quaternion* quat)
    {
        float s, q[4];
        int   i, j, k;
        int nxt[3] = {1, 2, 0};
        float tr = matrix[0][0] + matrix[1][1] + matrix[2][2];

        if (tr > 0.0)
        {
            s = sqrt (tr + 1.0);
            quat->w = s / 2.0;
            s = 0.5 / s;
            quat->x = (matrix[1][2] - matrix[2][1]) * s;
            quat->y = (matrix[2][0] - matrix[0][2]) * s;
            quat->z = (matrix[0][1] - matrix[1][0]) * s;
        }
        else
        {
            i = 0;
            if (matrix[1][1] > matrix[0][0]) 
                i = 1;
            if (matrix[2][2] > matrix[i][i]) 
                i = 2;
                
            j = nxt[i];
            k = nxt[j];

            s = sqrt ((matrix[i][i] - (matrix[j][j] + matrix[k][k])) + 1.0);
            q[i] = s * 0.5;

            if (s != 0.0) 
                s = 0.5 / s;

            q[3] = (matrix[j][k] - matrix[k][j]) * s;
            q[j] = (matrix[i][j] + matrix[j][i]) * s;
            q[k] = (matrix[i][k] + matrix[k][i]) * s;

            quat->x = q[0];
            quat->y = q[1];
            quat->z = q[2];
            quat->w = q[3];
        }
    }

    template<size_t _Rows, size_t _Columns, typename _Type = int>
    void MatrixToQuaternion(const Types::Matrix<_Rows, _Columns, _Type>& matrix,
                            Quaternion* quat) 
    {
        float s, q[4];
        int   i, j, k;
        int nxt[3] = { 1, 2, 0 };
        float tr = matrix.data[0][0] + matrix.data[1][1] + matrix.data[2][2];

        if (tr > 0.0)
        {
            s = sqrt(tr + 1.0);
            quat->w = s / 2.0;
            s = 0.5 / s;
            quat->x = (matrix.data[1][2] - matrix.data[2][1]) * s;
            quat->y = (matrix.data[2][0] - matrix.data[0][2]) * s;
            quat->z = (matrix.data[0][1] - matrix.data[1][0]) * s;
        }
        else
        {
            i = 0;
            if (matrix.data[1][1] > matrix.data[0][0])
                i = 1;
            if (matrix.data[2][2] > matrix.data[i][i])
                i = 2;

            j = nxt[i];
            k = nxt[j];

            s = sqrt((matrix.data[i][i] - (matrix.data[j][j] + matrix.data[k][k])) + 1.0);
            q[i] = s * 0.5;

            if (s != 0.0)
                s = 0.5 / s;

            q[3] = (matrix.data[j][k] - matrix.data[k][j]) * s;
            q[j] = (matrix.data[i][j] + matrix.data[j][i]) * s;
            q[k] = (matrix.data[i][k] + matrix.data[k][i]) * s;

            quat->x = q[0];
            quat->y = q[1];
            quat->z = q[2];
            quat->w = q[3];
        }
    }

    // Converting a quaternion to a matrix
    void QuaternionToMatrix(const Quaternion* quat,
                            float matrix[4][4])
    {
        float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
        x2 = quat->x + quat->x;
        y2 = quat->y + quat->y;
        z2 = quat->z + quat->z;
        xx = quat->x * x2;   xy = quat->x * y2;   xz = quat->x * z2;
        yy = quat->y * y2;   yz = quat->y * z2;   zz = quat->z * z2;
        wx = quat->w * x2;   wy = quat->w * y2;   wz = quat->w * z2;

        matrix[0][0]=1.0f-(yy+zz); matrix[0][1]=xy-wz;        matrix[0][2]=xz+wy;
        matrix[1][0]=xy+wz;        matrix[1][1]=1.0f-(xx+zz); matrix[1][2]=yz-wx;
        matrix[2][0]=xz-wy;        matrix[2][1]=yz+wx;        matrix[2][2]=1.0f-(xx+yy);

        matrix[0][3] = matrix[1][3] = matrix[2][3] = 0;
        matrix[3][0] = matrix[3][1] = matrix[3][2] = 0;
        matrix[3][3] = 1;
    }


    Quaternion MuliplyQuaternions(const Quaternion *q1,
                                  const Quaternion *q2)
    {
        float A, B, C, D, E, F, G, H;

        A = (q1->w + q1->x) * (q2->w + q2->x);
        B = (q1->z - q1->y) * (q2->y - q2->z);
        C = (q1->x - q1->w) * (q2->y + q2->z);
        D = (q1->y + q1->z) * (q2->x - q2->w);
        E = (q1->x + q1->z) * (q2->x + q2->y);
        F = (q1->x - q1->z) * (q2->x - q2->y);
        G = (q1->w + q1->y) * (q2->w - q2->z);
        H = (q1->w - q1->y) * (q2->w + q2->z);

        Quaternion res {};
        res.w = B + (-E - F + G + H) * 0.5;
        res.x = A - ( E + F + G + H) * 0.5; 
        res.y =-C + ( E - F + G - H) * 0.5;
        res.z =-D + ( E - F - G + H) * 0.5;
        return res;
    }

    // One of the most useful properties of quaternions is to achieve smooth animation during interpolation.
    // Consider the interpolation between two quaternions that define the horizontal.
    // In this case, the interpolation occurs along the shortest arc. 
    // This can be achieved using spherical Linear interpolation (SLERP-Spherical Linear intERPolation):

    void Slerp(Quaternion* res,
               Quaternion* q,
               Quaternion* p,
               float t)
    {
        float p1[4];
        double omega, cosom, sinom, scale0, scale1;

        // cosine of the angle
        cosom = q->x*p->x + q->y*p->y + q->z*p->z + q->w*p->w;

        if (cosom < 0.0 ) { 
            cosom = -cosom;
            p1[0] = - p->x;  p1[1] = - p->y;
            p1[2] = - p->z;  p1[3] = - p->w;
        } else {
            p1[0] = p->x;    p1[1] = p->y;
            p1[2] = p->z;    p1[3] = p->w;
        }

        // TODO: hack value
        int DELTA = 0.6;
        if ((1.0 - cosom) > DELTA )  {// standard case (slerp)
            omega = acos(cosom);
            sinom = sin(omega);
            scale0 = sin((1.0 - t) * omega) / sinom;
            scale1 = sin(t * omega) / sinom;
        } else { // if the angle is small-linear interpolation
            scale0 = 1.0 - t;
            scale1 = t;
        }

        res->x = scale0 * q->x + scale1 * p1[0];
        res->y = scale0 * q->y + scale1 * p1[1];
        res->z = scale0 * q->z + scale1 * p1[2];
        res->w = scale0 * q->w + scale1 * p1[3];
    }
};


namespace Testing {
    using namespace Quaternions;

    // home/andtokm/Projects/data/cases/2878/Treatment plan_01_2021-07-15-15:43:00.json
    //    step_matrices --> Lower --> Last step for 31 tooth
    // OR matrices --> Lower --> Last step for 31 tooth
    // ----> To 'keyframes' -->  Last for 31 tooth

    using namespace Types;


    void Matrix_To_Quaternion() {
        float matrix [4][4] {};

        /*
        std::array<float, 16> values{
            0.9988237836840151,
            0.044008217428877487,
            -0.020354998056510224,
            0.0,
            -0.04536624999068998,
            0.9963775959122178,
            -0.07192767009970339,
            0.0,
            0.017115855483443206,
            0.07276649753095299,
            0.9972021281205471,
            0.0,
            0.48029344348659377,
            1.522392316398431,
            0.23803454003004186,
            1.0
        };
        */

        std::array<float, 16> values{
            0.99496885826,
            -0.000148690641046,
            -0.100184574579,
            0,
            -0.00123423571394,
            0.99990481716,
            -0.0137416622585,
            0,
            0.100177081984,
            0.0137961773878,
            0.99487397078,
            0,
            -1.97846341075,
            -0.265615416485,
            1.26876530847,
            1,
        };

        size_t index {0};
        for (int i = 0; i < 4; ++i) {
            for (int n = 0; n < 4; ++n) {
                matrix[i][n] = values[index++];
            }
        }

        Quaternion quat;
        MatrixToQuaternion(matrix, &quat);

        std::cout << quat << std::endl;
    }

    void Matrix_To_Quaternion2() {

        constexpr std::array<float, 16> rotationValues{
            1,  0,  1,  0,
            0,  1,  0,  0,
           -1,  0,  1,  0,
            0,  0,  0,  1
        };

        /*
        constexpr std::array<float, 16> values {
            0.9988237836840151,    0.044008217428877487, -0.020354998056510224, 0.0,
            -0.04536624999068998,  0.9963775959122178,   -0.07192767009970339,  0.0,
            0.017115855483443206,  0.07276649753095299,   0.9972021281205471,   0.0,
            0.48029344348659377,   1.522392316398431,     0.23803454003004186,  1.0
        };
        */

        constexpr std::array<float, 16> values{
            0.99496885826,     -0.000148690641046,  -0.100184574579,  0,
            -0.00123423571394,  0.99990481716,      -0.0137416622585, 0,
            0.100177081984,     0.0137961773878,     0.99487397078,   0,
            -1.97846341075,    -0.265615416485,      1.26876530847,   1
        };
  
        Matrix<4, 4, float> matrix;
        Matrix<4, 4, float> rotation;

        size_t index{ 0 };
        for (int i = 0; i < 4; ++i) {
            for (int n = 0; n < 4; ++n) {
                matrix.data[i][n] = values[index];
                rotation.data[i][n] = rotationValues[index];
                index++;
            }
        }

        const auto M = matrix * rotation;

        std::cout << matrix << std::endl;
        std::cout << rotation << std::endl;
        std::cout << M << std::endl;
  
        
        Quaternion quat;
        MatrixToQuaternion(M, &quat);

        std::cout << quat << std::endl; 
    }
}

void Quaternions::TestAll()
{

    // Testing::Matrix_To_Quaternion();
    Testing::Matrix_To_Quaternion2();
}