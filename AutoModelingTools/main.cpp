//============================================================================
// Name        : main.cpp
// Created on  : 15.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Automodeling C++ project experiments
//============================================================================

#include <iostream>
#include <memory>
#include <cmath>
#include <string>
#include <string_view>
#include <algorithm>

#include <thread>
#include <atomic>
#include <future>
#include <mutex>

#include <vector>
#include <array>
#include <set>
#include <numeric>
// #include <numbers>

#include "TreatmentPlan/TreatmentPlan.h"
#include "Types.h"
using namespace std::string_view_literals;


namespace {
	constexpr float sqrt2 = 1.4142135623730951454746218587388284504413604736328125;
}

namespace Utilities {

	constexpr bool DEBUG_PRINT {false};

	class Integer
	{
	private:
		int value {0};

	public:
		Integer(int v): value{v} {
			if constexpr (DEBUG_PRINT) {
				std::cout << "Integer(" << value << ")\n";
			}
		}

		Integer(const Integer& v): value{v.value} {
			if constexpr (DEBUG_PRINT) {
				std::cout << "Copy Integer(" << value << ")\n";
			}
		}

		~Integer() {
			if constexpr (DEBUG_PRINT) {
				std::cout << "~Integer(" << value << ")\n";
			}
		}	

		inline int getValue() const noexcept {
			return value;
		}


		Integer& operator==(const Integer& rhs) = delete;
		Integer& operator==(Integer&& rhs)  = delete;
	};
};


namespace Points {

	template<typename _Ty = double>
	struct Point2D {
		static_assert(!std::is_same_v<_Ty, void>,
                      "Type of coordinate can not be void");
		_Ty x {};
		_Ty y {};
	};
	

	template<typename _Ty = double>
	struct Point3D {
		static_assert(!std::is_same_v<_Ty, void>,
                      "Type of coordinate can not be void");
		_Ty x {};
		_Ty y {};
		_Ty z {};
	};

	//-----------------------------------------------------------------------------------------

	template<typename T >
	decltype(auto) sqr(const T x) {
		return std::pow(x, 2);
	}

	double _vector2D_length(const double x, const double y) {
		if (x && y) {
			// register double w, h;
			double w, h;
			w = fabs(x);
			h = fabs(y);
			if (w != h) {
				if (w < h)
					return h * sqrt(1 + sqr(w / h));
				return w * sqrt(1 + sqr(h / w));
			}
			return w * sqrt2;
		}
		if (x)
			return fabs(x);
		return fabs(y);
	}


	double length2D_XZ(const Point3D<double>& pt1, const Point3D<double>& pt2) {
		return _vector2D_length(pt2.x - pt1.x, pt2.z - pt1.z);
	}

	
	double length2D_XZ_2(const Point3D<double>& pt1, const Point3D<double>& pt2) {
		return std::hypot(pt2.x - pt1.x, pt2.z - pt1.z);
	}

	//=============================================================================================================//

	namespace Testing {

		void All_Tests() {
			// { Point-1 {X, Y, Z}, Point-2 {X, Y, Z}, LENGTH-EXPECTED }
			std::array<std::array<double, 7>, 16> testData {{
				{ -19.0749,  1.14287,    5.75369, -16.7655,   -0.678855,  12.4429, 7.07665 },
				{ -15.7657, -0.266473,  12.7281,  -10.0271,   -0.192312,  16.9147, 7.10354 },
				{ -10.5242, -0.906974,  17.737,    -4.99553,  -0.0219134, 19.238,  5.72879 },
				{ -4.96248,  0.0446403, 19.4445,   -0.289867, -0.510286,  20.1713, 4.7288  },
				{ -0.34467, -0.0814817, 19.2486,    4.66129,   0.606222,  19.3072, 5.0063  },
				{  4.73713,  0.739806,  19.0706,   10.0871,   -1.04015,   17.7358, 5.51394 },
				{  9.50685, -0.787424,  16.7118,   14.8711,   -0.111947,  12.1024, 7.07256 },
				{  15.3204,  0.0588215, 11.6055,   18.529,     1.27103,   5.44327, 6.94752 },
				{ -21.3752,  7.71654,    4.30993, -18.799,     7.52939,  11.1609,  7.31927 }, ////
				{ -20.3723,  8.27497,   11.9696,  -14.8938,    7.62997,  18.6534,  8.64217 }, ////
				{ -14.8773,  7.698,     18.7512,   -9.25861,   6.54667,  22.2049,  6.5953  }, ////
				{ -9.07854,  6.11164,   22.2515,   -0.474387,  5.95301,  26.0734,  9.41478 }, ////
				{ -0.278815, 5.09195,   25.3367,    8.83882,   6.69449,  22.9539,  9.42384 },
				{  8.87411,  6.58714,   22.4238,   14.2301,    7.18754,  18.5451,  6.61292 },
				{  14.4562,  7.47612,   18.6798,   18.4899,    7.74482,  11.3226,  8.3904  },
				{  17.9049,  7.63652,   11.0159,   20.2422,    7.58368,  4.18088,  7.22358 }
			}};

			using Point3DD = Point3D<double>;
			using Points   = std::pair<Point3DD, Point3DD>;

			auto getTestData = [](const std::array<double, 7>& data) -> std::pair<Points, double> {
				const Points pts {{ data[0], data[1], data[2]}, { data[3],  data[4] ,  data[5]}};
				return {pts, data[6]};
			};

			for (const auto& data: testData) {
				const std::pair<Points, double> ptsData = getTestData(data);
				std::cout << length2D_XZ(ptsData.first.first, ptsData.first.second)   << " = " << ptsData.second << "\n"
						  << length2D_XZ_2(ptsData.first.first, ptsData.first.second) << " = " << ptsData.second << std::endl<< std::endl;
			}
		}
	}
};

