//============================================================================
// Name        : PointsAndLines.h
// Created on  : 31.01.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : PointsAndLines
//============================================================================

#include "PointsAndLines.h"

#include <iostream>
#include <string>
#include <array>
#include <string_view>
#include <ostream>
#include <cmath>
#include <numeric>
#include <numbers>

#define CONST_GETTER(name) \
        [[nodiscard]] \
        constexpr inline value_type name() const noexcept { \
            return _##name; \
        } \

#define REF_GETTER(name) \
        [[nodiscard]] \
        constexpr inline value_type& name() noexcept { \
            return _##name; \
        } \

namespace PointsAndLines
{

    class Point2D {
    private:
        using value_type = double;

        value_type _x {0};
        value_type _y {0};

    public:
        constexpr Point2D() = default;
        constexpr Point2D(value_type x, value_type y): _x {x}, _y {y} {
        }

        /*
        Point2D(const Point2D& pt): _x { pt._x }, _y { pt._y } {
        }
        Point2D& operator=(const Point2D& pt) = default;
        */

        CONST_GETTER(x);
        CONST_GETTER(y);

        REF_GETTER(x);
        REF_GETTER(y);

        [[nodiscard]]
        friend constexpr Point2D operator+(const Point2D& pt1,
                                           const Point2D& pt2) noexcept {
            return Point2D {pt1._x + pt2._x, pt1._y + pt2._y};
        }

        [[nodiscard]]
        friend constexpr Point2D operator-(const Point2D& pt1,
                                           const Point2D& pt2) noexcept {
            return Point2D {pt1._x - pt2._x, pt1._y - pt2._y};
        }

        constexpr Point2D& operator+=(const Point2D& pt) noexcept {
            this->_x += pt._x;
            this->_y += pt._y;
            return *this;
        }

        constexpr Point2D& operator-=(const Point2D& pt) noexcept {
            this->_x -= pt._x;
            this->_y -= pt._y;
            return *this;
        }

        [[nodiscard]]
        friend constexpr Point2D operator*(const Point2D& pt1, value_type v) noexcept {
            return Point2D {pt1._x * v, pt1._y * v };
        }

        [[nodiscard]]
        friend constexpr Point2D operator/(const Point2D& pt1, value_type v) noexcept {
            return Point2D {pt1._x / v, pt1._y / v};
        }

        constexpr Point2D& operator*=(value_type v) noexcept {
            this->_x *= v;
            this->_y *= v;
            return *this;
        }

        constexpr Point2D& operator/=(value_type v) noexcept {
            this->_x /= v;
            this->_y /= v;
            return *this;
        }

        // INFO: Default comparisons
        auto constexpr operator<=>(const Point2D&) const noexcept = default;

        [[nodiscard]]
        constexpr value_type distanceTo(const Point2D& pt) const noexcept {
            return std::hypot(_x - pt._x, _y - pt._y);
        }

        friend std::ostream& operator<<(std::ostream& stream,
                                        const Point2D& pt) {
            stream << "[" << pt._x << ", " << pt._y << "]";
            return stream;
        }
    };

    class Point3D {
    private:
        using value_type = double;

        value_type _x { 0 };
        value_type _y { 0 };
        value_type _z { 0 };

    public:
        constexpr Point3D() = default;

        constexpr Point3D(value_type x, value_type y, value_type z):
                _x { x }, _y { y }, _z { z } {
        }

        CONST_GETTER(x);
        CONST_GETTER(y);
        CONST_GETTER(z);

        REF_GETTER(x);
        REF_GETTER(y);
        REF_GETTER(z);

        [[nodiscard]]
        friend constexpr Point3D operator+(const Point3D& pt1,
                                           const Point3D& pt2) noexcept {
            return Point3D {pt1._x + pt2._x, pt1._y + pt2._y, pt1._z + pt2._z};
        }

        [[nodiscard]]
        friend constexpr Point3D operator-(const Point3D& pt1,
                                           const Point3D& pt2) noexcept {
            return Point3D {pt1._x - pt2._x, pt1._y - pt2._y, pt1._z - pt2._z};
        }

