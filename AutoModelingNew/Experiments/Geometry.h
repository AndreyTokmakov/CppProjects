//============================================================================
// Name        : Geometry.h
// Created on  : 14.02.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Geometry
//============================================================================

#ifndef CPPPROJECTS_GEOMETRY_H
#define CPPPROJECTS_GEOMETRY_H

#include <numeric>
#include <ostream>
#include <cmath>

namespace Geometry
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

        [[nodiscard]]
        constexpr inline value_type x() const noexcept {
            return _x;
        }

        [[nodiscard]]
        constexpr inline value_type y() const noexcept {
            return _y;
        }

        [[nodiscard]]
        constexpr inline value_type& x() noexcept {
            return _x;
        }

        [[nodiscard]]
        constexpr inline value_type& y() noexcept {
            return _y;
        }

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

        void setPoint1(const Point2D& dest) noexcept {
            pt2 += dest - pt1;
            pt1 = dest;
        }

        void setPoint2(const Point2D& dest) noexcept {
            pt1 += dest - pt2;
            pt2 = dest;
        }

        void setCenter(const Point2D& newCenter) noexcept {
            const Point2D& step = newCenter - getMidPoint();
            pt1 += step;
            pt2 += step;
        }

        // TODO: Refactor??
        void swapPoints() noexcept {
            std::swap(pt1, pt2);
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
};

#endif //CPPPROJECTS_GEOMETRY_H
