/**============================================================================
Name        : pImpl.cpp
Created on  : 14.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : pImpl.cpp
============================================================================**/

#include "Indirect.hpp"

#include <cassert>
#include <string>
#include <memory>

namespace
{
    class Widget
    {
    public:
        explicit Widget(const std::string& name);

        Widget(const Widget&);
        Widget(Widget&&) noexcept;
        Widget& operator=(const Widget&);
        Widget& operator=(Widget&&) noexcept;
        ~Widget();

        void click();

        [[nodiscard]]
        int clickCount() const;

        [[nodiscard]]
        std::string label() const;

    private:
        struct Impl;
        std::indirect<Impl> pimpl;
    };
}

namespace
{
    struct Widget::Impl
    {
        std::string name;
        int clicks = 0;
        explicit Impl(std::string n) : name(std::move(n))
        {

        }
    };

    Widget::Widget(const std::string& name): pimpl(std::in_place, name)
    {}

    Widget::Widget(const Widget&) = default;
    Widget::Widget(Widget&&) noexcept = default;
    Widget& Widget::operator=(const Widget&) = default;
    Widget& Widget::operator=(Widget&&) noexcept = default;
    Widget::~Widget() = default;

    void Widget::click()
    {
        assert(!pimpl.valueless_after_move() && "use of moved-from Widget");
        ++pimpl->clicks;
    }

    int  Widget::clickCount() const {
        return pimpl->clicks;
    }

    std::string Widget::label() const{
        return pimpl->name;
    }
}


void memory::indirect::pimpl::TestAll()
{

}