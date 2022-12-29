
#include <iostream>
#include <memory>
#include <string>

#include <thread>
#include <future>

#include <mutex>
#include <atomic>

#include <cmath>
#include <numeric>
// #include <numbers>

#include <vector>
#include <algorithm>
#include <array>
#include <set>
// #include <span>

#include "LinearRegression.h"
#include "Types/Types.h"
#include "TypesEx/TypesEx.h"
#include "Quaternions/Quaternion.h"
#include "IntegralCalculations/IntegralCalculations.h"

#include <concepts>

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


/** Maths: **/
namespace Math {

	constexpr float __SQRT2 = 1.4142135623730950488016887242096980785696718753769f;
    constexpr float pi = M_PI;

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

	void PrintConstants() {

		std::cout << "Pi     = " << std::setprecision(16) << pi << std::endl;
		// std::cout << "Inv Pi = " << std::setprecision(16) << std::numbers::inv_pi << std::endl;
		// std::cout << "ln2    = " << std::setprecision(16) << std::numbers::ln2 << std::endl;
	}

	void Angle_To_Rarians() {
		constexpr float angle{90.0f};

		constexpr float radians = (pi / 180 )* angle;
		std::cout << angle << " = " << radians << std::endl;

	}
}



namespace Algoritms {

	void Accumulate_Object() {
		std::vector<Utilities::Integer> numbers;
		for (int i = 0; i < 10; ++i)
			numbers.emplace_back(i);

		/*
		for (const auto& v: numbers)
			std::cout << v.getValue() << std::endl;
		*/

		auto sum = [](const Utilities::Integer& a, const Utilities::Integer& b) -> decltype(auto) {
            return a.getValue() + b.getValue();
		};

		auto mean = std::accumulate(numbers.cbegin(), numbers.cend(), 0, sum) / numbers.size();
		std::cout << mean << std::endl;
	}


	void Fibonachi_Numbers(size_t N) {
		int a = 0, b = 1, c = 0;

		std::cout << a << "  " << b;
		for (size_t i = 0; i < N; ++i) {
			c = a + b;
			a = b;
			b = c;
			std::cout << "  " << c;
		}
		std::cout << std::endl;
	}
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
	// std::cout << "Tests\n";

	// Math::FuncTests();
	// Math::Test_Vector2D_Length();
	// Math::RotationAngle();

	// Math::PrintConstants();
	// Math::Angle_To_Rarians();


	// Algoritms::Accumulate_Object();
	// Algoritms::Fibonachi_Numbers(20);

	// Quaternions::TestAll();
	LinearRegression::TestAll();
	// Matrices::TestAll();
	// Types::Tests();
    // TypesEx::Test();
    // IntegralCalculations::LeftRectanglesMethodTests();
}

