#include <cstdint>
//#include <iostream>
//#include <cassert>

struct A
{
    uint64_t data_of_A;
};

struct B : A
{
    uint64_t data_of_B;
};

int main(int argc, char** argv)
{
    auto *pointer = new B();

    {
        void *address_of_B = pointer;
        void *address_of_A = static_cast<A*>(pointer);

        //std::cout << "address of B: " << address_of_B << std::endl;
        //std::cout << "address of A: " << address_of_A << std::endl;

        //assert(address_of_B == address_of_A);
    }

    delete pointer;

    return 0;
}