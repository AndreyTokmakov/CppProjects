//============================================================================
// Name        : Points.h
// Created on  : 26.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Points
//============================================================================

#ifndef POINTS_TESTS__H_
#define POINTS_TESTS__H_

#include <cmath>
#include <array>
#include <cassert>
#include <vector>
#include <iostream>
#include <iomanip>
#include <initializer_list>

/*********************************************************************************************************/
/*                                               POINT                                                   */
/*********************************************************************************************************/

namespace Types
{
    // TODO: Refactor this approach
    enum class Coordinate: uint8_t {
        X = 0,
        Y = 1,
        Z = 2
    };

    template<size_t _Size = 2, typename Ty = int>
    class Point {
    private:
        using value_type = Ty;
        using value_type_reference = value_type&;
        using const_value_type [[maybe_unused]] = const value_type;

        static_assert(!std::is_same_v<value_type, void>,
                      "The type of the coordinates in the Point can not be void");
        static_assert(0 != _Size, "Number of coordinates should not be equal zero");

    private:
        std::array<value_type, _Size> coordinates{};

    public:
        Point() = default;

        Point(std::initializer_list<value_type> list) {
            static_assert(0 != _Size, "Number of coordinates should not be equal zero");
            for (size_t pos{ 0 }; const auto & v : list)
            coordinates[pos++] = v;
        }

        Point(const std::vector<value_type>& vec) {
            for (size_t pos{ 0 }; auto & v : coordinates)
            v = vec[pos++];
        }

        // TODO: Constexpr constructor and assign operators
        /*
        template<typename ... Args>
        Point(Args&& ... params): coordinates { std::forward<Args>(params) ...} {
        }

        Point(const Point<_Size, Ty>& pt): coordinates {pt.coordinates} {
        }

        Point<_Size, Ty>& operator=(const Point<_Size, Ty>& pt) {
            if (this == &pt)
                return *this;
            this->coordinates = pt.coordinates;
        }
        */

    public:
        friend std::ostream& operator<<(std::ostream& stream, const Point<_Size, Ty>& pt) {
            for (const Ty& v : pt.coordinates)
                stream << v << " ";
            return stream;
        }

        [[nodiscard]]
        friend Point<_Size, Ty> operator+(const Point<_Size, Ty>& pt1,
                                          const Point<_Size, Ty>& pt2)
        {
            Point<_Size, Ty> pt;
            for (size_t n = 0; n < _Size; ++n)
                pt.coordinates[n] += (pt1.coordinates[n] + pt2.coordinates[n]);
            return pt;
        }

        [[nodiscard]]
        friend Point<_Size, Ty> operator-(const Point<_Size, Ty>& pt1,
                                          const Point<_Size, Ty>& pt2)
        {
            Point<_Size, Ty> pt;
            for (size_t n = 0; n < _Size; ++n)
                pt.coordinates[n] += (pt1.coordinates[n] - pt2.coordinates[n]);
            return pt;
        }

        [[nodiscard]]
        friend Point<_Size, Ty> operator*(const Point<_Size, Ty>& pt, int v) {
            Point<_Size, Ty> point;
            for (size_t n = 0; n < _Size; ++n)
                point.coordinates[n] = (pt.coordinates[n] * v);
            return point;
        }

        [[nodiscard]]
        friend Point<_Size, Ty> operator/(const Point<_Size, Ty>& pt, int v) {
            Point<_Size, Ty> point;
            for (size_t n = 0; n < _Size; ++n)
                point.coordinates[n] = (pt.coordinates[n] / v);
            return point;
        }

        friend bool operator==(const Point<_Size, Ty>& pt1,
                               const Point<_Size, Ty>& pt2) {
            return pt1.coordinates == pt2.coordinates;
        }

        friend bool operator!=(const Point<_Size, Ty>& pt1,
                               const Point<_Size, Ty>& pt2) {
            return !(pt1 == pt2);
        }

        [[nodiscard]]
        double distanceTo(const Point<_Size, Ty>& pt) const noexcept {
            double sqrDist{ 0 };
            for (size_t i{ 0 }; const auto & coord: coordinates)
            sqrDist += std::pow(coord - pt.coordinates[i++], 2);
            return std::sqrt(sqrDist);
        }

