/**============================================================================
Name        : FreeList.cpp
Created on  : 18.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FreeList.cpp
============================================================================**/

#include "FreeList.hpp"

#include <cstddef>

namespace
{
    struct FreeList
    {
        struct Node
        {
            Node* next;
        };

        Node* freeList = nullptr;

        static constexpr size_t blockSize = 4096;
        static constexpr size_t blocksPerChunk = 64;

        void* allocate(const size_t size)
        {
            if (size > blockSize)
                return ::operator new(size);

            if (!freeList)
                refill();

            Node* n = freeList;
            freeList = n->next;
            return n;
        }

        void deallocate(void* ptr, const size_t size)
        {
            if (size > blockSize) {
                return ::operator delete(ptr);
            }

            Node* n = static_cast<Node*>(ptr);
            n->next = freeList;
            freeList = n;
        }

    private:

        void refill()
        {
            char* chunk = static_cast<char*>(::operator new(blockSize * blocksPerChunk));
            for (size_t i = 0; i < blocksPerChunk; ++i)
            {
                Node* n = reinterpret_cast<Node*>(
                    chunk + i * blockSize);

                n->next = freeList;
                freeList = n;
            }
        }
    };

}

void free_list::TestAll()
{

}
