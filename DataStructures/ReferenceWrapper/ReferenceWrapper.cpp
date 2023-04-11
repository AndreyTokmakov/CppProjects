//============================================================================
// Name        : ReferenceWrapper.cpp
// Created on  : 08.03.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : ReferenceWrapper
//============================================================================


#include <iostream>
#include "ReferenceWrapper.h"

#include <experimental/propagate_const>

namespace ReferenceWrapper
{
    template<typename T>
    class ReferenceWrapper
    {
        using value_type = T;
        using pointer = value_type*;
        using reference = value_type&;

        pointer m_ptr;

    public:
        explicit ReferenceWrapper(reference t) noexcept : m_ptr(&t) {
        }

        operator reference() const noexcept {
            return *m_ptr;
        }

        [[nodiscard]]
        reference get() const noexcept {
            return *m_ptr;
        }
    };

}


void ReferenceWrapper::Test() {
    int a = 10;

    ReferenceWrapper<int> w(a);

    std::cout << "a = " << a << ", w = " <<  w << std::endl;

    w.get() = 11;

    std::cout << "a = " << a << ", w = " <<  w << std::endl;
}

