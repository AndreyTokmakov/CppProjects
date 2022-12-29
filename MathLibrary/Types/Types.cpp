//============================================================================
// Name        : Types.cpp
// Created on  : 26.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Points
//============================================================================

#include "Types.h"

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


#define ASSERT(expr) \
	if (false == (expr)) \
		std::cout << "Error at " << __FUNCTION__ << "." << __LINE__ << std::endl;

#define ASSERT_NOT(expr) \
	if (true == (expr)) \
		std::cout << "Error at " << __FUNCTION__ << "." << __LINE__ << std::endl;

namespace Types 
{
	template<size_t _Size, typename _Type>
	_Type Dot(const Point<_Size, _Type>& pt1, const Point<_Size, _Type>& pt2) {
		_Type val{};
		for (size_t i = 0; i < _Size; ++i)
			val += pt1[i] * pt2[i];
		return val;
	}

}

namespace Types::Testing
{
    void Create() {
        {
            Point<2, float> pt {};
            ASSERT(pt[0] == 0); ASSERT(pt[1] == 0);
        }

        {
            Point<3, float> pt {};
            ASSERT(pt[0] == 0); ASSERT(pt[1] == 0); ASSERT(pt[2] == 0);
        }

        {
            Point<3, float> pt {1, 2, 3};
            ASSERT(pt[0] == 1); ASSERT(pt[1] == 2); ASSERT(pt[2] == 3);
        }
    }


	void CreateFromVector() {
		std::vector<float> v {1,2,3};
		Point<3, float> pt;

		pt.assign(v);
		
		ASSERT(pt[0] == 1);
		ASSERT(pt[1] == 2);
		ASSERT(pt[2] == 3);
	}

    void StructuredBinding() {
        {
            Point<3, float> pt {1, 2, 3};
            auto [x, y, z] = pt.asArray();
            ASSERT(x == 1); ASSERT(y == 2); ASSERT(z == 3);
        }

        {
            Point<3, float> pt {1, 2, 3};

            auto& [x, y, z] = pt.asArray();
            ASSERT(x == 1); ASSERT(y == 2); ASSERT(z == 3);

            x +=1; y += 1; z += 1;
            ASSERT(pt[0] == 2); ASSERT(pt[1] == 3); ASSERT(pt[2] == 4);
        }
    }

	void MultiplyTest () {
		Point<3, float> pt {1, 2, 3};
		pt = pt * 3;

		ASSERT(pt[0] == 3);
		ASSERT(pt[1] == 6);
		ASSERT(pt[2] == 9);
	}

	void DevideTest() {
		Point<3, float> pt{ 3,6,9 };
		pt = pt / 3;

		ASSERT(pt[0] == 1);
		ASSERT(pt[1] == 2);
		ASSERT(pt[2] == 3);
	}

	void EqualTest() {
		Point<3, float> pt1{ 3,6,9 };
		Point<3, float> pt2{ 3,6,9 };

		ASSERT(pt1 == pt2);
	}

	void NotEqualTest() {
		Point<3, float> pt1{ 3,3,9 };
		Point<3, float> pt2{ 3,6,9 };

		ASSERT(pt1 != pt2);
	}

	void AccesByIndex() {
		Point<3, float> pt1{ 3,3,9 };
		std::cout << pt1[0] << std::endl;
	}

	void AccesByIndexConst() {
		const Point<3, float> pt1{ 3,3,9 };
		std::cout << pt1[0] << std::endl;
	}

	void SumTest() {
		{
			Point<3, float> pt1{ 1,2,3 }, pt2{ 2,3,4 }, pt3{3,5,7};
			ASSERT((pt1 + pt2) == pt3);
		}
		{
			Point<3, float> pt1{ 1,2 }, pt2{ 2,3 }, pt3{ 3,5 };
			ASSERT((pt1 + pt2) == pt3);
		}
	}

	void MinusTest() {
		{
			Point<3, float> pt1{ 1,2,3 }, pt2{ 2,3,4 }, pt3{ 3,5,7 };
			ASSERT((pt3 - pt2) == pt1);
		}
		{
			Point<3, float> pt1{ 1,2 }, pt2{ 2,3 }, pt3{ 3,5 };
			ASSERT((pt3 - pt2) == pt1);
		}
	}

	void DistanceTest() {
		/*
		{
			Types::Point<3, float> pt1{ 1,1,1 };
			Types::Point<3, float> pt2{ 3,3,3 };

			auto x = pt1[0] - pt2[0];
			auto y = pt1[1] - pt2[1];
			auto z = pt1[2] - pt2[2];

			ASSERT(pt1.distanceTo(pt2) == std::hypot(x, y, z));
		}
		*/

		/*
		Types::Point<3, float> pt1{ 1,1,1 };
		Types::Point<3, float> pt2{ 3,3,3 };

		auto x = pt1[0] - pt2[0];
		auto y = pt1[1] - pt2[1];
		auto z = pt1[2] - pt2[2];

		std::cout << std::setprecision(12) <<  pt1.distanceTo(pt2) << std::endl;
		std::cout << std::setprecision(12) << std::hypot(x, y, z) << std::endl;
		*/
	}

