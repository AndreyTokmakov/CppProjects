/**============================================================================
Name        : DrawingBridgeTwo.cpp
Created on  : 03.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DrawingBridgeTwo src
============================================================================**/

#include <iostream>
#include <memory>

namespace
{
    struct IDrawingAPI
    {
        virtual void drawCircle(double x, double y, double radius) = 0;
        virtual ~IDrawingAPI() = default;
    };

    struct IShape
    {
        virtual ~IShape() = default;
        virtual void draw() = 0;
        virtual void resizeByPercentage(double pct) = 0;
    };


    class DrawingAPI1 : public IDrawingAPI {
    public:
        void drawCircle(double x, double y, double radius) override {
            std::cout << "API1.circle at " << x << ':' << y << ' ' << radius << std::endl;
        }
    };

    class DrawingAPI2 : public IDrawingAPI {
    public:
        void drawCircle(double x, double y, double radius) override {
            std::cout << "API2.circle at " << x << ':' << y << ' ' <<  radius << std::endl;
        }
    };

    class CircleShape : public IShape
    {
        double x { 0.0 };
        double y { 0.0 };
        double radius { 0.0 };

        std::shared_ptr<IDrawingAPI> drawingAPI;

    public:
        CircleShape(double x, double y, double radius, std::shared_ptr<IDrawingAPI> api) :
                x { x }, y { y }, radius { radius }, drawingAPI {std::move(api)} {
        }

        void draw() override {
            drawingAPI->drawCircle(x, y, radius);
        }

        void resizeByPercentage(double pct) override {
            radius *= pct;
        }
    };
}

void Bridge_DrawingAPI_Two_Test()
{
    CircleShape circle1(1,2,3,std::make_shared<DrawingAPI1>());
    CircleShape circle2(5,7,11,std::make_shared<DrawingAPI2>());
    circle1.resizeByPercentage(2.5);
    circle2.resizeByPercentage(2.5);
    circle1.draw();
    circle2.draw();
}