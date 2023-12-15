/**============================================================================
Name        : TagDispatching_RegisterIO.cpp
Created on  : 15.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TagDispatching_RegisterIO.cpp
============================================================================**/

#include <cstdint>

namespace TagDispatching_RegisterIO
{

    class Read {};
    class Write {};
    class ReadWrite : public Read, public Write {};

    template <std::uint32_t Address, typename AccessType>
    class Register
    {
        volatile std::uint32_t* const m_reg = reinterpret_cast<volatile std::uint32_t* const>(Address);

        void write(std::uint32_t value, Write) noexcept {
            *m_reg = value;
        }

        [[nodiscard]]
        std::uint32_t read(Read) const noexcept {
            return *m_reg;
        }

    public:
        Register& operator= (const std::uint32_t value) noexcept {
            write(value, AccessType{});
            return *this;
        }

        operator std::uint32_t() const noexcept {
            return read(AccessType{});
        }
    };
}


void TagDispatching_RegisterIO_Test()
{
    using namespace TagDispatching_RegisterIO;

    Register<0x4000'0000, ReadWrite> r1;  /// OK
    r1 = 10;                              /// OK
    [[maybe_unused]]
    std::uint32_t value1 = r1;            /// OK

    Register<0x4000'0000, Write> r2;      /// OK
    r2 = 10U;                             /// OK
    // std::uint32_t value2 = r2;         /// ---> Compilation error

    Register<0x4000'0000, Read> r3;       /// OK
    // r3 = 10;                           /// ---> Compilation error
    [[maybe_unused]]
    std::uint32_t value3 = r3;            /// OK
}