        constexpr Point3D& operator+=(const Point3D& pt) noexcept {
            this->_x += pt._x;
            this->_y += pt._y;
            this->_z += pt._z;
            return *this;
        }

        constexpr Point3D& operator-=(const Point3D& pt) noexcept {
            this->_x -= pt._x;
            this->_y -= pt._y;
            this->_z -= pt._z;
            return *this;
        }

        [[nodiscard]]
        friend constexpr Point3D operator*(const Point3D& pt1, value_type v) noexcept {
            return Point3D {pt1._x * v, pt1._y * v, pt1._z * v };
        }

        [[nodiscard]]
        friend constexpr Point3D operator/(const Point3D& pt1, value_type v) noexcept {
            return Point3D {pt1._x / v, pt1._y / v, pt1._z / v};
        }

        constexpr Point3D& operator*=(value_type v) noexcept {
            this->_x *= v;
            this->_y *= v;
            this->_z *= v;
            return *this;
        }

        constexpr Point3D& operator/=(value_type v) noexcept {
            this->_x /= v;
            this->_y /= v;
            this->_z /= v;
            return *this;
        }

        // INFO: Default comparisons
        auto constexpr operator<=>(const Point3D&) const noexcept = default;

        [[nodiscard]]
        constexpr value_type distanceTo(const Point3D& pt) const noexcept {
            // return std::hypot(_x - pt._x, _y - pt._y, _z - pt._z);
            return std::sqrt((_x - pt._x) * (_x - pt._x) + (_y - pt._y) * (_y - pt._y) + (_z - pt._z) * (_z - pt._z));
        }

        friend std::ostream& operator<<(std::ostream& stream,
                                        const Point3D& pt) {
            stream << "[" << pt._x << ", " << pt._y << ", " << pt._z << "]";
            return stream;
        }
    };


    // INFO:
    class Line2D {
    private:
        Point2D pt1 {0, 0};
        Point2D pt2 {0, 0};

        using value_type = double;

    public:
        constexpr Line2D() noexcept = default;
        constexpr Line2D(Point2D p1, Point2D p2) noexcept: pt1 {p1}, pt2 { p2 } {
        }

        [[nodiscard]]
        constexpr inline value_type getLength() const noexcept {
            return pt1.distanceTo(pt2);
        }

        [[nodiscard]]
        constexpr Point2D getMidPoint() const noexcept {
            return Point2D {(pt1.x() + pt2.x()) / 2, (pt1.y() + pt2.y()) / 2};
        }

        [[nodiscard]]
        inline Point2D& getPoint1() noexcept {
            return pt1;
        }

        [[nodiscard]]
        inline Point2D& getPoint2() noexcept {
            return pt2;
        }

        [[nodiscard]]
        inline const Point2D& getPoint1() const noexcept {
            return pt1;
        }

        [[nodiscard]]
        inline const Point2D& getPoint2() const noexcept {
            return pt2;
        }

        void setCenter(const Point2D& newCenter) noexcept {
            const Point2D& step = newCenter - getMidPoint();
            pt1 += step;
            pt2 += step;
        }

        friend std::ostream& operator<<(std::ostream& stream,
                                        const Line2D& line) {
            stream << "Line2D[(" << line.pt1.x() << ", " << line.pt1.y() << "), ("
                   << line.pt2.x() << ", " << line.pt2.y() << ")]";
            return stream;
        }

    public:
        // TODO: Move to Utilities ?????
        [[nodiscard]]
        constexpr double degToRad(const double angle) noexcept {
            return angle * std::numbers::pi / 180;
        };

        void rotateAroundPoint(const Point2D& center,
                               const double angle) noexcept {
            const double rad { degToRad(angle) };
            const double sin { std::sin(rad) }, cos { std::cos(rad) };
            const Point2D p1 { pt1 }, p2 { pt2 };

            this->pt1 = Point2D(center.x() + cos * (p1.x() - center.x()) - sin * (p1.y() - center.y()),
                                center.y() + sin * (p1.x() - center.x()) + cos * (p1.y() - center.y()));
            this->pt2 = Point2D(center.x() + cos * (p2.x() - center.x()) - sin * (p2.y() - center.y()),
                                center.y() + sin * (p2.x() - center.x()) + cos * (p2.y() - center.y()));
        }

