//============================================================================
// Name        : Utilities.cpp
// Created on  : 10.10.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utilities src
//============================================================================

#include "Utilities.h"


#define ASSERT(expr) \
	if (false == (expr)) \
		std::cout << "Error at " << __FUNCTION__ << "." << __LINE__ << std::endl;

#define ASSERT_NOT(expr) \
	if (true == (expr)) \
		std::cout << "Error at " << __FUNCTION__ << "." << __LINE__ << std::endl;


namespace Utilities::VectorUtilsTests
{
    template<typename T,
             template <typename> typename Collection>
    T calc_mean(const Collection<T>& collection) {
        T sum {0};
        for (const T& v: collection)
            sum += v;
        return sum / collection.size();
    }

    template<typename T,
            template <typename> typename Collection>
    T mult_and_sum(const Collection<T>& a, const Collection<T>& b) {
        T sum {0};
        for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
            sum += a[i] * b[i];
        }
        return sum;
    }


    //------------------------------------------------------------------------------

    void Mean()
    {
        std::vector<float> v {1, 2, 3, 4 ,5 ,6}, v1 {0.5, 12.4, -3.06, 8.434};

        ASSERT( calc_mean(v) == VectorUtilities::mean(v))
        ASSERT( calc_mean(v1) == VectorUtilities::mean(v1))
    }

    void MultAndSum()
    {
        std::vector<float> a1 {1, 12, 3, 34 ,5 ,6}, a2 {12, -12, 44, 33, 3, 3};
        std::vector<float> b1 {-1.4, 2.003, 4.54 ,61}, b2 {1.5, -2.4, 13.06, 18.434};
        std::vector<float> c1 {-1.4, 3.3, 4.54 ,61}, c2 {1.5, -12.4, 13.06, 18.434, 1};


        ASSERT( mult_and_sum(a1, a2) == VectorUtilities::MultAndSum(a1, a2))
        ASSERT( mult_and_sum(b1, b2) == VectorUtilities::MultAndSum(b1, b2))
        ASSERT( mult_and_sum(c1, c2) == VectorUtilities::MultAndSum(c1, c2))

    }
}


void Utilities::TestAll()
{
    VectorUtilsTests::Mean();
    VectorUtilsTests::MultAndSum();


    std::cout << "Done\n";
}