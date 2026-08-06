/**============================================================================
Name        : PImplDemo2.cpp
Created on  : 03.06.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PImpl Pointer to implementation
============================================================================**/

#include "PImpl.hpp"

#include <iostream>
#include <memory>


namespace
{
    struct Interface
    {
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


    class MyClass: public Interface
    {
    public:
        explicit MyClass(std::shared_ptr<Interface> impl) : m_pImpl { std::move(impl)} {
        }

        ~MyClass() override = default;

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

        void DoSth() override {
            Pimpl()->DoSth();
        }

        void DoConst() const override {
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

void pimpl::pimpl_demo_2::TestAll()
{

    std::shared_ptr<Interface> impl1 { std::make_shared<MyClassImpl>() };
    std::shared_ptr<Interface> impl2 { std::make_shared<MyClassImplEx>() };
    std::unique_ptr<Interface> client { std::make_unique<MyClass>(impl1)};

    client->DoSth();
    client->DoConst();

    client= std::make_unique<MyClass>(impl2);

    client->DoSth();
    client->DoConst();
}