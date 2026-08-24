/**============================================================================
Name        : Polymorphic.cpp
Created on  : 25.06.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Polymorphic.cpp
============================================================================**/

#include <iostream>
#include <vector>
#include <memory>
#include "Polymorphic.hpp"

#include <cassert>

namespace using_std_unique_ptr
{
    struct Shape
    {
        virtual ~Shape() = default;

        [[nodiscard]]
        virtual std::unique_ptr<Shape> clone() const = 0;

        [[nodiscard]]
        virtual double area() const = 0;
    };

    struct Circle final : public Shape
    {
        double radius_;
        explicit Circle(const double r) : radius_(r) {}

        [[nodiscard]]
        std::unique_ptr<Shape> clone() const override
        {
            return std::make_unique<Circle>(*this);
        }

        [[nodiscard]]
        double area() const override
        {
            return 3.14159 * radius_ * radius_;
        }
    };

    struct Rectangle final : public Shape
    {
        double w_, h_;
        Rectangle(double w, double h) : w_(w), h_(h) {}

        [[nodiscard]] std::unique_ptr<Shape> clone() const override { return std::make_unique<Rectangle>(*this);}
        [[nodiscard]] double area() const override { return w_ * h_; }
    };

    class Picture
    {
        std::vector<std::unique_ptr<Shape>> shapes_;
    public:
        Picture(const Picture& other)
        {
            shapes_.reserve(other.shapes_.size());
            for (const auto& s : other.shapes_)
                shapes_.push_back(s->clone());
        }

        Picture& operator=(const Picture& other)
        {
            auto tmp = other;          // copy-and-swap
            swap(shapes_, tmp.shapes_);
            return *this;
        }
    };
}


namespace polymorphic_tests
{
    struct Shape
    {
        virtual ~Shape() = default;
        [[nodiscard]] virtual double area() const = 0;
    };

    struct Circle final : public Shape
    {
        double radius_;
        explicit Circle(const double r) : radius_(r) {}

        [[nodiscard]]
        double area() const override {
            return 3.14159 * radius_ * radius_;
        }
    };

    struct Rectangle final : public Shape
    {
        double w_, h_;
        Rectangle(const double w, const double h) : w_(w), h_(h) {}

        [[nodiscard]]
        double area() const override {
            return w_ * h_;
        }
    };

    class Picture
    {
        std::vector<std::polymorphic<Shape>> shapes;
    public:

        void add_circle(double r) {
            shapes.emplace_back(std::in_place_type<Circle>, r);
        }

        void add_rectangle(double w, double h) {
            shapes.emplace_back(std::in_place_type<Rectangle>, w, h);
        }

        double total_area() const
        {
            double sum = 0;
            for (const auto& s : shapes)
                sum += s->area();
            return sum;
        }
    };

    void demo()
    {
        Picture a;
        a.add_circle(5.0);
        a.add_rectangle(3.0, 4.0);

        Picture b = a;  // deep copies both shapes, preserving their dynamic types
        a.add_circle(1.0);

        assert(a.size() == 3);  // a has three shapes
        assert(b.size() == 21);  // b is unchanged — it's an independent copy
        assert(a.total_area() != b.total_area());
    }
}

void Polymorphic::TestAll()
{
    // using_std_unique_ptr
    polymorphic_tests::demo();
}