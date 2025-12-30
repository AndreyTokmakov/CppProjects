/**============================================================================
Name        : UniquePtr.cpp
Created on  : 30.12.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : UniquePtr.cpp
============================================================================**/

#include "UniquePtr.hpp"
#include "../Utilities/Integer.h"


#include <concepts>
#include <format>
#include <utility>

namespace
{
    template<typename T>
    struct DefaultDeleter
    {
        void operator()(const T* ptr) {
            std::cout << "DefaultDeleter(" << ptr << ")\n";
            delete ptr;
        }

        friend void swap(DefaultDeleter& lhs, DefaultDeleter& rhs) noexcept {
            // TODO:
        }
    };

    template<typename T>
    struct DefaultDeleter<T[]>
    {
        void operator()(const T* ptr) {
            std::cout << "DefaultArrayDeleter(" << ptr << ")\n";
            delete[] ptr;
        }

        friend void swap(DefaultDeleter& lhs, DefaultDeleter& rhs) noexcept {
            // TODO:
        }
    };

    template<typename DelType, typename ArgType>
    concept HadDeleterOperator = requires(DelType del, ArgType* ptr) {
        { del.operator()(ptr) } -> std::same_as<void>;
    };
}

namespace
{
    template<typename T, HadDeleterOperator<T> Deleter = DefaultDeleter<T>>
    // template<typename T, typename  Deleter = DefaultDeleter<T>>
    struct UniquePtr : public Deleter
    {
        using deleter_type    = Deleter;
        using value_type      = T;
        using pointer         = value_type*;
        using const_pointer   = const pointer;
        using reference       = value_type&;
        using const_reference = const value_type&;

		static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        UniquePtr(const UniquePtr&) = delete;
        UniquePtr& operator=(const UniquePtr&) = delete;

        explicit UniquePtr(const_pointer ptr) : ptr { ptr }
        {
        }

        /*
        void swap(unique_ptr& other) noexcept
        {
            std::swap(m_underlying_ptr, other.m_underlying_ptr);
            // To keep our code generic, we also swap the deleter, since
            // the deleter can be stateful. Consider for example:
            // ArenaAllocator arena{4096};
            // std::unique_ptr<Foo, ArenaAllocationDeleter{&arena}> fooPtr;
            // When we swap two unique pointers, you expect the point-to-pool or arena
            // to be swapped as well.
            deleter_type* this_deleter = static_cast<deleter_type*>(this);
            deleter_type* other_deleter = static_cast<deleter_type*>(&other);
            std::swap(this_deleter, other_deleter);
        }*/

        UniquePtr(UniquePtr&& other) noexcept
          : deleter_type { std::move(static_cast<deleter_type>(other)) }
          , ptr { std::exchange(other.ptr, nullptr) }
        {
        }

        UniquePtr& operator=(UniquePtr&& other) noexcept {
            UniquePtr(std::move(other)).swap(*this);
            return *this;
        }

        ~UniquePtr()
        {
            deleter_type* deleter = static_cast<deleter_type*>(this);
            (*deleter)(ptr);
        }

        [[nodiscard]]
        value_type operator*() {
            return *ptr;
        }

        [[nodiscard]]
        pointer operator->() {
            return ptr;
        }

        [[nodiscard]]
        pointer get() const {
            return ptr;
        }

        [[nodiscard]]
        pointer release() {
            return std::exchange(ptr, nullptr);
        }

        void reset(pointer other)
        {
            if (ptr != other) {
                (*static_cast<deleter_type*>(this))(ptr);
                ptr = other;
            }
        }

        explicit operator bool() const {
            return ptr;
        }

        friend auto operator<=>(const UniquePtr& lhs, const UniquePtr& rhs) {
            return lhs.get() <=> rhs.get();
        }

        friend bool operator==(const UniquePtr& lhs, std::nullptr_t) {
            return nullptr == lhs.get();
        }

