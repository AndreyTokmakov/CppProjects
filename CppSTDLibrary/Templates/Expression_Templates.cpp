/**============================================================================
Name        : Expression_Templates.cpp
Created on  : 27.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Expression_Templates
============================================================================**/

#include "Templates.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

namespace
{
    // CRTP Base class for all vector expressions.
    // E is the actual derived type (e.g., Vector, or VecAdd)
    template <typename E>
    struct  VecExpression
    {
        // Safely downcast to the actual expression type
        const E& cast() const
        {
            return static_cast<const E&>(*this);
        }
    };

    // Proxy class representing the sum of two expressions
    template <typename LHS, typename RHS>
    class VecAdd : public VecExpression<VecAdd<LHS, RHS>>
    {
        const LHS& lhs;
        const RHS& rhs;

    public:
        VecAdd(const LHS& lhs, const RHS& rhs) : lhs { lhs }, rhs { rhs } {}

        // Evaluate a single index on the fly!
        double operator[](const size_t idx) const {
            return lhs[idx] + rhs[idx];
        }
    };

    // Operator+ returns our lightweight proxy object, NOT a calculated vector
    template <typename LHS, typename RHS>
    VecAdd<LHS, RHS> operator+(const VecExpression<LHS>& lhs, const VecExpression<RHS>& rhs) {
        return VecAdd<LHS, RHS>(lhs.cast(), rhs.cast());
    }

    class Vector : public VecExpression<Vector>
    {
        std::vector<double> data;

    public:
        explicit Vector(const size_t n, const double val = 0.0) : data(n, val) {}

        double operator[](const size_t i) const {
            return data[i];
        }

        double& operator[](const size_t i) {
            return data[i];
        }

        [[nodiscard]]
        size_t size() const {
            return data.size();
        }

        // The single point of evaluation!
        template <typename E>
        Vector& operator=(const VecExpression<E>& expr)
        {
            const E& real_expr = expr.cast();
            // A single loop evaluating the entire tree at once
            for (size_t i = 0; i < data.size(); ++i) {
                data[i] = real_expr[i];
            }
            return *this;
        }
    };
}

void Templates::Expression_Templates::TestAll()
{
    const Vector v1 {10, 1}, v2 {10, 2};

    const auto& res = v1 + v2;
    for (int idx = 0; idx < v1.size(); ++idx) {
        std::cout << res[idx] << ' ';
    }
}