        [[nodiscard]]
        auto data() const noexcept -> decltype(auto) {
            return coordinates.data();
        }

        [[nodiscard]]
        auto data() noexcept -> decltype(auto) {
            return coordinates.data();
        }

        void assign(const std::vector<value_type>& vec) {
            for (size_t pos{ 0 }; auto & v : coordinates)
            v = vec[pos++];
        }

        [[nodiscard]]
        value_type_reference operator[](size_t index) noexcept {
            // TODO: Assert size match
            return coordinates[index];
        }

        [[nodiscard]]
        value_type_reference operator[](Coordinate coordinate) noexcept {
            // TODO: Assert size match
            return coordinates[static_cast<uint8_t>(coordinate)];
        }

        [[nodiscard]]
        value_type operator[](size_t index) const noexcept {
            // TODO: Assert size match
            return coordinates[index];
        }

        // TODO: Remove from the Point class?????
        // Given two linearly independent vectors a and b, the cross product, a � b, is a vector
        // that is perpendicular to both a and b,[2] and thus normal to the plane containing them.
        [[nodiscard]]
        friend Point<_Size, Ty> cross(const Point<_Size, Ty>& pt1,
                                      const Point<_Size, Ty>& pt2)
        {
            static_assert(3 == _Size, "Points cross product availableWorkers only for points with _Size == 3");
            Point<_Size, Ty> pt{
                    pt1.coordinates[1] * pt2.coordinates[2] - pt1.coordinates[2] * pt2.coordinates[1],
                    pt1.coordinates[2] * pt2.coordinates[0] - pt1.coordinates[0] * pt2.coordinates[2],
                    pt1.coordinates[0] * pt2.coordinates[1] - pt1.coordinates[1] * pt2.coordinates[0]
            };
            return pt;
        }

        [[nodiscard]]
        value_type dot(const Point<_Size, Ty>& pt) const noexcept {
            value_type dot { 0 };
            for (size_t i{ 0 }; const auto & coord: coordinates)
                dot += coord * pt.coordinates[i++];
            return dot;
        }

        [[nodiscard]]
        static value_type dot(const Point<_Size, Ty>& pt1,
                              const Point<_Size, Ty>& pt2) noexcept {
            value_type val{};
            for (size_t i = 0; i < _Size; ++i)
                val += pt1[i] * pt2[i];
            return val;
        }

        [[nodiscard]]
        static Point<_Size, Ty> midPoint(const Point<_Size, Ty>& pt1,
                                         const Point<_Size, Ty>& pt2) noexcept {
            Point<_Size, Ty> ptMiddle;
            for (size_t i = 0; i < _Size; ++i)
                ptMiddle[i] = (pt1[i] + pt2[i]) / 2;
            return ptMiddle;
        }


        // TODO: Add operator >
        // TODO: Add operator <
    };
}

/*********************************************************************************************************/
/*                                               LINE                                                   */
/*********************************************************************************************************/

namespace Types {

    template<size_t _Dimensions>
    class Line {
    private:
        using value_type = double;
        using point_type = Point<_Dimensions, value_type>;
        using point_type_reference = point_type&;
        using const_type_reference = const point_type&;
        using const_point_type [[maybe_unused]] = const point_type;

        static_assert(0 != _Dimensions, "Number of dimensions should not be equal zero");
        // TODO: Add concepts ??

    private:
        point_type pt1;
        point_type pt2;

    public:
        Line(const Point<_Dimensions, value_type>& p1,
             const Point<_Dimensions, value_type>& p2) : pt1{ p1 }, pt2{ p2 } {
        }

    public:
        [[nodiscard]]
        inline const_type_reference getFirstPoint() const noexcept {
            return pt1;
        }

        [[nodiscard]]
        inline const_type_reference getSecondPoint() const noexcept {
            return pt2;
        }

        inline value_type getLength() const noexcept {
            return pt1.distanceTo(pt2);
        }

        [[nodiscard]]
        value_type dot() const noexcept {
            value_type val{};
            for (size_t i = 0; i < _Dimensions; ++i)
                val += pt1[i] * pt2[i];
            return val;
        }

