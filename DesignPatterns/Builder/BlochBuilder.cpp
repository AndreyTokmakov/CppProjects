/**============================================================================
Name        : BlochBuilder.cpp
Created on  : 12.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : BlochBuilder demo
============================================================================**/

#include <iostream>

namespace BlochBuilder
{
    struct IntegerValue
    {
        int value {0};
    };

    struct XPos: IntegerValue {};
    struct YPos: IntegerValue {};
    struct Width: IntegerValue {};
    struct Height: IntegerValue {};


    class Widget
    {
        /// Some implementation
    public:
        template<typename ... Args>
        Widget(Args ... args)
        {
            (set(std::forward<Args>(args)), ...);
            // (set(std::move(args)), ...);
        }

    public:
        Widget& set([[maybe_unused]] XPos xPos) {
            std::cout << "Setting the xPos" << std::endl;
            return *this;
        }

        Widget& set([[maybe_unused]] YPos yPos) {
            std::cout << "Setting the yPos" << std::endl;
            return *this;
        }

        Widget& set([[maybe_unused]] Width width) {
            std::cout << "Setting the Width" << std::endl;
            return *this;
        }

        Widget& set([[maybe_unused]] Height height) {
            std::cout << "Setting the Height" << std::endl;
            return *this;
        }
    };
}

void BlochBuilder_Test()
{
    using namespace BlochBuilder;

    [[maybe_unused]]
    const Widget w1 ( XPos {12}, YPos {12}, Height {12}, Width {12});

    std::cout << std::endl;

    [[maybe_unused]]
    const Widget w2 ( Height {12}, Width {12});

    std::cout << std::endl;

    [[maybe_unused]]
    const Widget w3 = Widget{}.set(XPos {12})
            .set(YPos {12})
            .set(Height {12})
            .set(Width {12});
}