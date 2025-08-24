/**============================================================================
Name        : SmallFunctionWrapper.cpp
Created on  : 24.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SmallFunctionWrapper.cpp
============================================================================**/

#include "SmallFunctionWrapper.hpp"
#include <iostream>

namespace
{
    template <typename Signature, size_t StorageSize = 32>
    struct small_function; /** simplified std::function with SBO **/

    template <typename R, typename... Args, size_t StorageSize>
    struct small_function<R(Args...), StorageSize>
    {
        small_function() = default;

        template <typename F>
        small_function(F f)
        {
            static_assert(sizeof(F) <= StorageSize, "Callable too big for buffer");
            new (&storage) F(std::move(f));
            invoker = [](void* s, Args... args) -> R {
                return (*reinterpret_cast<F*>(s))(std::forward<Args>(args)...);
            };
            deleter = [](void* s) {
                reinterpret_cast<F*>(s)->~F();
            };
        }

        ~small_function()
        {
            if (deleter)
                deleter(&storage);
        }

        R operator()(Args... args) const {
            return invoker((void*)&storage, std::forward<Args>(args)...);
        }

    private:
        alignas(std::max_align_t) char storage[StorageSize];
        R (*invoker)(void*, Args...) { nullptr };
        void (*deleter)(void*) { nullptr };
    };

}



void SmallFunctionWrapper::TestAll()
{
    small_function<void()> f = [x=42]() { std::cout << "x=" << x << "\n"; };
    f();
}