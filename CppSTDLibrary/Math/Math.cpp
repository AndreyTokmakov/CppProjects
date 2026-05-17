//============================================================================
// Name        : Math.cpp
// Created on  : 20.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Math src
//============================================================================

#include "Math.h"
#include <cmath>

#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <iomanip>
#include <climits>
#include <limits>
#include <numeric>


namespace Math {

	void Min() {
		std::cout << "min(1, 3) = " << std::min(1, 3) << std::endl;
		std::cout << "min(1, -3) = " << std::min(1, -3) << std::endl;
	}

	void Max() {
		std::cout << "max(1, 3) = " << std::max(1, 3) << std::endl;
		std::cout << "max(1, -3) = " << std::max(1, -3) << std::endl;
	}

	void Pow() {
		std::cout << "pow(2, 1) = " << std::pow(2, 1) << std::endl;
		std::cout << "pow(2, 2) = " << std::pow(2, 2) << std::endl;
		std::cout << "pow(2, 3) = " << std::pow(2, 3) << std::endl;
		std::cout << "pow(3, 2) = " << std::pow(3, 2) << std::endl;
		std::cout << "pow(3, 3) = " << std::pow(3, 3) << std::endl;
		std::cout << "pow(1.5, 52) = " << std::pow(1.5, 52) << std::endl;
		std::cout << "pow(256, 2) = " << std::pow(256, 2) << std::endl;
	}

	void Sqrt() {
		std::cout << "sqrt(16) = " << std::sqrt(16) << std::endl;
		std::cout << "sqrt(27) = " << std::sqrt(27) << std::endl;
		std::cout << "sqrt(25) = " << std::sqrt(25) << std::endl;
	}

	void Cbrt() {
		std::cout << "cbrt(27) = " << std::cbrt(27) << std::endl;
		std::cout << "cbrt(8) = " << std::cbrt(8) << std::endl;
		std::cout << "cbrt(125) = " << std::cbrt(125) << std::endl;
	}

	void Hypot() {
		std::cout << "hypot(3,4) = " << std::hypot(3, 4) << std::endl;
	}
}

namespace Math::Exponential {

	//  It will calculate the exponential e raised to power p.
	void Exp() {
		std::cout << "exp(5) = " << std::exp(5) << std::endl;
	}

	//  It will calculate the base 2 exponential of p.
	void Exp2()
	{
		std::cout << "exp2(8) = " << exp2(8) << std::endl;
	}

	//  log(p): It will calculate the logarithm of p.
	void Log()
	{
		std::cout << "log(8)= " << log(8) << std::endl;
	}

	//  log2(p): It will calculate the base 2 logarithm of p.
	void Log2()
	{
		std::cout << "log2(8)= " << log2(8) << std::endl;
		std::cout << "log2(32)= " << log2(32) << std::endl;
		std::cout << "log2(128)= " << log2(128) << std::endl;
	}	

	//   It will calculate the common logarithm of p.
	void Log10()
	{
		std::cout << "log10(8) = " << log10(8) << std::endl;
		std::cout << "log10(100) = " << log10(100) << std::endl;
		std::cout << "log10(1000) = " << log10(1000) << std::endl;
	}
}
namespace Math::IntegerFunctions {

	//  it rounds up the value of z.
	void Ceil() {
		std::cout << "ceil(2.3) = " << ceil(2.3) << std::endl;
		std::cout << "ceil(3.8) = " << ceil(3.8) << std::endl;
		std::cout << "ceil(-2.3)= " << ceil(-2.3) << std::endl;
		std::cout << "ceil(-3.8) = " << ceil(-3.8) << std::endl;
	}

	// floor(z): it rounds down the value of z.
	void Floor() {
		std::cout << "floor(2.3) = " << floor(2.3) << std::endl;
		std::cout << "floor(3.8) = " << floor(3.8) << std::endl;
		std::cout << "floor(-2.3)= " << floor(-2.3) << std::endl;
		std::cout << "floor(-3.8) = " << floor(-3.8) << std::endl;
	}

	// fmod(z,y): It calculates the remainder of division z/y.
	void Fmod()
	{
		std::cout << "fmod(5.3, 2) = " << fmod(5.3, 2) << std::endl;
		std::cout << "fmod(18.5, 4.2) = " << fmod(18.5, 4.2) << std::endl;
		std::cout << "fmod(9, 4) = " << fmod(9, 4) << std::endl;
		std::cout << "fmod(9.0, 4.4) = " << fmod(9.0, 4.4) << std::endl;
	}

	// trunc(z): It will round off the z value towards zero.
	void Trunk() {
		std::cout << "trunc(5.4) = " << trunc(5.3) << std::endl;
		std::cout << "trunc(-1.3) = " << trunc(-1.3) << std::endl;
	}

