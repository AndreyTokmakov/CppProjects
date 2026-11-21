/**============================================================================
Name        : PImpl_Example.hpp
Created on  : 21.11.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PImpl_Example.hpp
============================================================================**/

#ifndef CPPPROJECTS_PIMPL_EXAMPLE_HPP
#define CPPPROJECTS_PIMPL_EXAMPLE_HPP

#include <string>
#include <memory>

namespace memory::indirect::pimpl_example
{
    void demo();
}


namespace memory::indirect::pimpl_example_oldstyle
{
    class Widget
    {
    public:
        explicit Widget(const std::string& name);
        ~Widget();

        Widget(const Widget& other);
        Widget& operator=(const Widget& other);

        Widget(Widget&& other) noexcept;
        Widget& operator=(Widget&& other) noexcept;

        void click() const;
        int clickCount() const;
        std::string label() const;

    private:
        struct Impl;
        Impl* pImpl;
    };
}

namespace memory::indirect::pimpl_example_new_style
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
        int clickCount() const;
        std::string label() const;

    private:
        struct Impl;
        std::indirect<Impl> pImpl;
    };
}


#endif //CPPPROJECTS_PIMPL_EXAMPLE_HPP
