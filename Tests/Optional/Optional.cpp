/**============================================================================
Name        : Optional.cpp
Created on  : 16.07.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Optional
============================================================================**/

#include "Optional.h"

#include <utility>
#include "../Helpers/Utilities.h"

namespace Optional
{
    template<typename T>
    struct MyOptional
    {
        T v {};

        /*
        template<class Type>
        void set(Type&& newVal) {
            v.~T();
            v = std::forward<Type>(newVal);
        }
        */

        template<class Type>
        void set(Type&& newVal) {
            //v.~T();
            std::exchange(v, std::forward<Type>(newVal));
        }
    };

}

void Optional::TestAll()
{
    using namespace Helpers;

    MyOptional<Long> opt;
    opt.set(Long {2});
}
