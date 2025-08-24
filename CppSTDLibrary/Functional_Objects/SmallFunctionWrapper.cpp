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

        template <typename Func_t>
        small_function(Func_t func)
        {
            static_assert(sizeof(Func_t) <= StorageSize, "Callable too big for buffer");

            new (&storage) Func_t(std::move(func));
            invoker = [](void* s, Args... args) -> R {
                return (*reinterpret_cast<Func_t*>(s))(std::forward<Args>(args)...);
            };
            deleter = [](void* s) {
                // reinterpret_cast<Func_t*>(s)->~Func_t();
                std::destroy_at(reinterpret_cast<Func_t*>(s));
            };
        }

        ~small_function()
        {
            if (deleter)
                deleter(&storage);
        }

        R operator()(Args... args) const
        {
            // return std::invoke((void*)&storage, std::forward<Args>(args)...);
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