        void modifyLength(double diff) noexcept {
            const auto [slope, intercept] = Line2D::getLineCoefficients(pt1, pt2);
            const double lengthOriginal = getLength(),
                         xCenter = std::midpoint(pt1.x() , pt2.x()),
                         ratio = lengthOriginal / (lengthOriginal + diff),
                         xLenNew = std::abs(pt1.x() - pt2.x()) / ratio;

            pt1.x() = xCenter - xLenNew / 2.0;
            pt2.x() = xCenter + xLenNew / 2.0;
            pt1.y() = slope * pt1.x() + intercept;
            pt2.y() = slope * pt2.x() + intercept;
        }

        [[nodiscard]]
        static constexpr std::pair<double, double> getLineCoefficients(const Line2D& line) noexcept {
            const double slope = (line.pt2.y() - line.pt1.y()) / (line.pt2.x() - line.pt1.x());
            const double intercept = (line.pt2.x()* line.pt1.y() - line.pt1.x() * line.pt2.y()) /
                    (line.pt2.x() - line.pt1.x());
            return std::make_pair(slope, intercept);
        };

        [[nodiscard]]
        static constexpr std::pair<double, double> getLineCoefficients(const Point2D& pt1,
                                                                       const Point2D& pt2) noexcept {
            double slope = (pt2.y() - pt1.y()) / (pt2.x() - pt1.x());
            double intercept = (pt2.x() * pt1.y() - pt1.x() * pt2.y()) / (pt2.x() - pt1.x());
            return std::make_pair(slope, intercept);
        };
    };
}

namespace PointsAndLines::Tests
{

    void Getters_Tests() {
        // Point3D pt1 {1.0, 2.3, 1}, pt2 {-1.2, 1.2, 22};
        constexpr Point2D pt1 {1.0, 2.3};

        static_assert(pt1.x() == 1.0);
        static_assert(pt1.y() == 2.3);

        constexpr Point3D pt3 {1.0, 2.3, 1.22};

        static_assert(pt3.x() == 1.0);
        static_assert(pt3.y() == 2.3);
        static_assert(pt3.z() == 1.22);
    }

    void Getters_Tests_Modify() {
        constexpr Point2D pt1 = [] {
            Point2D pt {};
            pt.x() = 1.3;
            pt.y() = -1.54321;
            return pt;
        } ();

        static_assert(pt1.x() == 1.3);
        static_assert(pt1.y() == -1.54321);

        constexpr Point3D pt3 = [] {
            Point3D pt {};
            pt.x() = 1.3;
            pt.y() = -1.54321;
            pt.z() = 13.1;
            return pt;
        } ();

        static_assert(pt3.x() == 1.3);
        static_assert(pt3.y() == -1.54321);
        static_assert(pt3.z() == 13.1);

    }

    void DistanceTests() {
        Point3D pt1 {1.0, 2.3, 1}, pt2 {-1.2, 1.2, 22};
        // Point2D pt1 {1.0, 2.3}, pt2 {-1.2, 1.2};

        std::cout << pt1.distanceTo(pt2) << std::endl;
    }

    void Modify_Length() {
        const Point2D pt1 {1, -1.2}, pt2 {-4, 2.33};
        constexpr double diff = 1.38;

        Line2D line {pt1, pt2 };
        const double length1 = line.getLength();
        const auto [slope1, intercept1] = Line2D::getLineCoefficients(line);

        line.modifyLength(diff);
        const auto [slope2, intercept2] = Line2D::getLineCoefficients(line);
        const double length2 = line.getLength();

        std::cout << "Diff : " << length2 - length1 << std::endl;
        std::cout << "(" << slope1 << ", " << intercept1 << ")" << std::endl;
        std::cout << "(" << slope2 << ", " << intercept2 << ")" << std::endl;
    }
}

/*
#define SUM(x,y) (_##x + ##y)
#define MAKE_STR(x) #x
#define DECL_MAKE2(x) x* make##_##x() { return new x(); }
*/

void PointsAndLines::TestAll()
{
    // Tests::Getters_Tests();
    // Tests::Getters_Tests_Modify();
    // Tests::DistanceTests();

    Tests::Modify_Length();
}
