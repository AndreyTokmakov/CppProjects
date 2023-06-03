/**============================================================================
Name        : PImplDemo2.h
Created on  : 03.06.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PImpl Pointer to implementation
============================================================================**/

#include "PImpl.h"

#include <iostream>
#include <memory>
#include <vector>


namespace PImpl::PImplDemo2
{
    struct Interface {
        virtual ~Interface() = default;

        virtual void DoSth() = 0;
        virtual void DoConst() const = 0;
    };

    class MyClassImpl: public Interface
    {
    public:
        ~MyClassImpl() override = default;

        void DoSth() override {
            std::cout << "MyClassImpl::DoSth()\n";
        }

        void DoConst() const override {
            std::cout << "MyClassImpl::DoConst()\n";
        }
    };

    class MyClass
    {
    public:
        MyClass(std::unique_ptr<Interface> impl) : m_pImpl { std::move(impl)} {
        }

        ~MyClass() = default;

        MyClass(const MyClass& rhs) = default;
        MyClass& operator=(const MyClass& rhs) = default;

        MyClass(MyClass &&) noexcept = default;
        MyClass& operator=(MyClass &&) noexcept = default;

        /*
        MyClass(const MyClass& rhs): m_pImpl(new Interface(*rhs.m_pImpl)) {
        }

        MyClass& operator=(const MyClass& rhs) {
            if (this != &rhs)
                m_pImpl.reset(rhs.m_pImpl.release());

            return *this;
        }*/

        void DoSth()
        {
            Pimpl()->DoSth();
        }

        void DoConst() const
        {
            Pimpl()->DoConst();
        }

    private:
        [[nodiscard]]
        const Interface* Pimpl() const {
            return m_pImpl.get();
        }

        Interface* Pimpl() {
            return m_pImpl.get();
        }

        std::unique_ptr<Interface> m_pImpl;
    };
}


void Demo2()
{
    using namespace PImpl::PImplDemo2;

    std::unique_ptr<Interface> impl { std::make_unique<MyClassImpl>() };


    std::unique_ptr<MyClass> obj { std::make_unique<MyClass>(std::move(impl))};
    obj->DoSth();
    obj->DoConst();
}