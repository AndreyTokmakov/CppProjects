//============================================================================
// Name        : TypesEx.cpp
// Created on  : 26.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Points
//============================================================================

#include "TypesEx.h"

namespace TypesEx {

    template<size_t _Size = 2, typename Ty = int>
    class Point: public std::array<Ty, _Size> {
    private:

    public:
        friend std::ostream& operator<<(std::ostream& stream, const Point<_Size, Ty>& pt) {
            // TODO: Is there better way to iterate???? std::array<Ty, _Size>::_M_elems
            for (const Ty& v : pt._M_elems)
                stream << v << " ";
            return stream;
        }

        [[nodiscard]]
        friend Point<_Size, Ty> operator+(const Point<_Size, Ty>& pt1,
                                          const Point<_Size, Ty>& pt2)
        {
            Point<_Size, Ty> pt;
            for (size_t n = 0; n < _Size; ++n)
                pt[n] += pt1[n] + pt2[n];
            return pt;
        }

        [[nodiscard]]
        friend Point<_Size, Ty> operator-(const Point<_Size, Ty>& pt1,
                                          const Point<_Size, Ty>& pt2)
        {
            Point<_Size, Ty> pt;
            for (size_t n = 0; n < _Size; ++n)
                pt[n] += pt1[n] - pt2[n];
            return pt;
        }

        [[nodiscard]]
        friend Point<_Size, Ty> operator*(const Point<_Size, Ty>& pt, Ty v) {
            Point<_Size, Ty> point;
            for (size_t n = 0; n < _Size; ++n)
                point[n] = pt[n] * v;
            return point;
        }

        [[nodiscard]]
        friend Point<_Size, Ty> operator/(const Point<_Size, Ty>& pt, Ty v) {
            Point<_Size, Ty> point;
            for (size_t n = 0; n < _Size; ++n)
                point[n] = pt[n] / v;
            return point;
        }

        /*
        friend bool operator==(const Point<_Size, Ty>& pt1,
                               const Point<_Size, Ty>& pt2) {
            return pt1 == pt2;
        }

        friend bool operator!=(const Point<_Size, Ty>& pt1,
                               const Point<_Size, Ty>& pt2) {
            return !(pt1 == pt2);
        }
        */

    };

}

void TypesEx::Test() {
    std::cout << "TypesEx test\n";

    Point<3, double> pt1 {1,1,1};

    Point<3, double> pt2 = pt1 * 2;
    Point<3, double> pt3 {1,1,1};

    std::cout << pt1 << std::endl;
    std::cout << pt2 << std::endl;
    std::cout << (pt3 == pt1) << std::endl;

    Point<3, double> pt4 {1,2,3};
    for (size_t i = 0; i < pt4.size(); ++i)
        std::cout << pt4.data()[i] << " " <<  std::endl;
}
