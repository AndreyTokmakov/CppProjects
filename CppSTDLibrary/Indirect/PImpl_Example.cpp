/**============================================================================
Name        : PImpl_Example.cpp
Created on  : 21.11.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PImpl_Example.cpp
============================================================================**/

#include "PImpl_Example.hpp"
#include <print>

namespace memory::indirect::pimpl_example_oldstyle
{
    struct Widget::Impl
    {
        std::string name;
        int clicks = 0;
        explicit Impl(std::string n) : name { std::move(n) } {}
    };

    Widget::Widget(const std::string& name): pImpl(new Impl(name))
    {
    }

    Widget::~Widget()
    {
        delete pImpl;
    }

    Widget::Widget(const Widget& other): pImpl(new Impl(*other.pImpl))
    {
    }

    Widget& Widget::operator=(const Widget& other)
    {
        if (this != &other)
        {
            Impl* tmp = new Impl(*other.pImpl);
            delete pImpl;
            pImpl = tmp;
        }
        return *this;
    }

    Widget::Widget(Widget&& other) noexcept: pImpl(other.pImpl)
    {
        other.pImpl = nullptr;
    }

    Widget& Widget::operator=(Widget&& other) noexcept
    {
        if (this != &other)
        {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
        return *this;
    }

    void Widget::click() const  {
        ++pImpl->clicks;
    }

    int Widget::clickCount() const {
        return pImpl->clicks;
    }

    std::string Widget::label() const {
        return pImpl->name;
    }
}

namespace memory::indirect::pimpl_example_new_style
{
    struct Widget::Impl
    {
        std::string name;
        int clicks = 0;
        explicit Impl(std::string n) : name { std::move(n) } {}
    };

    Widget::Widget(const std::string& name): pImpl { std::in_place, name }
    {}

    Widget::Widget(const Widget&) = default;
    Widget::Widget(Widget&&) noexcept = default;
    Widget& Widget::operator=(const Widget&) = default;
    Widget& Widget::operator=(Widget&&) noexcept = default;
    Widget::~Widget() = default;

    void Widget::click() {
        ++pImpl->clicks;
    }

    int Widget::clickCount() const {
        return pImpl->clicks;
    }

    std::string Widget::label() const {
        return pImpl->name;
    }
}


void memory::indirect::pimpl_example::demo()
{
    using Widget = pimpl_example_oldstyle::Widget;
    // using Widget = pimpl_example_new_style::Widget;

    Widget a("Button A");
    a.click();
    a.click();
    std::println("clicks {}", a.clickCount()); // prints "clicks 2"
    Widget b = a;
    b.click();
    std::println("clicks {}", b.clickCount()); // prints "clicks 3"
}