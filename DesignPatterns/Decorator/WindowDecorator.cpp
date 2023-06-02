/**============================================================================
Name        : WindowDecorator.cpp
Created on  : 01.05.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : WindowDecorator src
============================================================================**/

#include "MoneyTaxesDecoratorOne.h"

#include <iostream>
#include <string_view>
#include <memory>
#include <utility>

namespace WindowDecorator
{
    struct VisualComponent
    {
        virtual void draw() = 0;
        virtual ~VisualComponent() = default;
    };

    class TextView : public VisualComponent
    {
    public:
        void draw() override {
            std::cout << "TextView::draw ";
        }
    };

    class Decorator : public VisualComponent
    {
    public:
        Decorator() = default;

        explicit Decorator(std::shared_ptr<VisualComponent> component) :
            component {std::move( component )} {
        }

        Decorator(const Decorator&) = delete;
        Decorator& operator=(const Decorator&) = delete;

        void draw() override {
            component->draw();
        }

    protected:
        const std::shared_ptr<VisualComponent> component;
    };

    class BorderDecorator : public Decorator
    {
    public:
        BorderDecorator(std::shared_ptr<VisualComponent> component, int width):
                Decorator {std::move(component)}, width { width } {
        }

        void draw() override {
            Decorator::draw();
            drawBorder(width);
        }

    private:
        void drawBorder(int w) {
            std::cout << "BorderDecorator::drawBorder width=" << w << ' ';
        }

        int width;
    };

    class Window {
    public:
        void setContents (const std::shared_ptr<VisualComponent>& contents)
        {
            this->contents = contents;
        }

        void draw () {
            contents->draw();
            std::cout << '\n';
        }
    private:
        std::shared_ptr<VisualComponent> contents;
    };

}


void WindowDecoratorTest()
{
    using namespace WindowDecorator;

    std::unique_ptr<Window> window = std::make_unique<Window>();

    std::shared_ptr<TextView> textView = std::make_shared<TextView>();
    window->setContents(textView);
    window->draw();

    std::shared_ptr<BorderDecorator> bd1 = std::make_shared<BorderDecorator>(textView, 1);
    window->setContents(bd1);
    window->draw();

    std::shared_ptr<BorderDecorator> bd2 = std::make_shared<BorderDecorator>(textView, 2);
    std::shared_ptr<BorderDecorator> bd3 = std::make_shared<BorderDecorator>(bd2, 1);
    window->setContents(bd3);
    window->draw();
}