	void DotTests() {
		{
			Point<3, float> pt1{ 1,1,1 }, pt2{ 3,3,3 };
			ASSERT(pt1.dot(pt2) == Dot(pt1, pt2));
		}
		{
			Point<3, float> pt1{ 1,6 }, pt2{ 3,6 };
			ASSERT(pt1.dot(pt2) == Dot(pt1, pt2));
		}
		{
			Point<3, float> pt1{ 1,6 }, pt2{ 3,6 }, pt3{ 4, 6 };
			ASSERT_NOT(pt1.dot(pt2) == Dot(pt1, pt3));
		}
	}

	void DotTestsStatic() {
		{
			Point<3, float> pt1{ 1,1,1 }, pt2{ 3,3,3 };
			ASSERT((Point<3, float>::dot(pt1, pt2) == Dot(pt1, pt2)));
		}
		{
			Types::Point<3, float> pt1{ 1,6 }, pt2{ 3,6 };
			ASSERT((Point<3, float>::dot(pt1, pt2) == Dot(pt1, pt2)));
		}
		{
			Types::Point<3, float> pt1{ 1,6 }, pt2{ 3,6 }, pt3{ 5,6 };
			ASSERT_NOT((Point<3, float>::dot(pt1, pt2) == Dot(pt1, pt3)));
		}
	}

	void MidpointTestsStatic() 
	{
		struct TestData2d {
			Types::Point<2, float> pt1;
			Types::Point<2, float> pt2;
			Types::Point<2, float> mipoint;
		};

		struct TestData3d {
			Types::Point<3, float> pt1;
			Types::Point<3, float> pt2;
			Types::Point<3, float> mipoint;
		};

		{
			const std::vector<TestData2d> data{
				{{0.0, 2.0}, {8.0, 8.0}, {4.0, 5.0}},
				{{3.0, 2.0}, {5.0, 9.0}, {4.0, 5.5}},
				{{-33.0, 23.0}, {54.0,94.0}, {10.5, 58.5}},
				{{-2.0, 233.0}, {-54.0, 123.0}, {-28, 178}},
			};

			for (const auto& pts : data) {
				ASSERT((Point<2, float>::midPoint(pts.pt1, pts.pt2) == pts.mipoint));
			}
		}

		{
			const std::vector<TestData3d> data{
				{{0.0, 2.0, 0}, {8.0, 8.0, 0}, {4.0, 5.0, 0}},
				{{3.0, 2.0}, {5.0, 9.0}, {4.0, 5.5}},
				{{-33.0, 23.0}, {54.0,94.0}, {10.5, 58.5}},
				{{-2.0, 233.0}, {-54.0, 123.0}, {-28, 178}},
			};

			for (const auto& pts : data) {
				ASSERT((Point<3, float>::midPoint(pts.pt1, pts.pt2) == pts.mipoint));
			}
		}
	}
}

namespace Types::LineTesting
{
    void DotTests() {
        {
            const Point<2, double> pt1 {1, 3}, pt2{14, -1};
            const Line<2> line {pt1, pt2};

            ASSERT(pt1.dot(pt2) == line.dot());
        }
        {
            const Point<3, double> pt1 {1, 3, -3}, pt2{14, -1, 22};
            const Line<3> line {pt1, pt2};

            ASSERT(pt1.dot(pt2) == line.dot());
        }
    }

    //--------------------------------------------------------------------------------------------------

    void Calc2DLineEquation(const Line<2>& line) {
        // Formula: (x - x1) / (x2 - x1) = (y - y1) / (y2 - y1)
        const auto& pt1 = line.getFirstPoint();
        const auto& pt2 = line.getSecondPoint();
        const auto a = pt2[1] - pt1[1];
        const auto b = pt2[0] - pt1[0];
        const auto c = pt2[0] * pt1[1] - pt1[0] * pt2[1];
        std::cout << a << " * X  + (" << c << ") = " << b << " * y" << std::endl;
    }

    void Calc2DLineEquation2(const Line<2>& line) {
        const auto& pt1 = line.getFirstPoint();
        const auto& pt2 = line.getSecondPoint();

        // angular coefficient - tilt to the XS-axis
        const double slope = (pt2[1] - pt1[1]) / (pt2[0] - pt1[0]);
        // straight line offset - the segment cut off from the axis
        const double intercept = (pt2[0] * pt1[1] - pt1[0] * pt2[1])/(pt2[0] - pt1[0] );

        std::cout << "y = " << slope << "* x + " << intercept  << std::endl;
    }