        double getDistanceFromPoint(const Point<_Dimensions, value_type>& pt) {
            std::cout << pt1 << std::endl;
            std::cout << pt2 << std::endl;
            std::cout << pt << std::endl;
            const value_type double_area = std::abs((pt2[1] - pt1[1]) * pt[0] -
                                                    (pt2[0] - pt1[0]) * pt[1] +
                                                    pt2[0] * pt1[1] -
                                                    pt2[1] * pt1[0]);
            std::cout << double_area << std::endl;
            // const double line_segment_length = sqrt((x2*x2 - x1*x1) + (y2*y2 - y1*y1));
            return 0;
        }

        // TODO: add calc equation of the line
        // TODO: Inersection of lines
        // TODO: Is intersection?
        // TODO: The algorithm to find the point of intersection of two 3D line segment
        // TODO: Distance from a point to a line - 3-Dimensional
        // TODO: Angle between two lines https://onlinemschool.com/math/assistance/cartesian_coordinate/line_angle/
        // TODO: Perf src
    };
}

/*********************************************************************************************************/
/*                                              MATRICES                                                 */
/*********************************************************************************************************/

namespace Types {

    template<size_t N = 2, size_t M = 2, typename Ty = int>
    class Matrix {
    private:
        using value_type = Ty;
        using const_value_type = const value_type;
        // using size_type = decltype(data)::size_type;

        static_assert(!std::is_same_v<value_type, void>,
                      "Type of the Objects in the Matrix can not be void");
        static_assert(0 != N, "Nubmer of rows should not be equal zero");
        static_assert(0 != M, "Nubmer of rows should not be equal zero");

        static inline constexpr size_t rowsCount { N };
        static inline constexpr size_t columnsCount { M };

    private:
        template<size_t _Rows, size_t _Columns, typename _Type>
        friend std::ostream& operator<<(std::ostream& stream,
                                        const Matrix<_Rows, _Columns, _Type>& matrix);

        template<size_t _Rows1, size_t _Columns1, size_t _Rows2, size_t _Columns2, typename _Type>
        friend Matrix<_Rows1, _Columns2, _Type>  operator*(const Matrix<_Rows1, _Columns1, _Type>& m1,
                                                           const Matrix<_Rows2, _Columns2, _Type>& m2);

    public:
        using Column = std::array<value_type, rowsCount>;
        std::array<Column, columnsCount> data{};


    public:
        Matrix() = default;

        /*
        Matrix(std::initializer_list<value_type> list) {
            const value_type* iter = list.begin();
            for (size_t i = 0; i < rowsCount; ++i) {
                for (size_t n = 0; n < columnsCount; ++n) {
                    data[n][i] = *iter;
                    ++iter;;
                }
            }
        }
        */

        constexpr Matrix(const std::initializer_list<value_type> list)
        {

            const value_type* iter = list.begin();
            for (size_t n = 0; n < rowsCount; ++n) {
                for (size_t m = 0; m < columnsCount; ++m) {
                    //  std::cout << *iter << std::endl;
                    data[m][n] = *iter;
                    ++iter;;
                }
            }



            /*
            for (size_t n = 0, m = 0; const value_type & val: list) {
            }*/
        }
    };

    template<size_t _Rows, size_t _Columns, typename _Type>
    std::ostream& operator<<(std::ostream& stream, const Matrix<_Rows, _Columns, _Type>& matrix) {
        for (size_t n = 0; n < matrix.rowsCount; ++n) {
            for (size_t m = 0; m < matrix.columnsCount; ++m) {
                stream << std::setprecision(16) << matrix.data[m][n] << "      ";
            }
            stream << std::endl;
        }
        return stream;
    }


    template<size_t _Rows1, size_t _Columns1, size_t _Rows2, size_t _Columns2, typename _Type>
    Matrix<_Rows1, _Columns2, _Type> operator*(const Matrix<_Rows1, _Columns1, _Type>& m1,
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

}



////////////////////////////////////////////////////////////////////////////////////////

namespace Types {
    void Tests();
}

#endif /* POINTS_TESTS__H_ */
