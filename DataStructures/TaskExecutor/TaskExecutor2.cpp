/**============================================================================
Name        : TaskExecutor2.cpp
Created on  : 06.05.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TaskExecutor2.cpp
============================================================================**/

#include "Executors.hpp"

#include <iostream>
#include <string_view>
#include <vector>
#include <print>
#include <utility>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "DateTimeUtilities.hpp"

namespace
{
    using utilities::datetime::getCurrentTime;

#define LOG  std::osyncstream { std::cout } << getCurrentTime() << " "
}


namespace executor
{
    class Task
    {
    public:

        static constexpr size_t SsbSize { 64 };
        static constexpr size_t SsbAlignment { alignof(std::max_align_t) };

        Task() = default;

        ~Task()
        {
            reset();
        }

        Task(Task&& other) noexcept
        {
            moveFrom(std::move(other));
        }

        Task& operator=(Task&& other) noexcept
        {
            if (this != &other)
            {
                reset();
                moveFrom(std::move(other));
            }
            return *this;
        }

        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;

        template<typename FuncType>
        static Task create(FuncType&& func)
        {
            using Fn = std::decay_t<FuncType>;
            using FnPtr = Fn*;

            constexpr bool fitsInline = sizeof(Fn) <= SsbSize && alignof(Fn) <= SsbAlignment &&
                std::is_nothrow_move_constructible_v<Fn>;

            Task task;
            if constexpr (fitsInline)
            {
                new (task.storage.data()) Fn { std::forward<FuncType>(func) };
                task.ptr_ = task.storage.data();
                task.inlineCallback = true;
            }
            else
            {
                task.ptr_ = new Fn { std::forward<FuncType>(func) };
                task.inlineCallback = false;
            }

            task.invokeCallback = [](void* p) {
                (*static_cast<FnPtr>(p))();
            };
            task.destroyCallback = [](void* p) {
                static_cast<FnPtr>(p)->~Fn();
            };
            task.moveCallback = [](void* dst, void* src) {
                new (dst) Fn(std::move(*static_cast<FnPtr>(src)));
            };

            return task;
        }

        void operator()() const
        {
            invokeCallback(ptr_);
        }

        explicit operator bool() const
        {
            return invokeCallback != nullptr;
        }

        void reset()
        {
            if (invokeCallback == nullptr)
                return;
            if (inlineCallback) {
                destroyCallback(ptr_);
            } else {
                destroyCallback(ptr_);
                ::operator delete(ptr_);
            }

            ptr_ = nullptr;
            invokeCallback = nullptr;
            destroyCallback = nullptr;
            moveCallback = nullptr;
            inlineCallback = false;
        }

    private:

        void moveFrom(Task&& other)
        {
            invokeCallback = other.invokeCallback;
            destroyCallback = other.destroyCallback;
            moveCallback = other.moveCallback;
            inlineCallback = other.inlineCallback;

            if (other.ptr_ == nullptr)
                return;

            if (inlineCallback) {
                moveCallback(storage.data(), other.storage.data());
                ptr_ = storage.data();
                other.destroyCallback(other.ptr_);
            } else {
                ptr_ = other.ptr_;
            }

            other.ptr_ = nullptr;
            other.invokeCallback = nullptr;
            other.destroyCallback = nullptr;
            other.moveCallback = nullptr;
            other.inlineCallback = false;
        }


        alignas(SsbAlignment) std::array<char, SsbSize> storage {};

        void* ptr_ = nullptr;
        void (*invokeCallback)(void*) { nullptr };
        void (*destroyCallback)(void*) { nullptr };
        void (*moveCallback)(void*, void*) { nullptr };
        bool inlineCallback { false };
    };

}

void task_executor::TaskExecutor2::TestAll()
{
;
}
