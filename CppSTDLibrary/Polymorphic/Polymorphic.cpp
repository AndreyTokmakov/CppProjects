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
#include <print>
#include <numeric>
#include <source_location>
#include "Polymorphic.hpp"


namespace
{
    void Assert(const bool condition,
                 const std::source_location location = std::source_location::current())
    {
        if (!condition)
        {
            std::println(std::cerr, "\tFile: {}\n\tFunction: {}\n\tLine: {}",
                         location.file_name(), location.function_name(), location.line());
            std::terminate();
        }
    }
}


namespace using_std_unique_ptr
{
    struct IShape
    {
        virtual ~IShape() = default;

        [[nodiscard]]
        virtual std::unique_ptr<IShape> clone() const = 0;

        [[nodiscard]]
        virtual double area() const = 0;
    };

    struct Circle final : IShape
    {
        explicit Circle(const double r) : radius(r)
        {
        }

        [[nodiscard]]
        std::unique_ptr<IShape> clone() const override
        {
            return std::make_unique<Circle>(*this);
        }

        [[nodiscard]]
        double area() const override
        {
            return 3.14159 * radius * radius;
        }

    private:
        double radius { 0.0 };
    };

    struct Rectangle final : IShape
    {
        Rectangle(const double w, const double h) : wight(w), height(h)
        {
        }

        [[nodiscard]]
        std::unique_ptr<IShape> clone() const override
        {
            return std::make_unique<Rectangle>(*this);
        }

        [[nodiscard]]
        double area() const override
        {
            return wight * height;
        }

    private:
        double wight { 0.0 };
        double height { 0.0 };
    };

    struct Picture
    {
        Picture() = default;

        void addCircle(double r) {
            shapes.push_back(std::make_unique<Circle>(r));
        }

        void addRectangle(double w, double h) {
            shapes.push_back(std::make_unique<Rectangle>(w, h));
        }

        Picture(const Picture& other)
        {
            shapes.reserve(other.shapes.size());
            for (const std::unique_ptr<IShape>& s : other.shapes) {
                shapes.push_back(s->clone());
            }
        }

        Picture& operator=(const Picture& other)
        {
            Picture tmp = other;          // copy-and-swap
            std::swap(shapes, tmp.shapes);
            return *this;
        }

        Picture(Picture&& other) noexcept = default;
        Picture& operator=(Picture&& other) noexcept = default;


        double totalArea() const
        {
            return std::accumulate(shapes.cbegin(), shapes.cend(), 0.0, []
                 (const double val, const std::unique_ptr<IShape>& shape)
             {
                 return val + shape->area();
             });
        }

        size_t Size() const noexcept {
            return shapes.size();
        }

    private:
        std::vector<std::unique_ptr<IShape>> shapes;

    };

    void demo()
    {
        Picture pic1;
        pic1.addCircle(5.0);
        pic1.addRectangle(3.0, 4.0);

        Picture pic2 = pic1;

        pic1.addCircle(1.0);

        Assert(pic1.Size() == 3);  // pic1 has three shapes
        Assert(pic2.Size() == 2);  // pic2 is unchanged — it's an independent copy
        Assert(pic1.totalArea() != pic2.totalArea());

        std::cout << pic1.totalArea() << std::endl;
    }
}


namespace polymorphic_tests
{
    struct Shape
    {
        virtual ~Shape() = default;
        [[nodiscard]]
        virtual double area() const = 0;
    };

    struct Circle final : Shape
    {
        explicit Circle(const double r) : radius(r) {
        }

        [[nodiscard]]
        double area() const override {
            return 3.14159 * radius * radius;
        }

    private:
        double radius { 0.0 };
    };

    struct Rectangle final : Shape
    {
        Rectangle(const double w, const double h) : wight(w), height(h) {}

        [[nodiscard]]
        double area() const override {
            return wight * height;
        }

    private:
        double wight { 0.0 };
        double height { 0.0 };
    };

    class Picture
    {
        std::vector<std::polymorphic<Shape>> shapes;
    public:

        void addCircle(double r) {
            shapes.emplace_back(std::in_place_type<Circle>, r);
        }

        void addRectangle(double w, double h) {
            shapes.emplace_back(std::in_place_type<Rectangle>, w, h);
        }

        double totalArea() const
        {
            return std::accumulate(shapes.cbegin(), shapes.cend(), 0.0, []
                (const double val, const std::polymorphic<Shape>& shape)
            {
                return val + shape->area();
            });
        }

        size_t Size() const noexcept
        {
            return shapes.size();
        }
    };

    void demo()
    {
        Picture a;
        a.addCircle(5.0);
        a.addRectangle(3.0, 4.0);

        const Picture b = a;  // deep copies both shapes, preserving their dynamic types
        a.addCircle(1.0);

        Assert(a.Size() == 3);  // a has three shapes
        Assert(b.Size() == 2);  // b is unchanged — it's an independent copy
        Assert(a.totalArea() != b.totalArea());

        std::cout << a.totalArea() << std::endl;
    }
}

void Polymorphic::TestAll()
{
    using_std_unique_ptr::demo();
    // polymorphic_tests::demo();
}