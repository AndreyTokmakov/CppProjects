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

namespace ReferenceWrapper
{
    template<typename T>
    class ReferenceWrapper {
        T *m_ptr;
    public:
        explicit ReferenceWrapper(T& t) noexcept : m_ptr(&t) {
        }

        operator T& () const noexcept {
            return *m_ptr;
        }

        [[nodiscard]]
        T& get() const noexcept {
            return *m_ptr;
        }
    };

}


void ReferenceWrapper::Test() {
    int a = 10;
    ReferenceWrapper<int> w(a);

    std::cout << "a = " << a << ", w = " <<  w << std::endl;

}