namespace Teeths {

	static constexpr size_t MAX_TEETH_COUNT { 32 };
	static constexpr size_t MAX_TEETH_NUM_PER_JAW_SIDE { MAX_TEETH_COUNT / 2 };


	// TODO: Check performance Sorted vs Unsorted
	// Teeth of the upper jaw according to the FDI notation
	static constexpr std::array<size_t, MAX_TEETH_NUM_PER_JAW_SIDE> upperTooths {
		18,  17,  16,  15,  14,  13,  12,  11,
		21,  22,  23,  24,  25,  26,  27,  28
	};

	// TODO: Check performance Sorted vs Unsorted
	// Teeth of the lower jaw according to the FDI notation
	static constexpr std::array<size_t, MAX_TEETH_NUM_PER_JAW_SIDE> lowerTooths {
		48,  47,  46,  45,  44,  43,  42,  41,
		31,  32,  33,  34,  35,  36,  37,  38
	};

	static constexpr std::array<std::string_view, MAX_TEETH_COUNT> toothsIds {
		"18"sv,  "17"sv,  "16"sv,  "15"sv, "14"sv,  "13"sv,  "12"sv,  "11"sv,
		"21"sv,  "22"sv,  "23"sv,  "24"sv, "25"sv,  "26"sv,  "27"sv,  "28"sv,

		"48"sv,  "47"sv,  "46"sv,  "45"sv, "44"sv,  "43"sv,  "42"sv,  "41"sv,
		"31"sv,  "32"sv,  "33"sv,  "34"sv, "35"sv,  "36"sv,  "37"sv,  "38"sv
	};


	static constexpr size_t MAX_LOWER_JAW_TOOTH = *std::max_element(lowerTooths.begin(), lowerTooths.end());
	static constexpr size_t MIN_LOWER_JAW_TOOTH = *std::min_element(lowerTooths.begin(), lowerTooths.end());

	static constexpr size_t MAX_UPPER_JAW_TOOTH = *std::max_element(upperTooths.begin(), upperTooths.end());
	static constexpr size_t MIN_UPPER_JAW_TOOTH = *std::min_element(upperTooths.begin(), upperTooths.end());


	bool isLowerTooth([[maybe_unused]] uint_fast16_t id) {
		return id >= MIN_LOWER_JAW_TOOTH && id <= MAX_LOWER_JAW_TOOTH && (20 != id && 19 != id);
	}

	bool isUpperTooth([[maybe_unused]] uint_fast16_t id) {
		return id >= MIN_UPPER_JAW_TOOTH && id <= MAX_UPPER_JAW_TOOTH && (40 != id && 39 != id);
	}
}


int main([[maybe_unused]] int argc,
		 [[maybe_unused]] char** argv)
{
	const std::vector<std::string_view> params = [&] {
		std::vector<std::string_view> params;
		for (int i = 1; i < argc; ++i)
			params.emplace_back(argv[i]);
		return params;
	}();

	/*
	Points::Point3D<double> pt1 {-19.0749, 1.14287, 5.75369};
	Points::Point3D<double> pt2 {-16.7655, -0.678855, 12.4429};

	double len = length2D_XZ(pt1, pt2);
	std::cout << len << std::endl;

	len = length2D_XZ_2(pt1, pt2);
	std::cout << len << std::endl;
	*/

    // Points::Testing::All_Tests();
    Types::Tests();
	// Teeths::isLowerTooth(23);

    // TreatmentPlan_Tests::TestAll();
    // TreatmentPlan_UnitTests::TestAll();

	return EXIT_SUCCESS;
}