	// Rounds x to an integral value, using the rounding direction specified by fegetround.
	void Nearbyint() {
		std::cout << "nearbyint(2.3) = " << nearbyint(2.3) << std::endl;
		std::cout << "nearbyint(2.6) = " << nearbyint(2.6) << std::endl;
		std::cout << "nearbyint(-2.3) = " << nearbyint(-2.3) << std::endl;
		std::cout << "nearbyint(-3.8) = " << nearbyint(-3.8) << std::endl;
	}

	// Returns the floating-point remainder of numer/denom (rounded to nearest):
	void Remainder() {
		std::cout << "remainder(5.3, 2) = " << remainder(5.3, 2) << std::endl;
		std::cout << "remainder(18.5, 4.2) = " << remainder(18.5, 4.2) << std::endl;
	}
}

/** Maths: **/
namespace Math::Experimets {

    constexpr float __SQRT2 = 1.4142135623730950488016887242096980785696718753769;

    void FuncTests() {
        double a = -1.2345f;
        std::cout << a << " = " << std::fabs(a) << std::endl;
    }


    template<typename T >
    inline T _sqr(const T &x) {
        return (x*x);
    }

    double _vector2D_length(const double x, const double y)
    {
        if (x && y) {
            double w, h;
            w = fabs(x);
            h = fabs(y);
            if (w != h) {
                if (w < h)
                    return h * sqrt(1 + _sqr(w / h));
                return w * sqrt(1 + _sqr(h / w));
            }
            return w * __SQRT2;
        }
        if (x)
            return fabs(x);
        return fabs(y);
    }

    void Test_Vector2D_Length() {
        auto x = _vector2D_length(1, 4);
        std::cout << x << std::endl;
    }


    void RotationAngle() {
        constexpr int rotationCount {50};

        for (int i = 1; i <= rotationCount; ++i) {
            auto angle = 360 / rotationCount * i;
            std::cout << angle << std::endl;
        }
    }
}

namespace Math::Types {

    constexpr size_t FLOAT_NUM_PRECISION {12};

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

namespace Math::Types {

    template<size_t N = 2, size_t M = 2, typename Ty = int>
    class Matrix {
    private:
        using value_type = Ty;
        using const_value_type = const value_type;
        // using pointer = object_type*;
        // using size_type = std::vector<pointer>::size_type;

        static_assert(!std::is_same_v<value_type, void>,
                      "Type of the Objects in the Matrix can not be void");
        static_assert(0 != N, "Nubmer of rows should not be equal zero");
        static_assert(0 != M, "Nubmer of rows should not be equal zero");

        static inline constexpr size_t rowsCount {N};
        static inline constexpr size_t columnsCount {M};

    private:
        template<size_t _Rows, size_t _Columns, typename _Type>
        friend std::ostream& operator<<(std::ostream& stream, const Matrix<_Rows, _Columns, _Type>& matrix);

        template<size_t _Rows1, size_t _Columns1, size_t _Rows2, size_t _Columns2, typename _Type>
        friend Matrix<_Rows1, _Columns2, _Type>  operator*(const Matrix<_Rows1, _Columns1, _Type>& m1,
                                                           const Matrix<_Rows2, _Columns2, _Type>& m2);

    public:
        std::array<std::array<value_type, M>, N> data {};



    };

    // ------------------------ Operations--------------------------------------------------------

    template<size_t _Rows, size_t _Columns, typename _Type = int>
    std::ostream& operator<<(std::ostream& stream, const Matrix<_Rows, _Columns, _Type>& matrix) {
        for (size_t n = 0; n < matrix.rowsCount; ++n) {
            for (size_t m = 0; m < matrix.columnsCount; ++m) {
                stream << matrix.data[m][n] << "      ";
            }
            stream << std::endl;
        }
        return stream;
    }


    template<size_t _Rows1, size_t _Columns1, size_t _Rows2, size_t _Columns2, typename _Type>
    Matrix<_Rows1, _Columns2, _Type>  operator*(const Matrix<_Rows1, _Columns1, _Type>& m1,
                                                const Matrix<_Rows2, _Columns2, _Type>& m2)
    {
        Matrix<_Rows1, _Columns2, _Type> matrix;
        for (size_t n = 0; n < m1.rowsCount; ++n) {
            for (size_t m = 0; m < m2.columnsCount; ++m) {
                for (size_t i = 0; i < m2.rowsCount; ++i) {
                    matrix.data[m][n] += (m1.data[i][n] * m2.data[m][i]);
                }
            }
        }
        return matrix;
    }

    void CreateTest_Matrix() {
        Matrix<2, 2> l;
        Matrix<2, 2> m;

        l.data[0][0] = 18;
        l.data[0][1] = 12;
        l.data[1][0] = 11;
        l.data[1][1] = 10;
        m.data[0][0] = 12;
        m.data[0][1] = 13;
        m.data[1][0] = 17;
        m.data[1][1] = 14;

        std::cout << m << std::endl;
        std::cout << std::endl;
        std::cout << l << std::endl;

        const Matrix<2, 2> x = m * l;
        std::cout << x << std::endl;
    }

