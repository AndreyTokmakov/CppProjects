/**============================================================================
Name        : LazyComputation.cpp
Created on  : 25.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : LazyComputation.cpp
============================================================================**/

#include "LazyComputation.h"

#include <iostream>
#include <vector>


namespace LazyComputation::DemoOne
{
    template<typename T, typename Collection, typename Derived>
    struct View
    {
        using value_type = T;
        using size_type  = size_t;
        using index_type = size_t;

        const Collection& left;
        const Collection& right;

        [[nodiscard]]
        inline size_type Size() const noexcept {
            return left.size();
        }

        [[nodiscard]]
        value_type operator[](const index_type idx) {
            return static_cast<Derived&>(*this).operation(idx, left, right);
        }
    };

    template<typename T, typename Collection = std::vector<T>>
    struct Add
    {
        [[nodiscard]]
        T operation(size_t idx, const Collection& l, const Collection& r) const {
            return l[idx] + r[idx];
        }
    };

    template<typename T, typename Collection = std::vector<T>>
    struct Mult
    {
        [[nodiscard]]
        T operation(size_t idx, const Collection& l, const Collection& r) const {
            return l[idx] * r[idx];
        }
    };

    template<typename T, typename Collection = std::vector<T>>
    struct AddView: Add<T, Collection>, View<T, Collection, AddView<T,Collection>>
    {
        AddView(const Collection& l, const Collection& r): View<T, Collection, AddView<T, Collection>> { l, r } {
        }
    };

    template<typename T, typename Collection = std::vector<T>>
    struct MultView: Mult<T, Collection>, View<T, Collection, MultView<T,Collection>>
    {
        MultView(const Collection& l, const Collection& r): View<T, Collection, MultView<T, Collection>> { l, r } {
        }
    };

    template<typename T>
    AddView<T> operator+(const std::vector<T>& a, const std::vector<T>& b) {
        return AddView<T> { a, b };
    }

    template<typename T>
    MultView<T> operator*(const std::vector<T>& a, const std::vector<T>& b) {
        return MultView<T> { a, b };
    }


    void test()
    {
        std::vector<int> a {1,2,3,4,5}, b {2,3,4,5,6};
        AddView<int> sum = a + b;
        MultView<int> prod = a * b;

        for (size_t size = sum.Size(), idx = 0; idx < size; ++idx) {
            std::cout << sum[idx] << " ";
        }
        std::cout << std::endl;
        for (size_t size = prod.Size(), idx = 0; idx < size; ++idx) {
            std::cout << prod[idx] << " ";
        }
    }
}

void LazyComputation::TestAll()
{
    DemoOne::test();
}