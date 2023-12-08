
#include <iostream>
#include <algorithm>
#include <numeric>

/*
void print()
{
    std::cout << 1 << std::endl;
}
*/


#include <array>

constexpr int sum()
{
    constexpr size_t size { 10 };
    std::array<int, size> numbers {};
    std::iota(numbers.begin(), numbers.end(), 0);
    return std::accumulate(numbers.begin(), numbers.end(), 0);
}

void test()
{
    static_assert(45 == sum());
}