/**============================================================================
Name        : ShapeFactory.h
Created on  : 27.11.2023
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : ShapeFactory
============================================================================**/

#include <iostream>
#include <array>
#include <memory>

namespace
{
    struct Shape
    {
        Shape() {
            id = total++;
        }

        virtual void draw() const = 0;

    protected:

        int id { 0 };
        static inline int total { 0 };
    };

    struct Circle : public Shape {
        void draw() const override {
            std::cout << "circle " << id << ": draw" << std::endl;
        }
    };

    struct Square : public Shape {
        void draw() const override {
            std::cout << "square " << id << ": draw" << std::endl;
        }
    };

    struct Ellipse : public Shape {
        void draw() const override {
            std::cout << "ellipse " << id << ": draw" << std::endl;
        }
    };

    struct Rectangle : public Shape {
        void draw() const override {
            std::cout << "rectangle " << id << ": draw" << std::endl;
        }
    };

    class Factory {
    public:
        virtual Shape* createCurvedInstance() = 0;
        virtual Shape* createStraightInstance() = 0;

        virtual ~Factory() = default;
    };

    class SimpleShapeFactory : public Factory {
    public:
        Shape* createCurvedInstance() override {
            return new Circle;
        }
        Shape* createStraightInstance() override {
            return new Square;
        }
    };

    class RobustShapeFactory : public Factory {
    public:
        Shape* createCurvedInstance() override {
            return new Ellipse;
        }
        Shape* createStraightInstance() override {
            return new Rectangle;
        }
    };
}


void ShapeFactory_Test()
{
    // std::unique_ptr<Factory> factory { std::make_unique<SimpleShapeFactory>()};
    std::unique_ptr<Factory> factory { std::make_unique<RobustShapeFactory>()};

    std::array<Shape*, 3> shapes {
        factory->createCurvedInstance(),
        factory->createStraightInstance(),
        factory->createCurvedInstance()
    };

    for (const Shape* shape: shapes)
        shape->draw();
}