    void Calc2DLineEquationTest() {
        const Point<2, double> pt1{ 1.0, 5.0 }, pt2{ 6.0, 3.0 };
        Calc2DLineEquation({pt1, pt2});
        Calc2DLineEquation2({pt1, pt2});
    }

    //--------------------------------------------------------------------------------------------------

	double getDistance2D(const Line<2>& line, const Point<2, double>& pt) {
		const auto& pt1 = line.getFirstPoint();
		const auto& pt2 = line.getSecondPoint();
		const double doubleArea = std::abs((pt2[1] - pt1[1]) * pt[0] -
			(pt2[0] - pt1[0]) * pt[1] +
			pt2[0] * pt1[1] - pt2[1] * pt1[0]);
		return  doubleArea / line.getLength();
	}

	// https://www.mathportal.org/calculators/analytic-geometry/line-point-distance.php
	double getDistance2D_2(const Line<2>& line, const Point<2, double>& pt) {
		const auto& pt1 = line.getFirstPoint();
		const auto& pt2 = line.getSecondPoint();
		const auto a = pt2[1] - pt1[1];
		const auto b = pt2[0] - pt1[0];
		const auto c = pt2[0] * pt1[1] - pt1[0] * pt2[1];
		return std::abs(a * pt[0] - b * pt[1] + c) / std::sqrt(a * a + b * b);
	}

	void DistanceFromPoint_Test_2D()
	{
		const Point<2, double> pt0{ 4.0, 8.0 };
		const Point<2, double> pt1{ 1.0, 2.0 }, pt2{ 2.0, 6.0 };


		const double dist1 = getDistance2D({ pt1, pt2 }, { pt0 });
		const double dist2 = getDistance2D_2({ pt1, pt2 }, { pt0 });

		std::cout << "Distance actual   = " << dist1 << std::endl;
		std::cout << "Distance actual   = " << dist2 << std::endl;
		std::cout << "Distance expected = " << (double)6 / 17 * std::sqrt(17) << std::endl;

		Calc2DLineEquation({ pt1, pt2 });
	}

	//=================================================================================


	void Calc3DLineEquation(const Line<3>& line) {
		const auto& pt1 = line.getFirstPoint();
		const auto& pt2 = line.getSecondPoint();
		const auto a = pt2[1] - pt1[1];
		const auto b = pt2[0] - pt1[0];
		const auto c = pt2[0] * pt1[1] - pt1[0] * pt2[1];
		std::cout << a << " * X  + (" << c << ") = " << b << " * y" << std::endl;
	}

	// Add src with current pre-calculated values
	void DistanceFromPoint_Test_3D()
	{
		const Point<3, double> A { 0.0, 4.0, 6.0 };
		const Point<3, double> B{ -6.0, 0.0, -2.0 }, C{ 2.0, 0.0, -2.0 };

		// Calc3DLineEquation({pt1, pt2});

		const Point<3, double> d = (C - B) / C.distanceTo(B);
		const Point<3, double> v = A - B;
		double t = Dot(v, d);
		const Point<3, double> P = B + d * t;

		std::cout << P << std::endl;
		std::cout << "Distance = " << P.distanceTo(A) << std::endl;
	}
}

namespace Types::MatricesTests 
{
	void CreateTest() {
		Matrix<2, 2> M {
			1, 2,
			3, 4
		};
		std::cout << M << std::endl;
	}

	void CreateTest1() {
		Matrix<4, 2> M {
			1, 2, 3, 4,
			5, 6, 7, 8
		};
		std::cout << M << std::endl;
	}

	void Multipy_3x3_Test()
	{
		Matrix<3, 3> a{
			1,  4,  3,
			2,  1,  5,
			3,  2,  1
		};

		Matrix<3, 3> b{
			5,  2,  1,
			4,  3,  2,
			2,  1,  5
		};

		/*
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
		*/


		std::cout << a << std::endl;
		std::cout << b << std::endl;

		auto c = a * b;
		std::cout << c << std::endl;
	}
}


void Types::Tests()
{
	std::cout << "Running src" << std::endl;
#if 1
    Testing::Create();
	Testing::CreateFromVector();
	Testing::StructuredBinding();
	Testing::DevideTest();
	Testing::MultiplyTest();
	Testing::EqualTest();
	Testing::NotEqualTest();
	Testing::DistanceTest();
	Testing::SumTest();
	Testing::MinusTest();
	Testing::DotTests();
	Testing::DotTestsStatic();
	Testing::MidpointTestsStatic();
#endif

    // LineTesting::DotTests();

    // LineTesting::Calc2DLineEquationTest();

	// LineTesting::DistanceFromPoint_Test_2D();
	// LineTesting::DistanceFromPoint_Test_3D();

	// MatricesTests::CreateTest();
	// MatricesTests::CreateTest1();


    std::cout << "All src passed\n";
}