    void Multipy_3x3_Test()
    {
        Matrix<3, 3> a;
        Matrix<3, 3> b;

        a.data[0][0] = 1;
        a.data[1][0] = 4;
        a.data[2][0] = 3;

        a.data[0][1] = 2;
        a.data[1][1] = 1;
        a.data[2][1] = 5;

        a.data[0][2] = 3;
        a.data[1][2] = 2;
        a.data[2][2] = 1;


        b.data[0][0] = 5;
        b.data[1][0] = 2;
        b.data[2][0] = 1;

        b.data[0][1] = 4;
        b.data[1][1] = 3;
        b.data[2][1] = 2;

        b.data[0][2] = 2;
        b.data[1][2] = 1;
        b.data[2][2] = 5;

        std::cout << a << std::endl;
        std::cout << b << std::endl;

        auto c = a * b;
        std::cout << c << std::endl;
    }
}

namespace Math::Types {

    void TEST_Matrix_To_Quaternion()
    {
        float matrix [4][4] {};

        /*
        std::array<float, 16> values {
                0.9988237836840151,
                0.044008217428877484,
                -0.020354998056510223,
                0.0,
                -0.04536624999068998,
                0.9963775959122178,
                -0.07192767009970338,
                0.0,
                0.017115855483443205,
                0.07276649753095299,
                0.9972021281205471,
                0.0,
                0.48029344348659375,
                1.522392316398431,
                0.23803454003004185,
                1.0
        };
        */

        // Expected:
        // -0.0362079404294
        // 0.00937662180513
        // 0.0223648641258
        // 0.999049961567

        std::array<float, 16> values {
                0.988979134838,
                -0.00356203132481,
                0.148012103516,
                0,
                0.00372102092014,
                0.999992759151,
                -0.000797276133737,
                0,
                -0.14800819186,
                0.00133924559459,
                0.988985228183,
                0,
                -0.32854455356,
                -0.0818552025501,
                3.91523461864,
                1
        };


        size_t index {0};
        for (int i = 0; i < 4; ++i) {
            for (int n = 0; n < 4; ++n) {
                matrix[n][i] = values[index++];
            }
        }

        Quaternion quat;
        MatrixToQuaternion(matrix, &quat);

        std::cout << quat << std::endl;
    }
}


namespace Math
{
    void Saturated_Operations()
    {
#if 0
        constexpr int a = std::add_sat(3, 4); /// NO saturation occurs, T = int
        static_assert(a == 7);

        constexpr uint8_t b = std::add_sat<uint8_t>(UCHAR_MAX, 4); /// saturated
        static_assert(b == std::numeric_limits<uint8_t>::max());

        constexpr uint8_t c = std::add_sat(UCHAR_MAX, 4); /// NOT saturated, T = int
        // add_sat(int, int) returns int tmp == 259, then assignment truncates 259 % 256 == 3
        static_assert(c == 3);

        //  unsigned char d = std::add_sat(252, c); // Error: inconsistent deductions for T

        constexpr uint8_t e = std::add_sat<uint8_t>(251, a); /// saturated
        static_assert(e == std::numeric_limits<uint8_t>::max());
        // 251 is of type T = unsigned char, `a` is converted to unsigned char value;
        // might yield an int -> unsigned char conversion warning for `a`

        constexpr int8_t f = std::add_sat<int8_t>(-123, -3);  /// NOT saturated  -->  g == -126
        constexpr int8_t g = std::add_sat<int8_t>(-123, -13); /// saturated      -->  g == -128 instead of -136

        static_assert(f == -126);
        static_assert(g == std::numeric_limits<int8_t>::min());
#endif
    }
}


void Math::TestAll()
{
	// Min();
	// Max();

	// Pow();
	
	// Sqrt();
	// Cbrt();

	// Hypot(); // hypotenuse of triangle

	// Exponential::Exp();
	// Exponential::Exp2();
	// Exponential::Log();
	// Exponential::Log2();
	// Exponential::Log10();


	// IntegerFunctions::Ceil();
	// IntegerFunctions::Floor();
	// IntegerFunctions::Fmod();
	// IntegerFunctions::Trunk();
	// IntegerFunctions::Nearbyint();
	// IntegerFunctions::Remainder();

    Saturated_Operations();


    // Experimets::FuncTests();
    // Experimets::Test_Vector2D_Length();
    // Experimets::RotationAngle();


    // Quaternions src
    /*
    Types::MuliplyQuaternions();
    Types::SphericalToQuaternion();
    Types::MatrixToQuaternion();
    Types::QuaternionToMatrix();
    Types::MuliplyQuaternions();
    */

    // Types::CreateTest_Matrix();
    // Types::Multipy_3x3_Test();
    // Types::TEST_Matrix_To_Quaternion();
}
