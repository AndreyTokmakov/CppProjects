/**============================================================================
Name        : BitReader.cpp
Created on  : 20.10.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : BitReader.cpp
============================================================================**/

#include <cstdint>
#include <iostream>

#include "BitReader.h"

#if 0

class BitReader
        {
  const uint8_t* buf; // Указатель на начало буфера.
  size_t bits; // Ширина буфера в битах.
  size_t pos { 0 };

public:

  BitReader(const uint8_t* _buf, size_t _bits) : buf(_buf), bits(_bits) {}


    /**
    * Возвращает true если в потоке еще есть как минимум 'width' непрочитанных бит.
    */
    inline bool available(const uint8_t width) const {
    }

    /**
     * Читает 'b' бит из потока начиная со старших бит в байте.
     * 0b10101100 0b00001111
     * read(3) -> 0b101
     * read(4) -> 0b0110
     * read(8) -> 0b00000111
     */

    uint64_t read(uint8_t b)
    {
        uint64_t result {0};
        const int iter = b / 8;
        // b = b % 8 - 1;
        /*
         for  (int n = 0; n < iter; ++n)
         {

         }*/

        for (; pos < pos + b; ++pos)
        {
            const uint8_t byte = buf[pos / 8];
            const uint8_t bit  = pos % 8;

        }

        return result;
    }

};
#endif

namespace BitReader
{
    struct BitReader
    {
        const uint8_t* buf; // Указатель на начало буфера.
        size_t bits;        // Ширина буфера в битах.

        BitReader(const uint8_t* _buf, size_t _bits): buf(_buf), bits(_bits) {
        }
    };
}

void BitReader::TestAll()
{

}