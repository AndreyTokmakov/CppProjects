/**============================================================================
Name        : CircularBuffer.cpp
Created on  : 07.07.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CircularBuffer.cpp
============================================================================**/

#include "CircularBuffer.h"

#include <iostream>
#include <vector>
#include <array>
#include <memory>

namespace CircularBuffer
{
    template<typename T, size_t Capacity>
    struct CircularBuffer
    {
        using value_type = T;
        using size_type = size_t;

        static_assert(!std::is_same_v<value_type, void>, "ERROR: Type of the CircularBuffer can not be void");
        static_assert(0 != Capacity, "Please try a little bigger buffer");

        // TODO: ????
        // static_assert(is_pow_of_2(Capacity), "Capacity shall be power of 2");

        std::array<value_type, Capacity> buffer {};
        size_type writePos {0};
        size_type readPos {0};
        bool overflow { false };

        bool read(value_type& output) noexcept
        {
            if (writePos == readPos)
                return false;

            output = buffer[readPos++];
            if (readPos >= Capacity) {
                readPos = 0;
                overflow = false;
            }

            return true;
        }

        bool add(value_type value) noexcept
        {
            if (overflow && writePos == readPos)
            {
                return false;
            }

            buffer[writePos++] = value;

            if (writePos >= Capacity) {
                writePos = 0;
                overflow = true;
            }
            return true;
        }

        void print()
        {
            for (int i = 0; i < Capacity; ++i)
                std::cout << '[' << i << "] = " << buffer[i] << ", ";
            std::cout << "(" << readPos << ", " << writePos << ") | overflow = " << std::boolalpha << overflow << std::endl;
        }
    };
}

void CircularBuffer::TestAll()
{
    CircularBuffer<int, 5> buffer;

    auto get = [&]() {
        if (int val = 0; buffer.read(val))
            std::cout << val << std::endl;
        else
            std::cout << "GET False" << std::endl;
    };

    auto add = [&](int v) {
        if (buffer.add(v))
            std::cout << v << " added" << std::endl;
        else
            std::cout << "ADD False" << std::endl;
    };

    buffer.print();

    add(1);
    add(2);
    add(3);

    buffer.print();

    get();

    buffer.print();

    get();
    get();
    get();

    add(4);
    add(5);
    add(6);
    add(7);
    //add(8);

    buffer.print();

    get();
    get();
    get();
    get();

    buffer.print();
}
