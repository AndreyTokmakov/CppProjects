/**============================================================================
Name        : UnorderedPair.h
Created on  : 06.11.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : UnorderedPair
============================================================================**/

#ifndef CPPPROJECTS_UNORDEREDPAIR_H
#define CPPPROJECTS_UNORDEREDPAIR_H

#include <iostream>

namespace UnorderedPair::Utilities
{
    template<typename T>
    struct UnorderedPair: std::pair<T, T>
    {
        UnorderedPair(T a, T b): std::pair<T, T> {a, b} {
        }

        template<typename Ty>
        friend std::ostream& operator<<(std::ostream& stream, const UnorderedPair<Ty>& pair);

        template<typename Ty>
        friend bool operator==(const UnorderedPair<Ty>& p1, const UnorderedPair<Ty>& p2);
    };

    template<typename T>
    struct PairHashUnordered
    {
        std::size_t operator()(const UnorderedPair<T>& pair) const noexcept
        {
            const std::size_t h1 = std::hash<T>{}(std::min(pair.first, pair.second));
            const std::size_t h2 = std::hash<T>{}(std::max(pair.first, pair.second));
            return h1 ^ (h2 << 1);
        }
    };

    template<typename Ty>
    std::ostream& operator<<(std::ostream& stream, const UnorderedPair<Ty>& pair)
    {
        stream << "(" << pair.first << ", " << pair.second << ")";
        return stream;
    }

    template<typename Ty>
    bool operator==(const UnorderedPair<Ty>& p1, const UnorderedPair<Ty>& p2)
    {
        return (p1.first == p2.first && p1.second == p2.second) ||
               (p1.second == p2.first && p1.first == p2.second);
    }
}

#endif //CPPPROJECTS_ITERATORS_H
