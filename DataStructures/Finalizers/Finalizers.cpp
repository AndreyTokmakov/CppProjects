/**============================================================================
Name        : Finalizers.cpp
Created on  : 21.06.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Finalizers.cpp
============================================================================**/

#include "Finalizers.h"

#include <iostream>
#include <functional>


namespace
{
    struct Resource
    {
        Resource()
        {
            std::cout << "Resource::Resource()"  << std::endl;
        }

        ~Resource()
        {
            std::cout << "Resource::~Resource()"  << std::endl;
        }

        void close()
        {
            std::cout << "Resource::close()"  << std::endl;
        }
    };
}

namespace Finalizers::CallCustomClassMethod
{
    template <class Type>
    struct Finalizer
    {
        Finalizer(Type& obj, void(Type::*m)()):
            object {obj}, method {m} {
        }

        ~Finalizer() {
            std::invoke(method, object);
        }

    private:

        Type& object { nullptr };
        void (Type::*method)();
    };


    void Test()
    {
        try
        {
            Resource res;
            Finalizer<Resource> finalizer {res, &Resource::close};
            throw std::runtime_error("Boom");
        }
        catch (const std::exception& exc)
        {
            std::cout << exc.what() << std::endl;
        }
    }
}


namespace Finalizers::CallDestructor
{
    template <class Type>
    struct Finalizer
    {
        explicit Finalizer(Type& obj): object { obj } {
        }

        ~Finalizer() {
            if (!success)
                object.~Type();
        }

        void setOk() {
            success = true;
        }

    private:

        bool success {false};
        Type& object;
    };

    void Test()
    {
        try
        {
            Resource res;
            Finalizer<Resource> finalizer {res};

            throw std::runtime_error("Boom");
            finalizer.setOk();
        }
        catch (const std::exception& exc)
        {
            std::cout << exc.what() << std::endl;
        }
    }
};

void Finalizers::TestAll()
{
    CallCustomClassMethod::Test();
    // CallDestructor::Test();

};