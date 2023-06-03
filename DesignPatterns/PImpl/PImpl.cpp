/**============================================================================
Name        : PImpl.h
Created on  : 24.02.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PImpl" Pointer to implementation
============================================================================**/

#include "PImpl.h"

#include <iostream>
#include <memory>
#include <vector>


namespace PImpl::DemoSimple
{
    class MyClassImpl {
    public:
        MyClassImpl() {
            std::cout << "MyClassImpl ctor\n";
        }

        ~MyClassImpl() {
            std::cout << "MyClassImpl dtor\n";
        }

        int invoke(int arg) {
            return arg;
        }

    private:
        // int a {0};
        // std::string s {};
    };


    class MyClass {
    public:
        MyClass(): pimpl { std::make_unique<MyClassImpl>() } {
        }

        int invoke(int arg) {
            return pimpl->invoke(arg);
        }

    private:
        std::unique_ptr<MyClassImpl> pimpl;
    };

    void test()
    {
        MyClass a;
        std::cout << a.invoke( 5 ) << std::endl;

    }
}

namespace PImpl::DemoOne
{

    class ptr_vector_base
    {
        struct impl
        {
            std::vector<void *> vp;

            void push_back(void *p) {
                vp.push_back(p);
            }

            void print() const {
                for (void const *const p: vp)
                    std::cout << p << '\n';
            }
        };

        std::unique_ptr<impl> pImpl;

    protected:
        void push_back_fwd(void *ptr) {
            pImpl->push_back(ptr);
        }

        void print() const {
            pImpl->print();
        }

    public:
        ptr_vector_base() : pImpl{std::make_unique<impl>()} {
        }

        ~ptr_vector_base() = default;
    };

    template<class T>
    class ptr_vector : private ptr_vector_base {
    public:
        void push_back(T *p) {
            push_back_fwd(p);
        }

        void print() const {
            ptr_vector_base::print();
        }
    };


    void test()
    {
        int x{}, y{}, z{};
        ptr_vector<int> v;

        v.push_back(&x);
        v.push_back(&y);
        v.push_back(&z);

        v.print();
    }
}

void Demo2();

void PImpl::TestAll()
{
    // DemoSimple::test();
    // DemoOne::test();

    Demo2();
}