/**============================================================================
Name        : VirtualTables.cpp
Created on  : 09.08.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Virtual tables experiments
============================================================================**/

#include "VirtualTables.h"

#include <iostream>

namespace ObjectOrientedExperiments::VirtualTables
{
    struct VBase
    {
        virtual void method() = 0;
        virtual ~VBase() = default;
    };

    struct VDerived: public VBase
    {
        void method() override {
            std::cout << "Derived::method()" << std::endl;
        }

        ~VDerived() override = default;
    };

    void simpleVirtualCall()
    {
        VBase* m = new VDerived();
        m->method();
        delete m;
    }

    void access_vtbl_fields()
    {
        VBase* m = new VDerived();
        const u_int64_t ***mVtable = reinterpret_cast<const u_int64_t***>(&m);

        // the value of the pointer is the address of the variable on the stack
        std::cout << "Derived VTABLE: " << **mVtable << std::endl;

        // the first dereference of mVtable gives us the address of the actual object on the heap
        std::cout << "First entry of Derived VTABLE: " << (void*) mVtable[0][0][0] << std::endl;

        // the second dereference of mVtable gives us the address of the vtable we are looking for
        //  [vtable for Derived+16]
        std::cout << "Second entry of Derived VTABLE: " << (void*) mVtable[0][0][1] << std::endl;

        // the third dereference of mVtable gives us the address of the first entry in the vtable
        // [Derived::~Derived()]
        std::cout << "Third entry of Derived VTABLE: " << (void*) mVtable[0][0][2] << std::endl;

        // nullptr
        std::cout << "4-th entry of Derived VTABLE: " << (void*) mVtable[0][0][3] << std::endl;

        //printf("Address of FCT: %p\n", (void*) &assignableFct1);
        //mVtable[0][0][2] = (long)&assignableFct1;
    }
};

// g++-14.2 -fdump-class-hierarchy VirtualTables.cpp    - should print out

void ObjectOrientedExperiments::VirtualTables::TestAll()
{
    // simpleVirtualCall();
    access_vtbl_fields();
};
