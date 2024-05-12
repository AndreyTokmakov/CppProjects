#include <cstdint>

struct A
{
    uint64_t data_of_A;
};

struct B : A
{
    uint64_t data_of_B;
};

struct C : B
{
    uint64_t data_of_C;
};

int main(int argc, char** argv)
{
    return sizeof(C);
}