        friend bool operator==(std::nullptr_t, const UniquePtr& rhs) {
            return nullptr == rhs.get();
        }

        friend void swap(UniquePtr& lhs, UniquePtr& rhs) noexcept {
            lhs.swap(rhs);
        }

    private:

        pointer ptr { nullptr };
    };
}

#if 0
namespace
{
    template<typename T, typename Deleter>
    struct UniquePtr<T[], Deleter> : public Deleter
    {
        using deleter_type    = Deleter;
        using value_type      = T;
        using pointer         = value_type*;
        using const_pointer   = const pointer;
        using reference       = value_type&;
        using const_reference = const value_type&;

		static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");

        UniquePtr(const UniquePtr&) = delete;
        UniquePtr& operator=(const UniquePtr&) = delete;

        explicit UniquePtr(const_pointer ptr) : ptr { ptr }
        {
        }

        /*
        void swap(unique_ptr& other) noexcept
        {
            std::swap(m_underlying_ptr, other.m_underlying_ptr);
            // To keep our code generic, we also swap the deleter, since
            // the deleter can be stateful. Consider for example:
            // ArenaAllocator arena{4096};
            // std::unique_ptr<Foo, ArenaAllocationDeleter{&arena}> fooPtr;
            // When we swap two unique pointers, you expect the point-to-pool or arena
            // to be swapped as well.
            deleter_type* this_deleter = static_cast<deleter_type*>(this);
            deleter_type* other_deleter = static_cast<deleter_type*>(&other);
            std::swap(this_deleter, other_deleter);
        }*/

        UniquePtr(UniquePtr&& other) noexcept
          : deleter_type { std::move(static_cast<deleter_type>(other)) }
          , ptr { std::exchange(other.ptr, nullptr) }
        {
        }

        UniquePtr& operator=(UniquePtr&& other) noexcept {
            UniquePtr(std::move(other)).swap(*this);
            return *this;
        }

        ~UniquePtr()
        {
            deleter_type* deleter = static_cast<deleter_type*>(this);
            (*deleter)(ptr);
        }

        [[nodiscard]]
        value_type operator*() {
            return *ptr;
        }

        [[nodiscard]]
        pointer operator->() {
            return ptr;
        }

        [[nodiscard]]
        pointer get() const {
            return ptr;
        }

        [[nodiscard]]
        pointer release() {
            return std::exchange(ptr, nullptr);
        }

        void reset(pointer other)
        {
            if (ptr != other) {
                (*static_cast<deleter_type*>(this))(ptr);
                ptr = other;
            }
        }

        explicit operator bool() const {
            return ptr;
        }

        friend auto operator<=>(const UniquePtr& lhs, const UniquePtr& rhs) {
            return lhs.get() <=> rhs.get();
        }

        friend bool operator==(const UniquePtr& lhs, std::nullptr_t) {
            return nullptr == lhs.get();
        }

        friend bool operator==(std::nullptr_t, const UniquePtr& rhs) {
            return nullptr == rhs.get();
        }

        friend void swap(UniquePtr& lhs, UniquePtr& rhs) noexcept {
            lhs.swap(rhs);
        }

    private:

        pointer ptr { nullptr };
    };
}
#endif


namespace
{
    template<typename Ty, typename ... ArgsTypes>
    UniquePtr<Ty> makeUnique(ArgsTypes&& ... args) {
        return UniquePtr<Ty>(new Ty(std::forward<ArgsTypes>(args)...));
    }
}

void Memory::UniquePtrTests()
{
    UniquePtr<Integer> iuPtr1 { makeUnique<Integer>(11) };
    UniquePtr<Integer> iuPtr2 { makeUnique<Integer>(22) };

    /*
    using IntArray = Integer*;
    // IntArray* ptr = new IntArray[3];
    UniquePtr<IntArray> iuPtr { new IntArray[3]};
    // IntArray array = {Integer{1}, Integer{2}, Integer{3}, Integer{4}, Integer{5}};
    */
}

