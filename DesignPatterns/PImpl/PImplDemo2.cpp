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

    class MyClassImplEx: public Interface
    {
    public:
        ~MyClassImplEx() override = default;

        void DoSth() override {
            std::cout << "MyClassImplEx::DoSth()\n";
        }

        void DoConst() const override {
            std::cout << "MyClassImplEx::DoConst()\n";
        }
    };


    class MyClass
    {
    public:
        explicit MyClass(std::shared_ptr<Interface> impl) : m_pImpl { std::move(impl)} {
        }

        ~MyClass() = default;

        // MyClass(const MyClass& rhs) = default;
        // MyClass& operator=(const MyClass& rhs) = default;

        MyClass(MyClass &&) noexcept = default;
        MyClass& operator=(MyClass &&) noexcept = default;

        MyClass(const MyClass& rhs) {
            m_pImpl = rhs.m_pImpl;
        }

        MyClass& operator=(const MyClass& rhs) {
            if (this != &rhs)
                m_pImpl = rhs.m_pImpl;
            return *this;
        }

        void DoSth() {
            Pimpl()->DoSth();
        }

        void DoConst() const {
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

        // std::unique_ptr<Interface> m_pImpl;
        std::shared_ptr<Interface> m_pImpl;
    };
}


void Demo2()
{
    using namespace PImpl::PImplDemo2;

    std::shared_ptr<Interface> impl1 { std::make_shared<MyClassImpl>() };
    std::shared_ptr<Interface> impl2 { std::make_shared<MyClassImplEx>() };

    std::unique_ptr<MyClass> obj1 { std::make_unique<MyClass>(impl1)};
    obj1->DoSth();
    obj1->DoConst();

    std::unique_ptr<MyClass> obj2 { std::make_unique<MyClass>(impl2)};
    obj2->DoSth();
    obj2->DoConst();

    /*
    MyClass obj1 (*obj);
    obj1.DoSth();
    obj1.DoConst();
    */
}