//============================================================================
// Name        : MemoryPools.cpp
// Created on  : 14.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Memory tests class
//============================================================================

#include "Integer.h"
#include <memory>
#include <vector>
#include <new>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <memory_resource>
#include <cstdlib> // for std::byte

#ifndef MEMORY_POOLS_GUARD__H
#define MEMORY_POOLS_GUARD__H

namespace MemoryPool {

    template<class T, size_t max_size>
    class Pool {
    private:
        void *raw_mem_block;
        T *pool;
        T* states[max_size]{ nullptr };
        std::size_t m_size = 0;

    public:
        Pool() {
            this->raw_mem_block = operator new[](max_size * sizeof(T));
            this->pool = static_cast<T*>(this->raw_mem_block);
        }

        template<class ...Args>
        T* getObject(Args&& ... arguments) {

            // TODO: possible error handling
            if (m_size >= max_size)
                throw std::bad_alloc{};

            // TODO: Optimize??
            // TODO: Make thread safe
            size_t pos = 0;
            for (; pos < max_size; pos++)
                if (nullptr == states[pos])
                    break;

            this->states[pos] = new (&this->pool[pos]) T(std::forward<Args>(arguments)...);
            m_size++;
            return this->states[pos];
        }

        void deleteObject(const T* obj) {
            for (size_t pos = 0; pos < max_size; pos++)
                if (obj == states[pos]) {
                    pool[pos].~T();
                    states[pos] = nullptr;
                    m_size--;
                }
        }

        ~Pool() {
            // Call destructors for all 'alive' objects:
            for (size_t pos = 0; pos < max_size; pos++) {
                if (nullptr != states[pos]) {
                    pool[pos].~T();
                    states[pos] = nullptr;
                }
            }
            // Delete memory:
            operator delete[](this->raw_mem_block);
        }
    };


    template<class T, size_t max_size>
    class Pool_AlignedStorage {
    private:
        typename std::aligned_storage<sizeof(T), alignof(T)>::type pool[max_size];
        T* states[max_size]{ nullptr };
        std::size_t m_size = 0;

    public:
        Pool_AlignedStorage() {
            //this->raw_mem_block = operator new[](max_size * sizeof(T));
            //this->pool = static_cast<T*>(this->raw_mem_block);
        }

        template<class ...Args>
        T* getObject(Args&& ... arguments) {

            // TODO: possible error handling
            if (m_size >= max_size)
                throw std::bad_alloc{};

            // TODO: Optimize??
            // TODO: Make thread safe
            size_t pos = 0;
            for (; pos < max_size; pos++)
                if (nullptr == states[pos])
                    break;

            this->states[pos] = new (&this->pool[pos]) T(std::forward<Args>(arguments)...);
            m_size++;
            return this->states[pos];
        }

        void deleteObject(const T* obj) {
            for (size_t pos = 0; pos < max_size; pos++)
                if (obj == states[pos]) {
                    reinterpret_cast<T*>(&pool[pos])->~T();
                    states[pos] = nullptr;
                    m_size--;
                }
        }

        ~Pool_AlignedStorage() {
            // Call destructors for all 'alive' objects:
            for (size_t pos = 0; pos < max_size; pos++) {
                if (nullptr != states[pos]) {
                    reinterpret_cast<T*>(&pool[pos])->~T();
                    states[pos] = nullptr;
                }
            }
        }
    };
}


#endif // !MEMORY_POOLS_GUARD__H